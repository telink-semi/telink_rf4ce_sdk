/********************************************************************************************************
 * @file    sys.h
 *
 * @brief   This is the header file for sys.h
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

enum{
	SYS_WAKEUP_FROM_OPR,
	SYS_WAKEUP_FROM_TIMER,
};

enum{
	SYS_SUSPEND_NOTIFY_SUSPEND,
	SYS_SUSPEND_NOTIFY_DEEPSLEEP,
};

enum{
	SYS_INIT_TYPE_POWERON,
	SYS_INIT_TYPE_DEEPSLEEP,
};

typedef struct {
	u8 init_type;
#if(APPLICATION_DONGLE)
	u8 host_suspend_mode;		// enter host_suspend_mode when the host ask me to suspend
#else	
	u8 sending_wakeup_key;		// enter host_suspend_mode when the host ask me to suspend
	u8 wakeup_chn;
	u8 active;
	u32 sending_wakeup_key_start_time;
#endif
}sys_stat_ctrl_t;

extern sys_stat_ctrl_t sys_stat_ctrl;
extern u32			dbg_tick[8];
void sys_init(void);
void sys_enter_host_suspend_mode();
void sys_exit_host_suspend_mode();
void sys_suspendTimerSet(u32 ms);
_attribute_ram_code_ u8 sys_reboot(u32 addr, u32 size);


