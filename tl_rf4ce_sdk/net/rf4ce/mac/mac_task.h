/********************************************************************************************************
 * @file    mac_task.h
 *
 * @brief   This is the header file for mac_task.h
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
  Filename:       mac_task.h
  Revised:        $Date: 2013-05-27 $

  Description:    Interface of MAC main task 
**************************************************************************************************/

#pragma once


typedef enum {
    MAC_STATE_IDLE,
    MAC_STATE_SCAN,
    MAC_STATE_ASSOCIATING,
    MAC_STATE_POLLING,
    MAC_STATE_STARTING,
    MAC_STATE_DEV,
    MAC_STATE_COORD,
    
} mac_state_t;


typedef struct {
    u8 primitive;
    u8 request[1];
} mac_generalReq_t;

typedef struct {
    u8 primitive;
    u8 status;
    u8 data[1];
} mac_generalCnf_t;

typedef void ( *mac_evHandler_t )( u8* pData );

/** @brief  MAC state machine */
typedef struct
{
    mac_state_t curState;            /*! The MAC State in which the event handler can be used */
    u8 primitive;                    /*! The primitive id for which the event handler is to be invoked */
    mac_state_t nxtState;            /*! The MAC State in which the event handler can be used */
    mac_evHandler_t evHandlerFunc;   /*! The corresponding event handler */
} mac_stateMachine_t;


void mac_sendConfirm(u8* pData);
void mac_sendIndication(u8* pData);
void mac_restoreState(void);
void mac_setState(mac_state_t status);
mac_state_t mac_getState(void);
void mac_setPanCoord(u8 fCoord);
u8 mac_getPanCoord(void);
void mac_getCoordAddr(addr_t *pAddr);
u8 mac_getSelfAddrMode(void);
void mac_genCommStatusInd(u8* pData, u8 status, addr_t* pSrcAddr, addr_t*pDstAddr);
