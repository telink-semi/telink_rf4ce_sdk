/********************************************************************************************************
 * @file    StdRequestType.h
 *
 * @brief   This is the header file for StdRequestType.h
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

/* Public Interface - May be used in end-application: */
/* Macros: */
	/** Mask for the request type parameter, to indicate the direction of the request data (Host to Device
	 *  or Device to Host). The result of this mask should then be compared to the request direction masks.
	 *
	 *  \see \c REQDIR_* macros for masks indicating the request data direction.
	 */
	#define CONTROL_REQTYPE_DIRECTION  0x80
	#define CONTROL_REQTYPE_TYPE       0x60
	#define CONTROL_REQTYPE_RECIPIENT  0x1F

	#define REQDIR_HOSTTODEVICE        (0 << 7)
	#define REQDIR_DEVICETOHOST        (1 << 7)

	#define REQTYPE_STANDARD           (0 << 5)
	#define REQTYPE_CLASS              (1 << 5)
	#define REQTYPE_VENDOR             (2 << 5)

	#define REQREC_DEVICE              (0 << 0)
	#define REQREC_INTERFACE           (1 << 0)
	#define REQREC_ENDPOINT            (2 << 0)
	#define REQREC_OTHER               (3 << 0)
	//@}

/* Type Defines: */
	/** \brief Standard USB Control Request
	 */
	typedef struct
	{
		u8  bmRequestType;
		u8  bRequest;
		u16 wValue;
		u16 wIndex;
		u16 wLength;
	}USB_Request_Header_t;

/* Enums: */
	enum USB_Control_Request_t
	{
		REQ_GetStatus           = 0,
		REQ_ClearFeature        = 1,
		REQ_SetFeature          = 3,
		REQ_SetAddress          = 5,
		REQ_GetDescriptor       = 6,
		REQ_SetDescriptor       = 7,
		REQ_GetConfiguration    = 8,
		REQ_SetConfiguration    = 9,
		REQ_GetInterface        = 10,
		REQ_SetInterface        = 11,
		REQ_SynchFrame          = 12,
	};

	/** Feature Selector values for Set Feature and Clear Feature standard control requests directed to the device, interface
	 *  and endpoint recipients.
	 */
	enum USB_Feature_Selectors_t
	{
		FEATURE_SEL_EndpointHalt       = 0x00,
		FEATURE_SEL_DeviceRemoteWakeup = 0x01,
		FEATURE_SEL_TestMode           = 0x02,
	};

	#define FEATURE_SELFPOWERED_ENABLED     (1 << 0)
	#define FEATURE_REMOTE_WAKEUP_ENABLED   (1 << 1)

	
/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif

/** @} */


