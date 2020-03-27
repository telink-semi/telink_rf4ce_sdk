/********************************************************************************************************
 * @file     ir.c
 *
 * @brief	 for ir sending
 *
 * @author
 * @date     July. 20, 2017
 *
 * @par      Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd.
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

#include "../../tl_common.h"
#include "ir.h"
#include "ir_database.h"
#include "../../os/timer.h"
#include "../../os/ev_buffer.h"

static u8 IR_CHAANEL;
#define IR_DATA_LEN_MAX		128

typedef struct{
	int totalCnt;
	int curCnt;
	u16 *interval;//[IR_DATA_LEN_MAX];
	ev_time_event_t *repeatTimer;
	ev_time_event_t *stopTimer;
	u8 sta;
}ir_info_t;
ir_info_t irInfo;

void ir_init(u8 channel){
	IR_CHAANEL = channel;

#if MCU_CORE_826x
	pwm_Init(0);   //set PWM divider
	pwm_INVInvert(IR_CHAANEL);
#elif MCU_CORE_8258 || MCU_CORE_8278
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
	pwm_set_mode(IR_CHAANEL, PWM_NORMAL_MODE);
	pwm_set_phase(IR_CHAANEL, 0);   //no phase at pwm beginning
	pwm_n_revert(IR_CHAANEL);
	pwm_stop(IR_CHAANEL);
#endif
}

void ir_set(int hz, int low_duty){
#if MCU_CORE_826x
	u16 max_tick = CLOCK_SYS_CLOCK_HZ/hz;
	u16 cmp_tick = max_tick / low_duty;
	pwm_Open(IR_CHAANEL, NORMAL, 0, cmp_tick, max_tick, 0x2fff);
#elif MCU_CORE_8258 || MCU_CORE_8278
	u16 max_tick = CLOCK_SYS_CLOCK_HZ/hz;
	u16 cmp_tick = max_tick / low_duty;
	drv_pwm_cfg(IR_CHAANEL,cmp_tick,max_tick);
#endif
}

void MARK(u32 t_us){
#if MCU_CORE_826x
	EN_PWM(IR_CHAANEL);
#elif MCU_CORE_8258 || MCU_CORE_8278
	pwm_start(IR_CHAANEL);
#endif
}

void SPACE(u32 t_us){
#if MCU_CORE_826x
	DIS_PWM(IR_CHAANEL);
#elif MCU_CORE_8258 || MCU_CORE_8278
	pwm_stop(IR_CHAANEL);
#endif
}


u8 ir_isBusy(void){
	return (irInfo.sta);
}

void irInfoInit(void){
	memset(&irInfo, 0, sizeof(ir_info_t));
}

int ir_repeatTimerCb(void *arg){
	ev_buf_free((u8 *)irInfo.interval);
	irInfo.interval = NULL;
	irInfo.repeatTimer = NULL;

	return -1;
}

int ir_stopTimerCb(void *arg){
	irInfo.stopTimer = NULL;
	irInfo.sta = 0;

	return -1;
}

volatile u8 T_irTimer1IrqCbCnt = 0;
_attribute_ram_code_ int irTimer1IrqCb(void *arg){
	int t_us = irInfo.interval[irInfo.curCnt];

	hwTmr_setInterval(TIMER_IDX_1, t_us * tickPerUs - 128);

	T_irTimer1IrqCbCnt++;
	if(irInfo.curCnt == irInfo.totalCnt){
		SPACE(0);
		//cancel the timer1
		hwTmr_cancel(TIMER_IDX_1);
		irInfo.repeatTimer = ev_on_timer(ir_repeatTimerCb, NULL, 50*1000);
		irInfo.stopTimer = ev_on_timer(ir_stopTimerCb, NULL, 100*1000);
		return -1;
	}

	if (!(irInfo.curCnt&0x01)) {
		MARK(t_us);
		// change timer interval to t_us
	} else {
		SPACE(t_us);
	}
	irInfo.curCnt += 1;
	return 0;
}


void ir_send_serial(u16 *serial, u16 rawLen)
{

    if(irInfo.repeatTimer){
		ev_unon_timer(&irInfo.repeatTimer);
	}

    if(irInfo.stopTimer){
		ev_unon_timer(&irInfo.stopTimer);
	}

    irInfo.interval = (u16 *)ev_buf_allocate(LARGE_BUFFER);
    if(rawLen > IR_DATA_LEN_MAX || irInfo.interval == NULL){
    	return;
    }

    memcpy(irInfo.interval, serial, 2*rawLen);
    irInfo.totalCnt = rawLen;
    irInfo.curCnt = 0;
    hwTmr_set(TIMER_IDX_1, 1000 * tickPerUs, irTimer1IrqCb, NULL);
    irInfo.sta = 1;
}

/* Here the special bit means the level is the  tsame with it's previous one */
void ir_send_raw(u8 *raw, u32 rawLen, u8 specialIndex, u8 specialBase)
{
    u32 i;
    u32 t_us;
    if(irInfo.repeatTimer){
		ev_unon_timer(&irInfo.repeatTimer);
	}
    if(irInfo.stopTimer){
		ev_unon_timer(&irInfo.stopTimer);
	}

    irInfo.interval = (u16 *)ev_buf_allocate(LARGE_BUFFER);
    if(rawLen > IR_DATA_LEN_MAX || irInfo.interval == NULL){
    	return;
    }

    for (i = 0; i < rawLen; i++) {
        if (i == specialIndex) {
            t_us = raw[i] * specialBase;
        } else {
            t_us = raw[i] * 50;
        }
        irInfo.interval[i] = t_us;
    }
    irInfo.totalCnt = rawLen;
    irInfo.curCnt = 0;
    hwTmr_set(TIMER_IDX_1, 1000 * tickPerUs, irTimer1IrqCb, NULL);
    irInfo.sta = 1;
}


