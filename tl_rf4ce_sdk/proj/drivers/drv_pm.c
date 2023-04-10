/********************************************************************************************************
 * @file    drv_pm.c
 *
 * @brief   This is the source file for drv_pm.c
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

#include "../tl_common.h"
#include "drv_pm.h"

void platform_wakeup_init(void){
#if defined(MCU_CORE_8278)
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);
#else
	cpu_wakeup_init();
#endif
}

void platform_wakeup_pad_cfg(u32 pin, platform_wakeup_level_e pol, int en){
#if defined (MCU_CORE_826x) || defined (MCU_CORE_HAWK)
	PM_PadSet(pin, pol, en);
#else
	cpu_set_gpio_wakeup(pin, pol, en);
#endif
}

platform_wakeup_e platform_lowpower_enter(platform_mode_e mode, platform_wakeup_e src, u32 cycle_ms ){
#if defined (MCU_CORE_826x)
	u8 sleep_mode = 0;
	if(mode == PLATFORM_MODE_DEEPSLEEP){
		sleep_mode = 1;
	}

	WakeupSrc_TypeDef srcType = 0;
	if(src & PLATFORM_WAKEUP_PAD){
		srcType |= WAKEUP_SRC_PAD;
	}
	if(src & PLATFORM_WAKEUP_TIMER){
		srcType |= WAKEUP_SRC_TIMER;
	}
	int wakeupSrc = pm_sleep_wakeup (sleep_mode, srcType, clock_time() + cycle_ms*1000*CLOCK_SYS_CLOCK_1US);
	if(wakeupSrc & BIT(1)){
		return PLATFORM_WAKEUP_TIMER;
	}else{
		return PLATFORM_WAKEUP_PAD;
	}
#else
	platform_wakeup_e ws = PLATFORM_WAKEUP_TIMER;
	SleepMode_TypeDef sleep_mode = SUSPEND_MODE;
	if(mode == PLATFORM_MODE_SUSPEND){
		sleep_mode = SUSPEND_MODE;
	}else if(mode == PLATFORM_MODE_DEEPSLEEP){
		sleep_mode = DEEPSLEEP_MODE;
	}

	SleepWakeupSrc_TypeDef srcType = 0;
	if(src & PLATFORM_WAKEUP_PAD){
		srcType |= PM_WAKEUP_PAD;
	}
	if(src & PLATFORM_WAKEUP_TIMER){
		srcType |= PM_WAKEUP_TIMER;
	}

	int wakeupSrc = cpu_sleep_wakeup (sleep_mode, srcType, clock_time() + cycle_ms*1000*S_TIMER_CLOCK_1US);
	if(wakeupSrc & BIT(1)){
		ws = PLATFORM_WAKEUP_TIMER;
	}else{
		ws = PLATFORM_WAKEUP_PAD;
	}
	/* reconfigure some module used */
	ZB_RADIO_INIT();
	rf_setTxPower(PHY_TX_POWER_MAX);
#if 0
	u8 value;
	u8 len;
	tl_zbMacAttrGet(MAC_PHY_ATTR_CURRENT_CHANNEL, &value, &len);
	ZB_TRANSCEIVER_SET_CHANNEL(value);
#endif
	return ws;
#endif
}
