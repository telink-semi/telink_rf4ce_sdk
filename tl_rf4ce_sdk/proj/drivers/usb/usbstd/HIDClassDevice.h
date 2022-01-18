/********************************************************************************************************
 * @file    HIDClassDevice.h
 *
 * @brief   This is the header file for HIDClassDevice.h
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
#include "../common/types.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif


/* Type Defines: */
typedef struct usbhid_config_t{
	u8  InterfaceNumber;
	u8  ReportINEndpointNumber;
	u16 ReportINEndpointSize;
	bool ReportINEndpointDoubleBank;
	void* PrevReportINBuffer;
	u8  PrevReportINBufferSize;
} usbhid_config_t;


struct usbhid_state_t{
	bool UsingReportProtocol;
	u16 PrevFrameNum;
	u16 IdleCount;
	u16 IdleMSRemaining;
} usbhid_state_t;

typedef struct{
} USB_ClassInfo_HID_Device_t;


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif

/** @} */


