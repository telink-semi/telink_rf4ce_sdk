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


volatile u32 tick_usb_enum=0;


int main (void) {
	u8 isDeepBack = drv_platform_init();

	sysIdleTaskInit();

	ev_buf_init();

	task_sched_init();

    user_init ();

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





