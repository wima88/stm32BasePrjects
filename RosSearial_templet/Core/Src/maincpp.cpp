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
#include <tf/transform_broadcaster.h>
#include <tf/tf.h>
#include <sensor_msgs/Imu.h>
#include "bno055_stm32.h"

#include "xl430_address.h"
#include <stdio.h>

ros::NodeHandle nh;
struct prsRxData __speedData[2];


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
tf::TransformBroadcaster odom_broadcaster;
/**************************/

/*********************/
extern I2C_HandleTypeDef hi2c1;
bno055_vector_t v;
bno055_vector_t v1;
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

	 while(! nh.advertise(imu))
	{
		nh.spinOnce();
	}
	 odom_broadcaster.init(nh);
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

/**void StartDefaultTask(void *argument)
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
	  osDelay(20);
	  }
}*/


void setup_dummy(void)
{
	last_time = current_time;
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	HAL_Delay(5);
	xl430_syncRead(&PRESENT_VELOCITY, servo_ID,uint8_t(2));
	HAL_Delay(5);

	xl430_readBroadcastBuffer(2, __speedData);
	HAL_Delay(25);

	// validate the speed data and update
	if(__speedData[0].crc_check && __speedData[1].crc_check)
	{
		leftSpeed_regval = __speedData[0].data;
		rightSpeed_regval = __speedData[1].data;
	}

	// ----Beginning of the calculation and relative position update-----
	float l_wheelSpeed_ = ((leftSpeed_regval * 0.23) / 60)
			* (_wheelRadius * 6.2831);
	float r_wheelSpeed_ = ((rightSpeed_regval * 0.23) / 60)
			* (_wheelRadius * 6.2831);

	vth = (r_wheelSpeed_ - l_wheelSpeed_) / _roverBase_width;
	vx = (l_wheelSpeed_ + r_wheelSpeed_) / 2;

	current_time = nh.now();

	double dt = (current_time.toSec() - last_time.toSec()); // time difference
	double delta_x = (vx * cos(th) - vy * sin(th)) * dt;  	//x distance
	double delta_y = (vx * sin(th) + vy * cos(th)) * dt;  	//y distance
	double delta_th = vth * dt;                           	//Angular distance

	x += delta_x;
	y += delta_y;
	th += delta_th;
	// ----End of the calculation and relative position update-----

	//since all odometry is 6DOF we'll need a quaternion created from yaw
	geometry_msgs::Quaternion odom_quat = tf::createQuaternionFromYaw(th);

	//first, we'll publish the transform over tf
	geometry_msgs::TransformStamped odom_trans;
	odom_trans.header.stamp = nh.now();
	odom_trans.header.frame_id = odom;
	odom_trans.child_frame_id = base_link;

	odom_trans.transform.translation.x = x;
	odom_trans.transform.translation.y = y;
	odom_trans.transform.translation.z = 0.0;
	odom_trans.transform.rotation = odom_quat;

	odom_broadcaster.sendTransform(odom_trans);


	//imu publishing

	  imu_msg.header.stamp = nh.now();
	  imu_msg.header.frame_id= imu_id;

	  v = bno055_getVectorGyroscope();
	  HAL_Delay(5);
	  v1 = bno055_getVectorLinearAccel();
	  imu_msg.angular_velocity.x= v.x;
	  imu_msg.angular_velocity.y= v.y;
	  imu_msg.angular_velocity.z= v.z;

	  v1 = bno055_getVectorLinearAccel();
	  imu_msg.linear_acceleration.x= v1.x;
	  imu_msg.linear_acceleration.y= v1.y;
	  imu_msg.linear_acceleration.z= v1.z;




/*
	        nh.loginfo("-----------------------------");
	        sprintf(log_msg,"Total x distance %f ",x);
	        nh.loginfo(log_msg);
	        sprintf(log_msg,"Total Y distance %f ",y);
	        nh.loginfo(log_msg);
	        sprintf(log_msg,"Total anguler distance %f ",th);
	        nh.loginfo(log_msg);*/

	/*	sprintf(log_msg,"num of bits %d", int(_rxData.dataSize));
	nh.loginfo(log_msg);
	for(int i=0 ; i<_rxData.dataSize;i++)
	{
		sprintf(log_msg+3*i," %02X",_rxData.data[i]);
	}
	nh.loginfo(log_msg);*/

	imu.publish( &imu_msg );
	nh.spinOnce();
	if(!nh.connected())
	{
		HAL_Delay(5);
		xl430_writeToAddress(0xFE, 0, &TORQUE_ENABLE,&WRITE);
	}
	HAL_Delay(100);



}
