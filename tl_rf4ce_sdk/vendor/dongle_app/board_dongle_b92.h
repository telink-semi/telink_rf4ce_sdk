/********************************************************************************************************
 * @file    board_dongle_b92.h
 *
 * @brief   This is the header file for board_dongle_8278.h
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
#include "app_config.h"


//#if (MODULE_USB_ENABLE)
//#define	PA5_FUNC				AS_USB
//#define	PA6_FUNC				AS_USB
//#define PA5_INPUT_ENABLE		1
//#define PA6_INPUT_ENABLE		1
//#endif
#define FLASH_CAP_SIZE_1M			1


/*
 * GPIO PIN configuration for printf DEBUG
 * DEBUG_INFO_TX_PIN: used in function "soft_uart_putc()"
 *
 * */
#if UART_PRINTF_MODE
#define	DEBUG_INFO_TX_PIN	    GPIO_PA4
#endif



#define	PA3_FUNC				AS_GPIO
#define PA3_INPUT_ENABLE		0
#define PA3_OUTPUT_ENABLE		1




