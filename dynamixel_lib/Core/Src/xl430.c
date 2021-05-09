/*
 * xl430.c
 *
 *  Created on: Feb 16, 2021
 *      Author: Wimansha
 */

#include "xl430.h"
#include "xl430_address.h"


 uint8_t rx_buffer[64];
 char debug_buffer[46];

 extern DMA_HandleTypeDef hdma_usart3_rx;

/*
 * @breif initilaize the xl430 with uart hardware descriptor
 */
void xl430_int(UART_HandleTypeDef *huart)
{
	_huart = *huart;
	HAL_UART_Receive_DMA(&_huart, rx_buffer, 64);
	_expected_return_msgs= DEVICES_CONNECT-1;
	rply_msg_count = 0;
	// use same uart to send debug msgs
	sprintf(debug_buffer, "xl430lib Initialized\n\r");
	xl430_asci_tx(debug_buffer,sizeof(debug_buffer));

}

HAL_StatusTypeDef xl430_debug_uart_init(UART_HandleTypeDef *huart)
{
	char _debug_buffer[64];
	_debug_huart = *huart;
	memset(_debug_buffer,'\0',sizeof(_debug_buffer));
	sprintf(_debug_buffer,"Serial Debug interface Initialized\n\r");
	HAL_StatusTypeDef x= HAL_UART_Transmit_DMA(huart, (uint8_t *)_debug_buffer, sizeof(debug_buffer));
	HAL_Delay(3);
	return x;

	}
/*
 * @breif basic write function that sends the byte stream to the device
 */
void xl430_writebuffer(uint8_t * dataBuf,uint16_t data_length)
{
	if(HAL_HalfDuplex_EnableTransmitter(&_huart) != HAL_OK)
	{
		xl430_error_handler();
	}
	else
	{
		HAL_UART_Transmit(&_huart, dataBuf, data_length, 100);

	}

	if(HAL_HalfDuplex_EnableReceiver(&_huart) !=HAL_OK)
	{
		xl430_error_handler();
	}
	HAL_Delay(4);
}
/*
 * @breif read the DMA and reset it and processed data according to protocol 2.0
 * @brief not work for broadcast msgs
 */
struct prsRxData xl430_readbuffer()
{

	//HAL_Delay(5);
	HAL_UART_DMAStop(&_huart) ;
	_rxData.dataSize  = MAX_DATA_LENGTH - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
	memcpy(_rxData.data,rx_buffer,_rxData.dataSize);
	xl430_setRxData(&_rxData);
	HAL_UART_Receive_DMA(&_huart, rx_buffer, 64);
	//HAL_Delay(20);

	struct prsRxData _retData;
	_retData.id 		= _rxData.data[4];
	_retData.errorFlag 	= _rxData.data[8];
	_retData.crc_rx 	= _rxData.data[_rxData.dataSize-2] | (_rxData.data[_rxData.dataSize-1] <<8);
	_retData.crc_cal 	= update_crc(0, _rxData.data, _rxData.dataSize-2);
	_retData.dat_len 	= _rxData.data[5] | (_rxData.data[6] <<8);
	_retData.data = 0;
	if(_retData.crc_cal == _retData.crc_rx)
	{
		_retData.crc_check = true;
	}
	else
	{
		_retData.crc_check = false;
	}

	for(uint16_t n =0; n<_retData.dat_len-4;n++)
	{
		_retData.data = _retData.data | (_rxData.data[9+n] <<8*n);
	}

	return _retData;

}
/*
 * @breif read the DMA and reset it and processed data according to protocol 2.0
 * @brief work for broadcast msgs
 *
 * @ToDo impliment the DMA copy and reset
 */
void xl430_readBroadcastBuffer(uint8_t numOf_IDs, struct prsRxData *_rxDataArr)
{
	struct prsRxData _retData;
	uint16_t partialDataLen = _rxData.dataSize/numOf_IDs;
	uint8_t __dataBucket[64];

	 memcpy (__dataBucket,_rxData.data,_rxData.dataSize);

	for(uint8_t i =0; i <numOf_IDs;i++ )
	{
	_retData.id 		= __dataBucket[4+(partialDataLen*i)];
	_retData.errorFlag 	= __dataBucket[8+(partialDataLen*i)];

	/* need to double check math and logic*/
	_retData.crc_rx 	= __dataBucket[partialDataLen-2+(partialDataLen*i)] | (__dataBucket[partialDataLen-1+(partialDataLen*i)] <<8);
	_retData.crc_cal 	= update_crc(0, __dataBucket+(partialDataLen*i), partialDataLen-2);
	_retData.dat_len 	= __dataBucket[5+(partialDataLen*i)] | (__dataBucket[6+(partialDataLen*i)] <<8);
	_retData.data = 0;
	/*-------------------------*/

	if(_retData.crc_cal == _retData.crc_rx)
	{
		_retData.crc_check = true;
	}
	else
	{
		_retData.crc_check = false;
	}

	for(uint16_t n =0; n<_retData.dat_len-4;n++)
	{
		_retData.data = _retData.data | (__dataBucket[9+n+((partialDataLen*i))] <<8*n);
	}
	_rxDataArr[i] = _retData ;
	}//end of for loop



}
/*
 * @brief calculate the crc for all the read and write operation
 */
