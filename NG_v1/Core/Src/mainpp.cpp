/*
 * main.cpp

 *
 *  Created on: 2018/01/17
 *      Author: yoneken
 */
#include <mainpp.h>
#include <ros.h>
#include <std_msgs/String.h>
// include following headers

#include "stm32f7xx_hal.h"
#include "main.h"

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
	while(!nh.connected()) //wait until connection established
	{
		nh.spinOnce();
	}

	while(! nh.advertise(chatter)) // wait until  advertise
	{
		nh.spinOnce();
	}

	nh.negotiateTopics(); // void any topic complications when sync

}

void loop(void)
{
  HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
  str_msg.data = hello;
  chatter.publish(&str_msg);
  nh.spinOnce();
  HAL_Delay(1000);
}

