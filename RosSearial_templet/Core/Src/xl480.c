/*
 * xl480.c
 *
 *  Created on: Feb 16, 2021
 *      Author: Wimansha
 */

#include "xl480.h"


void xl480_int(UART_HandleTypeDef *huart)
{
	_huart = *huart;
	HAL_UART_Receive_DMA(&_huart, rx_buffer, 64);
	HAL_HalfDuplex_EnableReceiver(&_huart);
}

void xl480_writebuffer(uint8_t * dataBuf)
{
	HAL_HalfDuplex_EnableTransmitter(&_huart);
	HAL_UART_Transmit(&_huart, dataBuf, 10, 100);
	HAL_HalfDuplex_EnableReceiver(&_huart);
}
