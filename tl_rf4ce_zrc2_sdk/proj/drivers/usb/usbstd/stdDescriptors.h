
#pragma once

/* Includes: */
#include "../../../common/types.h"
#include "../../../common/compiler.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif

#define NO_DESCRIPTOR                     0

#define USB_CONFIG_POWER_MA(mA)           ((mA) >> 1)

#define USB_STRING_LEN(UnicodeChars)      (sizeof(USB_Descriptor_Header_t) + ((UnicodeChars) << 1))

#define LANGUAGE_ID_ENG                   0x0409

/** \name USB Configuration Descriptor Attribute Masks */
//@{
#define USB_CONFIG_ATTR_RESERVED          0x80
#define USB_CONFIG_ATTR_SELFPOWERED       0x40
#define USB_CONFIG_ATTR_REMOTEWAKEUP      0x20
//@}

/** \name Endpoint Descriptor Attribute Masks */
//@{
#define ENDPOINT_ATTR_NO_SYNC             (0 << 2)
#define ENDPOINT_ATTR_ASYNC               (1 << 2)
#define ENDPOINT_ATTR_ADAPTIVE            (2 << 2)
#define ENDPOINT_ATTR_SYNC                (3 << 2)
//@}

/** \name Endpoint Descriptor Usage Masks */
//@{
#define ENDPOINT_USAGE_DATA               (0 << 4)
#define ENDPOINT_USAGE_FEEDBACK           (1 << 4)
#define ENDPOINT_USAGE_IMPLICIT_FEEDBACK  (2 << 4)
//@}

/* Enums: */
/** Enum for the possible standard descriptor types, as given in each descriptor's header. */
enum USB_DescriptorTypes_t
{
	DTYPE_Device                    = 0x01,
	DTYPE_Configuration             = 0x02,
	DTYPE_String                    = 0x03,
	DTYPE_Interface                 = 0x04,
	DTYPE_Endpoint                  = 0x05,
	DTYPE_DeviceQualifier           = 0x06,
	DTYPE_Other                     = 0x07,
	DTYPE_InterfacePower            = 0x08,
	DTYPE_InterfaceAssociation      = 0x0B,
	DTYPE_CSInterface               = 0x24,
	DTYPE_CSEndpoint                = 0x25,
};

/** Enum for possible Class, Subclass and Protocol values of device and interface descriptors. */
enum USB_Descriptor_ClassSubclassProtocol_t
{
	USB_CSCP_NoDeviceClass          = 0x00,
	USB_CSCP_NoDeviceSubclass       = 0x00,
	USB_CSCP_NoDeviceProtocol       = 0x00,
	USB_CSCP_VendorSpecificClass    = 0xFF,
	USB_CSCP_VendorSpecificSubclass = 0xFF,
	USB_CSCP_VendorSpecificProtocol = 0xFF,
	USB_CSCP_IADDeviceClass         = 0xEF,
	USB_CSCP_IADDeviceSubclass      = 0x02,
	USB_CSCP_IADDeviceProtocol      = 0x01,
};

/* Type Defines: */
typedef struct{
	u8 Size;
	u8 Type;
}USB_Descriptor_Header_t;

/** \brief Standard USB Descriptor Header (USB-IF naming conventions).
 */
typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
}USB_StdDescriptor_Header_t;

/** \brief Standard USB Device Descriptor (LUFA naming conventions).
 */
typedef struct{
	USB_Descriptor_Header_t Header;
	u16 USBSpecification;
	u8  Class;
	u8  SubClass;
	u8  Protocol;
	u8  Endpoint0Size;
	u16 VendorID;
	u16 ProductID;
	u16 ReleaseNumber;
	u8  ManufacturerStrIndex;
	u8  ProductStrIndex;
	u8  SerialNumStrIndex;
	u8  NumberOfConfigurations;
}USB_Descriptor_Device_t;


