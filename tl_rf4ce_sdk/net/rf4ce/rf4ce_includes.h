/********************************************************************************************************
 * @file    rf4ce_includes.h
 *
 * @brief   This is the header file for rf4ce_includes.h
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

#ifndef  __RF4CE_INCLUDES_H__ 
#define  __RF4CE_INCLUDES_H__

#include "./mac/mac_phy.h"
#include "./mac/mac_const.h"
#include "./mac/mac_api.h"
#include "./mac/mac_pib.h"
#include "./mac/mac_task.h"
#include "./mac/mac_phy.h"
#include "./nwk/nwk_const.h"
#include "./nwk/nwk_pib.h"
#include "./nwk/nwk_api.h"
#include "./profile/profile_api.h"
#include "./profile/zrc_profileCommands.h"

extern const u8 ZRC2_APP_VENDOR_STRING[];
extern const u8 ZRC2_APP_USER_STRING[];
u8 RF4CE_Channel2Idx(u8 chn);
u8 RF4CE_Idx2Channel(u8 idx);
u8 RF4CENextChannel(u8 ch);
#endif
