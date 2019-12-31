/********************************************************************************************************
 * @file     board_C59T80A5_v1P1.h
 *
 * @brief    C59T80A5_v1P1 board configurtion
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


#define	MAXTRIX_GPIO_PULL_UP        PM_PIN_PULLUP_10K
#define	MAXTRIX_GPIO_PULL_DOWN      PM_PIN_PULLDOWN_100K

#define VK_DTA_POWER    ZRCmdRC_PowerToggleFunction
#define VK_TV_POWER     0xfa
#define VK_VOLUME_UP    ZRCmdRC_VolumeUp
#define VK_VOLUME_DOWN  ZRCmdRC_VolumeDown
#define VK_MUTE         ZRCmdRC_Mute
#define VK_INPUT	    ZRCmdRC_InputSelect
#define VK_BACK         0xa8
#define VK_INFO         ZRCmdRC_DisplayInfo
#define VK_UPKEY        ZRCmdRC_Up
#define VK_DOWNKEY      ZRCmdRC_Down
#define VK_OKKEY        ZRCmdRC_Select
#define VK_LEFTKEY      ZRCmdRC_Left
#define VK_RIGHTKEY     ZRCmdRC_Right
#define VK_EXIT         ZRCmdRC_Exit
#define VK_RECORED      ZRCmdRC_Record
#define VK_REWIND       ZRCmdRC_Rewind
#define VK_PLAYKEY		ZRCmdRC_PlayFunction
#define VK_STOPKEY      ZRCmdRC_StopFunction
#define VK_PAUSEKEY     ZRCmdRC_PausePlayFunction
#define VK_LASTKEY      ZRCmdRC_PreviousChannel

#define VK_FORWARD      ZRCmdRC_FastForward
#define VK_RETURN       ZRCmdRC_Enter
#define VK_LIVE         ZRCmdRC_SubPicture
#define VK_CH_UP        ZRCmdRChannelUp
#define VK_CH_DOWN      ZRCmdRChannelDown
#define VK_GUIDE        ZRCmdRC_EPc
#define VK_MENU         ZRCmdRC_SetupMenu
#define VK_HOMEKEY		ZRCmdRC_RootMenu
#define VK_PAGE_UP      ZRCmdRC_PageUp
#define VK_PAGE_DOWN    ZRCmdRC_PageDown
#define VK_LAST		    0xf5
#define VK_PROG		    0xf6
#define VK_TV		    0xf7
#define VK_ONDEMAND     0xf8

#define VK_NUM_0        ZRCmdRC_Numpad0_or_10
#define VK_NUM_1        ZRCmdRC_Numpad1
#define VK_NUM_2        ZRCmdRC_Numpad2
#define VK_NUM_3        ZRCmdRC_Numpad3
#define VK_NUM_4        ZRCmdRC_Numpad4
#define VK_NUM_5        ZRCmdRC_Numpad5
#define VK_NUM_6        ZRCmdRC_Numpad6
#define VK_NUM_7        ZRCmdRC_Numpad7
#define VK_NUM_8        ZRCmdRC_Numpad8
#define VK_NUM_9        ZRCmdRC_Numpad9

#define VK_SEARCH       ZRCmdRC_F1
//#define VK_OPTIONS      0xea

#define VK_XING      0xea
#define VK_JING 	 0xe6

#define VK_NONE_KEY     VK_NONE


/*
 * IR code value defined here
 *  */
