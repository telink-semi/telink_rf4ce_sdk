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
#include "../../proj/drivers/audio/drv_audio.h"
#include "../../proj/drivers/usb/usb_uart.h"
#include "../../vendor/common/user_config.h"
#include "../../net/rf4ce/rf4ce_includes.h"
#include "../../vendor/common/user_trace.h"

#if MCU_CORE_826x
SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_32M_PLL;
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
SYS_CLK_TypeDef g_sysClk = SYS_CLK_32M_Crystal;
#endif
u32 tick_usb_enum=0;
extern void user_init();
static void platform_init(void){
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	extern void bss_section_clear(void);
	extern void data_section_load();
	bss_section_clear();
	data_section_load();
#endif

#if defined(MCU_CORE_8278)
	pm_select_internal_32k_rc();
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);
#else
	cpu_wakeup_init();
#endif

	clock_init(g_sysClk);

	gpio_init(TRUE);

	usb_dp_pullup_en (0);

	irq_disable();

#if (MODULE_USB_ENABLE)
    usb_init();
#endif

    ZB_RADIO_INIT();

#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
		internalFlashSizeCheck();
#endif
}


int main (void) {
	platform_init();

    sys_init();

	ev_buf_init();

    user_init ();

    irq_enable();

#if (MODULE_USB_ENABLE)
    usb_set_pin_en();
#endif
#if (MODULE_WATCHDOG_ENABLE)
    wd_set_interval_ms(1000);
    wd_start();
#endif

    tick_usb_enum = clock_time ();

    while(1)
    {
#if (MODULE_USB_ENABLE)
        usb_handle_irq();
        usb_uart_loopQ();
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



