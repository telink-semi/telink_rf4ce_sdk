/********************************************************************************************************
 * @file     ir_learning.c
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

#include "../../tl_common.h"
#include "ir.h"
#include "../drv_flash.h"
#include "../nv.h"
#include "ir_learn.h"

#if (MODULE_IR_LEARN_ENABLE)

#define CLOCK_TICK_TO_US(t)				((t)/CLOCK_SYS_CLOCK_1US)

#define IR_FLASH_PAGE_SIZE 				0x100 //256bytes
#define IR_CARR_CHECK_CNT				10
#define IR_LEARN_START_MINLEN			(200)

#define IR_LEARN_NONE_CARR_MIN			(200)//old is 80
#define IR_LEARN_CARR_GLITCH_MIN		(3)
#define IR_LEARN_CARR_MIN				(7)

#define NEC_LEAD_CARR_MIN_INTERVAL			(8700)
#define NEC_LEAD_CARR_MAX_INTERVAL			(9300)
#define NEC_LEAD_NOCARR_MIN_INTERVAL		(4200)
#define NEC_LEAD_NOCARR_MAX_INTERVAL		(4800)
#define TOSHIBA_LEAD_MIN_INTERVAL			(4200)
#define TOSHIBA_LEAD_MAX_INTERVAL			(4800)
#define FRAXEL_LEAD_CARR_MIN_INTERVAL		(2100)
#define FRAXEL_LEAD_CARR_MAX_INTERVAL		(2700)
#define FRAXEL_LEAD_NOCARR_MIN_INTERVAL		(900)
#define FRAXEL_LEAD_NOCARR_MAX_INTERVAL		(1500)
#define IR_NEC_TYPE							1
#define IR_TOSHIBA_TYPE 					2
#define IR_FRAXEL_TYPE						3
#define IR_HIGH_LOW_MIN_INTERVAL			(1000)
#define IR_HIGH_LOW_MAX_INTERVAL			(2000)
#define TC9012_FRAME_CYCLE					(108*1000)
#define FRAXEL_LEVEL_NUM 					19 
#define NEC_TOSHIBA_LEVEL_NUM 				67

static ir_learn_ctrl_t ir_learn_ctrl;
static ir_learn_pattern_t ir_learn_pattern;

learnSetStateCb learnStateCb = NULL;
ir_learn_save_t ir_learn_dat;
void setlearningState(learnSetStateCb cb)
{
	learnStateCb = cb;
}


static int ir_write_universal_data(u8 flash_index){
	u8 ir_start_cnt = 0;
	u8 ir_lead_search_cnt = ir_learn_pattern.series_cnt < 50 ? ir_learn_pattern.series_cnt : 50;
	foreach(i,ir_lead_search_cnt){			
		if((ir_learn_pattern.series_tm[i] > NEC_LEAD_CARR_MIN_INTERVAL)&&
			(ir_learn_pattern.series_tm[i] < NEC_LEAD_CARR_MAX_INTERVAL)&&
			(ir_learn_pattern.series_tm[i+1] > NEC_LEAD_NOCARR_MIN_INTERVAL)&&
			(ir_learn_pattern.series_tm[i+1] < NEC_LEAD_NOCARR_MAX_INTERVAL)){
			ir_learn_pattern.ir_protocol = IR_NEC_TYPE;	
			ir_learn_pattern.series_tm[i]=(9000);
			ir_learn_pattern.series_tm[i+1]=(4500);
			ir_start_cnt = i+2;
			break;
		}else if((ir_learn_pattern.series_tm[i] > TOSHIBA_LEAD_MIN_INTERVAL)&&
			(ir_learn_pattern.series_tm[i] < TOSHIBA_LEAD_MAX_INTERVAL)&&
			(ir_learn_pattern.series_tm[i+1] > TOSHIBA_LEAD_MIN_INTERVAL)&&
			(ir_learn_pattern.series_tm[i+1] < TOSHIBA_LEAD_MAX_INTERVAL)){
			ir_learn_pattern.ir_protocol = IR_TOSHIBA_TYPE;
			ir_learn_pattern.series_tm[i]=(4500);
			ir_learn_pattern.series_tm[i+1]=(4500);
			ir_start_cnt = i+2;
			break;
		}else if((ir_learn_pattern.series_tm[i] > FRAXEL_LEAD_CARR_MIN_INTERVAL)&&
			(ir_learn_pattern.series_tm[i] < FRAXEL_LEAD_CARR_MAX_INTERVAL)&&
			(ir_learn_pattern.series_tm[i+1] > FRAXEL_LEAD_NOCARR_MIN_INTERVAL)&&
			(ir_learn_pattern.series_tm[i+1] < FRAXEL_LEAD_NOCARR_MAX_INTERVAL)){
			ir_learn_pattern.ir_protocol = IR_FRAXEL_TYPE;	
			ir_learn_pattern.series_tm[i]=(2400);
			ir_learn_pattern.series_tm[i+1]=(1200);
			ir_start_cnt = i+2;
			break;
		}else{//do nothing
		}
	}
		
	if(ir_learn_pattern.ir_protocol == IR_TOSHIBA_TYPE){
		ir_learn_pattern.series_cnt = NEC_TOSHIBA_LEVEL_NUM;
		if(ir_learn_pattern.series_tm[ir_start_cnt+1] > (ir_learn_pattern.series_tm[ir_start_cnt]<<1))
			ir_learn_pattern.toshiba_c0flag = 1;
		else
			ir_learn_pattern.toshiba_c0flag = 0;			
	}
	
	if(ir_learn_pattern.ir_protocol == IR_FRAXEL_TYPE)
		ir_learn_pattern.series_cnt = FRAXEL_LEVEL_NUM;
	if(ir_learn_pattern.ir_protocol == IR_NEC_TYPE)
		ir_learn_pattern.series_cnt = NEC_TOSHIBA_LEVEL_NUM;	
	if(ir_learn_pattern.ir_protocol != 0){
		for(int i = ir_start_cnt ;i < ir_learn_pattern.series_cnt;i++){
			if(ir_learn_pattern.series_tm[i] < IR_HIGH_LOW_MIN_INTERVAL){
				ir_learn_pattern.series_tm[i]=(560);
			}
			else if(ir_learn_pattern.series_tm[i] < IR_HIGH_LOW_MAX_INTERVAL){
				if(ir_learn_pattern.ir_protocol == IR_NEC_TYPE)
					ir_learn_pattern.series_tm[i]= 1680;
				else
					ir_learn_pattern.series_tm[i]= 1690;
			}else{
			}		
		}
	}

	return 0;	
}


u8 kb_keyindex_pressed;
static u8 ir_learn_ready = 0;
static u8 ir_learning_flag;
static u32 key_irlearn_timeoutTick;
int ir_record_end(void *data){
	
	if( ir_learn_pattern.series_cnt < IR_LEARN_SERIES_CNT ){
		++ir_learn_pattern.series_cnt;//plus the last carrier.
	}		
	gpio_clr_interrupt(GPIO_IR_LEARN_IN);	
	ir_write_universal_data(kb_keyindex_pressed);
	ir_learn_ready = 0;
	return -1;
}

u16 calculate_mid (u16 a,u16 b,u16 c)
{
	u16 temp=0;
	if(a>b){temp=b;b=a;a=temp;}
	if(c<b)
	{
		if(c<a){return a;}
		else{ return c;}
	}
	return b;
}

_attribute_ram_code_ void ir_record(u32 tm, u32 pol)
{
	ir_learn_ctrl.curr_trigger_tm = tm;
	if( ir_learn_pattern.series_cnt >= IR_LEARN_SERIES_CNT - 2 ){
		return;
	}
    if(ir_learn_pattern.ir_int_cnt!=0 ){
    	ir_learn_ctrl.time_interval = ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.last_trigger_tm;
    	if(ir_learn_ctrl.time_interval < 71*CLOCK_SYS_CLOCK_1US)    //记载载波，中断处理时间太长。无法记录载波
		{
			ir_learn_pattern.carr_high_temp[ir_learn_pattern.carr_high_cnt]=CLOCK_TICK_TO_US(ir_learn_ctrl.time_interval);
			ir_learn_pattern.carr_high_cnt++;
			if(ir_learn_pattern.carr_high_cnt>1)    //采样3次，取当中值
				{
				ir_learn_pattern.carr_high_cnt=0;
				ir_learn_pattern.carr_high_tm= calculate_mid(ir_learn_pattern.carr_high_temp[0],ir_learn_pattern.carr_high_temp[1],ir_learn_pattern.carr_high_tm);
				}
		}
    	else if((ir_learn_ctrl.time_interval > 70*CLOCK_SYS_CLOCK_1US)&&(ir_learn_ctrl.time_interval < 200001*CLOCK_SYS_CLOCK_1US))
		{
    		if(CLOCK_TICK_TO_US(  ir_learn_ctrl.last_trigger_tm - ir_learn_ctrl.record_last_time ))
    		{
			ir_learn_pattern.series_tm[ir_learn_pattern.series_cnt]= CLOCK_TICK_TO_US(  ir_learn_ctrl.last_trigger_tm - ir_learn_ctrl.record_last_time );
			ir_learn_pattern.series_cnt++;
    		}
    		if(CLOCK_TICK_TO_US(ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.last_trigger_tm))
    		{
			ir_learn_pattern.series_tm[ir_learn_pattern.series_cnt]= CLOCK_TICK_TO_US(ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.last_trigger_tm);
			ir_learn_pattern.series_cnt++;
    		}
			ir_learn_ctrl.record_last_time=ir_learn_ctrl.curr_trigger_tm;
		}
        else if(ir_learn_ctrl.time_interval > 200000*CLOCK_SYS_CLOCK_1US)
		{
    		if(CLOCK_TICK_TO_US( ir_learn_ctrl.last_trigger_tm - ir_learn_ctrl.record_last_time ))
    		{
			ir_learn_pattern.series_tm[ir_learn_pattern.series_cnt]=CLOCK_TICK_TO_US( ir_learn_ctrl.last_trigger_tm - ir_learn_ctrl.record_last_time );
			ir_learn_pattern.series_cnt++;
    		}
    		if(CLOCK_TICK_TO_US(ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.last_trigger_tm))
    		{
			ir_learn_pattern.series_tm[ir_learn_pattern.series_cnt]=CLOCK_TICK_TO_US(ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.last_trigger_tm);
    		}
			ir_learning_flag = 0;

			ir_stop_learn();
		}
	}
    else
    {
    	ir_learn_pattern.ir_int_cnt=1;
    	ir_learn_ctrl.record_last_time=ir_learn_ctrl.curr_trigger_tm;
    	ir_learning_flag=1;
    	key_irlearn_timeoutTick=clock_time();
    }
    //********************************************
    ir_learn_ctrl.last_trigger_tm = ir_learn_ctrl.curr_trigger_tm;
}

void ir_learn_intf_init(int en){
	if(en){
		gpio_set_func(GPIO_IR_OUT, AS_GPIO);
		gpio_write(GPIO_IR_OUT,		0);
		gpio_set_output_en(GPIO_IR_OUT, 1);
		gpio_write(GPIO_IR_CTRL,	0);
	}else{
		gpio_write(GPIO_IR_CTRL, 1);
		gpio_set_func(GPIO_IR_OUT, IR_PWM_ID);
	}
}


void  ir_stop_learn(void){
	gpio_clr_interrupt(GPIO_IR_LEARN_IN);

	if(CLOCK_TICK_TO_US( ir_learn_ctrl.last_trigger_tm - ir_learn_ctrl.record_last_time ))
	{
    ir_learn_pattern.series_tm[ir_learn_pattern.series_cnt]=CLOCK_TICK_TO_US( ir_learn_ctrl.last_trigger_tm - ir_learn_ctrl.record_last_time );
	}
    else
	{
		if(ir_learn_pattern.series_cnt)
			ir_learn_pattern.series_cnt--;
	}

    if(ir_learn_pattern.series_tm[0] < 200)     //包头小于200us 舍弃
    {
    	memset4(&ir_learn_pattern, 0, sizeof(ir_learn_pattern));
        if(learnStateCb)
        	learnStateCb(IR_LEARN_FAILED);
    }else{
    	nv_ir_write(kb_keyindex_pressed, ir_learn_pattern.carr_high_tm, ir_learn_pattern.series_tm, ir_learn_pattern.series_cnt+1);
        if(learnStateCb)
        	learnStateCb(IR_LEARN_SUCCESS);
    }
    ir_learn_ready = 0;
    ir_learning_flag = 0;

    ir_learn_intf_init(0);
	reg_gpio_wakeup_irq &= ~FLD_GPIO_CORE_INTERRUPT_EN;
	reg_irq_src &= ~FLD_IRQ_GPIO_EN;
	reg_irq_mask &= ~FLD_IRQ_GPIO_EN;
}

void ir_start_learn(u8 keyCode){
	ir_learn_ready = 1;
	ir_learning_flag = 0;

	kb_keyindex_pressed = keyCode;
    // Init data
    memset4(&ir_learn_ctrl, 0, sizeof(ir_learn_ctrl));
    memset4(&ir_learn_pattern, 0, sizeof(ir_learn_pattern));

    ir_learn_intf_init(1);
    if(learnStateCb)
    	learnStateCb(IR_LEARN_START);
    // Enable interrupt.
    gpio_set_interrupt(GPIO_IR_LEARN_IN, 1);
    gpio_en_interrupt(GPIO_IR_LEARN_IN, 1);

	reg_gpio_wakeup_irq |= FLD_GPIO_CORE_INTERRUPT_EN;
	reg_irq_src |= FLD_IRQ_GPIO_EN;
	reg_irq_mask |= FLD_IRQ_GPIO_EN;
}


u8 ir_learn_check(void){
	if(ir_learn_ready){
		if(ir_learning_flag){
			if(clock_time_exceed(key_irlearn_timeoutTick,400000)){
				ir_stop_learn();
				return IR_LEARN_IDLE;
			}
			return IR_LEARN_DOING;
		}
		return IR_LEARN_READY;
	}else{
		return IR_LEARN_IDLE;
	}
}

u32 ircount=0;     //kevin
int irsend_Tick=0; //kevin
u8 irflag=0;		//kevin
void ir_learn_send(void)
{
	gpio_clr_interrupt(GPIO_IR_LEARN_IN);

	gpio_write(GPIO_IR_CTRL, 1);

	ir_init(IR_PWM_ID);

	IR_PWM_PIN_CFG;

	ir_set(1000000/ir_learn_pattern.carr_high_tm, 3);//

	ir_send_serial(ir_learn_pattern.series_tm, ir_learn_pattern.series_cnt+1);

#if 0
	irflag=0;
	ircount=0;
	for(ircount=0;ircount<(ir_learn_pattern.series_cnt+1);ircount++)
	{
		irflag=~irflag;
		if(irflag){MARK(0);}
		else {SAPCE(0);}

		irsend_Tick=clock_time();
		while(!clock_time_exceed(irsend_Tick,ir_learn_pattern.series_tm[ircount]));
	}
	SAPCE(0);
#endif
}

volatile u32 T_Debug_gpio_irq[8] = {0};
u8 ir_learn_send_nv(u8 key)
{
	u32 irFreq = 0;
	u8 ret = SUCCESS;

	memset4(&ir_learn_pattern, 0, sizeof(ir_learn_pattern));

	if(NV_SUCC != nv_ir_read(key, &irFreq, (u16 *)ir_learn_pattern.series_tm, &ir_learn_pattern.series_cnt))
		return FAILURE;

	if(irFreq==0||ir_learn_pattern.series_cnt==0||ir_learn_pattern.series_tm==NULL)
		return FAILURE;

	gpio_clr_interrupt(GPIO_IR_LEARN_IN);

	gpio_write(GPIO_IR_CTRL, 1);

	ir_init(IR_PWM_ID);

	IR_PWM_PIN_CFG;

	ir_set(1000000/irFreq, 3);//
#if IR_DMA_FIFO_EN
	Get_CarrierCycleTick(1000000/irFreq);
	ir_dma_send_serial((u16 *)ir_learn_pattern.series_tm, ir_learn_pattern.series_cnt);
#else
	ir_send_serial((u16 *)ir_learn_pattern.series_tm, ir_learn_pattern.series_cnt);
#endif
	return ret;
}



_attribute_ram_code_ void gpio_user_irq_handler(void){
	if(reg_irq_src & FLD_IRQ_GPIO_EN){
		if(ir_learn_check() != IR_LEARN_IDLE){
		    ir_record(clock_time(), 1);
		}
	}
}



nv_sts_t nv_ir_write(u8 keycode,u32 freq,  u16 *buf, u16 len)
{
	ir_learn_dat.carr_high_tm = freq;
	ir_learn_dat.series_cnt = len;
	memset(&ir_learn_dat.series_tm[0],0xff,IR_LEARN_SERIES_CNT1*2);
	memcpy(&ir_learn_dat.series_tm[0],buf,len*2);
	nv_sts_t sta = nv_flashWriteNew(1, DS_IR_LEARN_MODULE, keycode,  sizeof(ir_learn_dat), (u8 *)&ir_learn_dat);
	return sta;
}

nv_sts_t nv_ir_read(u8 keyCode, u32 *freq, u16 *buf, u16 *len)
{
	nv_sts_t sta = nv_flashReadNew(1, DS_IR_LEARN_MODULE, keyCode, sizeof(ir_learn_dat), (u8 *)&ir_learn_dat);
	if(sta==NV_SUCC&&ir_learn_dat.series_cnt)
	{
	 *freq = ir_learn_dat.carr_high_tm;
	 *len =  ir_learn_dat.series_cnt;
	 memcpy(buf,&ir_learn_dat.series_tm[0],ir_learn_dat.series_cnt*2);
	}
	return sta;
}





#endif

