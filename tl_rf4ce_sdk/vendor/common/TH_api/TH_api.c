/********************************************************************************************************
 * @file    TH_api.c
 *
 * @brief   This is the source file for TH_api.c
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

#include "../../../proj/tl_common.h"


#ifdef WIN32
#include "../../../proj/simu/socket_api.h"
#include "../../../proj/app/usbcdc.h"
#include "../../../proj/drivers/usb_uart.h"
#else
#include "../../../proj/drivers/usb/app/usbcdc.h"
#include "../../../proj/drivers/usb/usb_uart.h"
#endif

#include "TH_api.h"




int recvCmdFromTH(u8 *recvBuf, u8 bufLen)
{
#ifdef WIN32
    return simu_recvFromTH(recvBuf, bufLen);
#else
    /* TODO: add USB mode */
    return 0;
#endif

}

int sendCmdToTH(usbcdc_txBuf_t *buf)
{
#ifdef WIN32
    return simu_sendToTH(buf, len);
#else
#if USB_CDC_ENABLE
    usb_uart_write(buf);
#else
    //usb
	usb_command_t *p_usbCmd = (usb_command_t *)0x808004;
    if(!p_usbCmd->device_cmdID && !p_usbCmd->host_cmdId){
    	p_usbCmd->len = buf->len - 1;
    	if(p_usbCmd->len < 8){
    		p_usbCmd->device_cmdID = buf->data[1];
    		memcpy(&p_usbCmd->param[0], &buf[2], p_usbCmd->len);
    	}    	
    }
	ev_buf_free((u8 *)buf);
#endif
    return 0;
#endif
}





void TH_trxInit(uart_recvCb_t cb)
{
#ifdef WIN32
    socket_param_init(TEST_SOCKET);
#else
#if USB_CDC_ENABLE
	usb_init();
    /* Init USB-UART Transceiver */
    usb_uart_open(DFLT_BAUD_RATE, cb);
#endif
#endif
}