#define IR_TV_POWER		0x08
#define IR_DTA_POWER	0X48//0x07
#define IR_VOLUME_UP	0X5E//0x20
#define IR_VOLUME_DOWN	0X46//0x21
#define IR_MUTE			0X56//0x22
#define IR_INPUT		0x0d
#define IR_BACK			0x31
#define IR_INFO			0x08
#define IR_UPKEY		0X1F//0x02
#define IR_DOWNKEY		0X1B//0x0B
#define IR_OKKEY		0X0B//0x0c
#define IR_LEFTKEY		0X5D//0x03
#define IR_RIGHTKEY		0X1E//0x04
#define IR_EXIT			0X5A//0x2c
#define IR_RECORD		0x35
#define IR_REWIND		0x29
#define IR_STOPKEY		0x34
#define IR_REWIND		0x29
#define IR_PAUSEKEY		0x33
#define IR_FORWARD		0x28
#define IR_RETURN		0x0E
#define IR_LIVE			0x1A
#define IR_CH_UP		0x1D
#define IR_CH_DOWN		0x1E
#define IR_GUIDE		0x06
#define IR_MENU			0X58//0x3E
#define IR_ONDEMAND		0x26
#define IR_RETURN		0x0E
#define IR_NUM_1		0X53//0x10
#define IR_NUM_2		0X10//0x11
#define IR_NUM_3		0X11//0x12
#define IR_NUM_4		0X4F//0x13
#define IR_NUM_5		0X0C//0x14
#define IR_NUM_6		0X0D//0x15
#define IR_NUM_7		0X4B//0x16
#define IR_NUM_8		0X08//0x17
#define IR_NUM_9		0X09//0x18
#define IR_NUM_0		0X04//0x19
#define IR_SEARCH		0x30
#define IR_OPTIONS		0x0F
#define IR_PAGE_UP		0x40
#define IR_PAGE_DOWN	0x41
#define IR_PROG			0x54
#define IR_TV		    0x55
#define IR_LASTKEY	    0x56

#define IR_XING	    0x47
#define IR_JING	    0x05

#define	IR_NONE_KEY		0xFF


// driver pins
#define	PA6_FUNC				AS_GPIO
#define	PA7_FUNC				AS_GPIO
#define	PB1_FUNC				AS_GPIO
#define	PB4_FUNC				AS_GPIO
#define	PB5_FUNC				AS_GPIO
#define	PB6_FUNC				AS_GPIO
#define	PB7_FUNC				AS_GPIO
#define	PULL_WAKEUP_SRC_PA6		MAXTRIX_GPIO_PULL_DOWN
#define	PULL_WAKEUP_SRC_PA7		MAXTRIX_GPIO_PULL_DOWN
#define	PULL_WAKEUP_SRC_PB1		MAXTRIX_GPIO_PULL_DOWN
#define	PULL_WAKEUP_SRC_PB4		MAXTRIX_GPIO_PULL_DOWN
#define	PULL_WAKEUP_SRC_PB5		MAXTRIX_GPIO_PULL_DOWN
#define	PULL_WAKEUP_SRC_PB6		MAXTRIX_GPIO_PULL_DOWN
#define	PULL_WAKEUP_SRC_PB7		MAXTRIX_GPIO_PULL_DOWN
#define	PA6_INPUT_ENABLE		1
#define	PA7_INPUT_ENABLE		1
#define	PB1_INPUT_ENABLE		1
#define	PB4_INPUT_ENABLE		1
#define	PB5_INPUT_ENABLE		1
#define	PB6_INPUT_ENABLE		1
#define	PB7_INPUT_ENABLE		1

#define	PA6_OUTPUT_ENABLE		0
#define	PA7_OUTPUT_ENABLE		0
#define	PB1_OUTPUT_ENABLE		0
#define	PB4_OUTPUT_ENABLE		0
#define	PB5_OUTPUT_ENABLE		0
#define	PB6_OUTPUT_ENABLE		0
#define	PB7_OUTPUT_ENABLE		0

