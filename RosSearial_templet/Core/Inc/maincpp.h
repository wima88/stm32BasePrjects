/*
 * maincpp.h
 *
 *  Created on: Feb 15, 2021
 *      Author: Wimansha
 */

#ifndef INC_MAINCPP_H_
#define INC_MAINCPP_H_


#ifdef __cplusplus
 extern "C" {
#endif

#include "xl430.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "cmsis_os.h"


void setup(void);

void StartDefaultTask(void *argument);
void setup_dummy(void);

#ifdef __cplusplus
}
#endif



#endif /* INC_MAINCPP_H_ */
