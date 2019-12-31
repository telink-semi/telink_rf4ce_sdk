
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

/* Defines: */
/** \name Endpoint Direction Masks */
//@{
#define ENDPOINT_DIR_MASK				   0x80
#define ENDPOINT_DIR_OUT				   0x00
#define ENDPOINT_DIR_IN 				   0x80
//@}

/** \name Endpoint/Pipe Type Masks */
//@{
/** Mask for determining the type of an endpoint from an endpoint descriptor. This should then be compared
 *	with the \c EP_TYPE_* masks to determine the exact type of the endpoint.
 */
#define EP_TYPE_MASK					   0x03
#define EP_TYPE_CONTROL 				   0x00
#define EP_TYPE_ISOCHRONOUS 			   0x01
#define EP_TYPE_BULK					   0x02
#define EP_TYPE_INTERRUPT				   0x03

/**If Isochronous endpoint,
Bits 3..2 = Synchronisation Type (Iso Mode)
00 = No Synchonisation
01 = Asynchronous
10 = Adaptive
11 = Synchronous
Bits 5..4 = Usage Type (Iso Mode)
00 = Data Endpoint
01 = Feedback Endpoint
10 = Explicit Feedback Data Endpoint
11 = Reserved
*/
#define EP_SYNC_TYPE_NO_SYNC                0
#define EP_SYNC_TYPE_ASYN                   1
#define EP_SYNC_TYPE_ADAPTIVE               2
#define EP_SYNC_TYPE_SYNC                   3

#define EP_USAGE_TYPE_DATA                  0
#define EP_USAGE_TYPE_FEEDBACK              1
#define EP_USAGE_TYPE_FEEDBACK_DATA         2
#define EP_USAGE_TYPE_RSV                   3

//@}

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

