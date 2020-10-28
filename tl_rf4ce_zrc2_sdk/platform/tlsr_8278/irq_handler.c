/********************************************************************************************************
 * @file     irq_handler.c
 *
 * @brief    Interface of interrupt handler
 *
 * @author
 * @date     Oct. 8, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#include "../../proj/tl_common.h"
#include "../../proj/os/timer.h"
#include "../../proj/drivers/ir/ir.h"
#include "../../proj/drivers/audio/drv_audio.h"
void gpio_user_irq_handler(void);
void timer_irq1_handler(void);
void usb_suspend_irq_handler(void);
void eth_rx_irq_handler(void);
void rf_rx_irq_handler(void);
void rf_tx_irq_handler(void);
void timer_irq2_handler(void);
void rc_ir_irq_prc(void);
// called by irq in cstartup.s

#if (MODULE_UART_ENABLE)
#include "../drivers/drv_uart.h"
#endif

_attribute_ram_code_ void  usb_endpoints_irq_handler (void) {
	/////////////////////////////////////
	// ISO IN
	/////////////////////////////////////
	if (reg_usb_irq & BIT(7)) {
		reg_usb_irq = BIT(7);	//clear interrupt flag of endpoint 7
		/////// get MIC input data ///////////////////////////////
#if ( __PROJECT_ZRC_2_DONGLE__ || __PROJECT_ZRC_DONGLE_APP__ || __PROJECT_MSO_DONGLE_APP__ || MODULE_AUDIO_DEBUG )
		abuf_dec_usb ();
#endif
	}
}


_attribute_ram_code_ void irq_handler(void){
	u32 src = irq_get_src();
	if(IRQ_TIMER1_ENABLE && (src & FLD_IRQ_TMR1_EN)){
		timer_irq1_handler();
//		reg_irq_src = FLD_IRQ_TMR1_EN;
		return;
	}


    if((src & FLD_IRQ_TMR2_EN)){
		reg_irq_src = FLD_IRQ_TMR2_EN;
		timer_irq2_handler();
	}

    if((src & FLD_IRQ_SYSTEM_TIMER)){
    	reg_irq_src = FLD_IRQ_SYSTEM_TIMER;
    	timer_irq3_handler();
    }

	u16  src_rf = rf_irq_src_get();
	if(src_rf & FLD_RF_IRQ_TX){
		rf_tx_irq_handler();
	}

	if(src_rf & FLD_RF_IRQ_RX){
		rf_rx_irq_handler();
	}

#if IR_DMA_FIFO_EN
	if(reg_pwm_irq_sta & FLD_IRQ_PWM0_IR_DMA_FIFO_DONE){
	 	 rc_ir_irq_prc();
	}
#endif

#if MODULE_IR_LEARN_ENABLE||IRQ_GPIO_ENABLE
    if(src & FLD_IRQ_GPIO_EN){
    	gpio_user_irq_handler();
    	reg_irq_src = FLD_IRQ_GPIO_EN;
    }
#endif

#if (MODULE_UART_ENABLE)
    u16  dma_irq_source = dma_chn_irq_status_get();
	if(dma_irq_source & FLD_DMA_CHN_UART_RX){
		dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
		uart_rx_irq_handler();
	}else if(dma_irq_source & FLD_DMA_CHN_UART_TX){
		dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);
		uart_tx_irq_handler();
	}else{
		dma_chn_irq_status_clr(~(FLD_DMA_CHN_UART_TX | FLD_DMA_CHN_UART_RX));
	}
#endif

#if (MODULE_USB_ENABLE & USB_MIC_ENABLE )
	// usb end-point packet transfer interrupt
	if(reg_irq_src & FLD_IRQ_EP_DATA_EN){
		reg_irq_src = FLD_IRQ_EP_DATA_EN;		
		usb_endpoints_irq_handler();
	}
#endif


	if(IRQ_TIMER0_ENABLE && (src & FLD_TMR_STA_TMR0)){
		timer_irq0_handler();
		return;
	}

}

