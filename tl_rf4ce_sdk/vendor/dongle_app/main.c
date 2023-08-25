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


volatile u32 tick_usb_enum=0;


//(*(volatile unsigned char*)0x80140303) |= 0x8;
//(*(volatile unsigned char*)0x80140313) &= ~0x4;

int main (void) {

	u8 isDeepBack = drv_platform_init();

	sysIdleTaskInit();

	ev_buf_init();

	task_sched_init();

    user_init ();

#if (MODULE_WATCHDOG_ENABLE)
    drv_wd_setInterval(1000);
    drv_wd_start();
#endif

    irq_enable();
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



