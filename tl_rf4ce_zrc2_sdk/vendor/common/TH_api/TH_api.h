
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
