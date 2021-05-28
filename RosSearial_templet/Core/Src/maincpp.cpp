/*
 * maincpp.cpp
 *
 *  Created on: Feb 15, 2021
 *      Author: Wimansha
 */




#include <maincpp.h>
#include <ros.h>
#include <ros/time.h>
#include <std_msgs/String.h>
#include "geometry_msgs/Twist.h"
#include <sensor_msgs/Imu.h>
#include "bno055_stm32.h"

#include "xl430_address.h"
#include <stdio.h>

ros::NodeHandle nh;
struct prsRxData __speedData[2];

geometry_msgs::Twist wheelFeedback_msgs;
ros::Publisher wheelFeedback_pub("wheelFeedback", &wheelFeedback_msgs);

sensor_msgs::Imu imu_msg;
ros::Publisher imu("imu", &imu_msg);

void cmd_velCallBack(const geometry_msgs::Twist& velInfo);
ros::Subscriber<geometry_msgs::Twist> _sub("/cmd_vel", &cmd_velCallBack);


int leftSpeed_regval,rightSpeed_regval;

/**************************/
double x = 0.0;
double y = 0.0;
double th = 0.0;

double vx = 0.0;
double vy = 0.0;
double vth =0.0;
ros::Time current_time;
ros::Time last_time;

char base_link[] = "/base_link";
char odom[] = "/odom";
char imu_id[] = "/imu";

int rc;
/**************************/

/*********************/
extern I2C_HandleTypeDef hi2c1;
bno055_vector_t v;
bno055_vector_t v1;
bno055_self_test_result_t test_rslts;
/*********************/
int count=0;
char log_msg[256];
float _roverBase_width =0.75;
float _wheelRadius = 0.06;


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1)
	{
  nh.getHardware()->flush();
	}
}

/*--------------/cmd_vel callBack function------------*/
void cmd_velCallBack(const geometry_msgs::Twist &velInfo)
{
 float lin_x = velInfo.linear.x;
 float ang_z = velInfo.angular.z;



 float leftWheelSpeed = lin_x - ((_roverBase_width/2)*ang_z); // linear vel
 float rightWheelSpeed = lin_x + ((_roverBase_width/2)*ang_z);// linear vel

 int leftRpm_reg_val = ((leftWheelSpeed)/(0.10472*_wheelRadius))/0.23;
 int righttRpm_reg_val = ((rightWheelSpeed)/(0.10472*_wheelRadius))/0.23;


 xl430_writeToAddress(2, leftRpm_reg_val, &GOAL_VELOCITY, &REG_WR);
 HAL_Delay(4);

 xl430_writeToAddress(1, righttRpm_reg_val, &GOAL_VELOCITY, &REG_WR);
 HAL_Delay(4);

 xl430_Action();
 HAL_Delay(4);


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

	/* nh.loginfo("pinging ID 02..");
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
	nh.loginfo("[ID 01] ping Success !");*/

	 while(! nh.advertise(imu))
	{
		nh.spinOnce();
	}

	while(! nh.advertise(wheelFeedback_pub))
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
		 _wheelRadius = 0.06;
	 }

	 xl430_writeToAddress(0xFE, 1, &TORQUE_ENABLE,&WRITE);



	  bno055_assignI2C(&hi2c1);
	  bno055_setup();
	  bno055_setOperationModeNDOF();

     //update the time parameters
	 current_time= nh.now();
	 last_time = nh.now();

}




void setup_dummy(void)
{
	last_time = current_time;
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	xl430_syncRead(&PRESENT_VELOCITY, servo_ID,uint8_t(2));
	HAL_Delay(5);

	xl430_readBroadcastBuffer(2, __speedData);
	HAL_Delay(5);

	// validate the speed data and update
	if(__speedData[0].crc_check && __speedData[1].crc_check)
	{
		leftSpeed_regval = __speedData[0].data;
		rightSpeed_regval = __speedData[1].data;
	}
	else
	{
		nh.logwarn("Error in validating wheel speed");
	}

	// ----Beginning of the calculation and relative position update-----
	float l_wheelSpeed_ = ((leftSpeed_regval * 0.23) / 60)
			* (_wheelRadius * 6.2831);
	float r_wheelSpeed_ = ((rightSpeed_regval * 0.23) / 60)
			* (_wheelRadius * 6.2831);

	vth = (r_wheelSpeed_ - l_wheelSpeed_) / _roverBase_width;
	vx = (l_wheelSpeed_ + r_wheelSpeed_) / 2;

	wheelFeedback_msgs.linear.x =vx;
	wheelFeedback_msgs.angular.z = vth;

	wheelFeedback_pub.publish(&wheelFeedback_msgs);

	current_time = nh.now();


	imu_msg.header.stamp = nh.now();
	imu_msg.header.frame_id= base_link;

	  test_rslts= bno055_getSelfTestResult();
	  if(test_rslts.accState ==0 || test_rslts.gyrState ==0)
	  {
		  HAL_GPIO_WritePin(IMU_rst_GPIO_Port, IMU_rst_Pin, GPIO_PIN_RESET);
		  HAL_Delay(20);
		  HAL_GPIO_WritePin(IMU_rst_GPIO_Port, IMU_rst_Pin, GPIO_PIN_SET);
		  HAL_Delay(10);
		  nh.loginfo("fails");
		  bno055_assignI2C(&hi2c1);
		  bno055_setup();
		  bno055_setOperationModeNDOF();
	  }
	  v = bno055_getVectorGyroscope();
	  imu_msg.angular_velocity.x= v.x;
	  imu_msg.angular_velocity.y= v.y;
	  imu_msg.angular_velocity.z= v.z;

	  v = bno055_getVectorLinearAccel();

	  imu_msg.linear_acceleration.x= v.x;
	  imu_msg.linear_acceleration.y= v.y;
	  imu_msg.linear_acceleration.z= v.z;

	  v = bno055_getVectorQuaternion();

	  imu_msg.orientation.w = v.w;
	  imu_msg.orientation.x = v.x;
	  imu_msg.orientation.y = v.y;
	  imu_msg.orientation.z = v.z;

	  imu.publish( &imu_msg );

	nh.spinOnce();
	if(!nh.connected())
	{
		HAL_Delay(5);
		xl430_writeToAddress(0xFE, 0, &TORQUE_ENABLE,&WRITE);
	}
	HAL_Delay(50);



}
