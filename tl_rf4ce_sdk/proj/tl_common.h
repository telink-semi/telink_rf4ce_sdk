/********************************************************************************************************
 * @file    tl_common.h
 *
 * @brief   This is the header file for tl_common.h
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

#include "common/types.h"
#include "common/bit.h"
#include "common/utility.h"
#include "common/static_assert.h"
#include "common/assert.h"
#include "common/string.h"
#include "common/compiler.h"
#include "common/tlPrintf.h"
#include "os/ev.h"
#include "os/ev_buffer.h"
#include "os/sys.h"
#include "os/task_queue.h"
#include "drivers/drv_timer.h"
#include "drivers/drv_hw.h"
#include "drivers/drv_pwm.h"
#include "drivers/tl_putchar.h"
#include "drivers/usb/usb.h"

#include "../platform/platform_includes.h"
#include "drivers/nv.h"

#include "../vendor/common/user_config.h"
#include "../vendor/common/user_trace.h"

#define _CODE_MAC_
#define _CODE_NWK_
#define _CODE_PBP_
#define _CODE_MSO_
#define _CODE_ZRC_
#define _CODE_GDP_
#define _CODE_DRV_


//#define _CODE_MAC_		__attribute__((section(".sect_mac")))
//#define _CODE_NWK_		__attribute__((section(".sect_nwk")))
//#define _CODE_PBP_		__attribute__((section(".sect_pbp")))
//#define _CODE_MSO_		__attribute__((section(".sect_mso")))
//#define _CODE_ZRC_		__attribute__((section(".sect_zrc")))
//#define _CODE_GDP_		__attribute__((section(".sect_gdp")))
//#define _CODE_DRV_		__attribute__((section(".sect_drv")))


