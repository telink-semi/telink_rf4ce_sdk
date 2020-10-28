/********************************************************************************************************
 * @file     board_cfg.h
 *
 * @brief    Telink 8258 Dongle board configurtion
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

#pragma  once

#if (MODULE_USB_ENABLE)
#define	PA5_FUNC				AS_USB
#define	PA6_FUNC				AS_USB
#define PA5_INPUT_ENABLE		1
#define PA6_INPUT_ENABLE		1
#endif

/*
 * ADC PIN configuration
 *
 * */
	#define ZRC_APP_ADC_PIN                  GPIO_PB7



