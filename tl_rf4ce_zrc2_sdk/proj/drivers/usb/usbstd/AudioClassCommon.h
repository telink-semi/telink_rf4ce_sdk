
#pragma once

/* Includes: */
#include "../../../common/types.h"
#include "StdDescriptors.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

/* Macros: */
/** \name Audio Channel Masks */
//@{
#define AUDIO_CHANNEL_LEFT_FRONT           (1 << 0)
#define AUDIO_CHANNEL_RIGHT_FRONT          (1 << 1)
#define AUDIO_CHANNEL_CENTER_FRONT         (1 << 2)
#define AUDIO_CHANNEL_LOW_FREQ_ENHANCE     (1 << 3)
#define AUDIO_CHANNEL_LEFT_SURROUND        (1 << 4)
#define AUDIO_CHANNEL_RIGHT_SURROUND       (1 << 5)
#define AUDIO_CHANNEL_LEFT_OF_CENTER       (1 << 6)
#define AUDIO_CHANNEL_RIGHT_OF_CENTER      (1 << 7)
#define AUDIO_CHANNEL_SURROUND             (1 << 8)
#define AUDIO_CHANNEL_SIDE_LEFT            (1 << 9)
#define AUDIO_CHANNEL_SIDE_RIGHT           (1 << 10)
#define AUDIO_CHANNEL_TOP                  (1 << 11)
//@}

/** \name Audio Feature Masks */
//@{
#define AUDIO_FEATURE_MUTE                 (1 << 0)
#define AUDIO_FEATURE_VOLUME               (1 << 1)
#define AUDIO_FEATURE_BASS                 (1 << 2)
#define AUDIO_FEATURE_MID                  (1 << 3)
#define AUDIO_FEATURE_TREBLE               (1 << 4)
#define AUDIO_FEATURE_GRAPHIC_EQUALIZER    (1 << 5)
#define AUDIO_FEATURE_AUTOMATIC_GAIN       (1 << 6)
#define AUDIO_FEATURE_DELAY                (1 << 7)
#define AUDIO_FEATURE_BASS_BOOST           (1 << 8)
#define AUDIO_FEATURE_BASS_LOUDNESS        (1 << 9)
//@}

/** \name Audio Terminal Types */
//@{
#define AUDIO_TERMINAL_UNDEFINED           0x0100
#define AUDIO_TERMINAL_STREAMING           0x0101
#define AUDIO_TERMINAL_VENDOR              0x01FF

#define AUDIO_TERMINAL_IN_UNDEFINED        0x0200
#define AUDIO_TERMINAL_IN_MIC              0x0201
#define AUDIO_TERMINAL_IN_DESKTOP_MIC      0x0202
#define AUDIO_TERMINAL_IN_PERSONAL_MIC     0x0203
#define AUDIO_TERMINAL_IN_OMNIDIR_MIC      0x0204
#define AUDIO_TERMINAL_IN_MIC_ARRAY        0x0205
#define AUDIO_TERMINAL_IN_PROCESSING_MIC   0x0206

#define AUDIO_TERMINAL_IN_OUT_UNDEFINED    0x0300
#define AUDIO_TERMINAL_OUT_SPEAKER         0x0301
#define AUDIO_TERMINAL_OUT_HEADPHONES      0x0302
#define AUDIO_TERMINAL_OUT_HEAD_MOUNTED    0x0303
#define AUDIO_TERMINAL_OUT_DESKTOP         0x0304
#define AUDIO_TERMINAL_OUT_ROOM            0x0305
#define AUDIO_TERMINAL_OUT_COMMUNICATION   0x0306
#define AUDIO_TERMINAL_OUT_LOWFREQ         0x0307
//@}

/** Convenience macro to fill a 24-bit \ref USB_Audio_SampleFreq_t structure with the given sample rate as a 24-bit number.
 *
 *  \param[in] freq  Required audio sampling frequency in HZ
 */
#define AUDIO_SAMPLE_FREQ(freq)           {.Byte1 = ((uint32_t)freq & 0xFF), .Byte2 = (((uint32_t)freq >> 8) & 0xFF), .Byte3 = (((uint32_t)freq >> 16) & 0xFF)}

#define AUDIO_EP_FULL_PACKETS_ONLY        (1 << 7)
#define AUDIO_EP_ACCEPTS_SMALL_PACKETS    (0 << 7)
#define AUDIO_EP_SAMPLE_FREQ_CONTROL      (1 << 0)
#define AUDIO_EP_PITCH_CONTROL            (1 << 1)

/* Enums: */
enum Audio_Descriptor_ClassSubclassProtocol_t{
	AUDIO_CSCP_AudioClass                     = 0x01, 
	AUDIO_CSCP_ControlSubclass                = 0x01, 
	AUDIO_CSCP_ControlProtocol                = 0x00, 
	AUDIO_CSCP_AudioStreamingSubclass         = 0x02, 
	AUDIO_CSCP_MIDIStreamingSubclass          = 0x03, 
	AUDIO_CSCP_StreamingProtocol              = 0x00, 
};

