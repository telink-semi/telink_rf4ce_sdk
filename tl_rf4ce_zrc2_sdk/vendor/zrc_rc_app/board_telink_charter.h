/********************************************************************************************************
 * @file     board_telink_charter.h
 *
 * @brief    charter board configurtion
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

///* if QRP enable, PM shall be disable */
//#if MODULE_QRP_ENABLE
//	#define MODULE_PM_ENABLE
//#endif

#define	MAXTRIX_GPIO_PULL_UP        PM_PIN_PULLUP_10K
#define	MAXTRIX_GPIO_PULL_DOWN      PM_PIN_PULLDOWN_100K

#define VK_DTA_POWER    ZRCmdRC_PowerToggleFunction//0x6B
#define VK_TV_POWER     ZRCmdRC_PowerOffFunction
#define VK_VOLUME_UP    ZRCmdRC_VolumeUp//0x41
#define VK_VOLUME_DOWN  ZRCmdRC_VolumeDown//0x42
#define VK_MUTE         ZRCmdRC_Mute//0x43
#define VK_INPUT	    ZRCmdRC_InputSelect
#define VK_BACK         0x78
#define VK_INFO         0x35
#define VK_UPKEY        0x01
#define VK_DOWNKEY      0x02
#define VK_OKKEY        ZRCmdRC_Select//0x00
#define VK_LEFTKEY      0x03
#define VK_RIGHTKEY     0x04
#define VK_EXIT         0x0D
#define VK_RECORED      0x47
#define VK_REWIND       ZRCmdRC_Rewind
#define VK_PLAYKEY		0x61
//#define VK_STOPKEY      ZRCmdRC_StopFunction
//#define VK_PAUSEKEY     ZRCmdRC_PausePlayFunction
#define VK_LASTKEY      0x32
//#define VK_ENTERKEY     ZRCmdRC_Enter

//#define VK_FORWARD      ZRCmdRC_FastForward
//#define VK_RETURN       ZRCmdRC_Enter
//#define VK_LIVE         ZRCmdRC_SubPicture
#define VK_CH_UP        0x30
#define VK_CH_DOWN      0x31
#define VK_GUIDE        0x53
#define VK_MENU         0x09
#define VK_DVR	        0x0B
#define VK_HOMEKEY		ZRCmdRC_EPc
//#define VK_HOMEKEY		ZRCmdRC_RootMenu
//#define VK_PAGE_UP      ZRCmdRC_PageUp
//#define VK_PAGE_DOWN    ZRCmdRC_PageDown
//#define VK_LAST		    0xf5
//#define VK_PROG		    0xf6
//#define VK_TV		    0xf7
//#define VK_ONDEMAND     0xf8
//#define VK_TV_VOL_UP     0xf9
//#define VK_TV_VOL_DOWN   0xfa

#define VK_NUM_0        0x20
#define VK_NUM_1        0x21
#define VK_NUM_2        0x22
#define VK_NUM_3        0x23
#define VK_NUM_4        0x24
#define VK_NUM_5        0x25
#define VK_NUM_6        0x26
#define VK_NUM_7        0x27
#define VK_NUM_8        0x28
#define VK_NUM_9        0x29

#define VK_SEARCH       0x2C
#define VK_OPTIONS      0x3A

#define VK_XING      0xea
#define VK_JING 	 0xe6

#define VK_NONE_KEY     0xFF//VK_NONE


/*
 * IR code value defined here
 *  */
#define IR_TV_POWER		0x01
#define IR_INPUT		0x02
#define IR_ALL_POWER	0x03
#define IR_DTA_POWER	0X03
#define IR_VOLUME_UP	0x04
#define IR_VOLUME_DOWN	0x05
#define IR_MUTE			0x06
#define IR_CH_UP		0x07
#define IR_CH_DOWN		0x08
#define IR_LASTKEY	    0x09
#define IR_RECORD		0x0A
#define IR_PLAY			0x0B
#define IR_INFO			0x0C
#define IR_UPKEY		0x0D
#define IR_DOWNKEY		0X0E
#define IR_LEFTKEY		0x0F
#define IR_RIGHTKEY		0X10
#define IR_OKKEY		0x11
#define IR_BACK			0x12
#define IR_EXIT			0x13
#define IR_GUIDE		0x14
#define IR_MENU			0x15
#define IR_DVR			0x16
#define IR_NUM_1		0x17//0x10
#define IR_NUM_2		0x18//0x11
#define IR_NUM_3		0X19//0x12
#define IR_NUM_4		0X1A//0x13
#define IR_NUM_5		0X1B//0x14
#define IR_NUM_6		0X1C//0x15
#define IR_NUM_7		0X1D//0x16
#define IR_NUM_8		0X1E//0x17
#define IR_NUM_9		0X1F//0x18
#define IR_NUM_0		0X20//0x19
#define IR_SEARCH		0x21
#define IR_OPTIONS		0x22

#define	IR_NONE_KEY		0xFF

//////////////////////////// KEYSCAN/MIC  GPIO //////////////////////////////////
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K

// driver pins
#define	PA4_FUNC				AS_GPIO
#define	PA7_FUNC				AS_GPIO
#define	PB1_FUNC				AS_GPIO
#define	PB4_FUNC				AS_GPIO
#define	PB5_FUNC				AS_GPIO
#define	PB6_FUNC				AS_GPIO
//drive pin need 100K pulldown
#define	PULL_WAKEUP_SRC_PA4		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PA7		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB1		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB4		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB5		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB6		MATRIX_ROW_PULL
//drive pin open input to read gpio wakeup level
#define PA4_INPUT_ENABLE		1
#define PA7_INPUT_ENABLE		1
#define PB1_INPUT_ENABLE		1
#define PB4_INPUT_ENABLE		1
#define PB5_INPUT_ENABLE		1
#define PB6_INPUT_ENABLE		1


