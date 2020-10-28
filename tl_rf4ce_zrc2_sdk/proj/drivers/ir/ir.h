/********************************************************************************************************
 * @file     ir.h
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

#if IR_DMA_FIFO_EN
#define IR_DMA_SERIES_CNT1		247//(125*2 )
typedef struct{
	u32 series_freq;
	u32 series_tick;
	u32 series_cnt;
	u16 series_tm[IR_DMA_SERIES_CNT1]; // the same as length of ir learn
}ir_dma_serial_t;

u8 Get_CarrierCycleTick(u32 freq);
#endif

#define DFLT_LARGE_BUF_SIZE 160


#if(IR_PROTOCOL == IR_PROTOCOL_NEC)
#define IR_HIGH_CARR_TIME			565			// in us
#define IR_HIGH_NO_CARR_TIME		1685
#define IR_LOW_CARR_TIME			560
#define IR_LOW_NO_CARR_TIME			565
#define IR_INTRO_CARR_TIME			9000
#define IR_INTRO_NO_CARR_TIME		4500

#define IR_SWITCH_CODE              0x0d
#define IR_ADDR_CODE                0x00
#define IR_CMD_CODE                 0xbf

#define IR_REPEAT_INTERVAL_TIME     40500
#define IR_REPEAT_NO_CARR_TIME      2250
#define IR_END_TRANS_TIME			563

#define IR_CARRIER_FREQ				38222
#define IR_CARRIER_DUTY				3
#else

#endif


#if 0
static inline void ir_init(void){
#if(IR_USE_PWM)
#if(IR_PROTOCOL == IR_PROTOCOL_NEC)
    pwm_set(IR_PWM_ID, IR_CARRIER_FREQ, -1, IR_CARRIER_DUTY);
#else
#endif
#else
#endif
}
#endif

void ir_init(u8 channel);

void ir_send_switch(u8 addr, u8 cmd);


#ifndef GPIO_IR_LEARN_IN
#define GPIO_IR_LEARN_IN			GPIO_GP26
#endif


void ir_send_nec(u16 addr, u16 cmd, u8 fRepeat);
void ir_send_upd6121g(u16 addr, u16 cmd, u8 fRepeat);
void ir_send_upd6121f(u16 addr, u16 cmd, u8 fRepeat);
void ir_send_sony(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_jvc(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_mitsubishi(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_tc9012(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_sharp(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_rc5(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_m50119p(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_lc7461(u16 addr, u16 cmd, u8 fRepeat);
void ir_send_mitc8d8(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_mn6014_c6d6(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_mn6014_c5d6(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_gemini(u16 addr, u16 cmd, u8 fRepeat);
void ir_send_victor(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_fb739A(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_nec2e2(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_lc7464m(u16 addr, u8 cmd, u8 fRepeat);
void ir_send_m50462(u8 addr, u8 cmd, u8 fRepeat);
void ir_send_philips(u8 addr, u8 cmd, u8 fRepeat);
u8 ir_isBusy(void);

u32  ir_get_repeat_interval(u8 type);

extern void MARK(u32 t_us);
extern void SPACE(u32 t_us);
void ir_send_serial(u16 *serial, u16 rawLen);
void ir_set(int hz, int low_duty);

extern void HalIrGenPachemFormat();
//extern void ir_send_raw_pachem(u16 *raw, u32 rawLen);
extern void ir_send_raw_pachem(u16 *raw, u32 rawLen, u16	*raw2, u32 rawLen2);

typedef void (*zrc_ir_dma_callback_t)(void);
void zrcDMAIrcallback(zrc_ir_dma_callback_t ir_cb);
void ir_dma_send_serial(u16 *serial, u16 rawLen);
void rc_ir_irq_prc(void);
//extern u16 *BitStrem;
//extern u16	BitRawData[128];
//extern u8 t_IrLength;




