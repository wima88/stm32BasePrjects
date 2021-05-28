/*
 * xl480_address.h
 *
 *  Created on: Feb 18, 2021
 *      Author: Wimansha
 */

#ifndef INC_XL430_ADDRESS_H_
#define INC_XL430_ADDRESS_H_

const uint8_t header[] = {0xFF,0xFF,0xFD,0x00};
const uint8_t servo_ID[]   = {0x01,0x02,0xFE}; //L,R,B


const uint8_t PING   =0x01;
const uint8_t READ   =0x02;
const uint8_t WRITE  =0x03;
const uint8_t REG_WR =0x04;
const uint8_t ACTION =0x05;
const uint8_t SYNC_RD=0x82;

const uint16_t	DRIVE_MODE	 =	10	;
const uint16_t	TORQUE_ENABLE	 =	64	;
const uint16_t	LED	 =	65	;
const uint16_t	STATUS_RETURN_LEVEL	 =	68	;
const uint16_t	REGISTERED_INSTRUCTION	 =	69	;
const uint16_t	HARDWARE_ERROR_STATUS	 =	70	;
const uint16_t	VELOCITY_I_GAIN	 =	76	;
const uint16_t	VELOCITY_P_GAIN	 =	78	;
const uint16_t	POSITION_D_GAIN	 =	80	;
const uint16_t	POSITION_I_GAIN	 =	82	;
const uint16_t	POSITION_P_GAIN	 =	84	;
const uint16_t	FEEDFORWARD_2ND_GAIN	 =	88	;
const uint16_t	FEEDFORWARD_1ST_GAIN	 =	90	;
const uint16_t	BUS_WATCHDOG	 =	98	;
const uint16_t	GOAL_PWM	 =	100	;
const uint16_t	GOAL_VELOCITY	 =	104	;
const uint16_t	PROFILE_ACCELERATION	 =	108	;
const uint16_t	PROFILE_VELOCITY	 =	112	;
const uint16_t	GOAL_POSITION	 =	116	;
const uint16_t	REALTIME_TICK	 =	120	;
const uint16_t	MOVING	 =	122	;
const uint16_t	MOVING_STATUS	 =	123	;
const uint16_t	PRESENT_PWM	 =	124	;
const uint16_t	PRESENT_LOAD	 =	126	;
const uint16_t	PRESENT_VELOCITY	 =	128	;
const uint16_t	PRESENT_POSITION	 =	132	;
const uint16_t	VELOCITY_TRAJECTORY	 =	136	;
const uint16_t	POSITION_TRAJECTORY	 =	140	;
const uint16_t	PRESENT_INPUT_VOLTAGE	 =	144	;
const uint16_t	PRESENT_TEMPERATURE	 =	146	;

#endif /* INC_XL430_ADDRESS_H_ */
