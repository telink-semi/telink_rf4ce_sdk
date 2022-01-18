/********************************************************************************************************
 * @file    ir_learn.h
 *
 * @brief   This is the header file for ir_learn.h
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
	u16 series_tm[IR_LEARN_SERIES_CNT]; // ʱ侫 us  pulse  65 ms
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
	u16 series_tm[IR_LEARN_SERIES_CNT]; // ʱ侫 us  pulse  65 ms
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
