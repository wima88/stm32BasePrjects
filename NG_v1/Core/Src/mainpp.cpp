/*
 * main.cpp

 *
 *  Created on: 2018/01/17
 *      Author: yoneken
 */
#include <mainpp.h>
#include <ros.h>
#include <std_msgs/String.h>
#include "geometry_msgs/Twist.h"
#include <string.h>
// include following headers

#include "stm32f7xx_hal.h"
#include "main.h"
#include "xl430.h"

//ros variables
ros::NodeHandle nh;
std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);
char hello[] = "Hello world!";
char ros_msg[64] ;
geometry_msgs::Twist wheelFeedback_msgs;

//bot variables
float _roverBase_width =0.75;
float _wheelRadius = 0.06;


//servo variables
struct prsRxData motor_info[1];
xl430_EEPROM_Typrdef l_servo,r_servo;

void servo_init();

//CalbBack functions
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  nh.getHardware()->flush();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  nh.getHardware()->reset_rbuf();
}

void cmd_velCallBack(const geometry_msgs::Twist& velInfo);

//subscribers and publishers
ros::Publisher wheelFeedback_pub("wheelFeedback", &wheelFeedback_msgs);
ros::Subscriber<geometry_msgs::Twist> _sub("/cmd_vel", &cmd_velCallBack);

void setup(void)
{
	nh.initNode();
	while(!nh.connected()) //wait until connection established
	{
		nh.spinOnce();
		HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
		HAL_Delay(50);
	}
	nh.loginfo("Connection established");

	//publishers
	nh.loginfo("advertising publishers");
	while(! nh.advertise(chatter)) // wait until  advertise
	{
		nh.spinOnce();
	}

	//subscribers
	nh.loginfo("Waiting for subscribers");
	while(! nh.subscribe(_sub))
	{
		nh.spinOnce();
	}

	nh.negotiateTopics(); // void any topic complications when sync

	nh.loginfo("Enabling motor Torque");

	servo_init(); // need to call only one time

	motor_info[0] = xl430_torqeEnable(1,true);
	motor_info[1] = xl430_torqeEnable(2,true);

	if((motor_info[0].crc_check && motor_info[0].errorFlag ==0)  &&
			(motor_info[1].crc_check && motor_info[1].errorFlag ==0))
	{
		nh.loginfo("motor Torque enabled");
		xl430_LED(1,true);
		xl430_LED(2,true);
	}
	else
	{
		nh.logerror("Error in motor drivers,Torques not enabled");
	}


}

void loop(void)
{
  HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
  str_msg.data = hello;
  chatter.publish(&str_msg);

 // xl430_writeMotorSpeeds(100,50);

  if(!nh.connected())
  {
	  motor_info[0] = xl430_torqeEnable(1,false);
	  motor_info[1] = xl430_torqeEnable(2,false);
	  if((motor_info[0].crc_check && motor_info[0].errorFlag ==0)  &&
	  			(motor_info[1].crc_check && motor_info[1].errorFlag ==0))
	  	{
	  		xl430_LED(1,false);
	  		xl430_LED(2,false);
	  	}
	  setup();
  }
  nh.spinOnce();
  HAL_Delay(500);
}

void cmd_velCallBack(const geometry_msgs::Twist &velInfo)
{
 float lin_x = velInfo.linear.x;
 float ang_z = velInfo.angular.z;



 float leftWheelSpeed = lin_x - ((_roverBase_width/2)*ang_z); // linear vel
 float rightWheelSpeed = lin_x + ((_roverBase_width/2)*ang_z);// linear vel

 int leftRpm_reg_val = ((leftWheelSpeed)/(0.10472*_wheelRadius))/0.23;
 int righttRpm_reg_val = ((rightWheelSpeed)/(0.10472*_wheelRadius))/0.23;

 xl430_writeMotorSpeeds(leftRpm_reg_val,righttRpm_reg_val);


}

void servo_init()
{
	r_servo.ID = 0x01;
	r_servo._driveDirection = REVERSE_MODE;
	r_servo._operatingMode = VELOCITY_CONTROL;
	r_servo._profile = VELOCITY_PROFILE;

	l_servo.ID = 0x02;
	l_servo._driveDirection = NORMAL_MODE;
	l_servo._operatingMode = VELOCITY_CONTROL;
	l_servo._profile = VELOCITY_PROFILE;

	 xl430_setDrivermode(l_servo);
	 HAL_Delay(500);
	 xl430_getDrivermode(0x01);
	 xl430_getDrivermode(0x02);
}


