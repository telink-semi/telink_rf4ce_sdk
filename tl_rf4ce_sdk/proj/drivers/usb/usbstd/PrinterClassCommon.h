/********************************************************************************************************
 * @file    PrinterClassCommon.h
 *
 * @brief   This is the header file for PrinterClassCommon.h
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
	/* Includes: */
#include "../../../common/types.h"
	
	/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif
	

/* Macros: */
	/** \name Virtual Printer Status Line Masks */
	//@{

	#define PRNT_PORTSTATUS_NOTERROR	(1 << 3)
	#define PRNT_PORTSTATUS_SELECT		(1 << 4)
	#define PRNT_PORTSTATUS_PAPEREMPTY	(1 << 5)
	//@}

/* Enums: */
	enum PRNT_Descriptor_ClassSubclassProtocol_t{
		PRNT_CSCP_PrinterClass			= 0x07,
		PRNT_CSCP_PrinterSubclass		= 0x01,
		PRNT_CSCP_BidirectionalProtocol = 0x02,
	};

	/** Enum for the Printer class specific control requests that can be issued by the USB bus host. */
	enum PRNT_ClassRequests_t{
		PRNT_REQ_GetDeviceID			= 0x00,
		PRNT_REQ_GetPortStatus			= 0x01,
		PRNT_REQ_SoftReset				= 0x02,
	};

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif

