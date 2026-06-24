/********************************************************************************************************
 * @file    ir_learn.c
 *
 * @brief   This is the source file for ir_learn.c
 *
 * @author  Zigbee GROUP
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

#include "../../tl_common.h"
#include "ir.h"
#include "../drv_flash.h"
#include "../nv.h"
#include "ir_learn.h"
#include "../drv_pwm.h"

#if (MODULE_IR_LEARN_ENABLE)

learnSetStateCb learnStateCb = NULL;

void setlearningState(learnSetStateCb cb)
{
    learnStateCb = cb;
}

u16 calculate_mid (u16 a,u16 b,u16 c)  //get the middle value
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

#if defined(MCU_CORE_TL321X)

#define IR_LEARN_TIMEOUT_TIME         0xea600//20ms
#define BUFF_DATA_LEN                 2000
#define IR_LEARN_PWM_CARRIEN_MAX      (71*CLOCK_PWM_CLOCK_1US)
#define IR_LEARN_PWM_NOCARRIEN_MAX    (20000 * CLOCK_PWM_CLOCK_1US)

u8 kb_keyindex_pressed;

u32 g_il_wave_receive_buff[BUFF_DATA_LEN] __attribute__((aligned(4))) = {0};
u32 g_il_index = 0;

ir_learn_save_t ir_learn_dat;
ir_learn_save_t ir_learn_data_read;

void ir_start_learn(u8 keyCode)
{
    kb_keyindex_pressed = keyCode;

    if (learnStateCb) {
        learnStateCb(IR_LEARN_START);
    }
    ir_learn_rx_t ir_learn_rx = {
        .high_data_en  = 0,
        .cycle_data_en = 1,
        .timeout_en    = 1,
        .rx_invert_en  = 1,
        .cnt_mode      = RISING_EDGE_START_CNT,
        .data_format = IR_LEARN_BIT_24_DATA,
        .rx_mode = ANALOG_RX_MODE,
    };
    ir_learn_rx_init(&ir_learn_rx);

    ir_learn_set_rx_timeout(IR_LEARN_TIMEOUT_TIME);

    ir_learn_rx_irq_trig_cnt(0x03);

    plic_interrupt_enable(IRQ_IR_LEARN);
    ir_learn_set_irq_mask(FLD_IR_LEARN_TIMEOUT_IRQ | FLD_IR_LEARN_RXFIFO_IRQ);

    ir_learn_en();
}

volatile u8 T_writeDataCnt = 0;
int ir_learn_success_callback(void *arg)
{
    T_writeDataCnt++;
    nv_sts_t sta = nv_flashWriteNew(1, DS_IR_LEARN_MODULE, kb_keyindex_pressed,  sizeof(ir_learn_dat), (u8 *)&ir_learn_dat);

    if (sta == NV_SUCC) {
        if (learnStateCb) {
            learnStateCb(IR_LEARN_SUCCESS);
        }
    }

    return -1;
}


int ir_learn_failed_callback(void *arg)
{
    if (learnStateCb) {
        learnStateCb(IR_LEARN_FAILED);
    }

    return -1;
}

void ir_cancel_learn(void)
{
    ir_learn_ana_rx_dis();
    ir_learn_dis();
    g_il_index = 0;
    memset(g_il_wave_receive_buff, 0, sizeof(g_il_wave_receive_buff));
    memset(&ir_learn_dat, 0x0, sizeof(ir_learn_dat));

    if (learnStateCb) {
        learnStateCb(IR_LEARN_CANCEL);
    }
}

void  ir_stop_learn(void){
    if((ir_learn_dat.series_tm[0] < 200) || (ir_learn_dat.series_cnt < 10)) {   //ͷС200us
        memset(&ir_learn_dat, 0x0, sizeof(ir_learn_dat));
        ev_on_timer(ir_learn_failed_callback, NULL, 1);
    }else{
        ev_on_timer(ir_learn_success_callback, NULL, 1);
    }
}

_attribute_ram_code_sec_ int drv_ir_data_process(void* arg){
    volatile u32 receive_idx = 0;
    volatile u32 record_idx = 0;
    u32 record_interval = 0;
    u8 sr = irq_disable();

    while (g_il_wave_receive_buff[receive_idx]) {
        if (g_il_wave_receive_buff[receive_idx] < IR_LEARN_PWM_CARRIEN_MAX) { //71us
            record_interval += g_il_wave_receive_buff[receive_idx];
        } else if (g_il_wave_receive_buff[receive_idx] < IR_LEARN_PWM_NOCARRIEN_MAX) {
            ir_learn_dat.series_tm[record_idx % IR_DMA_SERIES_CNT1] = (u16)(record_interval / CLOCK_PWM_CLOCK_1US);
            record_idx++;
            ir_learn_dat.series_tm[record_idx % IR_DMA_SERIES_CNT1] = (u16)(g_il_wave_receive_buff[receive_idx] / CLOCK_PWM_CLOCK_1US);
            record_idx++;
            record_interval = 0;
        } else {
            ir_learn_dat.series_tm[record_idx % IR_DMA_SERIES_CNT1] = (u16)(record_interval / CLOCK_PWM_CLOCK_1US);

            if (record_idx >= IR_DMA_SERIES_CNT1) {
                record_idx = 0;
                ir_learn_dat.series_cnt = 0;
            } else {
                ir_learn_dat.series_cnt = record_idx + 1;
                ir_learn_dat.carr_high_tm = (calculate_mid(g_il_wave_receive_buff[0], g_il_wave_receive_buff[1], g_il_wave_receive_buff[2])) / CLOCK_PWM_CLOCK_1US;
            }

            g_il_index = 0;
            memset(g_il_wave_receive_buff, 0, sizeof(g_il_wave_receive_buff));
            record_idx = 0;
//            memset(g_il_wave_receive_buff, 0, sizeof(g_il_wave_receive_buff));

            ir_learn_ana_rx_dis();
            ir_learn_dis();
            ir_stop_learn();

            irq_restore(sr);
            return -1;
        }
        receive_idx++;
    }

    irq_restore(sr);
    return -1;
}


_attribute_ram_code_sec_ void drv_ir_learn_rx_handler(void)
{
    unsigned char fifo_cnt = ir_learn_get_rx_fifo_status(FLD_IR_LEARN_FIFO_RX_CNT);
    for (unsigned char i = 0; i < fifo_cnt / 4; i++) {
        g_il_wave_receive_buff[g_il_index++] = ir_learn_get_data_by_word();
        g_il_index %= BUFF_DATA_LEN;
    }
}

_attribute_ram_code_sec_ void drv_ir_learn_timeout_handler(void)
{
    unsigned char fifo_cnt = ir_learn_get_rx_fifo_status(FLD_IR_LEARN_FIFO_RX_CNT);
    for (unsigned char i = 0; i < fifo_cnt / 4; i++) {
        g_il_wave_receive_buff[g_il_index++] = ir_learn_get_data_by_word();
        g_il_index %= BUFF_DATA_LEN;
    }

    ev_on_timer(drv_ir_data_process, NULL, 1);
}

u8 ir_learn_send_nv(u8 key)
{
    u32 irCycle = 0;
    u8 ret = SUCCESS;

    memset(&ir_learn_data_read, 0, sizeof(ir_learn_data_read));

    if (NV_SUCC != nv_flashReadNew(DS_IR_LEARN_MODULE, key, sizeof(ir_learn_data_read), (u8 *)&ir_learn_data_read)) {
        return FAILURE;
    }

    if((ir_learn_data_read.carr_high_tm == 0) || (ir_learn_data_read.series_cnt==0) || (ir_learn_data_read.series_tm==NULL)) {
        return FAILURE;
    }

    irCycle = ir_learn_data_read.carr_high_tm;

    ir_set(1000000/irCycle, 3);
//    ir_set(38000, 3);

#if IR_DMA_FIFO_EN
    Get_CarrierCycleTick(irCycle);
    ir_dma_send_serial((u16 *)ir_learn_data_read.series_tm, ir_learn_data_read.series_cnt);
#else
    ir_send_serial((u16 *)ir_learn_data_read.series_tm, ir_learn_data_read.series_cnt);
#endif
    return ret;
}


#else

#define CLOCK_TICK_TO_US(t)                ((t)/S_TIMER_CLOCK_1US)

#define IR_FLASH_PAGE_SIZE                 0x100//256bytes
#define IR_CARR_CHECK_CNT                  10
#define IR_LEARN_START_MINLEN              (200)

#define IR_LEARN_NONE_CARR_MIN             (200)//old is 80
#define IR_LEARN_CARR_GLITCH_MIN           (3)
#define IR_LEARN_CARR_MIN                  (7)

#define NEC_LEAD_CARR_MIN_INTERVAL         (8700)
#define NEC_LEAD_CARR_MAX_INTERVAL         (9300)
#define NEC_LEAD_NOCARR_MIN_INTERVAL       (4200)
#define NEC_LEAD_NOCARR_MAX_INTERVAL       (4800)
#define TOSHIBA_LEAD_MIN_INTERVAL          (4200)
#define TOSHIBA_LEAD_MAX_INTERVAL          (4800)
#define FRAXEL_LEAD_CARR_MIN_INTERVAL      (2100)
#define FRAXEL_LEAD_CARR_MAX_INTERVAL      (2700)
#define FRAXEL_LEAD_NOCARR_MIN_INTERVAL    (900)
#define FRAXEL_LEAD_NOCARR_MAX_INTERVAL    (1500)
#define IR_NEC_TYPE                        1
#define IR_TOSHIBA_TYPE                    2
#define IR_FRAXEL_TYPE                     3
#define IR_HIGH_LOW_MIN_INTERVAL           (1000)
#define IR_HIGH_LOW_MAX_INTERVAL           (2000)
#define TC9012_FRAME_CYCLE                 (108*1000)
#define FRAXEL_LEVEL_NUM                   19 
#define NEC_TOSHIBA_LEVEL_NUM              67

static ir_learn_ctrl_t ir_learn_ctrl;
static ir_learn_pattern_t ir_learn_pattern;


ir_learn_save_t ir_learn_dat;

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
        if(ir_learn_pattern.series_tm[ir_start_cnt+1] > (ir_learn_pattern.series_tm[ir_start_cnt]<<1)) {
            ir_learn_pattern.toshiba_c0flag = 1;
        }
        else {
            ir_learn_pattern.toshiba_c0flag = 0;
        }
    }
    
    if(ir_learn_pattern.ir_protocol == IR_FRAXEL_TYPE) {
        ir_learn_pattern.series_cnt = FRAXEL_LEVEL_NUM;
    }
    if(ir_learn_pattern.ir_protocol == IR_NEC_TYPE) {
        ir_learn_pattern.series_cnt = NEC_TOSHIBA_LEVEL_NUM;
    }
    if(ir_learn_pattern.ir_protocol != 0){
        for(int i = ir_start_cnt ;i < ir_learn_pattern.series_cnt;i++){
            if(ir_learn_pattern.series_tm[i] < IR_HIGH_LOW_MIN_INTERVAL){
                ir_learn_pattern.series_tm[i]=(560);
            }
            else if(ir_learn_pattern.series_tm[i] < IR_HIGH_LOW_MAX_INTERVAL){
                if(ir_learn_pattern.ir_protocol == IR_NEC_TYPE) {
                    ir_learn_pattern.series_tm[i]= 1680;
                }
                else {
                    ir_learn_pattern.series_tm[i]= 1690;
                }
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

_attribute_ram_code_ void ir_record(u32 tm, u32 pol)
{
    ir_learn_ctrl.curr_trigger_tm = tm;
    if( ir_learn_pattern.series_cnt >= IR_LEARN_SERIES_CNT - 2 ){
        return;
    }
    if(ir_learn_pattern.ir_int_cnt!=0 ){
        ir_learn_ctrl.time_interval = ir_learn_ctrl.curr_trigger_tm - ir_learn_ctrl.last_trigger_tm;
        if(ir_learn_ctrl.time_interval < 71*S_TIMER_CLOCK_1US)    //زжϴʱ̫޷¼ز
        {
            ir_learn_pattern.carr_high_temp[ir_learn_pattern.carr_high_cnt]=CLOCK_TICK_TO_US(ir_learn_ctrl.time_interval);
            ir_learn_pattern.carr_high_cnt++;
            if(ir_learn_pattern.carr_high_cnt>1)    //3Σȡֵ
            {
                ir_learn_pattern.carr_high_cnt=0;
                ir_learn_pattern.carr_high_tm= calculate_mid(ir_learn_pattern.carr_high_temp[0],ir_learn_pattern.carr_high_temp[1],ir_learn_pattern.carr_high_tm);
            }
        }
        else if((ir_learn_ctrl.time_interval > 70*S_TIMER_CLOCK_1US)&&(ir_learn_ctrl.time_interval < 200001*S_TIMER_CLOCK_1US))
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
        else if(ir_learn_ctrl.time_interval > 200000*S_TIMER_CLOCK_1US)
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
        gpio_write(GPIO_IR_OUT,        0);
        gpio_set_output_en(GPIO_IR_OUT, 1);
        gpio_write(GPIO_IR_CTRL,    0);
    }else{
        gpio_write(GPIO_IR_CTRL, 1);
        gpio_set_func(GPIO_IR_OUT, IR_PWM_FUNC);
    }
}

void ir_cancel_learn(void)
{
	gpio_clr_interrupt(GPIO_IR_LEARN_IN);
    ir_learn_ready = 0;
    ir_learning_flag = 0;

    ir_learn_intf_init(0);
    reg_gpio_wakeup_irq &= ~FLD_GPIO_CORE_INTERRUPT_EN;
    reg_irq_src &= ~FLD_IRQ_GPIO_EN;
    reg_irq_mask &= ~FLD_IRQ_GPIO_EN;

    memset(&ir_learn_dat, 0x0, sizeof(ir_learn_dat));

    if (learnStateCb) {
        learnStateCb(IR_LEARN_CANCEL);
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
        if(ir_learn_pattern.series_cnt) {
            ir_learn_pattern.series_cnt--;
        }
    }

    if(ir_learn_pattern.series_tm[0] < 200)     //ͷС200us
    {
        memset4(&ir_learn_pattern, 0, sizeof(ir_learn_pattern));
        ev_on_timer(ir_learn_failed_callback, NULL, 1);
    }else{
        ev_on_timer(ir_learn_success_callback, NULL, 1);
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
    if(learnStateCb) {
        learnStateCb(IR_LEARN_START);
    }
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
u8 irflag=0;        //kevin
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


u8 ir_learn_send_nv(u8 key)
{
    u32 irFreq = 0;
    u8 ret = SUCCESS;

    memset4(&ir_learn_pattern, 0, sizeof(ir_learn_pattern));

    if(NV_SUCC != nv_ir_read(key, &irFreq, (u16 *)ir_learn_pattern.series_tm, &ir_learn_pattern.series_cnt)) {
        return FAILURE;
    }

    if(irFreq==0||ir_learn_pattern.series_cnt==0||ir_learn_pattern.series_tm==NULL) {
        return FAILURE;
    }

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
    memset(&ir_learn_dat.series_tm[0],0xff,IR_LEARN_SERIES_CNT*2);
    memcpy(&ir_learn_dat.series_tm[0],buf,len*2);
    nv_sts_t sta = nv_flashWriteNew(1, DS_IR_LEARN_MODULE, keycode,  sizeof(ir_learn_dat), (u8 *)&ir_learn_dat);
    return sta;
}

nv_sts_t nv_ir_read(u8 keyCode, u32 *freq, u16 *buf, u16 *len)
{
    nv_sts_t sta = nv_flashReadNew(DS_IR_LEARN_MODULE, keyCode, sizeof(ir_learn_dat), (u8 *)&ir_learn_dat);
    if(sta==NV_SUCC&&ir_learn_dat.series_cnt)
    {
     *freq = ir_learn_dat.carr_high_tm;
     *len =  ir_learn_dat.series_cnt;
     memcpy(buf,&ir_learn_dat.series_tm[0],ir_learn_dat.series_cnt*2);
    }
    return sta;
}

int ir_learn_success_callback(void *arg)
{
    nv_ir_write(kb_keyindex_pressed, ir_learn_pattern.carr_high_tm, ir_learn_pattern.series_tm, ir_learn_pattern.series_cnt+1);
    if(learnStateCb) {
        learnStateCb(IR_LEARN_SUCCESS);
    }
    return -1;
}


int ir_learn_failed_callback(void *arg)
{
    if(learnStateCb) {
        learnStateCb(IR_LEARN_FAILED);
    }
    return -1;
}

#endif
#endif

