
/**
  ******************************************************************************
  * @file    vTask_list.c
  * @author  Wimansha Wijekoon  
  * @brief   Header file for the task list assist by freeRTOS.
  ******************************************************************************
  * @attention
  *   + Need to have "cmsis_os.h" to have osDelay to work or replace it with 
  *     suitable delay function 
  *   + uses the stm32f429xx eval board 
	*
  ******************************************************************************
  */ 
	
#include "vTask_List.h"
#include "fatfs.h"
#include "usb_host.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"


//#include "stm32f7xx_hal.h"




/**
  *@brief  Function implimenting a task, that handles usb write read 
  *@param  argument: not in use
  *@retval None
	*@depend usb_host.h,fatfs.h,string.h,stdbool.h
*/
void vTaskUSB(void *arg)
{
	//usb operation variables
	extern ApplicationTypeDef Appli_state;
	
	//fatfs variables
  extern FATFS USBfatFs;
	extern char USBHPath[4];   /* USBH logical drive path */
	
	//File IO Variables
	extern FIL myFile;
	extern char rwtext[100];
	extern char fileName[10];
	
	/*Stack monitoring utils*/
	extern UBaseType_t uxHighWaterMark;
	uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
	extern QueueHandle_t xQueue1;
	extern 	CAN_packet packetToWrite;

	sprintf(fileName, "test.txt");

	
	for(;;)
  {
		switch(Appli_state)
		{
			
			case APPLICATION_START :
			{
				if(f_mount(&USBfatFs,(TCHAR const*)USBHPath,0) == FR_OK)
				{
					HAL_GPIO_WritePin(LED_1_GPIO_Port,LED_1_Pin,GPIO_PIN_SET);
				}
				
				if( xQueueReceive( xQueue1, &( packetToWrite ), portMAX_DELAY ) )
						{}
							
				break;
			}
			
			case APPLICATION_IDLE :
			{
				if( xQueueReceive( xQueue1, &( packetToWrite ), portMAX_DELAY ) )
						{}

				break;
			}
			case APPLICATION_READY :
			{
				
				
					if( xQueueReceive( xQueue1, &( packetToWrite ), portMAX_DELAY ) )
						{
							if(f_open(&myFile,fileName, FA_OPEN_APPEND | FA_WRITE) == FR_OK) 
								{	
									sprintf(rwtext,"%x\t ",packetToWrite.Header.StdId);
									f_printf(&myFile,rwtext);
									
									for(int i=0;i<packetToWrite.Header.DLC;i++)
									{
										f_printf(&myFile,"%x ",packetToWrite.Data[i]);
									}
									f_printf(&myFile,"\n");
								}
						}
				f_close(&myFile);
				break;
			}
			
			case APPLICATION_DISCONNECT:
			{
				if( xQueueReceive( xQueue1, &( packetToWrite ), portMAX_DELAY ) )
						{}
				HAL_GPIO_WritePin(LED_1_GPIO_Port,LED_1_Pin,GPIO_PIN_RESET);
				break;  
			}
			
			
			default:
			{break;}
		}
	}

}



/**
  *@brief  THIS IS A TEST TASK implimenting a task, that handle the CAN msg  
  *@param  argument: not in use
  *@retval None
	*@depend  
*/

void vTaskCAN(void *arg)
{
	//variables
	
	extern CAN_TxHeaderTypeDef   TxHeader;
	extern uint8_t               TxData[8];
	extern CAN_HandleTypeDef 		 hcan1;
	extern uint32_t               TxMailbox;
	
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.TransmitGlobalTime = DISABLE;	
	
	for(;;)
	{
//--------Test data set--------------------------
	TxHeader.StdId = rand() % 0xF;
  TxHeader.DLC = rand() % 9;

		for(int i=0;i<TxHeader.DLC;i++)
		{
			TxData[i] = rand() %0xFF ;
		}

//-----------------------------------------------	
		
		if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
		{
			/* Transmission request Error */
			Error_Handler();
		}  
			/* Wait transmission complete */
		int r  = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
		while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3) {}
	}
}