//scan pin 18    COL
#define	PD3_FUNC				AS_GPIO
#define	PD4_FUNC				AS_GPIO
#define	PD5_FUNC				AS_GPIO
#define	PD6_FUNC				AS_GPIO
#define	PD7_FUNC				AS_GPIO
#define	PE0_FUNC				AS_GPIO
#define	PE1_FUNC				AS_GPIO
#define	PULL_WAKEUP_SRC_PD3		MAXTRIX_GPIO_PULL_UP  //C0
#define	PULL_WAKEUP_SRC_PD4		MAXTRIX_GPIO_PULL_UP
#define	PULL_WAKEUP_SRC_PD5		MAXTRIX_GPIO_PULL_UP  //C5
#define	PULL_WAKEUP_SRC_PD6		MAXTRIX_GPIO_PULL_UP  //C6
#define	PULL_WAKEUP_SRC_PD7		MAXTRIX_GPIO_PULL_UP  //C5
#define	PULL_WAKEUP_SRC_PE0		MAXTRIX_GPIO_PULL_UP  //C6
#define	PULL_WAKEUP_SRC_PE1		MAXTRIX_GPIO_PULL_UP

#define	PD3_INPUT_ENABLE		1
#define	PD4_INPUT_ENABLE		1
#define	PD5_INPUT_ENABLE		1
#define	PD6_INPUT_ENABLE		1
#define	PD7_INPUT_ENABLE		1
#define	PE0_INPUT_ENABLE		1
#define	PE1_INPUT_ENABLE		1

#define	PD3_OUTPUT_ENABLE		0
#define	PD4_OUTPUT_ENABLE		0
#define	PD5_OUTPUT_ENABLE		0
#define	PD6_OUTPUT_ENABLE		0
#define	PD7_OUTPUT_ENABLE		0
#define	PE0_OUTPUT_ENABLE		0
#define	PE1_OUTPUT_ENABLE		0

//////////////////// board gpio configuration //////////////////////////////
#define	PULL_WAKEUP_SRC_PB0		MAXTRIX_GPIO_PULL_UP  //SWS pull up


/* col * Row */
#define	KB_MAP_NORMAL	{\
	{VK_NUM_7,		VK_NUM_4,		VK_NUM_1,	0x14,  		VK_VOLUME_DOWN,		VK_VOLUME_UP, 	VK_BACK}, \
	{VK_LEFTKEY,	VK_INFO,		0x23,	VK_REWIND,  		VK_RECORED,		0x26, 	0x27}, \
	{VK_RIGHTKEY,	VK_HOMEKEY,		0x33,	VK_FORWARD,  		VK_STOPKEY,		0x36, 	0x37}, \
	{VK_NUM_9,		VK_NUM_6,		VK_NUM_3,	0x44,  		VK_CH_DOWN,		VK_CH_UP, 	VK_MENU}, \
	{0x51,		0x52,		0x53,	VK_DOWNKEY,  		VK_OKKEY,		VK_UPKEY, 	VK_HOMEKEY}, \
	{0x61,		0x62,		VK_NUM_0,	VK_NUM_8,  		VK_NUM_5,		VK_NUM_2, 	0x67}, \
	{VK_PAUSEKEY,		VK_PLAYKEY,		0x73,	0x74,  		0x75,		0x76, 	0x77,}, }
	/*{VK_TV_POWER,		VK_INPUT,		VK_DTA_POWER,	VK_BACK,  		VK_UPKEY,		VK_INFO, 	}, \
	{VK_LEFTKEY,		VK_OKKEY,		VK_RIGHTKEY,	VK_EXIT,  		VK_DOWNKEY,   	VK_RECORED, }, \
	{VK_REWIND,			VK_PAUSEKEY,    VK_FORWARD,		VK_VOLUME_UP,  	VK_RETURN,		VK_CH_UP,	}, \
	{VK_VOLUME_DOWN,	VK_MUTE, 		VK_CH_DOWN,     VK_GUIDE,   	VK_MENU,        VK_ONDEMAND,}, \
	{VK_NUM_1,	    	VK_NUM_2,		VK_NUM_3,	    VK_NUM_4,		VK_NUM_5,	    VK_NUM_6,	}, \
	{VK_NUM_7,      	VK_NUM_8,		VK_NUM_9,    	VK_SEARCH,  	VK_NUM_0, 		VK_OPTIONS,	}, \
	{VK_STOPKEY,		VK_LIVE,		VK_NONE_KEY,   	VK_NONE_KEY,	VK_NONE_KEY,	VK_NONE_KEY,}, }*/

