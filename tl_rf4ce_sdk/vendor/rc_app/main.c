/********************************************************************************************************
 * @file    main.c
 *
 * @brief   This is the source file for main.c
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

#include "../../proj/tl_common.h"
#include "../../vendor/common/user_config.h"
#include "../../net/rf4ce/rf4ce_includes.h"
#if (MODULE_AUDIO_ENABLE)
#include "../../proj/drivers/audio/drv_audio.h"
#endif
#include "../../proj/os/sys.h"
#include "../../proj/drivers/drv_pm.h"
#if defined(MCU_CORE_826x)
	#if(CLOCK_SYS_CLOCK_HZ == 32000000)
		SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_32M_PLL;
	#elif(CLOCK_SYS_CLOCK_HZ == 16000000)
		SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_16M_PLL;
	#else
		#error please config system clock
	#endif
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	#if(CLOCK_SYS_CLOCK_HZ == 32000000)
		SYS_CLK_TypeDef g_sysClk = SYS_CLK_32M_Crystal;
	#elif(CLOCK_SYS_CLOCK_HZ == 24000000)
		SYS_CLK_TypeDef g_sysClk = SYS_CLK_24M_Crystal;
	#else
		#error please config system clock
	#endif
#endif

enum{
	SYSTEM_RETENTION_NONE,
	SYSTEM_RETENTION_EN,
};


extern void user_init();
void audio_recTaskRun(void);
volatile u32 tick_usb_enum=0;
/*
 * platform initiation:
 * 		clock_init: 	system clock selection
 * 		ZB_RADIO_INIT: 	RF module
 * 		gpio_init:		gpio configuration
 * 		ZB_TIMER_INIT:  enable a timer for zigbee mac cca-csma
 *
 * */
static u8 platform_init(void){
	u8 ret = SYSTEM_RETENTION_NONE;

#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	extern void bss_section_clear(void);
	extern void data_section_load();
		bss_section_clear(); 		//power on
		data_section_load();
#endif

#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258)
	cpu_wakeup_init();
#elif defined(MCU_CORE_8278)
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);
#endif

	clock_init(g_sysClk);

#if defined(MCU_CORE_8278) || defined(MCU_CORE_8258)
	if(pm_get_mcu_status()==MCU_STATUS_BOOT)//power on
	{
		clock_32k_init(CLK_32K_RC);
		rc_32k_cal();
	}
	pm_select_internal_32k_rc();
#endif

	gpio_init(TRUE);

    ZB_RADIO_INIT();

    /*check internal Flash Size*/
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	internalFlashSizeCheck();
#endif
    return ret;
}





int main (void) {
	platform_init();

    sys_init();

    ev_buf_init();

    user_init();

    tick_usb_enum = clock_time ();

    irq_enable();

#if (MODULE_WATCHDOG_ENABLE)
    wd_set_interval_ms(1000);
    wd_start();
#endif

    while(1){
#if (MODULE_AUDIO_DEBUG)
        audio_debug_usb_direct_out(tick_usb_enum);
        continue;
#endif
        ev_main();

#if (MODULE_AUDIO_ENABLE)
		if (audio_recTaskStatusGet() == AUDIO_OPENED) {
			audio_recTaskRun();
		}
#endif
    }
}





