/********************************************************************************************************
 * @file    drv_pwm.c
 *
 * @brief   This is the source file for drv_pwm.c
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

#include "../tl_common.h"
#include "./drv_pwm.h"



void drv_pwm_init(void){
#if defined(MCU_CORE_826x)
	pwm_Init(0);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
#elif defined(MCU_CORE_B92)
    pwm_set_clk((unsigned char) (sys_clk.pclk*1000*1000/PWM_PCLK_SPEED-1));
#endif
}

void drv_pwm_cfg(u32  pwmId, unsigned short cmp_tick, unsigned short cycle_tick){
#if defined(MCU_CORE_826x)
	pwm_Open(pwmId, NORMAL, 0, cmp_tick, cycle_tick, 0x2fff);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
	pwm_set_mode(pwmId, PWM_NORMAL_MODE);
	pwm_set_phase(pwmId, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(pwmId, cycle_tick, cmp_tick);
#elif defined(MCU_CORE_B92)
	 pwm_set_tcmp(pwmId,cmp_tick);
	 pwm_set_tmax(pwmId,cycle_tick);
//	 pwm_set_pwm0_tcmp_and_tmax_shadow(200 * cycle_tick,cmp_tick);
	 pwm_set_dma_config(DMA_CHN);
#endif
}


void drv_ir_pwm_cfg(u32  pwmId, u32 hz, u32 low_duty){
#if defined(MCU_CORE_826x)
	u16 max_tick = CLOCK_SYS_CLOCK_HZ/hz;
	u16 cmp_tick = max_tick / low_duty;
	pwm_Open(pwmId, NORMAL, 0, cmp_tick, max_tick, 0x2fff);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	u16 max_tick = CLOCK_SYS_CLOCK_HZ/hz;
	u16 cmp_tick = max_tick / low_duty;
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);
	pwm_set_mode(pwmId, PWM_NORMAL_MODE);
	pwm_set_phase(pwmId, 0);   //no phase at pwm beginning
	pwm_set_cycle_and_duty(pwmId, max_tick, cmp_tick);
#elif defined(MCU_CORE_B92)
	u16 max_tick = PWM_PCLK_SPEED/hz;
	u16 cmp_tick = max_tick / low_duty;
	 pwm_set_tcmp(pwmId,cmp_tick);
	 pwm_set_tmax(pwmId,max_tick);
//	 pwm_set_pwm0_tcmp_and_tmax_shadow(200 * cycle_tick,cmp_tick);
	 pwm_set_dma_config(DMA_CHN);
#endif
}




//PWM IR FIFO DMA API for 825x,827x,B92 platforms
void drv_ir_dma_start(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	pwm_start_dma_ir_sending();
#elif defined(MCU_CORE_B92)
	pwm_ir_dma_mode_start(DMA_CHN);
#endif
}

//PWM IR FIFO DMA API for 825x,827x,B92 platforms
void drv_ir_dma_stop(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	pwm_stop_dma_ir_sending();
#elif defined(MCU_CORE_B92)
	dma_chn_dis(DMA_CHN);
#endif
}




unsigned short drv_ir_dma_plus_config(unsigned short plus_num,unsigned char carrien)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	return pwm_config_dma_fifo_waveform(carrien,PWM0_PULSE_NORMAL,plus_num);
#elif defined(MCU_CORE_B92)
	return pwm_cal_pwm0_ir_fifo_cfg_data(plus_num,0,carrien);
#endif
}




//buf data structure:
//length: buf[0]~buf[3]
//data:buf[4]~
void drv_ir_dma_set_buffer(void *buf)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	unsigned char* buff = (u8 *)buf;
	pwm_set_dma_address(buff);
#elif defined(MCU_CORE_B92)

	unsigned char *p = (unsigned char *) (buf+4);
	unsigned int len = *(unsigned int *) buf;
	pwm_set_dma_buf(DMA_CHN, (unsigned int) p ,len);
#endif
}



void drv_ir_dma_enable_irq(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
	reg_pwm_irq_mask |= FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask
#elif defined(MCU_CORE_B92)
    pwm_set_irq_mask(FLD_PWM0_IR_DMA_FIFO_IRQ);
	pwm_clr_irq_status(FLD_PWM0_IR_DMA_FIFO_IRQ);
	plic_interrupt_enable(IRQ16_PWM);
#endif
}

void drv_ir_dma_disable_irq(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	reg_pwm_irq_sta = FLD_IRQ_PWM0_IR_DMA_FIFO_DONE;   //clear  dma fifo mode done irq status
	reg_pwm_irq_mask &= ~FLD_IRQ_PWM0_IR_DMA_FIFO_DONE; //enable dma fifo mode done irq mask
#elif defined(MCU_CORE_B92)
    pwm_clr_irq_mask(FLD_PWM0_IR_DMA_FIFO_IRQ);
	pwm_clr_irq_status(FLD_PWM0_IR_DMA_FIFO_IRQ);
	plic_interrupt_disable(IRQ16_PWM);
#endif
}
