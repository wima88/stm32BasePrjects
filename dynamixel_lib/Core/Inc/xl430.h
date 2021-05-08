/*
 * xl430.h
 *
 *  Created on: Feb 16, 2021
 *      Author: Wimansha
 */

#ifndef INC_XL430_H_
#define INC_XL430_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"
#include <stdio.h>
#include "main.h"

#include <string.h>
#include <stdbool.h>


#define MAX_DATA_LENGTH 64
#define DEVICES_CONNECT 2


UART_HandleTypeDef _huart;
uint8_t rx_buffer[MAX_DATA_LENGTH];
int rply_msg_count;
uint8_t _expected_return_msgs;



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
void xl430_int(UART_HandleTypeDef *huart);
void xl430_writebuffer(uint8_t * dataBuf, uint16_t data_length);
struct prsRxData xl430_readbuffer();
uint16_t update_crc(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);
void xl430_asci_tx(char * msg, uint8_t l);
void xl430_error_handler();
void __itCallback(DMA_HandleTypeDef hdma_usartx_rx);// need to populate later


/*--------api functions--------*/
bool xl430_ping(uint8_t ID);
uint8_t xl430_getDrivermode(uint8_t ID);
void xl430_writeToAddress(uint8_t Id ,int tx_data,const uint16_t *address,const uint8_t *__inst);
void xl430_Action();

void xl430_syncRead(const uint16_t *address,const uint8_t *ID_array, uint8_t sizeofArray);
void xl430_readBroadcastBuffer(uint8_t numOf_IDs, struct prsRxData *_rxDataArr);

/*-----geters and setters-----*/
void xl430_setRxData(struct rxData *data);




#ifdef __cplusplus
}
#endif



#endif /* INC_xl430_H_ */