//scan pins
#define	PA5_FUNC				AS_GPIO
#define	PA1_FUNC				AS_GPIO
#define	PA2_FUNC				AS_GPIO
#define	PA3_FUNC				AS_GPIO
#define	PD1_FUNC				AS_GPIO
#define	PD0_FUNC				AS_GPIO

//scan  pin need 10K pullup
#define	PULL_WAKEUP_SRC_PA5		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA1		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA2		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA3		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD1		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD0		MATRIX_COL_PULL
//scan pin open input to read gpio level
#define PA5_INPUT_ENABLE		1
#define PA1_INPUT_ENABLE		1
#define PA2_INPUT_ENABLE		1
#define PA3_INPUT_ENABLE		1
#define PD1_INPUT_ENABLE		1
#define PD0_INPUT_ENABLE		1

#define	KB_MAP_NUM		KB_MAP_NORMAL
#define	KB_MAP_FN		KB_MAP_NORMAL

#define  KB_DRIVE_PINS  {GPIO_PA4, GPIO_PA7, GPIO_PB1, GPIO_PB4, GPIO_PB5, GPIO_PB6}
#define  KB_SCAN_PINS   {GPIO_PA5, GPIO_PA1, GPIO_PA2, GPIO_PA3, GPIO_PD1, GPIO_PD0}


#define KB_MAP_FOR_IR	{\
					{IR_TV_POWER,		IR_INPUT,		IR_DTA_POWER,	IR_VOLUME_UP,  		IR_SEARCH,  	IR_CH_UP,	},\
					{IR_MUTE,			IR_DVR,			IR_LASTKEY,		IR_VOLUME_DOWN,  	IR_PLAY,  		IR_CH_DOWN,	}, \
					{IR_BACK,			IR_UPKEY,		IR_INFO,		IR_LEFTKEY,  		IR_OKKEY,  		IR_RIGHTKEY,	},\
					{IR_BACK,			IR_DOWNKEY, 	IR_EXIT,    	IR_OPTIONS,   		IR_MENU,  		IR_RECORD,	},\
					{IR_NUM_1,      	IR_NUM_2,		IR_NUM_3,    	IR_NUM_4,  			IR_NUM_5,  		IR_NUM_6, 	},\
					{IR_NUM_7,			IR_NUM_8,		IR_NUM_9,   	IR_NUM_0,			IR_NONE_KEY,  	IR_NONE_KEY,} }

#define	KB_MAP_NORMAL	{\
						VK_TV_POWER,	VK_INPUT,		VK_DTA_POWER,		VK_VOLUME_UP, 		VK_SEARCH, 		VK_CH_UP,	 \
						VK_MUTE,		VK_DVR,			VK_LASTKEY,			VK_VOLUME_DOWN,  	VK_PLAYKEY, 	VK_CH_DOWN,	 \
						VK_HOMEKEY,		VK_UPKEY,		VK_INFO,			VK_LEFTKEY,  		VK_OKKEY, 		VK_RIGHTKEY,	 \
						VK_BACK,		VK_DOWNKEY,		VK_EXIT,			VK_OPTIONS,  		VK_MENU, 		VK_RECORED,	 \
						VK_NUM_1,		VK_NUM_2,		VK_NUM_3,			VK_NUM_4,  			VK_NUM_5, 		VK_NUM_6,	 \
						VK_NUM_7,		VK_NUM_8,		VK_NUM_9,			VK_NUM_0,  			VK_NONE_KEY, 	VK_NONE_KEY,	 }

#define KB_ARRAY_ROW	6
#define KB_ARRAY_COL	6

#define	KB_LINE_HIGH_VALID	0


/*
 * LED PIN configuration
 *
 * */
	#define GREEN_LED_ON_LEVEL		0
	#define GREEN_LED_OFF_LEVEL		(GREEN_LED_ON_LEVEL^1)
	#define RED_LED_ON_LEVEL		0
	#define RED_LED_OFF_LEVEL		(RED_LED_ON_LEVEL^1)

	#define	ZRC_APP_GREEN_LED_GPIO           GPIO_PE0
	#define ZRC_APP_RED_LED_GPIO             GPIO_PE0


/*
 * ADC PIN configuration
 *
 * */
	#define ZRC_APP_ADC_PIN                  GPIO_PB7


/*
 * IR PIN configuration
 *
 * */
	#define GPIO_IR_CTRL            		  	GPIO_PA0
	#define IR_PWN_ID							PWM0
	#define IR_PWM_PIN_CFG						PWM0_CFG_GPIO_A0()


/*
 * GPIO PIN configuration for printf DEBUG
 * DEBUG_INFO_TX_PIN: used in function "soft_uart_putc()"
 *
 * */
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_PC5//print
	#define PC5_OUTPUT_ENABLE		1
	#define PC5_INPUT_ENABLE		0
#endif


/*
 * SWS PIN configuration(must be pulled up)
 *
 * */
	#define	PULL_WAKEUP_SRC_PB0		MAXTRIX_GPIO_PULL_UP  //SWS pull up

/*
 * USB PIN configuration(must be pulled up)
 *
 * */
	#define PULL_WAKEUP_SRC_PE2           PM_PIN_PULLUP_1M  //DM
	#define PULL_WAKEUP_SRC_PE3           PM_PIN_PULLUP_1M  //DP

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
