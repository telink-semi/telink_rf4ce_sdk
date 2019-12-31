
#ifndef _CDC_CLASS_DEVICE_H_
#define _CDC_CLASS_DEVICE_H_

/* Includes: */
#include "../../../common/types.h"
#include "CDCClassCommon.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    extern "C" {
#endif


typedef struct{
	struct{
		u8       ControlInterfaceNumber;
		u8       DataINEndpointNumber;
		u16      DataINEndpointSize;
		bool     DataINEndpointDoubleBank;

		u8       DataOUTEndpointNumber;
		u16      DataOUTEndpointSize;
		bool     DataOUTEndpointDoubleBank;

		u8       NotificationEndpointNumber;
		u16      NotificationEndpointSize;
		bool     NotificationEndpointDoubleBank;
	} Config;

	struct{
		struct{
			u16      HostToDevice;
			u16      DeviceToHost;
		} ControlLineStates;
		CDC_LineEncoding_t LineEncoding;
	} State;

} USB_ClassInfo_CDC_Device_t;


#if defined(__cplusplus)
	}
#endif

#endif

/** @} */

