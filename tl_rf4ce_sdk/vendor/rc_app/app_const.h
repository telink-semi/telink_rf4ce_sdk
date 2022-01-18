/********************************************************************************************************
 * @file    app_const.h
 *
 * @brief   This is the header file for app_const.h
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

#pragma  once
#include "../../net/rf4ce/rf4ce_includes.h"
//#include "../../proj/drivers/usbkeycode.h"
#include "../../proj/tl_common.h"
#include "../../platform/platform_includes.h"



#define ZRC_APP_MAX_ERR_CNT              10
#define APP_INFO_NV_ITEM                  1


static const u16 batteryVoltage[] = {
		3000,
		2500,
		2300,
		2200,
		2000,
};

enum{
	BAT_LEVEL_FULL = 0,
	BAT_LEVEL_2P5_UP,
	BAT_LEVEL_2P3_0,
	BAT_LEVEL_2P2_0,
	BAT_LEVEL_CUTOFF  // <2.2v
};

#define 	BAT_THRESHOLD_OTA		BAT_LEVEL_2P5_UP
#define 	BAT_THRESHOLD_PAIR		BAT_LEVEL_2P3_0
#define 	BAT_THRESHOLD_FLASH		BAT_LEVEL_2P2_0
#define 	BAT_THRESHOLD_WARNING	BAT_LEVEL_2P2_0



#define 	APP_VID_SUPPPORT_MAX	1

typedef struct{
	u16 vendId;
	u8  vendString[7];
	u8  userString[15];
}appVendInfo_t;
extern const appVendInfo_t app_vendor_info_list[];



