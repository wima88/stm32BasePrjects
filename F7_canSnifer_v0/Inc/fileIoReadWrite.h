
/**
  ******************************************************************************
  * @file    fileIoReadWrite.h
  * @author  Wimansha Wijekoon  
  * @brief   Header file for the Read and write function with fatfs.
  ******************************************************************************
  * @attention
  *   + depends on the fatfs.h and usb_host.h
	*   + need to mount the usb with fatfs priore to use
	*     ++ if(f_mount(&USBfatFs,(TCHAR const*)USBHPath,0) != FR_OK)
  ******************************************************************************
  */ 
	
	
#ifndef __FILEIOREADWRITE_H
#define __FILEIOREADWRITE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <string.h>
#include <stdbool.h>

bool usbWrite(FIL *__myFile,char *__fileName,char *__writeBuf);

#endif
