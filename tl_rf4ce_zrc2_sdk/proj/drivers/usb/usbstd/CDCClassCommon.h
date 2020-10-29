
#ifndef _CDC_CLASS_COMMON_H_
#define _CDC_CLASS_COMMON_H_

/* Includes: */
#include "../../../common/types.h"
#include "stdDescriptors.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif



/* Macros: */

#define CDC_CONTROL_LINE_OUT_DTR         (1 << 0)
#define CDC_CONTROL_LINE_OUT_RTS         (1 << 1)
#define CDC_CONTROL_LINE_IN_DCD          (1 << 0)
#define CDC_CONTROL_LINE_IN_DSR          (1 << 1)
#define CDC_CONTROL_LINE_IN_BREAK        (1 << 2)
#define CDC_CONTROL_LINE_IN_RING         (1 << 3)
#define CDC_CONTROL_LINE_IN_FRAMEERROR   (1 << 4)
#define CDC_CONTROL_LINE_IN_PARITYERROR  (1 << 5)
#define CDC_CONTROL_LINE_IN_OVERRUNERROR (1 << 6)
//@}


#define CDC_FUNCTIONAL_DESCRIPTOR(DataSize)        \
	 struct                                        \
	 {                                             \
		  USB_Descriptor_Header_t Header;          \
		  uint8_t                 SubType;         \
		  uint8_t                 Data[DataSize];  \
	 }


/* Enums: */
enum CDC_Descriptor_ClassSubclassProtocol_t{
	CDC_CSCP_CDCClass               = 0x02,
	CDC_CSCP_NoSpecificSubclass     = 0x00,
	CDC_CSCP_ACMSubclass            = 0x02,
	CDC_CSCP_ATCommandProtocol      = 0x01,
	CDC_CSCP_NoSpecificProtocol     = 0x00,
	CDC_CSCP_VendorSpecificProtocol = 0xFF,
	CDC_CSCP_CDCDataClass           = 0x0A,
	CDC_CSCP_NoDataSubclass         = 0x00,
	CDC_CSCP_NoDataProtocol         = 0x00,
};

enum {
	CDC_REQ_SendEncapsulatedCommand = 0x00,
	CDC_REQ_GetEncapsulatedResponse = 0x01,
	CDC_REQ_SetLineEncoding         = 0x20,
	CDC_REQ_GetLineEncoding         = 0x21,
	CDC_REQ_SetControlLineState     = 0x22,
	CDC_REQ_SendBreak               = 0x23,
};

enum {
	CDC_NOTIF_SerialState = 0x20,
};


enum {
	CDC_DSUBTYPE_CSInterface_Header           = 0x00,
	CDC_DSUBTYPE_CSInterface_CallManagement   = 0x01,
	CDC_DSUBTYPE_CSInterface_ACM              = 0x02,
	CDC_DSUBTYPE_CSInterface_DirectLine       = 0x03,
	CDC_DSUBTYPE_CSInterface_TelephoneRinger  = 0x04,
	CDC_DSUBTYPE_CSInterface_TelephoneCall    = 0x05,
	CDC_DSUBTYPE_CSInterface_Union            = 0x06,
	CDC_DSUBTYPE_CSInterface_CountrySelection = 0x07,
	CDC_DSUBTYPE_CSInterface_TelephoneOpModes = 0x08,
	CDC_DSUBTYPE_CSInterface_USBTerminal      = 0x09,
	CDC_DSUBTYPE_CSInterface_NetworkChannel   = 0x0A,
	CDC_DSUBTYPE_CSInterface_ProtocolUnit     = 0x0B,
	CDC_DSUBTYPE_CSInterface_ExtensionUnit    = 0x0C,
	CDC_DSUBTYPE_CSInterface_MultiChannel     = 0x0D,
	CDC_DSUBTYPE_CSInterface_CAPI             = 0x0E,
	CDC_DSUBTYPE_CSInterface_Ethernet         = 0x0F,
	CDC_DSUBTYPE_CSInterface_ATM              = 0x10,
};

enum{
	CDC_LINEENCODING_OneStopBit          = 0,
	CDC_LINEENCODING_OneAndAHalfStopBits = 1,
	CDC_LINEENCODING_TwoStopBits         = 2,
};

enum{
	CDC_PARITY_None  = 0,
	CDC_PARITY_Odd   = 1,
	CDC_PARITY_Even  = 2,
	CDC_PARITY_Mark  = 3,
	CDC_PARITY_Space = 4,
};

/* Type Defines: */
typedef struct{
	USB_Descriptor_Header_t Header;
	u8                      Subtype;
	u16                     CDCSpecification;
} USB_CDC_Descriptor_FunctionalHeader_t;


typedef struct
{
	u8       bFunctionLength;
	u8       bDescriptorType;
	u8       bDescriptorSubType;
	u16      bcdCDC;
} USB_CDC_StdDescriptor_FunctionalHeader_t;


typedef struct{
	USB_Descriptor_Header_t Header;
	u8                      Subtype;
	u8                      Capabilities;
} USB_CDC_Descriptor_FunctionalACM_t;


typedef struct{
	u8      bFunctionLength;
	u8      bDescriptorType;
	u8      bDescriptorSubType;
	u8      bmCapabilities;
} USB_CDC_StdDescriptor_FunctionalACM_t;


typedef struct{
	USB_Descriptor_Header_t Header;
	u8                      Subtype;
	u8                      MasterInterfaceNumber;
	u8                      SlaveInterfaceNumber;
} USB_CDC_Descriptor_FunctionalUnion_t;


typedef struct{
	u8      bFunctionLength;
	u8      bDescriptorType;
	u8      bDescriptorSubType;
	u8      bMasterInterface;
	u8      bSlaveInterface0;
} USB_CDC_StdDescriptor_FunctionalUnion_t;


typedef struct{
	u32      BaudRateBPS;
	u8       CharFormat;
	u8       ParityType;
	u8       DataBits;
} CDC_LineEncoding_t;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif

#endif

/** @} */

