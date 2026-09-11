/********************************************************************************************************
 * @file    putchar.h
 *
 * @brief   This is the header file for B86
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
#include "../../proj/common/types.h"
#include "../../proj/common/utility.h"
#ifndef WIN32

enum {
	USB_EDP_PRINTER_IN = 8, // endpoint 8 is alias of enpoint 0,  becareful.  // default hw buf len = 64
	USB_EDP_KEYBOARD_IN = 1,	// default hw buf len = 8  -- /*Alen modify*/
	USB_EDP_HOTKEY = 2,	        // default hw buf len = 8  -- /*Alen modify*/
	USB_EDP_MOUSE = 3,          // default hw buf len = 16 -- /*Alen modify*/
	USB_EDP_AUDIO_IN = 4,		// default hw buf len = 64
	USB_EDP_PRINTER_OUT = 5,	// default hw buf len = 64
	USB_EDP_SPEAKER = 6,		// default hw buf len = 16
	USB_EDP_MIC = 7,			// default hw buf len = 16
	USB_EDP_MS_IN = USB_EDP_PRINTER_IN,		// mass storage
	USB_EDP_MS_OUT = USB_EDP_PRINTER_OUT,
	USB_EDP_SOMATIC_IN = USB_EDP_AUDIO_IN,		//  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
	USB_EDP_SOMATIC_OUT = USB_EDP_PRINTER_OUT,
    USB_EDP_CDC_IN = 4,
    USB_EDP_CDC_OUT = 5,
};

// #defined in the standard spec
enum {
	USB_HID_AUDIO       	= 1,
	USB_HID_KB_MEDIA    	= 1,// media
	USB_HID_MOUSE           = 1,
	USB_HID_KB_SYS      	= 2,// system : power,sleep,wakeup
	USB_HID_KB_SPEC         = 3,
	USB_HID_SOMATIC			= 5,// somatic sensor,  may have many report ids
	USB_HID_MAX,
};



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


static inline void usbhw_write_ctrl_ep_ctrl(u8 data) {
	reg_ctrl_ep_ctrl = data;
}

// Reset the buffer pointer
static inline void usbhw_reset_ctrl_ep_ptr(void) {
	reg_ctrl_ep_ptr = 0;
}

static inline void usbhw_write_ctrl_ep_data(u8 data) {
	reg_ctrl_ep_dat = data;
#ifdef WIN32
	printf("%02x,", data);
#endif
}

static inline void usbhw_clr_ctrl_ep_irq(int irq) {
#ifdef WIN32
	CLR_FLD(reg_ctrl_ep_irq_sta, irq);
#else
	reg_ctrl_ep_irq_sta = irq;
#endif
}

static inline u32 usbhw_get_ctrl_ep_irq(void) {
	return reg_ctrl_ep_irq_sta;
}

static inline u8 usbhw_read_ep_data(u32 ep) {
	return reg_usb_ep_dat(ep & 0x07);
}



int hw_usb_putc(int c);
void usbhw_disable_manual_interrupt(int m);
void usbhw_enable_manual_interrupt(int m);
void usbhw_write_ep(u32 ep, u8 * data, int len);
void usbhw_write_ctrl_ep_u16(u16 v);
u16 usbhw_read_ctrl_ep_u16(void);
#endif
