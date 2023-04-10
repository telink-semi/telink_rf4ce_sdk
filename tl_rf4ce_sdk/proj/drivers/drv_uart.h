/********************************************************************************************************
 * @file    drv_uart.h
 *
 * @brief   This is the header file for drv_uart.h
 *
 * @author	Zigbee GROUP
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#pragma once

#include "../common/types.h"
#include "../common/compiler.h"
typedef void (*uart_irq_callback)(void);

typedef unsigned char (* uart_data_send)(unsigned char *data);

/**
 *  @brief  Definition of uart message received callback function
 */
typedef s32 (*uart_recvCb_t)(u8 *buf);

typedef struct{
	uart_irq_callback recvCb;
	uart_data_send send;
	volatile u8 status;
}drv_uart_t;

enum {
	UART_STA_IDLE,
	UART_STA_TX_DOING,
	UART_STA_TX_DONE,
	UART_STA_RX_DONE,
};

/****************************************************************************************
*
*	@brief	init UART receive callback function
*
*
*	@param  baudRate		baud rate
*
*	@param  rxBuf			the buffer for data receiving
*
*	@param  rxBufLen		the lenght of the uart RX buffer
*
*	@param	*uart_recvCb:	UART receive callback function.
*
*	@return	none
*/
void drv_uart_init(u32 baudRate, u8 *rxBuf, u16 rxBufLen, uart_irq_callback uart_recvCb);


/*
 * uart Rx ISR
 *
 * */
void uart_rx_irq_handler(void);


/*
 * uart Tx ISR
 *
 * */
void uart_tx_irq_handler(void);


/****************************************************************************************
*
*	@brief	check if UART transmit is finished
*
*	@param
*
*	@return	1: finished, 0: not finished
*/
u8 uart_tx_done(void);


u8 uart_tx_start(u8 *data, u32 len);