void ir_convert_byte(u16 data, u8 bitLen, u8* p, u8 one_mark, u8 one_space, u8 fmls_1, u8 zero_mark, u8 zero_space, u8 fmls_0)
{
    u8  i;
    for (i = 0; i < bitLen; i++) {
        if (data & 0x0001) {
            if (fmls_1) {
                *p++ = one_mark;
                *p++ = one_space;
            } else {
                *p++ = one_space;
                *p++ = one_mark;
            }
        } else {
            if (fmls_0) {
                *p++ = zero_mark;
                *p++ = zero_space;
            } else {
                *p++ = zero_space;
                *p++ = zero_mark;
            }
        }
        data >>= 1;
    }
}


#if 0
void ir_send_upd6121f(u16 addr, u16 cmd, u8 fRepeat)
{
    //spwm_set(SPWM_1, 38);
	ir_set(38000, 2);
    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = NEC_BIT_MARK/IR_BASE_TIME;
    u8 one_space = NEC_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = NEC_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = NEC_ZERO_SPACE/IR_BASE_TIME;

    u8 first_addr = (addr & 0xff00)>>8;
    u8 second_addr = addr & 0x00ff;
    u8 first_cmd = (cmd & 0xff00)>>8;
    u8 second_cmd = cmd & 0x00ff;

    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = NEC_HDR_MARK/IR_BASE_TIME;
    *p++ = NEC_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(first_addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(second_addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(first_cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(second_cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);
#if (__DEBUG_BUFM__)
    if ( ERR_NONE != bufm_free(buf) ) {
		while(1);
    }
#else
    bufm_free(buf);
#endif
}
#endif

void ir_send_upd6121g(u16 addr, u16 cmd, u8 fRepeat)
{
	ir_set(38000, 3);
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = NEC_BIT_MARK/IR_BASE_TIME;
    u8 one_space = NEC_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = NEC_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = NEC_ZERO_SPACE/IR_BASE_TIME;

    u8 first_addr = (addr & 0xff00)>>8;
    u8 second_addr = addr & 0x00ff;
    u8 first_cmd = (cmd & 0xff00)>>8;
    u8 second_cmd = cmd & 0x00ff;

    if (!buf) {
        while(1);
    }


    if (!fRepeat) {

        /* Lead code */
        *p++ = NEC_HDR_MARK/IR_BASE_TIME;
        *p++ = NEC_HDR_SPACE/IR_BASE_TIME;

        ir_convert_byte(first_addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
        p += 16;
        ir_convert_byte(second_addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
        p += 16;
        ir_convert_byte(first_cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
        p += 16;
        ir_convert_byte(second_cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
        p += 16;

    } else {
        *p++ = NEC_HDR_MARK/IR_BASE_TIME;
        *p++ = NEC_RPT_SPACE/IR_BASE_TIME;
    }

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

#if (__DEBUG_BUFM__)
    if ( ERR_NONE != ev_buf_free(buf) ) {
		while(1);
    }
#else
    ev_buf_free(buf);
#endif
}

#if 0
void ir_send_sony(u8 addr, u8 cmd, u8 fRepeat)
{
//    spwm_set(SPWM_1, 38);
	ir_set(38000, 2);
//    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
//	if ( NULL == buf ) {
//		while(1);
//	}
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = SONY_ONE_MARK/IR_BASE_TIME;
    u8 one_space = SONY_BIT_MARK/IR_BASE_TIME;
    u8 zero_mark = SONY_ZERO_MARK/IR_BASE_TIME;
    u8 zero_space = SONY_BIT_MARK/IR_BASE_TIME;
    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = SONY_HDR_MARK/IR_BASE_TIME;
    *p++ = SONY_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(cmd, 7, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 14;
    ir_convert_byte(addr, 5, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 10;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

//#if (__DEBUG_BUFM__)
//    if ( ERR_NONE != bufm_free(buf) ) {
//		while(1);
//    }
//#else
//    bufm_free(buf);
//#endif
	#if (__DEBUG_BUFM__)
		if ( ERR_NONE != ev_buf_free(buf) ) {
			while(1);
		}
	#else
		ev_buf_free(buf);
	#endif
}

void ir_send_rc5(u8 addr, u8 cmd, u8 fRepeat)
{
//    spwm_set(SPWM_1, 38);
	ir_set(38000, 2);
//	u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
//	if ( NULL == buf ) {
//		while(1);
//	}
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
	u8 *p = buf;
	u8 one_mark = RC5_T1/IR_BASE_TIME;
	u8 one_space = RC5_T1/IR_BASE_TIME;
	int i = 0;
    u8 data;
    u8 lastBit;

	if (!buf) {
		while(1);
	}

    /* Start Bit */
    *p++ = RC5_T1/IR_BASE_TIME;  //mark

    // Bit 2
    *p++ = RC5_T1/IR_BASE_TIME;  //space
    *p++ = (RC5_T1*2)/IR_BASE_TIME; //mark
    *p++ = RC5_T1 / IR_BASE_TIME; //space

    lastBit = 0;

    /* Address */
    data = addr;
    for (i = 0; i < 5; i++) {
        if (data & 0x10) {
            if (lastBit) {
                *p++ = one_mark;
                *p++ = one_space;
            } else {
                *(p-1) += one_mark;
                *p++ = one_space;
            }
            lastBit = 1;
        } else {
        	if (lastBit) {
                *(p-1) += one_mark;
                *p++ = one_space;
            } else {
                *p++ = one_mark;
                *p++ = one_space;
            }
            lastBit = 0;
        }
        data <<= 1;

    }

    /* Command */
    data = cmd;
    for (i = 0; i < 6; i++) {
        if (data & 0x20) {
            if (lastBit) {
                *p++ = one_mark;
                *p++ = one_space;
            } else {
                *(p-1) += one_mark;
                *p++ = one_space;
            }
            lastBit = 1;
        } else {
        	if (lastBit) {
                *(p-1) += one_mark;
                *p++ = one_space;
            } else {
                *p++ = one_mark;
                *p++ = one_space;
            }
            lastBit = 0;
        }
        data <<= 1;
    }

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

//#if (__DEBUG_BUFM__)
//    if ( ERR_NONE != bufm_free(buf) ) {
//		while(1);
//    }
//#else
//    bufm_free(buf);
//#endif
	#if (__DEBUG_BUFM__)
		if ( ERR_NONE != ev_buf_free(buf) ) {
			while(1);
		}
	#else
		ev_buf_free(buf);
	#endif
	buf = NULL;
}


void ir_send_jvc(u8 addr, u8 cmd, u8 fRepeat)
{
//    spwm_set(SPWM_1, 38);
	ir_set(38000, 2);
//    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
//	if ( NULL == buf ) {
//		while(1);
//	}
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = JVC_BIT_MARK/IR_BASE_TIME;
    u8 one_space = JVC_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = JVC_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = JVC_ZERO_SPACE/IR_BASE_TIME;

    if (!buf) {
        while(1);
    }

    if (!fRepeat) {
        /* Lead code */
        *p++ = JVC_HDR_MARK/IR_BASE_TIME;
        *p++ = JVC_HDR_SPACE/IR_BASE_TIME;
    }

    ir_convert_byte(addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;


    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

//#if (__DEBUG_BUFM__)
//    if ( ERR_NONE != bufm_free(buf) ) {
//		while(1);
//    }
//#else
//    bufm_free(buf);
//#endif
	#if (__DEBUG_BUFM__)
		if ( ERR_NONE != ev_buf_free(buf) ) {
			while(1);
		}
	#else
		ev_buf_free(buf);
	#endif
}


void ir_send_mitsubishi(u8 addr, u8 cmd, u8 fRepeat)
{
//    spwm_set(SPWM_1, 38);
	ir_set(38000, 2);
//    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
//	if ( NULL == buf ) {
//		while(1);
//	}
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = MITSUBISHI_BIT_MARK/IR_BASE_TIME;
    u8 one_space = MITSUBISHI_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = MITSUBISHI_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = MITSUBISHI_ZERO_SPACE/IR_BASE_TIME;
    u8 i;

    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = MITSUBISHI_HDR_MARK/IR_BASE_TIME;
    *p++ = MITSUBISHI_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    *p++ = MITSUBISHI_BIT_MARK/IR_BASE_TIME;
    *p++ = MITUSBISHI_GAP/IR_BASE_TIME;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

    /* Stop bit */
    *p++ = one_mark;


    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

    //sleep_us(MITUSBISHI_STOP_TIME);
    //for(i = 0; i < MITUSBISHI_MIN_REPEAT; i++) {
    //    ir_send_raw(buf, p-buf, 0);
    //    sleep_us(MITUSBISHI_STOP_TIME);
    //}

//#if (__DEBUG_BUFM__)
//    if ( ERR_NONE != bufm_free(buf) ) {
//		while(1);
//    }
//#else
//    bufm_free(buf);
//#endif
	#if (__DEBUG_BUFM__)
		if ( ERR_NONE != ev_buf_free(buf) ) {
			while(1);
		}
	#else
		ev_buf_free(buf);
	#endif
}



void ir_send_tc9012(u8 addr, u8 cmd, u8 fRepeat)
{
//    spwm_set(SPWM_1, 38);
	ir_set(38000, 2);
//	u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
//	if ( NULL == buf ) {
//		while(1);
//	}
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = TC9012_BIT_MARK/IR_BASE_TIME;
    u8 one_space = TC9012_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = TC9012_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = TC9012_ZERO_SPACE/IR_BASE_TIME;

    if (!buf) {
        while(1);
    }


	/* Lead code */
    *p++ = TC9012_HDR_MARK/IR_BASE_TIME;
    *p++ = TC9012_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
	ir_convert_byte(~cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

	/* Stop bit */
    *p++ = one_mark;

	/* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

//#if (__DEBUG_BUFM__)
//    if ( ERR_NONE != bufm_free(buf) ) {
//		while(1);
//    }
//#else
//    bufm_free(buf);
//#endif
	#if (__DEBUG_BUFM__)
		if ( ERR_NONE != ev_buf_free(buf) ) {
			while(1);
		}
	#else
		ev_buf_free(buf);
	#endif
}


void ir_send_sharp(u8 addr, u8 cmd, u8 fRepeat)
{
//    spwm_set(SPWM_1, 38);
	ir_set(38000, 2);
//	u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
//	if ( NULL == buf ) {
//		while(1);
//	}
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = SHARP_BIT_MARK/IR_BASE_TIME;
    u8 one_space = SHARP_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = SHARP_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = SHARP_ZERO_SPACE/IR_BASE_TIME;
    u8 specialIdx;
    u8 specialBase;

    if (!buf) {
        while(1);
    }

	ir_convert_byte(addr, 5, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 10;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

    /* Expanse and Check bits */
	*p++ = one_mark;
	*p++ = one_space;
	*p++ = zero_mark;
	*p++ = zero_space;

	/* Stop bit */
    *p++ = one_mark;

	/* Delay for 45ms */
    specialBase = 200;
    specialIdx = p - buf;
    *p++ = 45000/specialBase;

	ir_convert_byte(addr, 5, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 10;
    ir_convert_byte(~cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

	*p++ = zero_mark;
	*p++ = zero_space;
    *p++ = one_mark;
	*p++ = one_space;

    /* Stop bit */
    *p++ = one_mark;

	/* Send data out */
    ir_send_raw(buf, p-buf, specialIdx, specialBase);

//#if (__DEBUG_BUFM__)
//    if ( ERR_NONE != bufm_free(buf) ) {
//		while(1);
//    }
//#else
//    bufm_free(buf);
//#endif
	#if (__DEBUG_BUFM__)
		if ( ERR_NONE != ev_buf_free(buf) ) {
			while(1);
		}
	#else
		ev_buf_free(buf);
	#endif
}


void ir_send_m50119p(u8 addr, u8 cmd, u8 fRepeat)
{
//    spwm_set(SPWM_1, 38);
	ir_set(38000, 2);
//    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
//	if ( NULL == buf ) {
//		while(1);
//	}
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = M50119P_BIT_MARK/IR_BASE_TIME;
    u8 one_space = M50119P_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = M50119P_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = M50119P_ZERO_SPACE/IR_BASE_TIME;

    if (!buf) {
        while(1);
    }

    ir_convert_byte(addr, 3, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 6;
    ir_convert_byte(cmd, 7, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 14;

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

//#if (__DEBUG_BUFM__)
//    if ( ERR_NONE != bufm_free(buf) ) {
//		while(1);
//    }
//#else
//    bufm_free(buf);
//#endif
#if (__DEBUG_BUFM__)
	if ( ERR_NONE != ev_buf_free(buf) ) {
		while(1);
	}
#else
	ev_buf_free(buf);
#endif
}
#endif

void ir_send_lc7461(u16 addr, u16 cmd, u8 fRepeat)
{
	ir_set(38000, 2);
	u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}

    u8 *p = buf;
    u8 one_mark = LC7461_BIT_MARK/IR_BASE_TIME;
    u8 one_space = LC7461_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = LC7461_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = LC7461_ZERO_SPACE/IR_BASE_TIME;
    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = LC7461_HDR_MARK/IR_BASE_TIME;
    *p++ = LC7461_HDR_SPACE/IR_BASE_TIME;

    if (!fRepeat) {
        ir_convert_byte(addr, 13, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
        p += 26;
        ir_convert_byte(~addr, 13, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
        p += 26;
        ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
        p += 16;
        ir_convert_byte(~cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
        p += 16;
    }

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

#if (__DEBUG_BUFM__)
    if ( ERR_NONE != ev_buf_free(buf) ) {
		while(1);
    }
#else
    ev_buf_free(buf);
#endif
}

#if 0
void ir_send_mitc8d8(u8 addr, u8 cmd, u8 fRepeat)
{
    spwm_set(SPWM_1, 38);
    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = MITC8D8_BIT_MARK/IR_BASE_TIME;
    u8 one_space = MITC8D8_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = MITC8D8_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = MITC8D8_ZERO_SPACE/IR_BASE_TIME;
    if (!buf) {
        while(1);
    }

    ir_convert_byte(addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

     /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

#if (__DEBUG_BUFM__)
    if ( ERR_NONE != bufm_free(buf) ) {
		while(1);
    }
#else
    bufm_free(buf);
#endif
}

#endif

void ir_send_mn6014_c6d6(u8 addr, u8 cmd, u8 fRepeat)
{
    //spwm_set(SPWM_1, 38);
    ir_set(38000, 2);
    u8 *buf = ev_buf_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = MN6014_C6D6_BIT_MARK/IR_BASE_TIME;
    u8 one_space = MN6014_C6D6_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = MN6014_C6D6_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = MN6014_C6D6_ZERO_SPACE/IR_BASE_TIME;
    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = MN6014_C6D6_HDR_MARK/IR_BASE_TIME;
    *p++ = MN6014_C6D6_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(addr, 6, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 12;
    ir_convert_byte(cmd, 6, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 12;
    ir_convert_byte(~addr, 6, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 12;
    ir_convert_byte(~cmd, 6, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 12;

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

#if (__DEBUG_BUFM__)
    if ( ERR_NONE != ev_buf_free(buf) ) {
		while(1);
    }
#else
    ev_buf_free(buf);
#endif
}

#if 1
void ir_send_mn6014_c5d6(u8 addr, u8 cmd, u8 fRepeat)
{
    //spwm_set(SPWM_1, 56,1,2);
	//ir_set(56000, 2);
	ir_set(56000, 2);

	u8 *buf = ev_buf_allocate(LARGE_BUFFER);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = MN6014_C6D6_BIT_MARK/IR_BASE_TIME;
    u8 one_space = MN6014_C6D6_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = MN6014_C6D6_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = MN6014_C6D6_ZERO_SPACE/IR_BASE_TIME;
    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = MN6014_C6D6_HDR_MARK/IR_BASE_TIME;
    *p++ = MN6014_C6D6_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(addr, 5, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 10;
    ir_convert_byte(cmd, 6, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 12;
    ir_convert_byte(~addr, 5, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 10;
    ir_convert_byte(~cmd, 6, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 12;

    /* Stop bit */
    *p++ = one_mark;





    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

#if (__DEBUG_BUFM__)
    if ( 0 != ev_buf_free(buf) ) {
		while(1);
    }
#else
    ev_buf_free(buf);
#endif
}
#endif

#if 0
void ir_send_gemini(u16 addr, u16 cmd, u8 fRepeat)
{
    spwm_set(SPWM_1, 31);
    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
	u8 *p = buf;
	u8 one_mark = GEMINI_BIT_MARK/IR_BASE_TIME;
	u8 one_space = GEMINI_BIT_MARK/IR_BASE_TIME;
	u8 zero_mark = GEMINI_BIT_MARK/IR_BASE_TIME;
	u8 zero_space = GEMINI_BIT_MARK/IR_BASE_TIME;
	int i = 0;
    u16 data;
    u8 lastBit;
    u8 specialBase, specialIdx;

	if (!buf) {
		while(1);
	}

    /* Lead code Bit */
    *p++ = GEMINI_HDR_MARK/IR_BASE_TIME;  //mark
    *p++ = GEMINI_HDR_SPACE/IR_BASE_TIME;  //space

    lastBit = 1;

    /* Address */
    data = addr;
    for (i = 0; i < 10; i++) {
        if (data & 0x0001) {
            if (lastBit) {
                *p++ = one_mark;
                *p++ = one_space;
            } else {
                *(p-1) += one_mark;
                *p++ = one_space;
            }
            lastBit = 1;
        } else {
        	if (lastBit) {
                *(p-1) += one_mark;
                *p++ = one_space;
            } else {
                *p++ = one_mark;
                *p++ = one_space;
            }
            lastBit = 0;
        }
        data >>= 1;

    }

    /* Gap 21ms */
    specialBase = 200;
    if (lastBit == 1) {
        *(p-1) = (GEMINI_GAP + GEMINI_BIT_MARK)/specialBase;
        specialIdx = p - 1 - buf;
    } else {
        specialIdx = p - buf;
        *p++ = GEMINI_GAP/specialBase;

    }

    /* Lead code Bit */
    *p++ = GEMINI_HDR_MARK/IR_BASE_TIME;  //mark
    *p++ = GEMINI_HDR_SPACE/IR_BASE_TIME;  //space

    *p++ = one_mark;
    *p++ = one_space;

    lastBit = 1;


    /* Command */
    data = cmd;
    for (i = 0; i < 9; i++) {
        if (data & 0x0001) {
            if (lastBit) {
                *p++ = one_mark;
                *p++ = one_space;
            } else {
                *(p-1) += one_mark;
                *p++ = one_space;
            }
            lastBit = 1;
        } else {
        	if (lastBit) {
                *(p-1) += one_mark;
                *p++ = one_space;
            } else {
                *p++ = one_mark;
                *p++ = one_space;
            }
            lastBit = 0;
        }
        data >>= 1;
    }



    /* Send data out */
    ir_send_raw(buf, p-buf, specialIdx, specialBase);

#if (__DEBUG_BUFM__)
    if ( ERR_NONE != bufm_free(buf) ) {
		while(1);
    }
#else
    bufm_free(buf);
#endif
}


void ir_send_victor(u8 addr, u8 cmd, u8 fRepeat)
{
    spwm_set(SPWM_1, 38);
    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = VICTOR_BIT_MARK/IR_BASE_TIME;
    u8 one_space = VICTOR_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = VICTOR_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = VICTOR_ZERO_SPACE/IR_BASE_TIME;
    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = VICTOR_HDR_MARK/IR_BASE_TIME;
    *p++ = VICTOR_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;


    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

#if (__DEBUG_BUFM__)
    if ( ERR_NONE != bufm_free(buf) ) {
		while(1);
    }
#else
    bufm_free(buf);
#endif
	buf = NULL;
}


void ir_send_fb739A(u8 addr, u8 cmd, u8 fRepeat)
{
    spwm_set(SPWM_1, 35);
}

void ir_send_nec2e2(u8 addr, u8 cmd, u8 fRepeat)
{
	spwm_set(SPWM_1, 43);

    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = NEC2E2_BIT_MARK/IR_BASE_TIME;
    u8 one_space = NEC2E2_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = NEC2E2_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = NEC2E2_ZERO_SPACE/IR_BASE_TIME;

    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = NEC2E2_HDR_MARK/IR_BASE_TIME;
    *p++ = NEC2E2_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(~addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(~cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);
#if (__DEBUG_BUFM__)
    if ( ERR_NONE != bufm_free(buf) ) {
		while(1);
    }
#else
    bufm_free(buf);
#endif
	buf = NULL;

}

void ir_send_lc7464m(u16 addr, u8 cmd, u8 fRepeat)
{
    u8 data;
	spwm_set(SPWM_1, 38);

    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = LC7464M_BIT_MARK/IR_BASE_TIME;
    u8 one_space = LC7464M_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = LC7464M_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = LC7464M_ZERO_SPACE/IR_BASE_TIME;

    if (!buf) {
        while(1);
    }

    /* Lead code */
    *p++ = LC7464M_HDR_MARK/IR_BASE_TIME;
    *p++ = LC7464M_HDR_SPACE/IR_BASE_TIME;

    ir_convert_byte(addr, 16, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 32;
    data = 0x80;
    ir_convert_byte(data, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    data = 0x00;
    ir_convert_byte(data, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(cmd|0x80, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);
#if (__DEBUG_BUFM__)
    if ( ERR_NONE != bufm_free(buf) ) {
		while(1);
    }
#else
    bufm_free(buf);
#endif
	buf = NULL;

}


void ir_send_m50462(u8 addr, u8 cmd, u8 fRepeat)
{
    u8 data;
	spwm_set(SPWM_1, 38);

    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
	if ( NULL == buf ) {
		while(1);
	}
    u8 *p = buf;
    u8 one_mark = M50462_BIT_MARK/IR_BASE_TIME;
    u8 one_space = M50462_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = M50462_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = M50462_ZERO_SPACE/IR_BASE_TIME;

    if (!buf) {
        while(1);
    }

    /* Lead code */
    ir_convert_byte(addr, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;
    ir_convert_byte(cmd, 8, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 16;

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);
    bufm_free(buf);


}


void ir_send_m50119l(u8 addr, u8 cmd, u8 fRepeat)
{
    spwm_set(SPWM_1, 38);
    u8 *buf = bufm_allocate(DFLT_LARGE_BUF_SIZE);
    u8 *p = buf;
    u8 one_mark = M50119L_BIT_MARK/IR_BASE_TIME;
    u8 one_space = M50119L_ONE_SPACE/IR_BASE_TIME;
    u8 zero_mark = M50119L_BIT_MARK/IR_BASE_TIME;
    u8 zero_space = M50119L_ZERO_SPACE/IR_BASE_TIME;

    if (!buf) {
        while(1);
    }

    ir_convert_byte(addr, 3, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 6;
    ir_convert_byte(cmd, 7, p, one_mark, one_space, 1, zero_mark, zero_space, 1);
    p += 14;

    /* Stop bit */
    *p++ = one_mark;

    /* Send data out */
    ir_send_raw(buf, p-buf, NO_SPECIAL, 0);

    bufm_free(buf);
}


void ir_send_philips(u8 addr, u8 cmd, u8 fRepeat)
{
    spwm_set(SPWM_1, 38);
}





u32 ir_get_repeat_interval(u8 type)
{
	switch (type) {
		case IR_TYPE_NEC:
			return NEC_RPT_INTERVAL;

        case IR_TYPE_SONY:
            return SONY_RPT_LENGTH;

		case IR_TYPE_RC5:
			return RC5_RPT_LENGTH;

		case IR_TYPE_JVC:
			return JVC_RPT_LENGTH;

		case IR_TYPE_MITSUBISHI:
			return MITUSBISHI_RPT_LENGTH;

		case IR_TYPE_TC9012:
            return TC9012_RPT_LENGTH;

		case IR_TYPE_SHARP:
			return SHART_RPT_LENGTH;

		case IR_TYPE_M50119P:
			return M50119P_RPT_LENGTH;

		case IR_TYPE_LC7461:
            return LC7461_RPT_LENGTH;

		case IR_TYPE_MITC8D8:
			return MITC8D8_RPT_LENGTH;

		case IR_TYEP_MN6014_C6D6:
			return MN6014_C6D6_RPT_LENGTH;

		case IR_TYPE_GEMINI:
			break;

		case IR_TYPE_VICTOR:
			return VICTOR_RPT_LENGTH;

        case IR_TYPE_LC7464M:
            return LC7464M_RPT_LENGTH;

        case IR_TYPE_NECE2:
            return NEC2E2_RPT_LENGTH;

        case IR_TYPE_M50642:
            return M50462_RPT_LENGTH;

		default:
			return 0;
	}

    return 0;
}
#endif

