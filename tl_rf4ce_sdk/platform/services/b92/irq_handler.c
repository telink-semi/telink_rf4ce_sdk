/********************************************************************************************************
 * @file    irq_handler.c
 *
 * @brief   This is the source file for b92
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

#include "tl_common.h"

extern void rf_rx_irq_handler(void);
extern void rf_tx_irq_handler(void);


volatile u8 T_DBG_testIrq[16] = {0};

void rf_irq_handler(void)
{
    T_DBG_testIrq[0]++;
    if(rf_get_irq_status(FLD_RF_IRQ_RX)){
        T_DBG_testIrq[1]++;
        rf_rx_irq_handler();
    }else if(rf_get_irq_status(FLD_RF_IRQ_TX)){
        T_DBG_testIrq[2]++;
        rf_tx_irq_handler();
    }else{
        T_DBG_testIrq[3]++;
        rf_clr_irq_status(0xffff);
    }
}



void gpio_irq_handler(void)
{
    T_DBG_testIrq[5]++;
//    gpio_clr_irq_status(FLD_GPIO_IRQ_CLR);
//    drv_gpio_irq_handler();
}

void gpio_risc0_irq_handler(void)
{
    T_DBG_testIrq[6]++;
//    gpio_clr_irq_status(FLD_GPIO_IRQ_GPIO2RISC0_CLR);
//    drv_gpio_irq_risc0_handler();
}

void gpio_risc1_irq_handler(void)
{
    T_DBG_testIrq[7]++;
    gpio_clr_irq_status(FLD_GPIO_IRQ_GPIO2RISC1_CLR);
//    drv_gpio_irq_risc1_handler();
}

void uart0_irq_handler(void)
{
    if(uart_get_irq_status(UART0, UART_TXDONE_IRQ_STATUS)){
        T_DBG_testIrq[8]++;
        uart_clr_irq_status(UART0,UART_TXDONE_IRQ_STATUS);
    }

    if(uart_get_irq_status(UART0, UART_RXDONE_IRQ_STATUS)){
        T_DBG_testIrq[9]++;
        uart_clr_irq_status(UART0,UART_RXDONE_IRQ_STATUS);
    }
}


_attribute_ram_code_sec_ void pwm_irq_handler(void)
{
    if(pwm_get_irq_status(FLD_PWM0_IR_DMA_FIFO_IRQ))
    {
       T_DBG_testIrq[10]++;
       pwm_clr_irq_status(FLD_PWM0_IR_DMA_FIFO_IRQ);
#if ( __PROJECT_ZRC_2_RC__ || __PROJECT_MSO_RC__)
       extern void rc_ir_irq_prc(void);
       rc_ir_irq_prc();
#endif
    }
}


void timer0_irq_handler(void)
{
    if(timer_get_irq_status(TMR_STA_TMR0)){
        timer_clr_irq_status(TMR_STA_TMR0);
        T_DBG_testIrq[12]++;
        drv_timer_irq0_handler();
    }
}

void timer1_irq_handler(void)
{
    if(timer_get_irq_status(TMR_STA_TMR1)){
        timer_clr_irq_status(TMR_STA_TMR1);
        T_DBG_testIrq[13]++;
        drv_timer_irq1_handler();
    }
}

void stimer_irq_handler(void)
{
    if(stimer_get_irq_status(FLD_SYSTEM_IRQ)){
        stimer_clr_irq_status(FLD_SYSTEM_IRQ);
        T_DBG_testIrq[14]++;
        drv_timer_irq3_handler();
    }
}


_attribute_ram_code_sec_ void  usb_endpoint_irq_handler (void)
{
    /////////////////////////////////////
    // ISO IN
    /////////////////////////////////////
    if (usbhw_get_eps_irq()& FLD_USB_EDP7_IRQ)
    {
        T_DBG_testIrq[15]++;
        usbhw_clr_eps_irq(FLD_USB_EDP7_IRQ);    //clear interrupt flag of endpoint 7
#if ( __PROJECT_ZRC_2_DONGLE__ || __PROJECT_MSO_DONGLE__ || __PROJECT_ZRC_DONGLE_APP__ || __PROJECT_MSO_DONGLE_APP__ || MODULE_AUDIO_DEBUG )
        extern void abuf_dec_usb(void);
        abuf_dec_usb ();
#endif
    }
}