#define KB_MAP_FOR_IR	{\
	{IR_TV_POWER,		IR_INPUT,		IR_DTA_POWER,	IR_BACK,  		IR_UPKEY,	IR_INFO, },\
	{IR_LEFTKEY,		IR_OKKEY,		IR_RIGHTKEY,	IR_EXIT,  		IR_DOWNKEY,	IR_RECORD,}, \
	{IR_REWIND,			IR_PAUSEKEY,	IR_FORWARD,		IR_VOLUME_UP,  	IR_RETURN,	IR_CH_UP,},\
	{IR_VOLUME_DOWN,	IR_MUTE, 		IR_CH_DOWN,    	IR_GUIDE,   	IR_MENU,	IR_ONDEMAND,},\
	{IR_NUM_1,    		IR_NUM_2,		IR_NUM_3,    	IR_NUM_4,		IR_NUM_5,	IR_NUM_6,},\
	{IR_NUM_7,      	IR_NUM_8,		IR_NUM_9,    	IR_SEARCH,  	IR_NUM_0, 	IR_OPTIONS,},\
	{IR_STOPKEY,		IR_LIVE,		IR_NONE_KEY,   	IR_NONE_KEY,	IR_NONE_KEY,IR_NONE_KEY,} }

#define	KB_MAP_NUM		KB_MAP_NORMAL
#define	KB_MAP_FN		KB_MAP_NORMAL

#define KB_DRIVE_PINS  {GPIO_PA6,  GPIO_PA7, GPIO_PB1, GPIO_PB4, GPIO_PB5, GPIO_PB6, GPIO_PB7 }
#define KB_SCAN_PINS   {GPIO_PD3, GPIO_PD4, GPIO_PD5, GPIO_PD6, GPIO_PD7, GPIO_PE0, GPIO_PE1 }
#define SYS_WAKEUP_PIN (GROUPA_PIN6|GROUPA_PIN7|GROUPB_PIN1|GROUPB_PIN4|GROUPB_PIN5|GROUPB_PIN6|GROUPB_PIN7)

#define KB_ARRAY_ROW	7
#define KB_ARRAY_COL	7

#define	KB_LINE_HIGH_VALID	0

/*
 * LED PIN configuration
 *
 * */
	#define GREEN_LED_ON_LEVEL		1
	#define GREEN_LED_OFF_LEVEL		(GREEN_LED_ON_LEVEL^1)
	#define RED_LED_ON_LEVEL		1
	#define RED_LED_OFF_LEVEL		(RED_LED_ON_LEVEL^1)
	#define	ZRC_APP_GREEN_LED_GPIO           GPIO_PD2
	#define ZRC_APP_RED_LED_GPIO             GPIO_PC7

	#define	PD2_FUNC				AS_GPIO
	#define PD2_INPUT_ENABLE		0
	#define PD2_OUTPUT_ENABLE		1
	#define PD2_DATA_OUT			GREEN_LED_OFF_LEVEL

	#define	PC7_FUNC				AS_GPIO
	#define PC7_INPUT_ENABLE		0
	#define PC7_OUTPUT_ENABLE		1
	#define PC7_DATA_OUT			RED_LED_OFF_LEVEL


/*
 * ADC PIN configuration
 *
 * */
	#define ZRC_APP_ADC_PIN                  GPIO_PB7

/*
 * IR PIN configuration
 *
 * */
	#define GPIO_IR_LEARN_IN			GPIO_PD1
	#define GPIO_IR_CTRL				GPIO_PD0
	#define IR_PWN_ID					PWM0
	#define IR_PWM_PIN_CFG				PWM0_CFG_GPIO_A0()

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

/*
 *
 * AMIC pin nconfiguration
 *
 * */
	#define APP_AMIC_PIN_CFG			AMIC_PIN_CFG(GPIO_PC4, GPIO_PC5, GPIO_PC6)

    /* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