uint16_t update_crc(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size)
{
    uint16_t i, j;
    uint16_t crc_table[256] = {
        0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
        0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
        0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
        0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
        0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
        0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
        0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
        0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
        0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
        0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
        0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
        0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
        0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
        0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
        0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
        0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
        0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
        0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
        0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
        0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
        0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
        0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
        0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
        0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
        0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
        0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
        0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
        0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
        0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
        0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
        0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
        0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
    };

    for(j = 0; j < data_blk_size; j++)
    {
        i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
        crc_accum = (crc_accum << 8) ^ crc_table[i];
    }

    return crc_accum;
}

/*
 * @brief transmite the debug msgs to the same uart
 */
void xl430_asci_tx(char * msg, uint8_t l)
{
#ifdef DEBUG_UART
		if(HAL_UART_Transmit(&_debug_huart, (uint8_t *)msg,l, 500)!= HAL_OK)
		{
			xl430_error_handler();
		}
		HAL_Delay(10);
#endif
}

/*
 * @brief hardware error handler -blocking
 * 		  it need manual implimenttaion on use by case
 */
void xl430_error_handler()
{
	while(1)
	{
	HAL_GPIO_TogglePin(LED_2_GPIO_Port, LED_2_Pin);
	HAL_Delay(100);
	}
}
/*
 * @brief NOT USING AT THE MOMENT
 */
void __itCallback(DMA_HandleTypeDef hdma_usartx_rx)
{
	if (__HAL_UART_GET_FLAG(&_huart, UART_FLAG_RXNE) == RESET) {
		__HAL_UART_CLEAR_IDLEFLAG(&_huart);
		rply_msg_count++;
		if(rply_msg_count == _expected_return_msgs)
		{
			HAL_UART_DMAStop(&_huart) ;

			_rxData.dataSize  = MAX_DATA_LENGTH - __HAL_DMA_GET_COUNTER(&hdma_usartx_rx);
			memcpy(_rxData.data,rx_buffer,_rxData.dataSize);
			xl430_setRxData(&_rxData);
			HAL_UART_Receive_DMA(&_huart, rx_buffer, 64);
			rply_msg_count =0;
		}
	}
}
/*
 * @brief write data to a spesific address
 * @param data_len is the number of data size(byte) needed to spesific adress
 * 		  refer to datasheet for more details
 *
 * 						|
 * 		                V
 * 		  ADRESS	size(Byte)			DataName		R/W
 * 		  10 		   1 			Drive Mode 			RW
		  11 		   1		 	Operating Mode 		RW
		  12 		   1 			Secondary(Shadow) IDRW
		  13 		   1 			Protocol Type 		RW
		  20 		   4 			Homing Offset 		RW
		  24 		   4 			Moving Threshold 	RW
		  31 		   1 			Temperature Limit 	RW
		  32 		   2 			Max Voltage Limit 	RW
 */
void xl430_writeToAddress(uint8_t Id ,int tx_data,const uint16_t *address,const uint8_t *__inst,uint8_t data_len )
{
	  uint16_t mem_size=12;
	  uint16_t crc;
	  char crc_[2];
	  uint8_t data_size =data_len;
	  uint8_t data_array[4];

	  data_array[0] = tx_data & 0x000000FF;
	  data_array[1] = (tx_data>>8) & 0x000000FF;
	  data_array[2] = (tx_data>>16) & 0x000000FF;
	  data_array[3] = (tx_data>>24) & 0x000000FF;

	  mem_size += data_size;
	  uint8_t m_tx_buffer [mem_size];
	  uint16_t m_len =data_size+5;


	  memcpy (m_tx_buffer,header,6);
	  memcpy (m_tx_buffer+sizeof(header),&Id,1);
	  memcpy (m_tx_buffer+sizeof(header)+1,&m_len,2);
	  memcpy (m_tx_buffer+sizeof(header)+3,__inst,1);
	  memcpy (m_tx_buffer+sizeof(header)+4,address,2);
	  memcpy (m_tx_buffer+sizeof(header)+6,data_array,data_size);

	  crc = update_crc(0,m_tx_buffer,mem_size -2);
	  crc_[0]=crc & 0x00FF;
	  crc_[1]=(crc>>8) & 0x00FF;
	  memcpy (m_tx_buffer+sizeof(header)+6+data_size,crc_,2);

	    xl430_writebuffer(m_tx_buffer,sizeof(m_tx_buffer));



}

