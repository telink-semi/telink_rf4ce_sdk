/********************************************************************************************************
 * @file    irq_handler.c
 *
 * @brief   This is the source file for B86
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

#include "../../proj/tl_common.h"
#include "../../proj/os/timer.h"
#include "../../proj/drivers/audio/drv_audio.h"
void gpio_user_irq_handler(void);
void timer_irq_handler(u8 tmrIdx);
void usb_suspend_irq_handler(void);
void eth_rx_irq_handler(void);
void rf_rx_irq_handler(void);
void rf_tx_irq_handler(void);
void timer_irq2_handler(void);
//void ZipIR_IRQHandlerEx(void);

#if MODULE_QRP_ENABLE
u8 QRP_IsRunning(void);
#endif

#if IRQ_I2C_ENABLE
void i2c_app_irq_handler(void);
#endif

#if (MODULE_UART_ENABLE)
extern void uart_rx_irq_handler(void);
extern void uart_tx_irq_handler(void);
#endif


_attribute_ram_code_ void  usb_endpoints_irq_handler (void) {
	/////////////////////////////////////
	// ISO IN
	/////////////////////////////////////
	if (reg_usb_irq & BIT(7)) {
		reg_usb_irq = BIT(7);	//clear interrupt flag of endpoint 7
		/////// get MIC input data ///////////////////////////////
#if ( __PROJECT_ZRC_2_DONGLE__ || __PROJECT_ZRC_DONGLE_APP__ || __PROJECT_MSO_DONGLE__|| __PROJECT_MSO_DONGLE_APP__ || MODULE_AUDIO_DEBUG )
		abuf_dec_usb ();
#endif
	}
}

typedef void (*irq_handler_t)(void);

irq_handler_t	p_irq_handler = 0;

_attribute_ram_code_ void irq_handler(void)
{
	u32 src = reg_irq_src;
	if(IRQ_TIMER1_ENABLE && (src & FLD_IRQ_TMR1_EN)){
		reg_irq_src = FLD_IRQ_TMR1_EN;
		timer_irq_handler(TIMER_IDX_1);
	}

#if MODULE_QRP_ENABLE
	/* enable for testing */
	QRP_IRQHandler();
#endif

#if (MODULE_RF_ENABLE)
	u16  src_rf = reg_rf_irq_status;
	if(src_rf & FLD_RF_IRQ_RX){
		rf_rx_irq_handler();
	}

	if(src_rf & FLD_RF_IRQ_TX){
		rf_tx_irq_handler();
	}
#endif

	if (p_irq_handler)
	{
		p_irq_handler();
		return;
	}

#if(MODULE_ETH_ENABLE)
	u8  eth_irq = reg_mac_irq_sta;
#endif

    if((src & FLD_IRQ_TMR0_EN)){
		reg_irq_src = FLD_IRQ_TMR0_EN;
		timer_irq_handler(TIMER_IDX_0);
	}
    if((src & FLD_IRQ_TMR2_EN)){
		reg_irq_src = FLD_IRQ_TMR2_EN;
		timer_irq_handler(TIMER_IDX_2);
	}

#if IRQ_I2C_ENABLE
    if(src & FLD_IRQ_HOST_CMD_EN){
    	reg_irq_src = FLD_IRQ_HOST_CMD_EN;
    	i2c_app_irq_handler();
    }
#endif

#if MODULE_IR_LEARN_ENABLE||IRQ_GPIO_ENABLE
    if(src & FLD_IRQ_GPIO_EN){
    	gpio_user_irq_handler();
    	reg_irq_src = FLD_IRQ_GPIO_EN;
    }
#endif

#if (MODULE_UART_ENABLE)
    u16  dma_irq_source = reg_dma_irq_src;
	if(dma_irq_source & FLD_DMA_UART_RX){
		reg_dma_irq_src = FLD_DMA_UART_RX;
		uart_rx_irq_handler();
	}else if(dma_irq_source & FLD_DMA_UART_TX){
		reg_dma_irq_src = FLD_DMA_UART_TX;
		uart_tx_irq_handler();
	}else{
		reg_dma_irq_src = ~(FLD_DMA_UART_TX | FLD_DMA_UART_RX);
	}
#endif


#if(0 && !APPLICATION_DONGLE)
	if(IRQ_GPIO0_ENABLE && (src & FLD_IRQ_GPIO_RISC2_EN)) {
#ifndef WIN32
		gpio0_user_irq_handler();
#endif
		reg_irq_src = FLD_IRQ_GPIO_RISC2_EN;
	}
#endif


#if (MODULE_USB_ENABLE & USB_MIC_ENABLE )
	// usb end-point packet transfer interrupt
	if(reg_irq_src & FLD_IRQ_EP_DATA_EN){
		reg_irq_src = FLD_IRQ_EP_DATA_EN;
		usb_endpoints_irq_handler();
	}
#endif

//	}


//	irq_clr_src(); 	// write clear
	//irq_enable();
}


