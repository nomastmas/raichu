/*
 * uart1.h
 *
 *  Created on: Apr 18, 2012
 *      Author: User
 */

#ifndef UART1_H_
#define UART1_H_
#ifdef __cplusplus
extern "C" {
#endif


#include "../FreeRTOS/FreeRTOS.h"
#include "../FreeRTOS/queue.h"
#include "../FreeRTOS/task.h"


char uart1Init (unsigned long ulWantedBaud, unsigned long uxQueueLength);
unsigned long uart1GetChar (char *pcRxedChar, portTickType xBlockTime);
unsigned long uart1PutChar (char cOutChar, portTickType xBlockTime);


#ifdef __cplusplus
}
#endif

#endif /* UART1_H_ */
