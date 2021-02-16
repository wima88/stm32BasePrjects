/*
 * xl480.h
 *
 *  Created on: Feb 16, 2021
 *      Author: Wimansha
 */

#ifndef INC_XL480_H_
#define INC_XL480_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define MAX_DATA_LENGTH 64

UART_HandleTypeDef _huart;
uint8_t rx_buffer[MAX_DATA_LENGTH];



struct rxData {
	uint16_t dataSize;
	uint8_t data[MAX_DATA_LENGTH];
};

struct prsRxData {
	uint8_t id;
	uint16_t crc;
	int data;
	uint8_t errorFlag;
};

struct txData {
	uint16_t length;
	uint8_t id;
	uint16_t crc;
	uint8_t inst;
};

void xl480_int(UART_HandleTypeDef *huart);
void xl480_writebuffer(uint8_t * dataBuf);

#ifdef __cplusplus
}
#endif

#endif /* INC_XL480_H_ */
