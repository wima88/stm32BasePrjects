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

	 nh.loginfo("pinging ID 01..");
	 while(!xl480_ping(01))
	  {
	    //nh.loginfo("[ID 01] ping Fails");
	    HAL_Delay(10);
	  }
	 nh.loginfo("[ID 01] ping Success !");

	 nh.loginfo("pinging ID 02..");
	 while(!xl480_ping(02))
	  {
	    nh.loginfo("[ID 01] ping Fails");
	    HAL_Delay(10);
	  }
	 nh.loginfo("[ID 02] ping Success !");

	 while(! nh.advertise(chatter))
	{
		nh.spinOnce();
	}
	 nh.negotiateTopics();


}

void StartDefaultTask(void *argument)
{
	for(;;)
	 {
	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	  str_msg.data = hello;
	  chatter.publish(&str_msg);
	  nh.spinOnce();
	  osDelay(500);
	  }
}
