/********************************************************************************************************
 * @file    clock_826x.h
 *
 * @brief   This is the header file for B86
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

#pragma once

#ifdef WIN32
#include <time.h>
#endif
#include "../../proj/common/compiler.h"
#include "../../vendor/common/user_config.h"
// constants
// system clock

#define WATCHDOG_INIT_TIMEOUT		600

#define	CLOCK_TYPE_PLL		0
#define	CLOCK_TYPE_OSC		1
#define	CLOCK_TYPE_PAD		2
#define	CLOCK_TYPE_ADC		3

enum{
	CLOCK_SEL_32M_RC = 	0,
	CLOCK_SEL_HS_DIV = 	1,
	CLOCK_SEL_16M_PAD =	2,
	CLOCK_SEL_32M_PAD =	3,
	CLOCK_SEL_SPI  	  = 4,
	CLOCK_SEL_40M_INTERNAL = 5,
	CLOCK_SEL_32K_RC  =	6,
};


typedef enum{
	SYS_CLK_16M_PLL,
	SYS_CLK_32M_PLL,
	SYS_CLK_32M_RC,
	SYS_CLK_16M_RC,
	SYS_CLK_8M_RC,
}SYS_CLK_TYPEDEF;


extern unsigned long CLOCK_SYS_CLOCK_1US;
extern unsigned long CLOCK_SYS_CLOCK_1MS;
extern unsigned long tickPerUs;

#define MASTER_CLK_FREQ		tickPerUs

// we use clock insteady of timer, to differentiate OS timers utility
static inline void clock_enable_clock(int tmr, int en){
	if(0 == tmr){
		SET_FLD_V(reg_tmr_ctrl, FLD_TMR0_EN, en);
	}else if(1 == tmr){
		SET_FLD_V(reg_tmr_ctrl, FLD_TMR1_EN, en);
	}else{
		SET_FLD_V(reg_tmr_ctrl, FLD_TMR2_EN, en);
	}
}


void clock_init(SYS_CLK_TYPEDEF SYS_CLK);

_attribute_ram_code_ void sleep_us (unsigned int microsec);		//  use register counter to delay

static inline u32 clock_time(){
	return reg_system_tick;
}

static inline u32 clock_time2(int tmr){
	return reg_tmr_tick(tmr);
}

// check if the current time is exceed span_us from ref time
static inline u32 clock_time_exceed(u32 ref, u32 span_us){
	return ((u32)(clock_time() - ref) > span_us * CLOCK_SYS_CLOCK_1US);
}

// more efficient than clock_set_interval
static inline void clock_set_tmr_interval(int tmr, u32 intv){
	reg_tmr_capt(tmr) = intv;
}

static inline void clock_set_tmr_mode(int tmr, u32 m){
	if(0 == tmr){
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR0_MODE, m);
	}else if(1 == tmr){
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR1_MODE, m);
	}else{
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR2_MODE, m);
	}
}

static inline u32 clock_get_tmr_status(int tmr){
	if(0 == tmr){
		return reg_tmr_ctrl & FLD_TMR0_STA;
	}else if(1 == tmr){
		return reg_tmr_ctrl & FLD_TMR1_STA;
	}else{
		return reg_tmr_ctrl & FLD_TMR2_STA;
	}
}


//  watchdog use timer 2
static inline void wd_set_interval_ms(u32 interval_ms)
{
	reg_tmr_ctrl = MASK_VAL(FLD_TMR_WD_CAPT, (interval_ms * CLOCK_SYS_CLOCK_1US * 1000 >> WATCHDOG_TIMEOUT_COEFF));
}

//  watchdog use timer 2
static inline void wd_start(void)
{
	SET_FLD(reg_tmr_ctrl, FLD_TMR_WD_EN);
}

static inline void wd_stop(void)
{
	CLR_FLD(reg_tmr_ctrl, FLD_TMR_WD_EN);
}

static inline void wd_clear(void)
{
	SET_FLD(reg_tmr_ctrl, FLD_CLR_WD);
}


/**
 * @brief This function servers to set system timer irq mask.
 * @param[in]	none.
 * @return  	none.
 */
static inline void stimer_set_irq_mask(void)
{
	reg_irq_mask |= FLD_IRQ_SYSTEM_TIMER;
}

/**
 * @brief This function servers to clear system timer irq mask.
 * @param[in] 	none.
 * @return  	none.
 */
static inline void stimer_clr_irq_mask(void)
{
	reg_irq_mask &= ~(unsigned long)FLD_IRQ_SYSTEM_TIMER;
}

static inline void timer_set_init_tick(unsigned char tmrIdx, unsigned int init_tick)
{
	reg_tmr_tick(tmrIdx) = init_tick;
}


/*
 * @brief     This function set to capture tick for timer.
 * @param[in] type - the specified timer.
 * @param[in] cap_tick - cap tick value.
 * @return    none
 */
static inline void timer_set_cap_tick(unsigned char tmrIdx, unsigned int cap_tick)
{
	reg_tmr_capt(tmrIdx) = cap_tick;
}

/*
 * @brief     This function set capture tick for system timer.
 * @param[in] cap_tick - cap tick value.
 * @return    none
 */
static inline void stimer_set_irq_capture(unsigned long cap_tick)
{
	reg_system_tick_irq = cap_tick;
}

/*
 * @brief     This function to enable the timer interrupt.
 * @param[in] type - the specified timer.
 * @return    none
 */
static inline void timer_irq_enable(unsigned char tmrIdx)
{
	reg_irq_mask |= (1 << tmrIdx);
}

/*
 * @brief     This function to enable the system timer interrupt.
 * @param[in] type - the specified timer.
 * @return    none
 */
static inline void stimer_irq_enable(void)
{
	reg_system_tick_mode |= FLD_SYSTEM_TICK_IRQ_EN;
}



#define TIMER_STATE_CLEAR(idx) 			reg_tmr_sta |= (1 << tmrIdx)
#define TIMER_STOP(idx)					clock_enable_clock(idx, 0)
#define TIMER_START(idx)				clock_enable_clock(idx, 1)
#define TIMER_TICK_CLEAR(idx)   		reg_tmr_tick(tmrIdx) = 0
#define TIMER_INTERVAL_SET(idx, cyc)	reg_tmr_capt(idx) = cyc
#define TIMER_INIT(tmrIdx, mode)			do{ \
										   clock_set_tmr_mode(tmrIdx, mode);\
										   reg_tmr_sta |= 1 << tmrIdx;	\
										   reg_irq_src |= 1 << tmrIdx;	\
										   reg_irq_mask |= 1 << tmrIdx;	\
									   }while(0)

#define timer_set_mode(tmrIdx, mode) 	TIMER_INIT(tmrIdx, mode)
#define timer_stop(tmrIdx) 				TIMER_STOP(tmrIdx)
#define timer_start(tmrIdx) 			TIMER_START(tmrIdx)

extern void sleep_us (unsigned int us);

#define WaitUs(t)			sleep_us(t)
#define WaitMs(t)			sleep_us((t)*1000)
#define sleep_ms(t)			sleep_us((t)*1000)
#define delay_us(t)			sleep_us(t)
