/********************************************************************************************************
 * @file    board_dongle_826x.h
 *
 * @brief   This is the header file for board_dongle_826x.h
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

#pragma  once


/*
 * ADC PIN configuration
 *
 * */
	#define ZRC_APP_ADC_PIN                  GPIO_PB7



/*
 * GPIO PIN configuration for printf DEBUG
 * DEBUG_INFO_TX_PIN: used in function "soft_uart_putc()"
 *
 * */
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_PB4//print
	#define PB4_OUTPUT_ENABLE		1
	#define PB4_INPUT_ENABLE		0
#endif
