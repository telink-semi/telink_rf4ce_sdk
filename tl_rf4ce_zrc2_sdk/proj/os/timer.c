#include "../tl_common.h"
#include "sys.h"
#include "timer.h"
#include "../../platform/platform_includes.h"
#ifdef WIN32
#include "ev_timer.h"
#endif


#define TIMER_SAFE_BOUNDARY_IN_US   10*CLOCK_SYS_CLOCK_1US
#define TIMER_OVERFLOW_VALUE        0xFFFFFFFE


typedef struct {
    /* Expire time and callback parameters */
    ext_clk_t expireInfo;
    timerCb_t cb;
    void*     arg;

    /* Flags for useful information */
    union {
        struct {
            u32 status:2;
            u32 reserved:30;
        }bf;
        u32 byteVal;
    } flags; 
} hwTmr_info_t;



typedef struct {
    hwTmr_info_t timerInfo[TIMER_NUM];
} hwTmr_ctrl_t;


hwTmr_ctrl_t hwTmr_vars;

void hwTmr_reset(u8 tmrIdx){
    memset((u8*)&hwTmr_vars.timerInfo[tmrIdx], 0x00, sizeof(hwTmr_info_t));
}

void hwTmr_init(u8 tmrIdx, u8 mode){
    hwTmr_reset(tmrIdx);
    TIMER_INIT(tmrIdx, mode);
}



void hwTmr_cancel(u8 tmrIdx)
{
   u8 r = irq_disable();

	/* Write 1 to clear, even if the timer is already set, cancel */
	TIMER_STATE_CLEAR(tmrIdx);
	TIMER_STOP(tmrIdx);

	hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];
	memset(pTimer, 0, sizeof(hwTmr_info_t));

	irq_restore(r);
}

_attribute_ram_code_  void hwTmr_setInterval(u8 tmrIdx, u32 interval){
	TIMER_TICK_CLEAR(tmrIdx);
	TIMER_INTERVAL_SET(tmrIdx, interval);
}

hw_timer_sts_t hwTmr_set(u8 tmrIdx, u32 interval, timerCb_t func, void* arg){
	hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];

	pTimer->cb = func;

	TIMER_TICK_CLEAR(tmrIdx);
	TIMER_INTERVAL_SET(tmrIdx, interval);
	/* Enable Timer */
	TIMER_START(tmrIdx);

	return HW_TIMER_SUCC;
}


_attribute_ram_code_ void timer_irq_handler(u8 tmrIdx)
{
    hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];

    /* Write 1 to clear */

    if(pTimer->cb){
    	pTimer->cb(NULL);
    }
    TIMER_STATE_CLEAR(tmrIdx);
}

void timer_irq0_handler(void)
{
    timer_irq_handler(TIMER_IDX_0);
}

_attribute_ram_code_  void timer_irq1_handler(void)
{
    timer_irq_handler(TIMER_IDX_1);
}

void timer_irq2_handler(void)
{
    timer_irq_handler(TIMER_IDX_2);
}
