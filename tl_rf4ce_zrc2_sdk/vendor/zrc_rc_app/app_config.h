/********************************************************************************************************
 * @file     app_config.h
 *
 * @brief    application configuration
 *
 * @author
 * @date     July. 1, 2019
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

#define _USER_CONFIG_DEFINED_	1	// must define this macro to make others known
#define __DEBUG__ 0
#define	__LOG_RT_ENABLE__		0   // disable printf to avoid invaild trace output


//////////// product  Infomation  //////////////////////////////
#define ID_VENDOR				  0x248a			// for report
#define ID_PRODUCT_BASE			  0x8267
// If ID_PRODUCT left undefined, it will default to be combination of ID_PRODUCT_BASE and the USB config USB_SPEAKER_ENABLE/USB_MIC_ENABLE...
// #define ID_PRODUCT			  0x8869

#define STRING_VENDOR			L"Telink"
#define STRING_PRODUCT			L"Telink RF4CE"
#define STRING_SERIAL			L"TLSR8267"

#define APPLICATION_DONGLE		0					// or else APPLICATION_DEVICE
#define	FLOW_NO_OS				1

#define MODULE_PM_ENABLE        1
#define MODULE_KEY_SCAN_ENABLE  1
#define MODULE_FLASH_ENABLE     1


/////////////////// FLASH_PROTECT /////////////////////////////////
#define FLASH_PROTECT           0
#define FLASH_PROTECT_CMD_GIGADEVICE  0x6c  //flash of telink evb
#define FLASH_PROTECT_CMD_ADESTO      0x3c  //flash of customer selection

#define FLASH_PROTECT_CMD       FLASH_PROTECT_CMD_ADESTO

/////////////////// WATCHDOG  /////////////////////////////
#define MODULE_WATCHDOG_ENABLE	  1
#define MODULE_BUFM_ENABLE        1
#define MODULE_PRIQ_ENABLE        1
#define EV_POST_TASK_ENABLE       1

/////////////////// USB  //////////////////////////////////
#define MODULE_USB_ENABLE         0 // must enable when audio direct usb out test

/////////////////// AUDIO /////////////////////////////////
#define MODULE_AUDIO_ENABLE       1
#define MODULE_AUDIO_DEBUG        0 // enable MODULE_USB_ENABLE/USB_MIC_ENABLE/USB_PRINTER_ENABLE/MODULE_AUDIO_ENABLE

/////////////////// AUDIO_DEBUG   /////////////////////////////////
#if (MODULE_AUDIO_DEBUG)
#define USB_PRINTER_ENABLE      1
#define	USB_MIC_ENABLE          1
#else
#define USB_PRINTER_ENABLE      0
#define	USB_MIC_ENABLE          0
#endif

/////////////////// UART /////////////////////////////////
#define USB_UART_ENABLE           0 // UART conflict to USB_PRINTER_ENABLE
/////////////////// CDC /////////////////////////////////
#define USB_CDC_ENABLE            0 // UART work together to CDC
/////////////user defined values////////////////////////////////
#define USE_NEW_EV_MAIN         (0)
//////////////////// Timer  //////////////////////////////
#define IRQ_TIMER1_ENABLE         1

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	32000000		// 32000000//16000000


///////////////////  Mouse  Keyboard //////////////////////////////
#define MOUSE_HAS_BUTTON		1

#define	BATTERY_DETECT_ENABLE	1

///////////////////  MIC  /////////////////////////////////
#define MIC_RESOLUTION_BIT		16
#define MIC_SAMPLE_RATE			16000
#define MIC_CHANNLE_COUNT		1
#define	MIC_ENOCDER_ENABLE		0

#define	MIC_ADPCM_FRAME_SIZE		(80+4)// 128 //128 == 124 +4 bytes header
#define	MIC_SHORT_DEC_SIZE			(80*2)// 248 //248 == 124 *2

//#define TL_NOISE_SUPRESSION_ENABLE		1
//#define AUD_HPF_ALC	            0x04 //  hpf reg0xb05[3:0] ->Factor gain0~6 = [ 1  1/2  3/4  7/8  15/16   31/32    63/64]

////////////////////////// MIC BUFFER /////////////////////////////
#define	ADPCM_PACKET_LEN				84// 128
#define TL_MIC_ADPCM_UNIT_SIZE			160//248
#define	TL_SDM_BUFFER_SIZE				0// 992

#if MCU_CORE_826x
#define	TL_MIC_32K_FIR_16K				1 // 1 can have Drift problem
#elif MCU_CORE_8258
#define	TL_MIC_32K_FIR_16K				0
#endif

#if TL_MIC_32K_FIR_16K
	#define	TL_MIC_BUFFER_SIZE			(80*8*2)   
#else
	#define	TL_MIC_BUFFER_SIZE			(80*8)//992 //  992 == 124 * 8
#endif

///////////////////  POWER MANAGEMENT  //////////////////

#define PM_SUSPEND_WAKEUP_TIME  		20		// in ms

#define	RF_MODE_250K					1

#define RF4CE_MAX_PAIR_ENTRY_NUM           1


#include "board_cfg.h"

    ///////////////////  RF4CE Profile Configuration /////////////////////////////////
#include "profile_config.h"

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

