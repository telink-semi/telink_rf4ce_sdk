#include "../tl_common.h"
#include "sys.h"
#include "timer.h"
#include "../../platform/platform_includes.h"
#ifdef WIN32
#include "ev_timer.h"
#endif




#define TIMER_SAFE_BOUNDARY_IN_US(v)		(10*v)
#define TIMER_OVERFLOW_VALUE        		0xFFFFFFFE


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

_attribute_ram_code_  void hwTmr_reset(u8 tmrIdx){
	if(tmrIdx < TIMER_NUM){
		memset((u8*)&hwTmr_vars.timerInfo[tmrIdx], 0x00, sizeof(hwTmr_info_t));
	}
}

void hwTmr_init(u8 tmrIdx, u8 mode){
	if(tmrIdx < TIMER_NUM){
		hwTmr_reset(tmrIdx);

		if(tmrIdx < TIMER_IDX_3){
			TIMER_INIT(tmrIdx, mode);
		}else{

#if defined (MCU_CORE_8278)
			reg_system_irq_mask &= (~BIT(2));   						//disable system timer irq
#else
			reg_system_tick_mode &= ~(u8)FLD_SYSTEM_TICK_IRQ_EN;
#endif
			reg_irq_mask &= ~(u32)FLD_IRQ_SYSTEM_TIMER;
		}
	}
}



void hwTmr_cancel(u8 tmrIdx)
{
	if(tmrIdx < TIMER_NUM){
		u8 r = irq_disable();

		if(tmrIdx < TIMER_IDX_3){
			/* Write 1 to clear, even if the timer is already set, cancel */
			TIMER_STATE_CLEAR(tmrIdx);
			TIMER_STOP(tmrIdx);
		}else{
#if defined (MCU_CORE_8278)
			reg_system_irq_mask &= (~BIT(2));   						//disable system timer irq
#else
			reg_system_tick_mode &= ~(u8)FLD_SYSTEM_TICK_IRQ_EN;
#endif
			reg_irq_mask &= ~(u32)FLD_IRQ_SYSTEM_TIMER;
		}

		hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];
		memset(pTimer, 0, sizeof(hwTmr_info_t));

		irq_restore(r);
	}
}

_attribute_ram_code_ void hwTmr_idle(u8 tmrIdx)
{
	if(tmrIdx < TIMER_NUM)
	hwTmr_vars.timerInfo[tmrIdx].flags.bf.status = TIMER_IDLE;
}


_attribute_ram_code_  void hwTmr_setInterval(u8 tmrIdx, u32 t_us){
    ext_clk_t t;
    t.high = 0;
    if(tmrIdx < TIMER_IDX_3){
    	t.low = t_us * H_TIMER_CLOCK_1US;
    }else{
    	t.low = t_us * S_TIMER_CLOCK_1US;
    }
    hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];

    /* Set expire information */
    pTimer->expireInfo.high = t.high;
    pTimer->expireInfo.low  = t.low;
}


