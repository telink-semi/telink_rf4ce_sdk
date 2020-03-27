/********************************************************************************************************
 * @file     board_cfg.h
 *
 * @brief    select user board here
 *
 * @author
 * @date     July. 1, 2019
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

#define 	BOARD_TELINK_RC					1
#define		BOARD_C59T80A5_V1P1_8267		2
#define 	BOARD_TELINK_RC_8258			3
#define 	BOARD_TELINK_RC_8278			4


#if (MCU_CORE_8258==1)
#define		BOARD							BOARD_TELINK_RC_8258
#elif (MCU_CORE_8278==1)
#define		BOARD							BOARD_TELINK_RC_8278
#elif (MCU_CORE_826x==1)
#define		BOARD							BOARD_TELINK_RC_8269
#endif


#if(BOARD == BOARD_TELINK_RC_8269)
	#include "board_telink_rc.h"
#elif(BOARD == BOARD_C59T80A5_V1P1_8267)
	#include "board_C59T80A5_v1P1_8267.h"
#elif(BOARD == BOARD_TELINK_RC_8258)
	#include "board_telink_rc_8258.h"
#elif(BOARD == BOARD_TELINK_RC_8278)
	#include "board_telink_rc_8278.h"
#endif

#define MSO_APP_MAX_ERR_CNT              10
#define MSO_APP_NV_ITEM                  1

    /* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

