#pragma once

#include "./app/usbcdc.h"
#include "../drv_uart.h"
#define DFLT_BAUD_RATE  115200



void usb_uart_open(u32 baudrate, uart_recvCb_t cb);
usbcdc_sts_t usb_uart_write(usbcdc_txBuf_t *pTxBuf);
void usb_uart_clearQ(void);
void usb_uart_loopQ(void);