_attribute_ram_code_ hw_timer_sts_t hwTmr_setAbs(u8 tmrIdx, ext_clk_t* absTimer, timerCb_t func, void* arg)
{
    u8 r;
	if ( tmrIdx >= TIMER_NUM ) {
		return HW_TIMER_IS_RUNNING;
	}
    hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];

    /* Validation */
    if (TIMER_IDLE != pTimer->flags.bf.status) {
        return HW_TIMER_IS_RUNNING;
    }

    r = irq_disable();

    /* Set expire information */
    pTimer->expireInfo.high = absTimer->high;
    pTimer->expireInfo.low  = absTimer->low;
    pTimer->cb = func;
    pTimer->arg = arg;

    pTimer->flags.bf.status = pTimer->expireInfo.high ? TIMER_WOF : TIMER_WTO;

    /* Safety Check - If time is already past, set timer as Expired */
    if (!pTimer->expireInfo.high && pTimer->expireInfo.low < TIMER_SAFE_BOUNDARY_IN_US(H_TIMER_CLOCK_1US)) {
        irq_restore(r);
        memset(pTimer, 0, sizeof(hwTmr_info_t));
		if ( func ) {
            func(arg);
		}
        return (hw_timer_sts_t)SUCCESS;
    } else {
    	if(tmrIdx < TIMER_IDX_3){
			/* Set compare value */
			TIMER_TICK_CLEAR(tmrIdx);
			TIMER_INTERVAL_SET(tmrIdx, pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);

			/* Enable Timer */
			TIMER_START(tmrIdx);
    	}else{
#if defined (MCU_CORE_8278)
    		reg_system_irq_mask |= BIT(2);   										//enable system timer irq
    		reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
    		reg_system_tick_irq_level = clock_time() + (pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
#else
    		reg_system_tick_mode |= FLD_SYSTEM_TICK_IRQ_EN;
    		reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
    		reg_system_tick_irq = clock_time() + (pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
#endif

    	}
    }

    irq_restore(r);

    return (hw_timer_sts_t)SUCCESS;
}




_attribute_ram_code_ hw_timer_sts_t hwTmr_set(u8 tmrIdx, u32 t_us, timerCb_t func, void *arg){
    ext_clk_t t;
    t.high = 0;
    if(tmrIdx < TIMER_IDX_3){
    	t.low = t_us * H_TIMER_CLOCK_1US;
    }else{
    	t.low = t_us * S_TIMER_CLOCK_1US;
    }
    return hwTmr_setAbs(tmrIdx, &t, func, arg);
}


_attribute_ram_code_ void timer_irq_handler(u8 tmrIdx)
{
    hwTmr_info_t *pTimer = &hwTmr_vars.timerInfo[tmrIdx];

    if(tmrIdx < TIMER_IDX_3){
		/* Write 1 to clear */
		TIMER_STATE_CLEAR(tmrIdx);
		TIMER_STOP(tmrIdx);
		TIMER_TICK_CLEAR(tmrIdx);
    }else{

#if defined (MCU_CORE_8278)
    	reg_system_tick_irq_level ^= BIT(31);
    	reg_system_irq_mask &= (~BIT(2));   						//disable system timer irq
#else
    	reg_system_tick_irq ^= BIT(31);
    	reg_system_tick_mode &= ~(u8)FLD_SYSTEM_TICK_IRQ_EN;
#endif
    	reg_irq_mask &= ~(u32)FLD_IRQ_SYSTEM_TIMER;
    }

    if (TIMER_WTO == pTimer->flags.bf.status) {
        /* Expired, callback */
        if (pTimer->cb) {
        	int t;

        	t = pTimer->cb(pTimer->arg);

        	if(t < 0){
        		memset(pTimer, 0, sizeof(hwTmr_info_t));
        	}else if(t == 0){
        		if(tmrIdx < TIMER_IDX_3){
					TIMER_INTERVAL_SET(tmrIdx, pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
					/* Enable Timer */
					TIMER_START(tmrIdx);
        		}else{

#if defined (MCU_CORE_8278)
					reg_system_tick_irq_level = clock_time() + (pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
					reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
					reg_system_irq_mask |= BIT(2);   										//enable system timer irq
#else
					reg_system_tick_irq = clock_time() + (pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
					reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
					reg_system_tick_mode |= FLD_SYSTEM_TICK_IRQ_EN;
#endif
        		}
        	}else{
        		pTimer->expireInfo.low = t * H_TIMER_CLOCK_1US;

        		if(tmrIdx < TIMER_IDX_3){
					TIMER_INTERVAL_SET(tmrIdx, pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
					/* Enable Timer */
					TIMER_START(tmrIdx);
        		}else{

#if defined (MCU_CORE_8278)
					reg_system_tick_irq_level = clock_time() + (pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
					reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
					reg_system_irq_mask |= BIT(2);   										//enable system timer irq
#else
					reg_system_tick_irq = clock_time() + (pTimer->expireInfo.high ? TIMER_OVERFLOW_VALUE : pTimer->expireInfo.low);
					reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
					reg_system_tick_mode |= FLD_SYSTEM_TICK_IRQ_EN;
#endif
        		}
        	}
        }
    } else{
        if (--pTimer->expireInfo.high) {
        	if(tmrIdx < TIMER_IDX_3){
        		TIMER_INTERVAL_SET(tmrIdx, TIMER_OVERFLOW_VALUE);
        	}else{
#if defined (MCU_CORE_8278)
        		reg_system_tick_irq_level = clock_time() + TIMER_OVERFLOW_VALUE;
#else
        		reg_system_tick_irq = clock_time() + TIMER_OVERFLOW_VALUE;
#endif
        	}
        } else {
        	if(tmrIdx < TIMER_IDX_3){
        		TIMER_INTERVAL_SET(tmrIdx, pTimer->expireInfo.low);
        	}else{
#if defined (MCU_CORE_8278)
        		reg_system_tick_irq_level = clock_time() + pTimer->expireInfo.low;
#else
        		reg_system_tick_irq = clock_time() + pTimer->expireInfo.low;
#endif

        	}
            pTimer->flags.bf.status = TIMER_WTO;
        }

        /* Enable Timer again */
        if(tmrIdx < TIMER_IDX_3){
        	TIMER_START(tmrIdx);
        }else{
        	reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
#if defined (MCU_CORE_8278)
					reg_system_irq_mask |= BIT(2);   										//enable system timer irq
#else
					reg_system_tick_mode |= FLD_SYSTEM_TICK_IRQ_EN;
#endif
        }
    }
}

_attribute_ram_code_ void timer_irq0_handler(void)
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

void timer_irq3_handler(void)
{
    timer_irq_handler(TIMER_IDX_3);
}

