/********************************************************************************************************
 * @file    tlPrintf.h
 *
 * @brief   This is the header file for tlPrintf.h
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


int Tl_printf(const char *format, ...);




#if (UART_PRINTF_MODE || USB_PRINTF_MODE)
	#define	arrayPrint(arrayAddr,len)					\
	{													\
		Tl_printf("\n*********************************\n");		\
		unsigned short	i = 0;							\
		do{												\
			Tl_printf(" 0x%x,",((unsigned char *)arrayAddr)[i++]);	\
			if(i%16==0)											\
					Tl_printf("\n");							\
		}while(i<len);										\
		Tl_printf("\n*********************************\n");		\
	}

	#define	DEBUG(compileFlag,...)						\
			do{											\
				if(compileFlag) Tl_printf(__VA_ARGS__);					\
			}while(0)

	#define printf			Tl_printf
	#define	printfArray		arrayPrint
#else
	#define printf
	#define	printfArray
	#define	DEBUG(compileFlag,...)
#endif

#if (FLASH_PRINTF_MODE)

#define quick_printf ftl_write
#else
#define quick_printf
#endif


