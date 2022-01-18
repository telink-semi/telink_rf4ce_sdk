/********************************************************************************************************
 * @file    drv_pm.h
 *
 * @brief   This is the header file for drv_pm.h
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

#include "../common/types.h"
#include "../common/compiler.h"
#include "../../net/rf4ce/mac/mac_phy.h"
typedef enum{
	PLATFORM_MODE_SUSPEND,
	PLATFORM_MODE_DEEPSLEEP,
	PLATFORM_MODE_MCU_STALL,
}platform_mode_e;

typedef enum{
	PLATFORM_WAKEUP_PAD = (1 << 0),
	PLATFORM_WAKEUP_TIMER = (1 << 1),
}platform_wakeup_e;

typedef enum{
	PLATFORM_WAKEUP_LEVEL_LOW,
	PLATFORM_WAKEUP_LEVEL_HIGH,
}platform_wakeup_level_e;

void platform_wakeup_init(void);

void platform_wakeup_pad_cfg(u32 pin, platform_wakeup_level_e pol, int en);

platform_wakeup_e platform_lowpower_enter(platform_mode_e mode, platform_wakeup_e src, u32 cycle_ms);

void platform_longLowpower_enter(platform_mode_e mode, platform_wakeup_e src, u32 durationUs);
