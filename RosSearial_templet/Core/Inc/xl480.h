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
#include <string.h>
#include <stdbool.h>

#define MAX_DATA_LENGTH 64
#define DEVICES_CONNECT 2

UART_HandleTypeDef _huart;
uint8_t rx_buffer[MAX_DATA_LENGTH];



struct rxData {
	uint16_t dataSize;
	uint8_t data[MAX_DATA_LENGTH];
};

struct prsRxData {
	uint8_t id;
	uint8_t errorFlag;
	uint16_t crc_rx;
	uint16_t crc_cal;
	uint16_t dat_len;
	int data;

	bool crc_check;

};

struct txData {
	uint16_t length;
	uint8_t id;
	uint16_t crc;
	uint8_t inst;
};

struct rxData _rxData;

/*---------core functions-------*/
void xl480_int(UART_HandleTypeDef *huart);
void xl480_writebuffer(uint8_t * dataBuf, uint16_t data_length);
struct prsRxData xl480_readbuffer();
uint16_t update_crc(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);
void __itCallback();// need to populate later

/*--------api functions--------*/
bool xl480_ping(uint8_t ID);

/*-----geters and setters-----*/
void xl480_setRxData(struct rxData *data);

#ifdef __cplusplus
}
#endif

#endif /* INC_XL480_H_ */
