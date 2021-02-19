/*
 * maincpp.cpp
 *
 *  Created on: Feb 15, 2021
 *      Author: Wimansha
 */




#include <maincpp.h>
#include <ros.h>
#include <std_msgs/String.h>
#include "xl430_address.h"

ros::NodeHandle nh;
extern UART_HandleTypeDef huart3;
std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);
char hello[] = "Hello world!";

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	if(huart == &huart1)
	{
  nh.getHardware()->flush();
	}
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
	 nh.negotiateTopics();

	 xl430_writeToAddress(0xFE, 1, &TORQUE_ENABLE,&WRITE);


}

void StartDefaultTask(void *argument)
{
	for(;;)
	 {
	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	  xl430_writeToAddress(2, 60, &GOAL_VELOCITY, &WRITE);
	  osDelay(5);
	  xl430_writeToAddress(1, -60, &GOAL_VELOCITY, &WRITE);

	  str_msg.data = hello;
	  chatter.publish(&str_msg);
	  nh.spinOnce();
	  if(!nh.connected())
	  {
		  osDelay(5);
		  xl430_writeToAddress(0xFE, 0, &TORQUE_ENABLE,&WRITE);
	  }
	  osDelay(500);
	  }
}


void setup_dummy(void)
{

	 xl430_writeToAddress(0xFE, 1, &TORQUE_ENABLE,&WRITE);


}
