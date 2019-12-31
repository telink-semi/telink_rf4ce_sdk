#pragma once

#include "../../common/types.h"
#include "../../common/bit.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif

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

void usbhw_disable_manual_interrupt(int m);
void usbhw_enable_manual_interrupt(int m);
void usbhw_write_ep(u32 ep, u8 * data, int len);
void usbhw_write_ctrl_ep_u16(u16 v);
u16 usbhw_read_ctrl_ep_u16(void);

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