enum Audio_CSInterface_AC_SubTypes_t{
	AUDIO_DSUBTYPE_CSInterface_Header         = 0x01, 
	AUDIO_DSUBTYPE_CSInterface_InputTerminal  = 0x02, 
	AUDIO_DSUBTYPE_CSInterface_OutputTerminal = 0x03, 
	AUDIO_DSUBTYPE_CSInterface_Mixer          = 0x04, 
	AUDIO_DSUBTYPE_CSInterface_Selector       = 0x05, 
	AUDIO_DSUBTYPE_CSInterface_Feature        = 0x06, 
	AUDIO_DSUBTYPE_CSInterface_Processing     = 0x07, 
	AUDIO_DSUBTYPE_CSInterface_Extension      = 0x08, 
};

enum Audio_CSInterface_AS_SubTypes_t{
	AUDIO_DSUBTYPE_CSInterface_General        = 0x01, 
	AUDIO_DSUBTYPE_CSInterface_FormatType     = 0x02, 
	AUDIO_DSUBTYPE_CSInterface_FormatSpecific = 0x03, 
};

enum {
	AUDIO_DSUBTYPE_CSEndpoint_General         = 0x01, /**< Audio class specific endpoint general descriptor. */
};

enum Audio_ClassRequests_t{
	AUDIO_REQ_SetCurrent    = 0x01, 
	AUDIO_REQ_SetMinimum    = 0x02, 
	AUDIO_REQ_SetMaximum    = 0x03, 
	AUDIO_REQ_SetResolution = 0x04, 
	AUDIO_REQ_SetMemory     = 0x05, 
	AUDIO_REQ_GetCurrent    = 0x81, 
	AUDIO_REQ_GetMinimum    = 0x82, 
	AUDIO_REQ_GetMaximum    = 0x83, 
	AUDIO_REQ_GetResolution = 0x84, 
	AUDIO_REQ_GetMemory     = 0x85, 
	AUDIO_REQ_GetStatus     = 0xFF, 
};

enum Audio_EndpointControls_t{
	AUDIO_EPCONTROL_SamplingFreq = 0x01, 
	AUDIO_EPCONTROL_Pitch        = 0x02, 
};

/* Type Defines: */
/** \brief Audio class-specific Input Terminal Descriptor (LUFA naming conventions).
 */
typedef struct
{
	USB_Descriptor_Header_t Header; 
	u8                 Subtype; 
	u8                 TerminalID; 
	u16                TerminalType; 
	u8                 AssociatedOutputTerminal; 
	u8                 TotalChannels;
	u16                ChannelConfig;
	u8                 ChannelStrIndex; 
	u8                 TerminalStrIndex;
}USB_Audio_Descriptor_InputTerminal_t;

/** \brief Audio class-specific Input Terminal Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8  bLength; 
	u8  bDescriptorType; 
	u8  bDescriptorSubtype; 
	u8  bTerminalID;
	u16 wTerminalType; 
	u8  bAssocTerminal; 
	u8  bNrChannels; 
	u16 wChannelConfig; 
	u8  iChannelNames; 
	u8  iTerminal;
}USB_Audio_StdDescriptor_InputTerminal_t;

/** \brief Audio class-specific Output Terminal Descriptor (LUFA naming conventions).
 *
 */
typedef struct
{
	USB_Descriptor_Header_t Header;
	u8                 Subtype; 
	u8                 TerminalID; 
	u16                TerminalType;
	u8                 AssociatedInputTerminal;
	u8                 SourceID;
	u8                 TerminalStrIndex;
}USB_Audio_Descriptor_OutputTerminal_t;

/** \brief Audio class-specific Output Terminal Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8  bLength; 		
	u8  bDescriptorType; 	
	u8  bDescriptorSubtype; 
	u8  bTerminalID; 	
	u16 wTerminalType; 	
	u8  bAssocTerminal; 	
	u8  bSourceID; 		
	u8  iTerminal; 		
}USB_Audio_StdDescriptor_OutputTerminal_t;

/** \brief Audio class-specific Interface Descriptor (LUFA naming conventions).
 *
 */
typedef struct
{
	USB_Descriptor_Header_t Header; 	
	u8            	   	Subtype; 	
	u8                 ACSpecification[2]; 
	u8                 TotalLength[2]; 	
	u8                 InCollection; 	
	u8                 InterfaceNumber; 	
}USB_Audio_Descriptor_Interface_AC_t;

/*surport mic && speaker*/
typedef struct
{
	USB_Descriptor_Header_t Header; 	
	u8                 Subtype; 		
	u8                 ACSpecification[2];
	u8                 TotalLength[2]; 	
	u8                 InCollection; 	
	u8                 InterfaceNumber_speaker;
	u8                 InterfaceNumber_mic; 
}USB_Audio_Descriptor_Interface_AC_TL_t;

