/********************************************************************************************************
 * @file    usb_uart.c
 *
 * @brief   This is the source file for usb_uart.c
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

#include "../../tl_common.h"

#if USB_CDC_ENABLE

/**********************************************************************
 * INCLUDES
 */
#include "usb_uart.h"
#include "./app/usbcdc.h"
#if (RF4CE_TARGET)
#include "../../os/ev_queue.h"
#endif



/**********************************************************************
 * LOCAL CONSTANTS
 */



/**********************************************************************
 * LOCAL TYPES
 */
typedef struct {
    u8* txBuf;
    u8  len;
} usb_uart_txPendingEvt_t;



/**********************************************************************
 * LOCAL VARIABLES
 */
uart_recvCb_t usb_uart_cbFn = NULL;
ev_queue_t    usb_uart_txPendingQ;

usb_uart_txPendingEvt_t usb_uart_txPendingEvt_v;
/**********************************************************************
 * LOCAL FUNCTIONS
 */



/*********************************************************************
  * @fn      usb_uart_rxHandler
  *
  * @brief   Internal rx handler from usb_cdc module. This function is 
  *          called from USB interrupt.
  *          In this function, the user reigsted handler will be called 
  *          through task mechnism, and then allocate a new buffer for 
  *          usb driver to continue receive.
  *
  * @param   pData - The received usb-uart packets
  *
  * @return  None
  */
static void usb_uart_rxHandler(u8* pData)
{
	u8* pBuf;

	/* Post task to handle the received command */
	//EV_SCHEDULE_TASK((ev_task_callback_t)usb_uart_cbFn, pData);
	usb_uart_cbFn(pData);
	//ev_on_timer(usb_uart_cbFn,pData, 1);
	/* Set the USB RX buffer again */
    pBuf = ev_buf_allocate(LARGE_BUFFER);
	if (!pBuf) {
		while(1);
	}

    memset(pBuf, 0, 150);
	usbcdc_setRxBuf(pBuf);
}

/*********************************************************************
  * @fn      usb_uart_tXFinishCb
  *
  * @brief   This function is called by usb interrupt.
  *          In this function, we free the tx buffer first and then check
  *          is there any packets in the tx pending queue. If any, send it.
  *
  * @param   pData - Pointer to the already sent tx buffer.
  *
  * @return  None
  */
static void usb_uart_tXFinishCb(u8* pData)
{
    /* Free the TX buffer at first */
    ev_buf_free(pData);
}



/*********************************************************************
  * @fn      usb_uart_open
  *
  * @brief   API to open usb-uart.
  *
  * @param   baudrate - The baudrate for the uart connection
  *          cb       - The uart receive callback function
  *
  * @return  None
  */
void usb_uart_open(u32 baudrate, uart_recvCb_t cb)
{
	u8* pBuf;

	/* Initialize USB-UART */
    usbcdc_setCB(usb_uart_rxHandler, usb_uart_tXFinishCb);
	pBuf = ev_buf_allocate(LARGE_BUFFER);
	if (!pBuf) {
		while(1);
	}

	/* Set RX buffer to USB-CDC */
	usbcdc_setRxBuf(pBuf);

	/* Register callback to handle received command */
	usb_uart_cbFn = cb;

    /* Initialize USB tx pending Queue */
    ev_queue_init(&usb_uart_txPendingQ, NULL);
}

/*********************************************************************
  * @fn      usb_uart_write
  *
  * @brief   API to send usb-uart data.
  *
  * @param   buf - The buffer to be sent
  *          len - The length to be sent
  *
  * @return  status
  */
usbcdc_sts_t usb_uart_write(usbcdc_txBuf_t *pTxBuf)
{
    /* Check the usb status first. If it is idle, send data dirctly.
    If it is busy, put data to the pending queue and send it after
    current operation done. */
    if (usbcdc_isAvailable()) {
        return usbcdc_sendData(pTxBuf);
    } else {
    	if(usb_uart_txPendingQ.curNum>=3)//limit the audio occupies 3 buffers
    	{
			u8 *pData = (u8 *)ev_queue_pop(&usb_uart_txPendingQ);
			ev_buf_free(pData);
    	}
        ev_queue_push(&usb_uart_txPendingQ, (u8*)pTxBuf);
        return USB_MULTIBLOCK;
    }
}

/*********************************************************************
  * @fn      usb_uart_clearQ
  *
  * @brief   clear the queue of usb cdc
  *
  * @param   none
  *
  * @return  none
  */
void usb_uart_clearQ(void)
{
	u8 size = usb_uart_txPendingQ.curNum;
	if(size)
	{
		for(u32 i=0;i<size;i++)
		{
		        u8 *pData = (u8 *)ev_queue_pop(&usb_uart_txPendingQ);
			    ev_buf_free(pData);
		}
	}
}

/*********************************************************************
  * @fn      usb_uart_loopQ
  *
  * @brief   transmit the data of usbcdc queue when usbcdc is idle
  *
  * @param   none
  *
  * @return  none
  */
void usb_uart_loopQ(void)
{
    /* If there is pending data, send it again */
    if(usb_uart_txPendingQ.curNum && usbcdc_isAvailable()) {
        u8 *p = (u8 *)ev_queue_pop(&usb_uart_txPendingQ);
        usb_uart_write((usbcdc_txBuf_t *)p);
    }
}




#endif  /* USB_CDC_ENABLE */
