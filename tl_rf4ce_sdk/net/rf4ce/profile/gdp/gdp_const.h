/********************************************************************************************************
 * @file    gdp_const.h
 *
 * @brief   This is the header file for gdp_const.h
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
  Filename:       gdp_const.h
  Revised:        $Date: 2013-07-09 $

  Description:    Constant definition in RF4CE GDP layer 
**************************************************************************************************/

#pragma once

#include "../../../../proj/tl_common.h"


/**
 *  @brief  Definition for frame control field mask
 */
#define GDP_FCF_CMD_FRAME_FIELD                0x40
#define GDP_FCF_DATA_PENDING_FIELD             0x80
#define GDP_FCF_CMD_ID_MASK                    0x4F


/**
 *  @brief  Definition for GDP command id
 */
#define GDP_CMD_GENERIC_RSP                    0x40
#define GDP_CMD_CFG_COMPLETE                   0x41
#define GDP_CMD_HEARTBEAT                      0x42
#define GDP_CMD_GET_ATTR                       0x43
#define GDP_CMD_GET_ATTR_RSP                   0x44
#define GDP_CMD_PUSH_ATTR                      0x45
#define GDP_CMD_SET_ATTR                       0x46
#define GDP_CMD_PULL_ATTR                      0x47
#define GDP_CMD_PULL_ATTR_RSP                  0x48
#define GDP_CMD_CHECK_VALIDATION               0x49

#define GDP_CMD_CLIENT_NOTIFICATION            0x4A
#define GDP_CMD_KEY_EXCHANGE                   0x4B

#define GDP_OP_EVENT_TX_ERROR					0x50
#define GDP_OP_EVENT_TIMEOUT					0x51

#define GDP_CMD_GENERIC_RSP_LEN                0x02


/**
 *  @brief  Definition for response code of GDP_CMD_GENERIC_RSP command
 */
#define GDP_RSP_CODE_SUCC                      0x00
#define GDP_RSP_CODE_UNSUPPORT_REQ             0x01
#define GDP_RSP_CODE_INVALID_PARA              0x02
#define GDP_RSP_CODE_CFG_FAIL                  0x03



#define GDP_KEY_EXC_CHALLENGE          0x00
#define GDP_KEY_EXC_CHALLENGE_RESP     0x01
#define GDP_KEY_EXC_RESP               0x02
#define GDP_KEY_EXC_CONF               0x03

#define GDP_SINGLE_POLL_CONSTRAINT_SIZE  13
#define GDP_DEFAULT_POLL_CONSTRAINT_NUM  3
#define GDP_POLL_CONSTRAIN_SIZE  ( 1 + (GDP_DEFAULT_POLL_CONSTRAINT_NUM) * GDP_SINGLE_POLL_CONSTRAINT_SIZE )

/**
 *  @brief  Definition for GDP profile constants
 */

#define GDP_aplcConfigBlackoutTime             100   // in ms
#define GDP_aplcMaxPollingTimeout              100  //ms

#define GDP_aplcAfterBindingConfigBlackoutTime 400  //ms

#define GDP_aplcGdpMaxPairIndicationWaitTime   1200  // in ms
#define GDP_aplcMaxRxOnWaitTime                100   // in ms

extern u32 GDP_aplcMaxResponseWaitTime ;  // in ms
#define GDP_aplcMaxConfigWaitTime              100   // in ms

#define GDP_aplcMinKeyExchangeTransferCount    3