/** \brief Standard USB Device Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8  bLength;
	u8  bDescriptorType;
	u16 bcdUSB;
	u8  bDeviceClass;
	u8  bDeviceSubClass;
	u8  bDeviceProtocol;
	u8  bMaxPacketSize0;
	u16 idVendor;
	u16 idProduct;
	u16 bcdDevice;
	u8  iManufacturer;
	u8  iProduct;
	u8 iSerialNumber;
	u8  bNumConfigurations;
}USB_StdDescriptor_Device_t;


typedef struct
{
	USB_Descriptor_Header_t Header;
	u16 USBSpecification;
	u8  Class;
	u8  SubClass;
	u8  Protocol;
	u8  Endpoint0Size;
	u8  NumberOfConfigurations;
	u8  Reserved;
}USB_Descriptor_DeviceQualifier_t;


typedef struct
{
	u8  bLength;
	u8  bDescriptorType;
	u16 bcdUSB;
	u8  bDeviceClass;
	u8  bDeviceSubClass;
	u8  bDeviceProtocol;
	u8  bMaxPacketSize0;
	u8  bNumConfigurations;
	u8  bReserved;
}USB_StdDescriptor_DeviceQualifier_t;


typedef struct
{
	USB_Descriptor_Header_t Header;
	u16 TotalConfigurationSize;
	u8  TotalInterfaces;
	u8  ConfigurationNumber;
	u8  ConfigurationStrIndex;
	u8  ConfigAttributes;
	u8  MaxPowerConsumption;
}USB_Descriptor_Configuration_Header_t;


typedef struct
{
	u8  bLength;
	u8  bDescriptorType;
	u16 wTotalLength;
	u8  bNumInterfaces;
	u8  bConfigurationValue;
	u8  iConfiguration;
	u8  bmAttributes;
	u8  bMaxPower;
}USB_StdDescriptor_Configuration_Header_t;


typedef struct
{
	USB_Descriptor_Header_t Header;
	u8 InterfaceNumber;
	u8 AlternateSetting;
	u8 TotalEndpoints;
	u8 Class;
	u8 SubClass;
	u8 Protocol;
	u8 InterfaceStrIndex;
}USB_Descriptor_Interface_t;


typedef struct
{
	u8 bLength;
	u8 bDescriptorType;
	u8 bInterfaceNumber;
	u8 bAlternateSetting;
	u8 bNumEndpoints;
	u8 bInterfaceClass;
	u8 bInterfaceSubClass;
	u8 bInterfaceProtocol;
	u8 iInterface;
}USB_StdDescriptor_Interface_t;


typedef struct{
	USB_Descriptor_Header_t Header;
	u8 FirstInterfaceIndex;
	u8 TotalInterfaces;
	u8 Class;
	u8 SubClass;
	u8 Protocol;
	u8 IADStrIndex;
}USB_Descriptor_Interface_Association_t;


typedef struct{
	u8 bLength;
	u8 bDescriptorType;
	u8 bFirstInterface;
	u8 bInterfaceCount;
	u8 bFunctionClass;
	u8 bFunctionSubClass;
	u8 bFunctionProtocol;
	u8 iFunction;
}USB_StdDescriptor_Interface_Association_t;


typedef struct{
	USB_Descriptor_Header_t Header;
	u8  EndpointAddress;
	u8  Attributes;
	u16 EndpointSize;
	u8  PollingIntervalMS;
}USB_Descriptor_Endpoint_t;


typedef struct{
	u8  bLength;
	u8  bDescriptorType;
	u8  bEndpointAddress;
	u8  bmAttributes;
	u16 wMaxPacketSize;
	u8  bInterval;
}USB_StdDescriptor_Endpoint_t;


typedef struct{
	USB_Descriptor_Header_t Header;
	wchar_t UnicodeString[];
}USB_DescriptorString_t;


typedef struct{
	u8 bLength;
	u8 bDescriptorType;
	wchar_t bString[];
}USB_StdDescriptor_String_t;

#ifndef __GNUC__
#pragma warning(pop)
#endif


#if defined(__cplusplus)
	}
#endif

/** @} */