/*---------api functions----------*/

/*
 * @brief Ping specific ID and return bool true upon a successful ping
 */
bool xl430_ping(uint8_t ID)
{
	uint8_t __buffer[10] = {0xFF,0xFF,0xFD,0x00,0X00,0x03,0x00,0x01,0x00,0x00};
	__buffer[4] = ID;
	uint16_t crc =update_crc(0,__buffer,8);
	__buffer[9] = (crc>>8) & 0x00FF;
	__buffer[8] = (crc & 0x00FF);


	xl430_writebuffer(__buffer,10);

	struct prsRxData _data;
	_data = xl430_readbuffer();

	if(_data.crc_check && (!_data.errorFlag))
	{

		sprintf(debug_buffer,"[ ID %d ] Ping Successful\n\r", ID);
		xl430_asci_tx(debug_buffer,sizeof(debug_buffer));

		sprintf(debug_buffer,"[ ID %d ] Firmware version %d \n\r", ID , _rxData.data[_rxData.dataSize-3]);
	    xl430_asci_tx(debug_buffer,sizeof(debug_buffer));


		return true;
	}
	else
	{
		sprintf(debug_buffer,"[ ID %d ] Ping Fail Try again\n\r", ID);
		xl430_asci_tx(debug_buffer,sizeof(debug_buffer));
		return false;

	}


}


/*
 * @brief read the driver configuration from eeprom of the device
 */
xl430_EEPROM_Typrdef xl430_getDrivermode(uint8_t ID)
{
	xl430_EEPROM_Typrdef _retval;
	struct prsRxData _data;
	xl430_writeToAddress(ID,0x1,&DRIVE_MODE,&READ,4);


	_data = xl430_readbuffer();

	if(_data.crc_check && (!_data.errorFlag))
	{
		_retval.ID = ID;
		_retval._profile = _data.data & 0x04;
		_retval._driveDirection = _data.data &0x01;
		_retval.errorFlag = _data.errorFlag;

		sprintf(debug_buffer,"[ ID %d ] Drive Direction %d \n\r", ID,_retval._driveDirection);
		xl430_asci_tx(debug_buffer,sizeof(debug_buffer));
		return _retval;
	}
	else
	{
		_retval.errorFlag = _data.errorFlag;
		return _retval;
	}
}


/*
 * @brief set the driver configuration if fails return the error code
 *
 * ErrorCode			Description
 * 0x01 				Result Fail 		Failed to process the sent Instruction Packet
 * 0x02 				Instruction Error 	Undefined Instruction has been used
 * 0x03 				CRC Error 			CRC of the sent Packet does not match
 * 0x04 				Data Range Error 	Data to be written in the corresponding Address is outside the range of the minimum/maximum value
 * 0x05 				Data Length Error 	Attempt to write Data that is shorter than the data length of the corresponding Address
											(ex: when you attempt to only use 2 bytes of a item that has been defined as 4 bytes)
 * 0x06 				Data Limit Error 	Data to be written in the corresponding Address is outside of the Limit value
 * 0x07 				Access Error 		Attempt to write a value in an Address that is Read Only or has not been defined
											Attempt to read a value in an Address that is Write Only or has not been defined
											Attempt to write a value in the ROM domain while in a state of Torque Enable(ROM Lock)
 */
uint8_t xl430_setDrivermode( xl430_EEPROM_Typrdef eeprom)
{
	struct prsRxData _data;
	xl430_writeToAddress(eeprom.ID,(eeprom._profile<<2) | eeprom._driveDirection,&DRIVE_MODE,&WRITE,1);
	_data = xl430_readbuffer();

	if(_data.crc_check && (!_data.errorFlag))
	{

		return _data.errorFlag;
	}
	else
		return _data.errorFlag;
}
/*
 * @brief send the action instruction
 */
void xl430_Action()
{
  uint8_t _dataArr[]={0xFF,0xFF,0xFD,0x00,0xFE,0x03,0x00,0x05,0x00,0x00};
  uint16_t crc;
  crc = update_crc(0,_dataArr,8);
  memcpy (_dataArr+8,&crc,2);
  _expected_return_msgs=1;
  xl430_writebuffer(_dataArr,10);

}
/*
 * @brief Enable torque on device with ID
 * 		  This will lock the eeprom area and cannot be perform writes
 *
 */

