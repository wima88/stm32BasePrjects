/*
 * maincpp.cpp
 *
 *  Created on: Feb 15, 2021
 *      Author: Wimansha
 */




#include <maincpp.h>
#include <ros.h>
#include <std_msgs/String.h>
#include "geometry_msgs/Twist.h"
#include "xl430_address.h"

ros::NodeHandle nh;
extern UART_HandleTypeDef huart3;
std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);
void cmd_velCallBack(const geometry_msgs::Twist& velInfo);
ros::Subscriber<geometry_msgs::Twist> _sub("/cmd_vel", &cmd_velCallBack);


char hello[] = "Hello world!";
float _roverBase_width =0.1;
float _wheelRadius = 0.001;


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1)
	{
  nh.getHardware()->flush();
	}
}

void cmd_velCallBack(const geometry_msgs::Twist &velInfo)
{
 float lin_x = velInfo.linear.x;
 float ang_z = velInfo.angular.z;

 float leftWheelSpeed = lin_x - ((_roverBase_width/2)*ang_z); // linear vel
 float rightWheelSpeed = lin_x + ((_roverBase_width/2)*ang_z);// linear vel

 int leftRpm_reg_val = ((leftWheelSpeed)/(0.10472*_wheelRadius))/0.23;
 int righttRpm_reg_val = ((rightWheelSpeed)/(0.10472*_wheelRadius))/0.23;

 xl430_writeToAddress(2, leftRpm_reg_val, &GOAL_VELOCITY, &REG_WR);
 osDelay(5);
 xl430_writeToAddress(1, righttRpm_reg_val, &GOAL_VELOCITY, &REG_WR);
 osDelay(5);
 xl430_Action();


}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  nh.getHardware()->reset_rbuf();
}

void setup(void)
{
	nh.initNode();
	while(!nh.connected())
	 {
	   nh.spinOnce();
	 }
	 nh.loginfo("stm32 Connected");

	 nh.loginfo("pinging ID 02..");
	 while(!xl430_ping(02))
	  {
	    nh.loginfo("[ID 02] ping Fails");
	    HAL_Delay(5);
	  }
	 nh.loginfo("[ID 02] ping Success !");
	   HAL_Delay(5);
	nh.loginfo("pinging ID 01..");
	 while(!xl430_ping(01))
	  {
	   nh.loginfo("[ID 01] ping Fails");
	    HAL_Delay(5);
	  }
	nh.loginfo("[ID 01] ping Success !");

	 while(! nh.advertise(chatter))
	{
		nh.spinOnce();
	}

	 nh.subscribe(_sub);
	 nh.negotiateTopics();


	 if(!nh.getParam("base_width", &_roverBase_width))
	 {
		 _roverBase_width=0.2;

	 }
	 if(!nh.getParam("wheel_radius",&_wheelRadius))
	 {
		 _wheelRadius = 0.02;
	 }




	 xl430_writeToAddress(0xFE, 1, &TORQUE_ENABLE,&WRITE);


}

void StartDefaultTask(void *argument)
{
	for(;;)
	 {
	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	  str_msg.data = hello;
	  chatter.publish(&str_msg);
	  nh.spinOnce();
	  if(!nh.connected())
	  {
		  osDelay(5);
		  xl430_writeToAddress(0xFE, 0, &TORQUE_ENABLE,&WRITE);
	  }
	  osDelay(10);
	  }
}


void setup_dummy(void)
{

	 xl430_writeToAddress(0xFE, 1, &TORQUE_ENABLE,&WRITE);


}