/** \brief Audio class-specific Interface Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8  bLength; 	
	u8  bDescriptorType; 
	u8  bDescriptorSubtype;
	u8  bcdADC[2]; 	       
	u8 wTotalLength[2];    
	u8  bInCollection;     
	u8  bInterfaceNumbers; 
}USB_Audio_StdDescriptor_Interface_AC_t;

/** \brief Audio class-specific Feature Unit Descriptor (LUFA naming conventions).
 *
 */
typedef struct
{
	USB_Descriptor_Header_t Header; 
	u8                 Subtype; 	
	u8                 UnitID; 	
	u8                 SourceID; 	
	u8                 ControlSize; 
	u8                 ChannelControls[3]; 
	u8                 FeatureUnitStrIndex;
}USB_Audio_Descriptor_FeatureUnit_t;

/*just for usb_mic*/
typedef struct
{
	USB_Descriptor_Header_t Header; 
	u8                 Subtype; 
	u8                 UnitID; 
	u8                 SourceID; 
	u8                 ControlSize; 
	u8                 ChannelControls[2]; 
	u8                 FeatureUnitStrIndex; 
}USB_Audio_Descriptor_FeatureUnit_Mic_t;

/** \brief Audio class-specific Feature Unit Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8 bLength; 		
	u8 bDescriptorType; 	
	u8 bDescriptorSubtype; 
	u8 bUnitID; 		
	u8 bSourceID; 		
	u8 bControlSize; 	
	u8 bmaControls[3]; 	
	u8 iFeature; 		
}USB_Audio_StdDescriptor_FeatureUnit_t;

/** \brief Audio class-specific Streaming Audio Interface Descriptor (LUFA naming conventions).
 *
 */
typedef struct
{
	USB_Descriptor_Header_t Header; 
	u8                 Subtype; 
	u8                 TerminalLink; 
	u8                 FrameDelay; 
	u8                 AudioFormat[2]; 
}USB_Audio_Descriptor_Interface_AS_t;

/** \brief Audio class-specific Streaming Audio Interface Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8  bLength; 
	u8  bDescriptorType; 
	u8  bDescriptorSubtype; 
	u8  bTerminalLink; 
	u8  bDelay; 
	u16 wFormatTag; 
}USB_Audio_StdDescriptor_Interface_AS_t;

/** \brief Audio class-specific Format Descriptor (LUFA naming conventions).
 *
 */
typedef struct
{
	USB_Descriptor_Header_t Header; 
	u8                 Subtype; 
	u8                 FormatType; 
	u8                 Channels; 
	u8                 SubFrameSize; 
	u8                 BitResolution;
	u8                 TotalDiscreteSampleRates; 
}USB_Audio_Descriptor_Format_t;

/** \brief 24-Bit Audio Frequency Structure.
 *
 */
typedef struct
{
	u8 Byte1; 
	u8 Byte2; 
	u8 Byte3; 
}USB_Audio_SampleFreq_t;

/** \brief Audio class-specific Format Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8 bLength; 
	u8 bDescriptorType; 
	u8 bDescriptorSubtype;
	u8 bFormatType; 
	u8 bNrChannels; 
	u8 bSubFrameSize; 
	u8 bBitResolution;
	u8 bSampleFrequencyType; 
}USB_Audio_StdDescriptor_Format_t;

/** \brief Audio class-specific Streaming Endpoint Descriptor (LUFA naming conventions).
 *
 */
typedef struct
{
	USB_Descriptor_Endpoint_t Endpoint; 

	u8                   Refresh; 
	u8                   SyncEndpointNumber; 
}USB_Audio_Descriptor_StreamEndpoint_Std_t;

/** \brief Audio class-specific Streaming Endpoint Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8  bLength; 		
	u8  bDescriptorType; 	
	u8  bEndpointAddress; 	
	u8  bmAttributes; 	
	u16 wMaxPacketSize; 	
	u8  bInterval; 		
	u8  bRefresh; 		
	u8  bSynchAddress; 	
}USB_Audio_StdDescriptor_StreamEndpoint_Std_t;

/** \brief Audio class-specific Extended Endpoint Descriptor (LUFA naming conventions).
 *
 */
typedef struct
{
	USB_Descriptor_Header_t Header; 	
	u8                 Subtype; 		
	u8                 Attributes; 		
	u8                 LockDelayUnits; 
	u8                 LockDelay[2]; 
}USB_Audio_Descriptor_StreamEndpoint_Spc_t;

/** \brief Audio class-specific Extended Endpoint Descriptor (USB-IF naming conventions).
 *
 */
typedef struct
{
	u8  bLength; 		
	u8  bDescriptorType; 	
	u8  bDescriptorSubtype; 
	u8  bmAttributes; 	
	u8  bLockDelayUnits; 	
	u16 wLockDelay; 	
}USB_Audio_StdDescriptor_StreamEndpoint_Spc_t;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

