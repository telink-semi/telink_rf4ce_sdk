#pragma once

#include "../common/types.h"
#include "../common/static_assert.h"
#include "../common/bit.h"
#include "../usbstd/HIDClassCommon.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif


#define MEDIAKEY_REPORT_DATA_LEN	2
#define MOUSE_REPORT_DATA_LEN     (sizeof(mouse_data_t))
#define SPECIAL_REPORT_DATA_LEN	  2


int usbmouse_hid_report(u8 report_id, u8 *data, int cnt);


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif