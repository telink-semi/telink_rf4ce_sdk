/********************************************************************************************************
 * @file     main.c
 *
 * @brief    the Main function for ZRC2 controller project
 *
 * @author
 * @date     July. 1, 2019
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
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
		SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_32M_Crystal;
	#elif(CLOCK_SYS_CLOCK_HZ == 24000000)
		SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_24M_Crystal;
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
u32 tick_usb_enum=0;
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
#if defined(MCU_CORE_8278)
	blc_pm_select_internal_32k_crystal();
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);
#else
	cpu_wakeup_init();
#endif
	clock_init(g_sysClk);

	gpio_init();

    ZB_RADIO_INIT();

    return ret;
}

int main (void) {
	platform_init();

    sys_init();

    ev_buf_init();

    user_init ();

    tick_usb_enum = clock_time ();
    irq_enable();

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

