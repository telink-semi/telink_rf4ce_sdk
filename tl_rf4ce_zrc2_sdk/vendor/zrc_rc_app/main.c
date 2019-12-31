
#include "../../proj/tl_common.h"
#include "../../vendor/common/user_config.h"
#include "../../net/rf4ce/rf4ce_includes.h"
#if (MODULE_AUDIO_ENABLE)
#include "../../proj/drivers/audio/drv_audio.h"
#endif
#include "../../proj/os/sys.h"

#if defined(MCU_CORE_826x)
	#if(CLOCK_SYS_CLOCK_HZ == 32000000)
		SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_32M_PLL;
	#elif(CLOCK_SYS_CLOCK_HZ == 16000000)
		SYS_CLK_TYPEDEF g_sysClk = SYS_CLK_16M_PLL;
	#else
		#error please config system clock
	#endif
#elif defined(MCU_CORE_8258)
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

#if MCU_CORE_8258
	extern void bss_section_clear(void);
	extern void data_section_load();
		bss_section_clear(); 		//power on
		data_section_load();

#endif

	cpu_wakeup_init();

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

    while (1) {
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

