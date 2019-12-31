
#pragma once

/* Includes: */
#include "../common/types.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif


/* Type Defines: */
typedef struct usbhid_config_t{
	u8  InterfaceNumber;
	u8  ReportINEndpointNumber;
	u16 ReportINEndpointSize;
	bool ReportINEndpointDoubleBank;
	void* PrevReportINBuffer;
	u8  PrevReportINBufferSize;
} usbhid_config_t;


struct usbhid_state_t{
	bool UsingReportProtocol;
	u16 PrevFrameNum;
	u16 IdleCount;
	u16 IdleMSRemaining;
} usbhid_state_t;

typedef struct{
} USB_ClassInfo_HID_Device_t;


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif

/** @} */


