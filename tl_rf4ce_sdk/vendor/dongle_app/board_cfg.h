/********************************************************************************************************
 * @file    board_cfg.h
 *
 * @brief   This is the header file for board_cfg.h
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

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define 	BOARD_DONGLE_826x				1
#define 	BOARD_DONGLE_8258				2
#define 	BOARD_DONGLE_8278				3
#define 	BOARD_DONGLE_B92				4
#if (MCU_CORE_826x)
	#define		BOARD						BOARD_DONGLE_826x
#elif (MCU_CORE_8258)
	#define		BOARD						BOARD_DONGLE_8258
#elif (MCU_CORE_8278)
	#define		BOARD						BOARD_DONGLE_8278
#elif (MCU_CORE_B92)
	#define		BOARD						BOARD_DONGLE_B92
#endif


#if(BOARD == BOARD_DONGLE_826x)
	#include "board_dongle_826x.h"
#elif(BOARD == BOARD_DONGLE_8258)
	#include "board_dongle_8258.h"
#elif(BOARD == BOARD_DONGLE_8278)
	#include "board_dongle_8278.h"
#elif(BOARD == BOARD_DONGLE_B92)
	#include "board_dongle_b92.h"
#endif


    /* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

