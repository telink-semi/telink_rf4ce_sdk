/********************************************************************************************************
 * @file    TH_api.h
 *
 * @brief   This is the header file for TH_api.h
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

#include "../../../proj/drivers/usb/usb_uart.h"

typedef void (*th_recvCb_t)(u8*);

#if !USB_CDC_ENABLE
typedef struct{
	u8 len;
	u8 host_cmdId;
	u8 device_cmdID;
	u8 param[8];
}usb_command_t;
#endif

int recvCmdFromTH(u8 *recvBuf, u8 bufLen);
int sendCmdToTH(usbcdc_txBuf_t *buf);
void TH_trxInit(uart_recvCb_t cb);
