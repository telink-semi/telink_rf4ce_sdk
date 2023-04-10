/********************************************************************************************************
 * @file    app_data.h
 *
 * @brief   This is the header file for app_data.h
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

#pragma once

enum{
	ZRC_APPID_OTA = 0x01,
	ZRC_APPID_AUDIO = 0x02,
}userAppDataId_e;

typedef struct{
	u8 	option;   //!< option byte, 08: means more data, others(must 0)
	u8 	sqeNo;    //!< sequence number
	u8	appId;	  //!< used to identify the specific features
	u8	opCode;	  //!< used to identify the specific operations within an appID
}user_appDataFrmHdr_t;





u8 zrc_sendIRData(u8 profileId, u8 keyCode, u8 single_key);
u8 zrc_sendRFData(u8 profileId, u8 keyCode);
void zrc_stopSendRepeateDataTimer(void);

void app_startSendRepeateDataTimer(u32 rptTime);
u8 app_sendRFData(u8 keyCode);
u8 app_sendIRData(u8 profileId, u8 keyCode, u8 single_key);
void app_stopSendRepeateDataTimer(void);
void app_sendDtvIrCmd(u8 keyCode, u8 repeat);
//queue
//void app_cmdQInit(cmdQ_t *q);
//u8 app_cmdQPush(cmdQ_t *q, u8 item);
//u8 app_cmdQGetSize(cmdQ_t *q);
//u8 app_cmdQPop(cmdQ_t *q);
