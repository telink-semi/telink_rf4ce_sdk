/********************************************************************************************************
 * @file    phy_radio_8278.h
 *
 * @brief   This is the header file for phy_radio_8278.h
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

#ifndef  __MAC_RADIO_b92_H_
#define  __MAC_RADIO_b92_H_

/*******************************************************************************************************
 * 									Radio interface for B92
 ******************************************************************************************************/
/* radio module reset */
#define ZB_RADIO_RESET()

/* trx swith */
#define ZB_RADIO_TRX_SWITCH(mode, chn) 					rf_set_trx_state(mode, chn)

/* set tx power */
#define ZB_RADIO_TX_POWER_SET(level)					rf_set_power_level_index((rf_power_level_index_e)level)

/* set rx gain mode: max gain: only use auto gain mode */
#define ZB_RADIO_MODE_MAX_GAIN()

/* set rx gain mode: auto gain, only use auto gain mode */
#define ZB_RADIO_MODE_AUTO_GAIN()

/* get RSSI */
#define ZB_RADIO_RSSI_GET()								rf_get_rssi()

/* start sending a packet */
#define ZB_RADIO_TX_START(txBuf)						rf_tx_pkt(txBuf)

/* tx done */
#define ZB_RADIO_TX_DONE								rf_get_irq_status(FLD_RF_IRQ_TX)

/* clear tx status(interrupt status) */
#define ZB_RADIO_TX_DONE_CLR							rf_clr_irq_status(FLD_RF_IRQ_TX)

/* rx done */
#define ZB_RADIO_RX_DONE								rf_get_irq_status(FLD_RF_IRQ_RX)

/* clear rx status(interrupt status) */
#define ZB_RADIO_RX_DONE_CLR							rf_clr_irq_status(FLD_RF_IRQ_RX)

/* enable tx */
#define ZB_RADIO_TX_ENABLE

/* disable tx */
#define ZB_RADIO_TX_DISABLE

#define RF_DMA_BUSY()									rf_receiving_flag()

/* enable rx */
#define ZB_RADIO_RX_ENABLE								dma_chn_en(DMA1)//todo: Doesn't seem to work

/* disable rx */
#define ZB_RADIO_RX_DISABLE								dma_chn_dis(DMA1)//todo: Doesn't seem to work

/* clear mask bit to disable tx irq */
#define ZB_RADIO_IRQ_MASK_CLR							rf_clr_irq_mask(FLD_RF_IRQ_TX)

/* clear mask bit to disable tx irq */
#define ZB_RADIO_IRQ_MASK_SET							rf_set_irq_mask(FLD_RF_IRQ_TX)

/* trx status get */
#define ZB_RADIO_TRX_STA_GET()							rf_get_trx_state()

/* rx buffer configure */
#define ZB_RADIO_RX_BUF_SET(addr)						rf_set_rx_buffer(addr)

/* rx buffer clear */
#define ZB_RADIO_RX_BUF_CLEAR(p)						do{ \
															p[0] = 0;		\
															p[4] = 0;		\
														}while(0)

/* set Rx mode, maxium receiver buffer size, enable Rx/Tx interrupt */
#define ZB_RADIO_TRX_CFG(size)							do{ \
															rf_set_rx_maxlen(size);							\
															rf_set_rx_dma_config();							\
															rf_set_rx_dma_fifo_num(0);						\
															rf_set_rx_dma_fifo_size(size);					\
															rf_set_tx_dma_config();							\
															rf_set_tx_dma_fifo_num(1);						\
															rf_set_tx_dma_fifo_size(128);					\
															rf_set_irq_mask(FLD_RF_IRQ_RX|FLD_RF_IRQ_TX); 	\
															plic_interrupt_enable(IRQ15_ZB_RT); 			\
														}while(0)

#define ZB_RADIO_DMA_HDR_BUILD(pBuf, len)				do{	\
															u8 *p = pBuf;								\
															u32 txLen = rf_tx_packet_dma_len(len + 1);	\
															p[3] = (txLen >> 24) & 0xFF;				\
															p[2] = (txLen >> 16) & 0xFF;				\
															p[1] = (txLen >> 8) & 0xFF;					\
															p[0] = txLen & 0xFF;						\
														}while(0)

/* tx packet header */
#define ZB_RADIO_TX_HDR_LEN								5

/* rx packet header */
#define ZB_RADIO_RX_HDR_LEN								5

/* delay after switch to Tx mode, and then start sending */
#define ZB_TX_WAIT_US									85

/* get real payload length */
#define ZB_RADIO_ACTUAL_PAYLOAD_LEN(p)					rf_zigbee_get_payload_len(p)

/* packet CRC check */
#define ZB_RADIO_CRC_OK(p)								rf_zigbee_packet_crc_ok(p)

/* packet length check */
#define ZB_RADIO_PACKET_LENGTH_OK(p)					rf_zigbee_packet_length_ok(p)

/* enable timestamp */
#define ZB_TIMESTAMP_ENABLE

/* get time timestamp */
#define ZB_RADIO_TIMESTAMP_GET(p)						( p[rf_zigbee_dma_rx_offset_time_stamp(p)]   | \
														  p[rf_zigbee_dma_rx_offset_time_stamp(p)+1] | \
														  p[rf_zigbee_dma_rx_offset_time_stamp(p)+2] | \
														  p[rf_zigbee_dma_rx_offset_time_stamp(p)+3] )

#define ZB_RADION_PKT_RSSI_GET(p)						(p[rf_zigbee_dma_rx_offset_rssi(p)])


/* tx power 0dBm*/
#define ZB_RADIO_TX_0DBM								RF_POWER_INDEX_P0p01dBm

/* radio initialization */
#define ZB_RADIO_INIT()									do{ \
															rf_mode_init(); 			\
															rf_set_zigbee_250K_mode();	\
														  }while(0)

/* sys timer initialization for mac-csma */
#define ZB_TIMER_INIT()									drv_hwTmr_init(TIMER_IDX_3, TIMER_MODE_SCLK)

#define rf_setIdleMode()								rf_set_tx_rx_off()

static inline u8 ZB_RADIO_RSSI_TO_LQI(rf_rxGainMode_t mode, u8 inRssi){
		u8 lqi;

		s8 rssi = inRssi - 110;

		s16 minEd = -99;
		s16 maxEd = -15;  /* AGC */
		if(rssi > maxEd){rssi = maxEd;}
		if(rssi < minEd){rssi = minEd;}
		lqi = 255*(rssi - minEd)/(maxEd - minEd);
		return lqi;
}




#endif  /* __RF_H__ */
