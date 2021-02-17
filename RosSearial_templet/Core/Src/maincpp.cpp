/*
 * maincpp.cpp
 *
 *  Created on: Feb 15, 2021
 *      Author: Wimansha
 */




#include <maincpp.h>
#include <ros.h>
#include <std_msgs/String.h>

ros::NodeHandle nh;

std_msgs::String str_msg;
ros::Publisher chatter("chatter", &str_msg);
char hello[] = "Hello world!";

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
  nh.getHardware()->flush();
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
	 	  nh.advertise(chatter);
}

void loop(void)
{
  str_msg.data = hello;
  chatter.publish(&str_msg);
  nh.loginfo("[ID01] ping ..");
  nh.spinOnce();

}