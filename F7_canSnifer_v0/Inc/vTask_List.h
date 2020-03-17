
/**
  ******************************************************************************
  * @file    vTask_list.h
  * @author  Wimansha Wijekoon  
  * @brief   Header file for the task list assist by freeRTOS.
  ******************************************************************************
  * @attention
  *   +None
  ******************************************************************************
  */ 


#ifndef __VTASK_LIST_H
#define __VTASK_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"
#include "freertos.h"

#define SDRAM_BANK_ADDR                 ((uint32_t)0xC0000000)
#define WRITE_READ_ADDR     						((uint32_t)0x0800)



void vTaskUSB(void *argument);

void vTaskCAN(void *argument);

void vTaskLCDT(void *argument);

void vTaskSDRAM(void *argument);

void vTaskLIST(void *argument);

void vTaskLCDT_1(void *argument);
#endif
