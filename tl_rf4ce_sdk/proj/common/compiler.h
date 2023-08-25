/********************************************************************************************************
 * @file    compiler.h
 *
 * @brief   This is the header file for compiler.h
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


#define _attribute_packed_		__attribute__((packed))
#define _attribute_aligned_(s)	__attribute__((aligned(s)))
#define _attribute_session_(s)	__attribute__((section(s)))
#define _attribute_ram_code_  	_attribute_session_(".ram_code")
#define _attribute_custom_code_  	_attribute_session_(".custom") volatile
#define _attribute_no_inline_   __attribute__((noinline)) 
#define _attribute_ram_code_sec_      			__attribute__((section(".ram_code")))
#define _attribute_ram_code_sec_noinline_      	__attribute__((section(".ram_code"))) __attribute__((noinline))

#define _inline_ 				inline				//   C99 meaning


#ifndef	BLC_PM_DEEP_RETENTION_MODE_EN
#define	BLC_PM_DEEP_RETENTION_MODE_EN					0
#endif

#if (BLC_PM_DEEP_RETENTION_MODE_EN)
	#define _attribute_data_retention_   __attribute__((section(".retention_data")))
#else
	#define _attribute_data_retention_
#endif



#define _attribute_ram_code_sec_      			__attribute__((section(".ram_code")))

#define _attribute_ram_code_sec_noinline_      	__attribute__((section(".ram_code"))) __attribute__((noinline))

#define _attribute_text_sec_   					__attribute__((section(".text"))) __attribute__((noinline))//Inlining happens when __attribute__((noinline)) is not added.

#define _attribute_aes_data_sec_      			__attribute__((section(".aes_data")))

#define _attribute_data_retention_sec_   		__attribute__((section(".retention_data")))

#define _attribute_aligned_(s)					__attribute__((aligned(s)))

#define _always_inline                          inline __attribute__((always_inline))

/// Pack a structure field
#define __PACKED __attribute__ ((__packed__))


#define _inline_ 				inline				//   C99 meaning

