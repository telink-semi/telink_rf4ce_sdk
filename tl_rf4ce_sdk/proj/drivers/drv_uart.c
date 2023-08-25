/********************************************************************************************************
 * @file    drv_uart.c
 *
 * @brief   This is the source file for drv_uart.c
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
#include "./drv_uart.h"

#if defined(MCU_CORE_B92)
#define UART_IDX					UART0
#define UART_DMA_CHANNEL_RX			DMA2
#define UART_DMA_CHANNEL_TX			DMA3

static u8 *pUartRxBuf = NULL;
static u32 uartRxBufLen = 0;

#define UART_RCV_DMA_LEN_FIX()		do{	\
										u32 rcvDataLen = uart_get_dma_rev_data_len(UART_IDX, UART_DMA_CHANNEL_RX);		\
										if(pUartRxBuf){																	\
											pUartRxBuf[0] = (u8)(rcvDataLen);											\
											pUartRxBuf[1] = (u8)(rcvDataLen >> 8);										\
											pUartRxBuf[2] = (u8)(rcvDataLen >> 16);										\
											pUartRxBuf[3] = (u8)(rcvDataLen >> 24);										\
										}																				\
									}while(0)

static u8 uart_dma_send(u8 *pBuf);
#endif	/* MCU_CORE_B92 */


drv_uart_t myUartDriver = {
		.status = UART_STA_IDLE,
		.recvCb = NULL,
#if	defined(MCU_CORE_826x)
		.send = uart_pktSend,
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
		.send = uart_dma_send,
#endif
};

static u8 *uartDrvTxBuf = NULL;

void drv_uart_init(u32 baudrate, u8 *rxBuf, u16 rxBufLen, uart_irq_callback uart_recvCb){
#if	defined (MCU_CORE_826x)
	uart_Init(baudrate, 1, 1, NOCONTROL);

	uart_RecBuffInit(rxBuf, rxBufLen);	/* configure receive buffer */

#elif	defined(MCU_CORE_8278) ||defined(MCU_CORE_8258)
	uart_recbuff_init( rxBuf, rxBufLen);
	uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset
	uart_init_baudrate(baudrate, (H_TIMER_CLOCK_1US*1000*1000), PARITY_NONE, STOP_BIT_ONE);//CLOCK_SYS_CLOCK_HZ


	// dma mode
	uart_dma_enable(1, 1); 	//uart data in hardware buffer moved by dma, so we need enable them first
	irq_set_mask(FLD_IRQ_DMA_EN);
	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1); 	//uart Rx/Tx dma irq enable
	uart_irq_enable(0, 0);  	//uart Rx/Tx irq no need, disable them
#elif defined(MCU_CORE_B92)
	u16 div = 0;
	u8 bwpc = 0;

	pUartRxBuf = rxBuf;
	uartRxBufLen = rxBufLen;

	uart_reset(UART_IDX);

	uart_cal_div_and_bwpc(baudrate, (sys_clk.pclk * 1000 * 1000), &div, &bwpc);
	uart_set_dma_rx_timeout(UART_IDX, bwpc, 12, UART_BW_MUL1);
	uart_init(UART_IDX, div, bwpc, UART_PARITY_NONE, UART_STOP_BIT_ONE);

	dma_clr_irq_mask(UART_DMA_CHANNEL_TX, TC_MASK | ABT_MASK | ERR_MASK);
	dma_clr_irq_mask(UART_DMA_CHANNEL_RX, TC_MASK | ABT_MASK | ERR_MASK);
	uart_set_tx_dma_config(UART_IDX, UART_DMA_CHANNEL_TX);
	uart_set_rx_dma_config(UART_IDX, UART_DMA_CHANNEL_RX);

	uart_receive_dma(UART_IDX, pUartRxBuf + 4, uartRxBufLen);

	uart_clr_tx_done(UART_IDX);
	uart_clr_irq_mask(UART_IDX, UART_RX_IRQ_MASK | UART_TX_IRQ_MASK | UART_TXDONE_MASK | UART_RXDONE_MASK);
	uart_set_irq_mask(UART_IDX, UART_RXDONE_MASK | UART_TXDONE_MASK);
	plic_interrupt_enable((UART_IDX == UART0) ? IRQ19_UART0 : IRQ18_UART1);



#endif

	myUartDriver.recvCb = uart_recvCb;
}

void uart_rx_irq_handler(void){
	//TODO: Rx done
	if(myUartDriver.recvCb){
		myUartDriver.recvCb();
	}
}

void uart_tx_irq_handler(void){
	//TODO: Tx done
	if(uartDrvTxBuf){
		ev_buf_free(uartDrvTxBuf);
		uartDrvTxBuf = NULL;
	}
	myUartDriver.status = UART_STA_TX_DONE;
}

u8 uart_is_idel(void){
	return ((myUartDriver.status == UART_STA_IDLE) ? TRUE : FALSE);
}

u8 uart_tx_done(void){
	return ((myUartDriver.status == UART_STA_TX_DONE) ? TRUE : FALSE);
}

u8 uart_tx_start(u8 *data, u32 len){
	if(!uart_is_idel()){
		while(!uart_tx_done());
	}

	if(!uartDrvTxBuf){
		uartDrvTxBuf = (u8 *)ev_buf_allocate(LARGE_BUFFER);
		if(uartDrvTxBuf){
			myUartDriver.status = UART_STA_TX_DOING;
			uartDrvTxBuf[0] = (unsigned char)(len);
			uartDrvTxBuf[1] = (unsigned char)(len>>8);
			uartDrvTxBuf[2] = (unsigned char)(len>>16);
			uartDrvTxBuf[3] = (unsigned char)(len>>24);
			memcpy(uartDrvTxBuf+4, data, len);
			if(myUartDriver.send){
				while(!myUartDriver.send(uartDrvTxBuf));
				return 1;
			}
		}
	}
	return 0;
}

void uart_exceptionProcess(void){
#if	defined (MCU_CORE_826x)
	uart_ErrorCLR();
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
	if(uart_is_parity_error()){
		uart_clear_parity_error();
	}
#endif
}
