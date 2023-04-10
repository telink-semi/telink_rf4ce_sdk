/********************************************************************************************************
 * @file    mac_trx.h
 *
 * @brief   This is the header file for mac_trx.h
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

/**************************************************************************************************
  Filename:       mac_trx.h
  Revised:        $Date: 2013-05-30 $

  Description:    Interface of MAC TX and RX 
**************************************************************************************************/

#if (!HOST_ROLE)
#pragma once
//#include "mac_internal.h"

typedef enum {
    MAC_TX_IDLE,
    MAC_TX_CSMA,
    MAC_TX_UNDERWAY,
    MAC_TX_WAIT_ACK,
    MAC_TX_RETRY,
    MAC_TX_TIME_OUT,
} mac_txState_t;


typedef enum {
    MAC_TX_EV_NEW_DATA,
    MAC_TX_EV_CSMA_IDLE,
    MAC_TX_EV_CSMA_BUSY,
    MAC_TX_EV_SEND_SUCC,
    MAC_TX_EV_SEND_FAIL,
    MAC_TX_EV_ACK_RECV,
    MAC_TX_EV_ACK_RETRY,
    MAC_TX_EV_NO_ACK,
    MAC_TX_EV_HAVE_PENDING_DATA,
} mac_txEvt_t;

typedef enum{
	RX_BUSY = BIT(0),
	TX_BUSY = BIT(1),
	TX_ACKPACKET = BIT(2),
	RX_WAITINGACK = BIT(3),
	RX_DATAPENDING	= BIT(4),
	TX_UNDERWAY = BIT(5)
}rf_trxds_st_e;


typedef u8 ( *mac_ackPendingCheckFunc_t )( addr_t* pAddr );




#define IS_MAC_WAITING_ACK()     (mac_getTrxState() == MAC_TX_WAIT_ACK)


void mac_trxReset(void);
void mac_trxInit(void);

u8 mac_getTrxState(void);
void mac_ackCb(u8 fPendingFrame, u8 seq);
void mac_rxCb_1(void* pData);
void mac_txCb(void* status);
void macUserSpecicalHdandle(u8 msduHandle, void* data );
u8 *zb_macDataFilter(u8 *macPld, u8 len, u8 *needDrop, u8 *ackPkt);

#endif //(!HOST_ROLE)
