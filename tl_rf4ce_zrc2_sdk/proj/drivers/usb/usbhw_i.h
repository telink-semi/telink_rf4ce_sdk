
#pragma once

#include "../../tl_common.h"

#ifdef WIN32
	#include "../simu/usb_simu.h"
	#include <stdio.h>
#endif

static inline void usbhw_set_printer_threshold(u8 th) {
	reg_usb_ep8_send_thre = th;
}

static inline u32 usbhw_get_ctrl_ep_irq(void) {
	return reg_ctrl_ep_irq_sta;
}

static inline void usbhw_clr_ctrl_ep_irq(int irq) {
#ifdef WIN32
	CLR_FLD(reg_ctrl_ep_irq_sta, irq);
#else
	reg_ctrl_ep_irq_sta = irq;
#endif
}
static inline void usbhw_write_ctrl_ep_ctrl(u8 data) {
	reg_ctrl_ep_ctrl = data;
}

// Reset the buffer pointer
static inline void usbhw_reset_ctrl_ep_ptr(void) {
	reg_ctrl_ep_ptr = 0;
}

#if 0
#define usbhw_read_ctrl_ep_data()	(reg_ctrl_ep_dat)
#else
static inline u8 usbhw_read_ctrl_ep_data(void) {
#ifdef WIN32
	return 0;// usb_sim_ctrl_ep_buffer[usb_sim_ctrl_ep_ptr++];
#else
	return reg_ctrl_ep_dat;
#endif
}
#endif

static inline void usbhw_write_ctrl_ep_data(u8 data) {
	reg_ctrl_ep_dat = data;
#ifdef WIN32
	printf("%02x,", data);
#endif
}

static inline bool usbhw_is_ctrl_ep_busy() {
	return reg_ctrl_ep_irq_sta & FLD_USB_EP_BUSY;
}

static inline u8 usbhw_read_ep_data(u32 ep) {
	return reg_usb_ep_dat(ep & 0x07);
}

static inline void usbhw_write_ep_data(u32 ep, u8 data) {
	reg_usb_ep_dat(ep & 0x07) = data;
#ifdef WIN32
	printf("%02x,", data);
#endif
}

static inline u32 usbhw_is_ep_busy(u32 ep) {
	return reg_usb_ep_ctrl(ep & 0x07) & FLD_USB_EP_BUSY;
}

static inline void usbhw_data_ep_ack(u32 ep) {
	reg_usb_ep_ctrl(ep & 0x07) = FLD_USB_EP_BUSY;
}

static inline void usbhw_data_ep_stall(u32 ep) {
	reg_usb_ep_ctrl(ep & 0x07) = FLD_USB_EP_STALL;
}

static inline void usbhw_reset_ep_ptr(u32 ep) {
	reg_usb_ep_ptr(ep & 0x07) = 0;
}

