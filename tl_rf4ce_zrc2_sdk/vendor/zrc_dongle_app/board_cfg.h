/********************************************************************************************************
 * @file     board_cfg.h
 *
 * @brief    select user board here
 *
 * @author
 * @date     July. 1, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define 	BOARD_DONGLE_826x				1
#define 	BOARD_DONGLE_8258				2
#define 	BOARD_DONGLE_8278				3
#if (MCU_CORE_826x)
	#define		BOARD						BOARD_DONGLE_826x
#elif (MCU_CORE_8258)
	#define		BOARD						BOARD_DONGLE_8258
#elif (MCU_CORE_8278)
	#define		BOARD						BOARD_DONGLE_8278
#endif


#if(BOARD == BOARD_DONGLE_826x)
	#include "board_dongle_826x.h"
#elif(BOARD == BOARD_DONGLE_8258)
	#include "board_dongle_8258.h"
#elif(BOARD == BOARD_DONGLE_8278)
	#include "board_dongle_8278.h"
#endif


    /* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
