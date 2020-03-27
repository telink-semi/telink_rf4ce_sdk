/********************************************************************************************************
 * @file     main.c
 *
 * @brief    the Main function for ZRC2 target project
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
#include "../../vendor/common/user_trace.h"

#if MCU_CORE_826x
SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_32M_PLL;
#elif MCU_CORE_8258
SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_32M_Crystal;
#elif MCU_CORE_8278
SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_32M_Crystal;
#endif

extern void user_init();

static void platform_init(void){
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	extern void bss_section_clear(void);
	extern void data_section_load();
	bss_section_clear();
	data_section_load();
#endif
	cpu_wakeup_init();

	clock_init(g_sysClk);

	gpio_init();

	usb_dp_pullup_en (0);

	irq_disable();

#if (MODULE_USB_ENABLE)
    usb_init();
#endif

    ZB_RADIO_INIT();

}

int main (void) {
	platform_init();

    sys_init();

	ev_buf_init();

    user_init ();

    irq_enable();

#if (MODULE_USB_ENABLE)
    usb_dp_pullup_en (1);
#endif
    while (1){
#if (MODULE_USB_ENABLE)
        usb_handle_irq();
#endif

#if(MODULE_WATCHDOG_ENABLE)
        wd_clear();
#endif
        ev_main();

#if(MODULE_AUDIO_ENABLE)
        abuf_mic_dec();
#endif
    }
}


