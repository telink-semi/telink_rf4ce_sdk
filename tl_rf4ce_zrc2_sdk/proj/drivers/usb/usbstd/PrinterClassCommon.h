#pragma once
	/* Includes: */
#include "../../../common/types.h"
	
	/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif
	

/* Macros: */
	/** \name Virtual Printer Status Line Masks */
	//@{

	#define PRNT_PORTSTATUS_NOTERROR	(1 << 3)
	#define PRNT_PORTSTATUS_SELECT		(1 << 4)
	#define PRNT_PORTSTATUS_PAPEREMPTY	(1 << 5)
	//@}

/* Enums: */
	enum PRNT_Descriptor_ClassSubclassProtocol_t{
		PRNT_CSCP_PrinterClass			= 0x07,
		PRNT_CSCP_PrinterSubclass		= 0x01,
		PRNT_CSCP_BidirectionalProtocol = 0x02,
	};

	/** Enum for the Printer class specific control requests that can be issued by the USB bus host. */
	enum PRNT_ClassRequests_t{
		PRNT_REQ_GetDeviceID			= 0x00,
		PRNT_REQ_GetPortStatus			= 0x01,
		PRNT_REQ_SoftReset				= 0x02,
	};

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif

