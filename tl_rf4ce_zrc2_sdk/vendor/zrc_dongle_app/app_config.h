/********************************************************************************************************
 * @file     app_config.h
 *
 * @brief    application configuration
 *
 * @author
 * @date     July. 1, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define _USER_CONFIG_DEFINED_     1    // must define this macro to make others known

#define __DEBUG__ 1

//#define START_PERMIT_JOIN_OFF	1
    //////////// product  Infomation  //////////////////////////////
#define ID_VENDOR				  0x248a			// for report
#define ID_PRODUCT_BASE			  0x8600
// If ID_PRODUCT left undefined, it will default to be combination of ID_PRODUCT_BASE and the USB config USB_SPEAKER_ENABLE/USB_MIC_ENABLE...
// #define ID_PRODUCT			  0x8869

#define STRING_VENDOR             L"Telink"
#define STRING_PRODUCT			  L"Telink RF4CE"
#define STRING_SERIAL			  L"TLSR8266"

#define CHIP_TYPE				CHIP_TYPE_8636		// 8866-24, 8566-32


#define HID_KEYBORAD_ENABLE		0

#define	FLOW_NO_OS				1

#define RF4CE_TEST_ENABLE (0)
#define MODULE_PM_ENABLE        0
#define MODULE_KEY_SCAN_ENABLE  1
#define MODULE_FLASH_ENABLE     1
/////////////////// watchdog  //////////////////////////////

#define MODULE_WATCHDOG_ENABLE	0
#define MODULE_USB_ENABLE         1
#define MODULE_BUFM_ENABLE        1
#define MODULE_PRIQ_ENABLE        1
#define EV_POST_TASK_ENABLE       1
#define STATIC_V_INST            1
//--------------------- GPIO for KEY end---------------------------//

#define ENABLE_FREQ_AGILITY   0
//#define    CDC_TX_EPNUM					   8

//////////////////// Timer  //////////////////////////////
#define IRQ_TIMER1_ENABLE         0
#define IRQ_TIMER0_ENABLE         0

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
//#define CLOCK_SYS_TYPE  		CLOCK_TYPE_OSC	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	32000000

///////////////////  ADC  /////////////////////////////////
#define MODULE_MOUSE_ENABLE       0

#if HID_KEYBORAD_ENABLE							
#define APPLICATION_DONGLE			1					// or else APPLICATION_DEVICE
#define MODULE_KEYBOARD_ENABLE    	1
#else
#define APPLICATION_DONGLE			0					// or else APPLICATION_DEVICE
#define MODULE_KEYBOARD_ENABLE    	0
#endif


///////////////////  battery  /////////////////////////////////


#define BATT_ADC_CHANNEL		0
#define BATT_FULL_VOLT			(4100)	//  mV
#define BATT_LOW_VOLT			(3700)	//  mV
#define BATT_NO_PWR_VOLT		(3400)	//  mV
#define	ADC_CHN0_ANA_INPUT		ADC_CHN_INP_ANA_7
#define ADC_CHN0_REF_SRC		ADC_REF_SRC_INTERNAL

///////////////////  Mouse  Keyboard //////////////////////////////
#define MOUSE_HAS_WHEEL			0
#define MOUSE_HAS_BUTTON		1


///////////////////  MIC  /////////////////////////////////
#define MIC_RESOLUTION_BIT		16
#define MIC_SAMPLE_RATE			16000
#define MIC_CHANNLE_COUNT		1

#define	MIC_ADPCM_FRAME_SIZE    (80+4)//128
#define	MIC_SHORT_DEC_SIZE      (80*2)//248

#define	TL_MIC_BUFFER_SIZE      0//992//512 dongle side not need buufer_mic

///////////////////  USB   /////////////////////////////////
#define USB_PRINTER                    0

#if (MODULE_USB_ENABLE)
#define USB_PRINTER_ENABLE              USB_PRINTER

#define USB_SPEAKER_ENABLE              0
#define USB_MOUSE_ENABLE                0

#if HID_KEYBORAD_ENABLE	
#define USB_KEYBOARD_ENABLE             1
#define USB_CDC_ENABLE                  0   // work together with specific INF file
#else
#define USB_KEYBOARD_ENABLE             0
#define USB_CDC_ENABLE                  1   // work together with specific INF file
#endif

#define USB_SOMATIC_ENABLE              0   // when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
#define USB_MIC_ENABLE            		1
#define MODULE_AUDIO_ENABLE       		1
#define	USB_CDC_AUDIO					0
#endif

///////////////////  OTHER   /////////////////////////////////
#define USE_NEW_EV_MAIN             0
#define RF4CE_MAX_PAIR_ENTRY_NUM    1
#define ENABLE_FREQ_AGILITY         0
#define	RF_MODE_250K		        1
#define DFIFO1_SIZE                 0

//#define    CDC_TX_EPNUM           8
#define PM_SUSPEND_WAKEUP_TIME		20

#include "board_cfg.h"

///////////////////  RF4CE Profile Configuration /////////////////////////////////
#include "profile_config.h"
#define  USB_PID                        "880f"


/////////////////// set default   ////////////////
#include "../common/default_config.h"

/////////////////// main loop, event loop  ////////////////
typedef enum{
	//EV_SUSPEND_NOTIFY = 0,
	//EV_MOUSE_EVENT,
	//EV_CALI_START = 0,
	//EV_CALI_STOP,
	EV_EVENT_MAX = 1,
} ev_event_e;

enum{
	EV_FIRED_EVENT_MAX = 1
};


typedef enum{
    EV_POLL_USB_IRQ,
    EV_POLL_IDLE, //  Must be the last item in ev_poll_e
	EV_POLL_ED_DETECT,
	EV_POLL_EMI_TEST,
#if (HOST_ROLE)
	EV_TRANSCEIVER_POLL_TASK,
#endif
	EV_POLL_MAX,
}ev_poll_e;

    /* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