/**
  *@brief  Function implimenting a task, that handle the LCDT tasks   
  *@param  argument: not in use
  *@retval None
	*@depend stm32746g_discovery_lcd.h
*/
void vTaskLCDT(void *arg)
{
	 extern  UBaseType_t 		uxHighWaterMark_LCDT;
	 extern  CAN_packet 		packetToDisplay;
	 extern  QueueHandle_t 	xQueue2;
	 extern  char 					lcdText[50];
	 uint16_t lcdXpos =0;
	 uint16_t lcdYpos =0;	
	
	struct Node *start = NULL;
	
  /* LCD Initialization */
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
  /* Enable the LCD */
  BSP_LCD_DisplayOn();
  /* Select the LCD Background Layer  */
  BSP_LCD_SelectLayer(0);
  /* Clear the Background Layer */
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  /* Some sign */
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_SetFont(&Font12);
  //BSP_LCD_DisplayStringAt(0, 0, (uint8_t*) "Test Run round_1!", CENTER_MODE);
	for(;;)
	{
		if( xQueueReceive( xQueue2, &( packetToDisplay ), 0 ) )
		{
				sprintf(lcdText,"%x",packetToDisplay.Header.StdId);
				BSP_LCD_ClearStringLine(lcdYpos / 12);
				BSP_LCD_DisplayStringAt(lcdXpos,lcdYpos,(uint8_t*)lcdText,LEFT_MODE);
									for(int i=0;i<packetToDisplay.Header.DLC;i++)
									{
										sprintf(lcdText,"%x",packetToDisplay.Data[i]);
										BSP_LCD_DisplayStringAt(lcdXpos+80+(24*i)-8,lcdYpos,(uint8_t*)lcdText,LEFT_MODE);
									}
											lcdYpos=lcdYpos+12;
											lcdXpos = 0;
											if(lcdYpos>12*21){lcdYpos =0;} //reset xpos
			}
	}
}

/**
  *@brief  Function implimenting task to store data in linked list 
  *@param  argument: not in use
  *@retval None
	*@depend stm32746g_discovery_sdram.h
*/
void vTaskLIST(void *arg)
{
	 extern  UBaseType_t 		uxHighWaterMark_LCDT;
	 extern  CAN_packet 		packetToDisplay;
	 extern  QueueHandle_t 	xQueue2;
	 extern  uint64_t dataStr [BUFFER_SIZE][2];
	 extern	 uint64_t temp;

	for(;;)
	{
		if( xQueueReceive( xQueue2, &( packetToDisplay ), 0 ) )
		{
			for(uint8_t i=0;i<BUFFER_SIZE;i++)
			{
				if(dataStr[i][0] == packetToDisplay.Header.StdId)  // found the proper SID,store and end the iteration
				{
					dataStr[i][1] = ((uint64_t)packetToDisplay.Data[0] << 56) |
													((uint64_t)packetToDisplay.Data[1] << 48) |
													((uint64_t)packetToDisplay.Data[2] << 40) |
													((uint64_t)packetToDisplay.Data[3] << 32) |
													((uint64_t)packetToDisplay.Data[4] << 24) |
													((uint64_t)packetToDisplay.Data[5] << 16)	|
													((uint64_t)packetToDisplay.Data[6] << 8)  |
													((uint64_t)packetToDisplay.Data[7] << 0);
					break;			
				}
				else if(dataStr[i][0] == 0)		// SID is new to the List enter it and end iteration
				{
					dataStr[i][0]=packetToDisplay.Header.StdId;
					dataStr[i][1] = (uint64_t)packetToDisplay.Data;
//					dataStr[i][1] = ((uint64_t)packetToDisplay.Data[0] << 56) |
//													((uint64_t)packetToDisplay.Data[1] << 48) |
//													((uint64_t)packetToDisplay.Data[2] << 40) |
//													((uint64_t)packetToDisplay.Data[3] << 32) |
//													((uint64_t)packetToDisplay.Data[4] << 24) |
//													((uint64_t)packetToDisplay.Data[5] << 16)	|
//													((uint64_t)packetToDisplay.Data[6] << 8)  |
//													((uint64_t)packetToDisplay.Data[7] << 0);
					break;
				}
				else{}
			}
		}

	}
}

/**
  *@brief  Function implimenting a READ/WRITE operation to SDRAM 
  *@param  argument: not in use
  *@retval None
	*@depend stm32746g_discovery_sdram.h
*/
void vTaskSDRAM(void *arg)
{

	for(;;)
	{

	}
}


