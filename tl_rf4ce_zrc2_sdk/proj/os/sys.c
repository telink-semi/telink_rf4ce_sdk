
#include "../tl_common.h"
#include "sys.h"
#include "../os/ev.h"
#include "../../platform/platform_includes.h"
#include "../os/timer.h"
#include "../../net/rf4ce/rf4ce_includes.h"
#include "../drivers/drv_flash.h"
#include "../drivers/drv_pm.h"

extern void app_idle_handler(void);

#define FLASH_SECTOR_SIZE		4096
#define FLASH_READ_SIZE_PER_TIME 32


sys_stat_ctrl_t sys_stat_ctrl;
u32 sys_active_event_time = 0;
extern ev_poll_callback_t sys_idle_handler_ptr;


u32 sysSuspendTimeInMs = PM_SUSPEND_WAKEUP_TIME;
#if(MODULE_PM_ENABLE)
extern int app_saveInfoPm(void);

void sys_suspendTimerSet(u32 ms){
	sysSuspendTimeInMs = ms;
}

/*
 * system enter suspend mode
 *
 * */
static void sys_enter_suspend_mode(void){
	static u32 lastWakeupTime = 0;
	static u8 firstRun = 1;
	u32 realInterval = sysSuspendTimeInMs;

	if(!firstRun){
		u32 runTime = (clock_time() - lastWakeupTime) / CLOCK_SYS_CLOCK_1MS;
		if(lastWakeupTime != 0){
			if((runTime + 4)< sysSuspendTimeInMs){
				realInterval = sysSuspendTimeInMs - runTime;
			}else{
				realInterval = 4;
			}
		}
	}

	int wakeupSrc = PLATFORM_WAKEUP_TIMER;
#if 0
	extern u8 app_wakeupLevel[];
	extern u32 app_wakeupPins[];
	if(app_isOTAPendingstate()){
		for(s32 i = 0; i < KB_ARRAY_ROW; i++){
			if((gpio_read(app_wakeupPins[i]) && app_wakeupLevel[i]) ||
				(!gpio_read(app_wakeupPins[i]) && !app_wakeupLevel[i])){
				return;
			}
			PM_PadSet(app_wakeupPins[i], app_wakeupLevel[i], 1);
		}
		wakeupSrc |= PLATFORM_WAKEUP_PAD;
	}
#endif

	platform_wakeup_e ws = platform_lowpower_enter(PLATFORM_MODE_SUSPEND, wakeupSrc, realInterval);
	lastWakeupTime = clock_time();
	firstRun = 0;
//	app_validLevelForPm(0);
}


/*
 * system enter deep sleep mode
 *
 * */
void sys_enter_deepsleep_mode(void){
	int sysSta = app_saveInfoPm();

	if(sysSta == 1){
		extern u8 app_wakeupLevel[];
		extern u32 app_wakeupPins[];
		for(s32 i = 0; i < KB_ARRAY_ROW; i++)
		{
			if((gpio_read(app_wakeupPins[i]) && app_wakeupLevel[i]) ||
				(!gpio_read(app_wakeupPins[i]) && !app_wakeupLevel[i])){
				return;
			}
			platform_wakeup_pad_cfg(app_wakeupPins[i], app_wakeupLevel[i], 1);
		}
	}else if(sysSta == 0){
	}else{
		return;
	}

	platform_lowpower_enter(PLATFORM_MODE_DEEPSLEEP, PLATFORM_WAKEUP_PAD, 0);
	app_validLevelForPm(1);
}


void sys_on_suspend(void* data){
	u8 r = irq_disable();			// must

#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
	/* enter suspend mode */
	sys_enter_suspend_mode();

#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
	/* if idle time more than 600ms, move to deep sleep */
	if (app_allowedDeep() )  {
		sys_enter_deepsleep_mode();
	}

	irq_restore(r);
}
#endif

void sys_idle_handler(void){
	app_idle_handler();

#if(MODULE_PM_ENABLE)
    if( stack_allow_suspend() && app_isIdle() && ev_isTaskDone()){
        sys_on_suspend(0);
    }
#else
#if BATTERY_DETECT_ENABLE
    if (app_allowedDeep() )  {
    	app_saveInfoPm();
    }
#endif
#endif
}


void sys_init(void){
#if (0 || IRQ_TIMER1_ENABLE)
	/* Initialize hardware timer */
	hwTmr_init(TIMER_IDX_1, TIMER_MODE_SCLK);
#endif

    sys_idle_handler_ptr = sys_idle_handler;

#if (MODULE_PM_ENABLE)
	sys_active_event_time = clock_time();
#endif

}

#if (FLASH_PROTECT)
extern const u8 protect_flash_cmd;
#endif
/* the following code must be allocated at RAM!!!!!! */
_attribute_ram_code_ void sys_reboot(u32 addr, u32 size){
	u32 sect_num_total = size / FLASH_SECTOR_SIZE + 1;
	u32 page_num_total = size / FLASH_PAGE_SIZE + 1;
	u32 erase_addr = 0;

recopy_start:
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
#if (FLASH_PROTECT)
	if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
		goto recopy_start;
	}
#endif

	/* erase */
	for(int i = 0; i < sect_num_total; i++){
		flash_erase_sector(erase_addr);
		erase_addr += FLASH_SECTOR_SIZE;
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
	}

	/* write */
	u32 r_ptr = addr;
	u32 write_addr = 0;
	u32 image_buf[FLASH_READ_SIZE_PER_TIME/4];
	for(int i = 0; i < page_num_total; i++){
		for(int j = 0; j < FLASH_PAGE_SIZE/FLASH_READ_SIZE_PER_TIME; j++){
			flash_read(r_ptr + FLASH_READ_SIZE_PER_TIME * j,
			FLASH_READ_SIZE_PER_TIME, (u8*)image_buf);
			flash_write(write_addr+FLASH_READ_SIZE_PER_TIME*j, FLASH_READ_SIZE_PER_TIME, (u8*)image_buf);
#if(MODULE_WATCHDOG_ENABLE)
			wd_clear();
#endif
		}
		write_addr += FLASH_PAGE_SIZE;
        r_ptr += FLASH_PAGE_SIZE;
	}

	/* check */
    u32 image_CheckBuf[FLASH_READ_SIZE_PER_TIME/4];
	u32 p_src = addr;
	u32 p_dst= 0;
	/* read some timers to avoid read error at first time */
	for(u32 i = 0; i < size; ){
		flash_read(p_src + i, FLASH_READ_SIZE_PER_TIME, (u8*)image_buf);
        flash_read(p_dst + i, FLASH_READ_SIZE_PER_TIME, (u8*)image_CheckBuf);
        for (u32 k=0; k<FLASH_READ_SIZE_PER_TIME/4; k++) {
            if ( image_CheckBuf[k] != image_buf[k] ) {
                goto recopy_start;
            }
        }
        i += FLASH_READ_SIZE_PER_TIME;
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
	}

#if (FLASH_PROTECT)
	if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
		return NV_ENABLE_PROTECT_ERROR;
	}
#endif

	/* reset system */
	volatile int j;
	write_reg8(0x80006f,0x20);
	for(j=0;j<3200;j++);
}
