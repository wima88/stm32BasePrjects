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

#define DEBUG_UART 1 // comment me out at production


UART_HandleTypeDef _debug_huart;

UART_HandleTypeDef _huart;
uint8_t rx_buffer[MAX_DATA_LENGTH];
int rply_msg_count;
uint8_t _expected_return_msgs;



struct rxData {
	uint16_t dataSize;
	uint8_t data[MAX_DATA_LENGTH];
};

/* ENUM*/
enum profile {VELOCITY_PROFILE,TIME_PROFILE};
enum driveDirection { NORMAL_MODE,REVERSE_MODE};
enum operatingMode {VELOCITY_CONTROL=1, POSITION_CONTROL=3, EX_POSITION_CONTROL=4,PWM_CONTROL=16};

/*
 *profiles
 *const uint8_t VELOCITY_PROFILE = 0x00; 	//Create a Profile based on Velocity
 *const uint8_t TIME_PROFILE = 0x01;		// Create Profile based on time
 *
 *driveDirections
 *const uint8_t NORMAL_MODE  = 0x00; 	//Normal Mode: CCW(Positive), CW(Negative)
 *const uint8_t REVERSE_MODE = 0x01;	//Reverse Mode: CCW(Negative), CW(Positive)
 */

typedef struct xl430_EEPROM_Typrdefs{
		 uint8_t 		ID;
	enum profile  		_profile;
	enum driveDirection _driveDirection;
	enum operatingMode  _operatingMode;
		uint8_t 		errorFlag;
}xl430_EEPROM_Typrdef;

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
HAL_StatusTypeDef xl430_debug_uart_init(UART_HandleTypeDef *huart);
void xl430_writebuffer(uint8_t * dataBuf, uint16_t data_length);
struct prsRxData xl430_readbuffer();
uint16_t update_crc(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);
void xl430_asci_tx(char * msg, uint8_t l); // debug interface
void xl430_error_handler();
void __itCallback(DMA_HandleTypeDef hdma_usartx_rx);// need to populate later
void xl430_writeToAddress(uint8_t Id ,int tx_data,const uint16_t *address,const uint8_t *__inst,uint8_t data_len );
void xl430_setRxData(struct rxData *data);



/*--------api functions--------*/
bool xl430_ping(uint8_t ID);
xl430_EEPROM_Typrdef xl430_getDrivermode(uint8_t ID);
uint8_t xl430_setDrivermode( xl430_EEPROM_Typrdef eeprom);
void xl430_Action();
struct prsRxData xl430_torqeEnable(uint8_t ID,bool Torque_stat);
void xl430_LED(uint8_t ID, bool LED_stat);
void xl430_setSpeed(uint8_t ID , int speed);
struct prsRxData xl430_getSpeed(uint8_t ID);



void xl430_syncRead(const uint16_t *address,const uint8_t *ID_array, uint8_t sizeofArray);
void xl430_syncWrite(const uint16_t *address,const uint8_t *ID_array, uint8_t sizeofIDArray,int *data,uint8_t data_len);
void xl430_readBroadcastBuffer(uint8_t numOf_IDs, struct prsRxData *_rxDataArr);

/* --user specific functions-- */
void xl430_libTest(); // temp function for testing
void xl430_readMotorSpeeds();
void xl430_writeMotorSpeeds(int l_Speed,int r_Speed);




#ifdef __cplusplus
}
#endif



#endif /* INC_xl430_H_ */
