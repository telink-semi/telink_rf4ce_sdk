/********************************************************************************************************
 * @file    profile_config.h
 *
 * @brief   This is the header file for profile_config.h
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

#include "../../proj/common/types.h"
#include "rc_info.h"


#define NWK_ADD_FRAME_COUNTER       0x400
/**
 *  @brief  Definition for ZRC device type
 */
enum {
    DEVICE_RESERVED      = 0,
    DEVICE_RC,
    DEVICE_TV,
    DEVICE_PROJECTOR,
    DEVICE_PLAYER,
    DEVICE_RECORDER,
    DEVICE_DVD,
    DEVICE_CD,
    DEVICE_AUDIO,
    DEVICE_STB,
    DEVICE_ALL          =0xFF,
};

/**
 *  @brief  Macro for Remote Control device type
 */
#define RF4CE_DEVICE_TYPE(type)                  (1<<type)

/**********************************************************************
 * Following feature need user to enable
 */

/**
 *  @brief  Definition for which profile to use
 */
#if (__PROJECT_MSO_DONGLE__) || (__PROJECT_MSO_RC__)
#define RF4CE_ZID_ENABLE                          0
#define RF4CE_ZRC_ENABLE                          0
#define RF4CE_ZRC2_ENABLE                         0
#define RF4CE_MSO_ENABLE                          1
#elif (__PROJECT_ZRC_2_DONGLE__) || (__PROJECT_ZRC_2_RC__)
#define RF4CE_ZID_ENABLE                          0
#define RF4CE_ZRC_ENABLE                          0
#define RF4CE_ZRC2_ENABLE                         1
#define RF4CE_MSO_ENABLE                          0
#endif
/**
 *  @brief  Definition for Device Type, only one can be enabled
 */
#if (RF4CE_ZID_ENABLE)
    #define ZID_HID_ADAPTER                       0
    #define ZID_HID_CLASS_DEVICE                  1
#endif

#if (RF4CE_ZRC_ENABLE)
    #define ZRC_REMOTE_CONTROLLER                 1
    #define ZRC_TARGET_DEVICE                     0
#endif


#if (RF4CE_ZRC2_ENABLE)
	#define RF4CE_GDP_ENABLE                      1
    #define ZRC_REMOTE_CONTROLLER                 1
    #define ZRC_TARGET_DEVICE                     0
#endif

#if (RF4CE_MSO_ENABLE)
	#define RF4CE_GDP_ENABLE                      0
	#define MSO_REMOTE_CONTROLLER                 1
	#define MSO_TARGET_DEVICE                     0
#endif




/**
 *  @brief  Specify the product device type
 */
#define RF4CE_DEV_TYPE                            RF4CE_DEVICE_TYPE(DEVICE_RC)

/**
 *  @brief  Specify using Security or not
 */
#define SECURITY_ENABLE                           1

#define MSO_MAX_KEY_CNT_NUM                       50
#define MSO_MAX_GENERAL_PURPOSE_CNT               8

/**********************************************************************
 * Following configuration will calculated automatically
 */

#if (RF4CE_ZID_ENABLE)
    #define RF4CE_GDP_ENABLE                      1
#endif


#if (ZID_HID_ADAPTER)
    #define RF4CE_TARGET                          1
	#define RF4CE_CONTROLLER                      0
#endif

#if (ZID_HID_CLASS_DEVICE)
	#define RF4CE_CONTROLLER                      1
	#define RF4CE_TARGET                          0
#endif

#if (ZRC_TARGET_DEVICE)
    #define RF4CE_TARGET                          1
    #define RF4CE_CONTROLLER                      0
#endif

#if (ZRC_REMOTE_CONTROLLER)
    #define RF4CE_TARGET                          0
    #define RF4CE_CONTROLLER                      1
#endif


#if (MSO_TARGET_DEVICE)
    #define RF4CE_TARGET                          1
    #define RF4CE_CONTROLLER                      0
#endif

#if (MSO_REMOTE_CONTROLLER)
    #define RF4CE_TARGET                          0
    #define RF4CE_CONTROLLER                      1
#endif


enum {
	ACTION_BANK_HDMI_CEC = 0x0,
};

#if (RF4CE_ZRC2_ENABLE)
	extern const unsigned char ZRC2_DEFALUT_CLASS_DESP;
	extern const unsigned char ZRC2_DEFALUT_LQI_THRESHOLD;
	extern const u8 ZRC2_CLASS_FILTER;
	extern const u8 ZRC2_MIN_LQI_FILTER;
	extern const u16 ZRC2_FILTER_VENDOR_ID;	
//	extern const u16 APP_VENDOR_ID_LIST[];
#endif
#if (RF4CE_MSO_ENABLE)
extern const unsigned char MSO_APP_VENDOR_STRING[];
extern const unsigned char MSO_APP_USER_STRING[];
#endif

