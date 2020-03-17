/**
  ******************************************************************************
  * @file    fileIoReadWrite.c
  * @author  Wimansha Wijekoon  
  * @brief   Header file for the Read and write function with fatfs.
  ******************************************************************************
  * @attention
  *   + depends on the fatfs.h and usb_host.h
	*   + need to mount the usb with fatfs priore to use
	*     ++ if(f_mount(&USBfatFs,(TCHAR const*)USBHPath,0) != FR_OK)
  ******************************************************************************
  */ 
	#include "fatfs.h"
	#include <string.h>
  #include <stdbool.h>
	
	
	
	/**
  *@brief  Function implimenting a write operation to the usb drive, need to close the file
	*        by calling the f_close(&myFile)    
  *@param  argument: +FIL __myFile			: pontor to the file handler
	*									 +char *__fileName	: array of char for the file name;
	*									 +char *__writeBuf	: buffer to write 
  *@retval bool 0: failed to Write, 1: sucsessfull write 
*/

	bool usbWrite(FIL *__myFile,char *__fileName,char *__writeBuf)
	{
		FRESULT res;
		UINT byteswritten;
		
	// open the file for writing 
		if(f_open(__myFile,__fileName, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) 
			{
				return 0; //error
			}
		else
			{
	//write to file
					res = f_write(__myFile, (uint8_t *)__writeBuf, strlen(__writeBuf), &byteswritten);
				if((byteswritten == 0) || (res != FR_OK))
					{
						return 0; //error
					}
			}
	
		return 1;
	}

	/**
  *@brief  Function implimenting a read operation to the usb drive, need to close the file
	*        by calling the f_close(&myFile)    
  *@param  argument: None 
  *@retval bool 0: failed to Write, 1: sucsessfull write 
*/	
	
	bool usbRead()
	{
		return 1;
	}
	
	