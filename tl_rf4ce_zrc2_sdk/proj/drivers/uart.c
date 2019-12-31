#include "../tl_common.h"

/**********************************************************************
 * INCLUDES
 */
#include "uart.h"

#if USB_CDC_ENABLE
#include "./usb/app/usbcdc.h"
#endif

#if DMA_UART_ENABLE
#include "dma_uart_ctrl.h"
#endif


/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * LOCAL TYPES
 */
typedef struct {
    u8  len;
    u8* txBuf;
} uart_txPendingEvt_t;

/**********************************************************************
 * LOCAL VARIABLES
 */
uart_recvCb_t uart_cbFn = NULL;
//ev_queue_t    uart_txPendingQ;


/**********************************************************************
 * GLOBAL VARIABLES
 */
#if DMA_UART_ENABLE
extern DmaUart_ctrl_t *DmaUart_v;
#endif
uart_txPendingEvt_t uart_txPendingEvt_v;
/**********************************************************************
 * LOCAL FUNCTIONS
 */



/*********************************************************************
  * @fn      uart_rxHandler
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
void uart_rxHandler(u8* pData)
{

	/* Post task to handle the received command */
	//EV_SCHEDULE_TASK(uart_cbFn, pData);
	ev_on_timer(uart_cbFn, pData, 1*1000);


#if USB_CDC_ENABLE
	u8* pBuf;

	/* Allocate a new buffer to RX */
	#ifdef DMA_RX_BUFFER_SIZE
    pBuf = ev_buf_allocate(DMA_RX_BUFFER_SIZE);

	#else
	pBuf = ev_buf_allocate(LARGE_BUFFER);
	#endif
	if (!pBuf) {
		while(1); // Used in Debug
	}
    memset(pBuf, 0, 150);
	/* Set RX buffer to USB-CDC */
	usbcdc_setRxBuf(pBuf);

#elif DMA_UART_ENABLE
	/*Do nothing here for DMA UART*/

#endif



}

/*********************************************************************
  * @fn      uart_tXFinishCb
  *
  * @brief   This function is called by usb interrupt.
  *          In this function, we free the tx buffer first and then check
  *          is there any packets in the tx pending queue. If any, send it.
  *
  * @param   pData - Pointer to the already sent tx buffer.
  *
  * @return  None
  */
#if 0
void uart_txFinishCb(u8* pData)
{
	uart_txPendingEvt_t* pEvt;
    u8 len;
    u8 *p;

	/*For USB-UART, the data seperate packet is done by CDC mechanism,
	  and for DMA-UART, the data seperate packet is done directly here*/
#if USB_CDC_ENABLE
    /* Free the TX buffer at first */
    ev_buf_free(pData);

#elif DMA_UART_ENABLE
	/*If the packet being sent is larger than 20 bytes, DmaUart_v->lenToSend is larger than 0,
	  so we need to continue sending data via function DmaUart_sendBulkData*/
	if((DmaUart_v->lenToSend) > 0)
	{
		DmaUart_sendBulkData();
		return;
	}

	/*If DmaUart_v->lenToSend equal to 0, it means that last packet is sent completely,
	  we need to free the storage*/
	if(DmaUart_v->lenToSend == 0)
	{
		DmaUart_v->lastSendIndex = 0;
		/* Free the TX buffer*/
    	ev_buf_free(pData);
		DmaUart_v->txBuf = NULL;
	}
#endif

    /* If there is pending data, send it again */
    if ( uart_txPendingQ.curNum ) {
        pEvt = (uart_txPendingEvt_t*)ev_queue_pop(&uart_txPendingQ);
        p = pEvt->txBuf;
        len = pEvt->len;
        

        uart_write(p, len);
    }
}

#endif
static u8 uart_isAvailable(void)
{
#if USB_CDC_ENABLE
    return usbcdc_isAvailable();
#elif DMA_UART_ENABLE
	return DmaUart_isAvailable();
#endif
	return NULL;
}


/**
  * @brief      Initialize UART Module
  *
  * @param[in]  baudrate - Baudrate for the uart connection
  * @param[in]  cb       - Uart msg receive callback function
  *
  * @return     None
  */
#if 0
void uart_open(u32 baudrate, uart_recvCb_t cb)
{

#if USB_CDC_ENABLE
	u8* pBuf;

  #ifdef DMA_RX_BUFFER_SIZE
    pBuf = ev_buf_allocate(DMA_RX_BUFFER_SIZE);
  #else
    pBuf = ev_buf_allocate(LARGE_BUFFER);
  #endif
	if (!pBuf) {
		while(1);
	}
	usbcdc_setRxBuf(pBuf);
    /* Initialize USB-UART */
    usbcdc_setCB(uart_rxHandler, uart_txFinishCb);

#elif DMA_UART_ENABLE
	/*Because DMA-UART don't use usb cdc mechanism, so it call uart_rxHandler directly
	 *in the interrupt, we don't need use usbcdc_setCB to assign it to cdc_v->rxCb*/
    DmaUartInit(baudrate);

#else
    while(1); //for debug
#endif

	/* Register callback to handle received command */
	uart_cbFn = cb;

    /* Initialize USB tx pending Queue */
    ev_queue_init(&uart_txPendingQ, NULL);
}
#endif

/**
  * @brief      Send UART data
  *
  * @param[in]  buf - Buffer to be sent
  * @param[in]  len - Length of msg to be sent
  *
  * @return     status
  */
#if 0
uart_sts_t uart_write(u8* buf, u8 len)
{
	uart_txPendingEvt_t* pEvt;
	uart_sts_t SendDataResult = UART_SUCC; // if neither USB_CDC_ENABLE nor DMA_UART_ENABLE

    /* Check the usb/uart status first. If it is idle, send data dirctly.
     *If it is busy or it is failed to send data, put data to the pending queue and send it after
     *current operation done. */
    if (uart_isAvailable())
	{
		#if USB_CDC_ENABLE
			SendDataResult = usbcdc_sendData(buf, len);
		#elif DMA_UART_ENABLE
			SendDataResult = DmaUart_sendData(buf, len);
		#endif

		/*Send data successfully*/
		if(SendDataResult == UART_SUCC)
		{
			return UART_SUCC;
		}

	}

	/*In case of usb/uart not be available or send data unseccessfully*/

    pEvt = &uart_txPendingEvt_v;
    
    pEvt->len = len;
    pEvt->txBuf = buf;
    ev_queue_push(&uart_txPendingQ, (u8*)pEvt);
    return UART_MULTIBLOCK;

}
#endif
