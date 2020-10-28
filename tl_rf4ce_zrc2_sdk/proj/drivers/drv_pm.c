/********************************************************************************************************
 * @file     drv_pm.c
 *
 * @brief    API for platform low power
 *
 * @author
 * @date     June. 20, 2018
 *
 * @par      Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
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
	int wakeupSrc = PM_LowPwrEnter (sleep_mode, srcType, clock_time() + cycle_ms*1000*CLOCK_SYS_CLOCK_1US);
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

	int wakeupSrc = cpu_sleep_wakeup (sleep_mode, srcType, clock_time() + cycle_ms*1000*CLOCK_SYS_CLOCK_1US);
	if(wakeupSrc & BIT(1)){
		ws = PLATFORM_WAKEUP_TIMER;
	}else{
		ws = PLATFORM_WAKEUP_PAD;
	}
	/* reconfigure some module used */
	ZB_RADIO_INIT();

#if 0
	u8 value;
	u8 len;
	tl_zbMacAttrGet(MAC_PHY_ATTR_CURRENT_CHANNEL, &value, &len);
	ZB_TRANSCEIVER_SET_CHANNEL(value);
#endif
	return ws;
#endif
}
