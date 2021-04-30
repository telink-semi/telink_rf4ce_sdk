/********************************************************************************************************
 * @file     ir_learning.h
 *
 * @brief	 for ir learning
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
#pragma once

#define IR_STORED_SERIES_ADDRESS 	(0xa000)
#define IR_LEARN_SERIES_CNT     	247
typedef void (*learnSetStateCb)(u8 sta);

enum{
	IR_LEARN_IDLE,
	IR_LEARN_READY,
	IR_LEARN_DOING,
	IR_LEARN_DONE
};

enum{
	IR_LEARN_START=1,
	IR_LEARN_SUCCESS,
	IR_LEARN_FAILED,
};


typedef struct{
	u32 carr_high_tm;
	u16 series_cnt;
	u8 ir_flag;
	u8 ir_int_cnt;
	u8 ir_protocol;	
	u8 toshiba_c0flag;
	u8 carr_high_cnt;
	u8 carr_low_cnt;
	u16 carr_high_temp[3];
	u16 series_tm[IR_LEARN_SERIES_CNT]; // 时间精度 us， 所以最长的 pulse  是65 ms
}ir_learn_pattern_t;

typedef struct{
	u8 is_carr;
	u8 ir_protocol;
	u8 toshiba_c0flag;
	u8 learn_timer_started;
	int carr_first;
	u32 carr_switch_start_tm;
	u16 carr_check_cnt;
	u16 series_cnt;
	//u32 series_tm[IR_LEARN_SERIES_CNT];
	u32 time_interval;
	u32 last_trigger_tm;
	u32 curr_trigger_tm;
	u32 carr_high_tm;
	u32	carr_low_tm;
	int ir_int_cnt;
	u32 record_last_time;
}ir_learn_ctrl_t;


typedef struct{
	u32 carr_high_tm;
	u16 series_cnt;
	u16 series_tm[IR_LEARN_SERIES_CNT]; // 时间精度 us， 所以最长的 pulse  是65 ms
}ir_learn_save_t;

void ir_start_learn(u8 keyCode);
void  ir_stop_learn(void);
void ir_learn_send(void);
u8 ir_learn_check(void);
u8 ir_learn_send_nv(u8 key);
void setlearningState(learnSetStateCb cb);
nv_sts_t nv_ir_write(u8 keycode,u32 freq,  u16 *buf, u16 len);
nv_sts_t nv_ir_read(u8 keyCode, u32 *freq, u16 *buf, u16 *len);
int ir_learn_failed_callback(void *arg);
int ir_learn_success_callback(void *arg);