struct prsRxData xl430_torqeEnable(uint8_t ID,bool torque_stat)
{

	xl430_writeToAddress(ID, torque_stat, &TORQUE_ENABLE,&WRITE, 1);
	return xl430_readbuffer();
}
/*
 * @brief turn on off LED
 */
void xl430_LED(uint8_t ID,bool LED_stat)
{
	xl430_writeToAddress(ID, LED_stat, &LED,&WRITE, 1);
}

/*
 * @breif set the target velocity
 * 		  turn on torque before calling
 */
void xl430_setSpeed(uint8_t ID , int speed)
{
	xl430_writeToAddress(ID, speed, &GOAL_VELOCITY,&WRITE, 4);
}

struct prsRxData xl430_getSpeed(uint8_t ID)
{
	struct prsRxData _data;
	xl430_writeToAddress(ID, 4, &PRESENT_VELOCITY,&READ, 4);
	_data = xl430_readbuffer();

	if(_data.crc_check && (!_data.errorFlag))
		{

		sprintf(debug_buffer,"[ ID %d ] Present Velocity %d \n\r", ID,_data.data);
		xl430_asci_tx(debug_buffer,sizeof(debug_buffer));
		}
		else
		{
			sprintf(debug_buffer,"[ ID %d ] Error when reading buffer \n\r", ID);
			xl430_asci_tx(debug_buffer,sizeof(debug_buffer));
		}

	return _data;


	}

/*
 * @brief For multiple devices, Instruction to read data from the same Address with the same length at once
 */
void xl430_syncRead(const uint16_t *address,const uint8_t *ID_array, uint8_t sizeofArray)
{
  uint16_t mem_size=14+sizeofArray;
  uint16_t crc;
  uint16_t data_length = 4;

  uint8_t m_tx_buffer [mem_size];
  uint16_t m_len =7+sizeofArray;

  memcpy (m_tx_buffer,header,6);
  memcpy (m_tx_buffer+sizeof(header),servo_ID+2,1); //broadcast ID
  memcpy (m_tx_buffer+sizeof(header)+1,&m_len,2);
  memcpy (m_tx_buffer+sizeof(header)+3,&SYNC_RD,1);
  memcpy (m_tx_buffer+sizeof(header)+4,address,2);
  memcpy (m_tx_buffer+sizeof(header)+6,&data_length,2);
  memcpy (m_tx_buffer+sizeof(header)+8,ID_array,sizeofArray);
  crc = update_crc(0,m_tx_buffer,mem_size -2);
  memcpy (m_tx_buffer+mem_size-2,&crc,2);
 _expected_return_msgs = 2;
 xl430_writebuffer(m_tx_buffer, mem_size);
}

/*
 * @brief For multiple devices, Instruction to write data on the same Address with the same length at once
 * @param
 * 	address address which need writing
 * 	ID_array array of ID
 * 	sizeOFIDArray numberof elements in id array mention above
 * 	data array of data for each id followed by same order as array of ID
 * 	data_len length of the data field corresponding to the address
 *
 */
void xl430_syncWrite(const uint16_t *address,const uint8_t *ID_array, uint8_t sizeofIDArray,int *data,uint8_t data_len)
{
	uint16_t mem_size=14+sizeofIDArray+(data_len*sizeofIDArray);
	uint16_t crc;

	//allocate a memory
	uint8_t m_tx_buffer[mem_size];
	uint16_t m_len =7+sizeofIDArray+data_len*sizeofIDArray;

	memcpy (m_tx_buffer,header,6);
	memcpy (m_tx_buffer+sizeof(header),servo_ID+2,1); //broadcast ID
	memcpy (m_tx_buffer+sizeof(header)+1,&m_len,2);
	memcpy (m_tx_buffer+sizeof(header)+3,&SYNC_WT,1);
	memcpy (m_tx_buffer+sizeof(header)+4,address,2);
	memcpy (m_tx_buffer+sizeof(header)+6,&data_len,2);

	uint8_t tempCount = 8;
	for(uint8_t i=0 ;i <sizeofIDArray; i++)
	{
		memcpy (m_tx_buffer+sizeof(header)+tempCount,ID_array+i,1);
		memcpy (m_tx_buffer+sizeof(header)+tempCount+1,data+i,data_len);
		tempCount = tempCount+1+data_len;
	}
	crc = update_crc(0,m_tx_buffer,mem_size -2);
	memcpy (m_tx_buffer+mem_size-2,&crc,2);
	xl430_writebuffer(m_tx_buffer, mem_size);

	HAL_Delay(sizeofIDArray*3);

}


void xl430_setRxData(struct rxData *data)
{
	_rxData = *data;
	memset(rx_buffer,0,data->dataSize);
}
