/********************************************************************************************************
 * @file    ev.h
 *
 * @brief   This is the header file for ev.h
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
#include "../config/user_config.h"
#include "../../platform/platform_includes.h"
#include "./timer_event.h"


#define MEASURE_TASK_TICKS (0)

typedef void (*ev_poll_callback_t)(void);
enum{EV_FIRED_EVENT_MAX_MASK = EV_FIRED_EVENT_MAX - 1};

//enum { EV_TIMER_SAFE_MARGIN_US = 4000000, EV_TIMER_SAFE_MARGIN = (EV_TIMER_SAFE_MARGIN_US * CLOCK_SYS_CLOCK_1US) };        // in us,


#if USE_NEW_EV_MAIN
void ev_main(u32 expired_ticks, u8 mode);
#else
void ev_main(void);
#endif

void ev_rf4ceStackEnable(u8 en);

