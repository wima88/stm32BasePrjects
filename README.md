# stm32BasePrjects
contains several projects for stm32 
* dynamixcel_lib
  * this project is to create a lite foot print library to control dynamixcel xl430 lib. contains seperate UART interface for debug masg and main communication line contains Halfduplex UART with DMA

* seting up
 * GPIO
   * alternate function open drain 
   * pull up      
  * in xl430.c @line 15 change dma name acordingly
  * in xl430.h @line 15 and 16 change .h files acordingly to match mcu 


* ROS_Searial
  * communication node for the ROS wich comunicate with IMU and Dynamixcel servo. project uses f1 mcu with I2c and multiple uart
