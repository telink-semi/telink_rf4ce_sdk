/********************************************************************************************************
 * @file     board_telink_rc_826x.h
 *
 * @brief    Telink 8269 RCU board configuration
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
#define VK_ENTERKEY     ZRCmdRC_Enter

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

#define VK_XING      	0xea
#define VK_JING 	 	0xe6

#define VK_NONE_KEY     0x00//VK_NONE


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

#define	IR_NONE_KEY		0xFF

//PB3 IRout 100K pulldown when  IR not working,  when IR begin, disable this 100K pulldown
//#define	PULL_WAKEUP_SRC_PB3				PM_PIN_PULLDOWN_100K


//////////////////////////// KEYSCAN/MIC  GPIO //////////////////////////////////
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K

// driver pins
#define	PE0_FUNC				AS_GPIO
#define	PD5_FUNC				AS_GPIO
#define	PD7_FUNC				AS_GPIO
#define	PE1_FUNC				AS_GPIO
#define	PA1_FUNC				AS_GPIO
//drive pin need 100K pulldown
#define	PULL_WAKEUP_SRC_PE0		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PD5		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PD7		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PE1		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PA1		MATRIX_ROW_PULL
//drive pin open input to read gpio wakeup level
#define PE0_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#define PD7_INPUT_ENABLE		1
#define PE1_INPUT_ENABLE		1
#define PA1_INPUT_ENABLE		1

//scan pins
#define	PC7_FUNC				AS_GPIO
#define	PA2_FUNC				AS_GPIO
#define	PB1_FUNC				AS_GPIO
#define	PA6_FUNC				AS_GPIO
#define	PA5_FUNC				AS_GPIO
#define	PD6_FUNC				AS_GPIO
//scan  pin need 10K pullup
#define	PULL_WAKEUP_SRC_PC7		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA2		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB1		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA6		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA5		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD6		MATRIX_COL_PULL
//scan pin open input to read gpio level
#define PC7_INPUT_ENABLE		1
#define PA2_INPUT_ENABLE		1
#define PB1_INPUT_ENABLE		1
#define PA6_INPUT_ENABLE		1
#define PA5_INPUT_ENABLE		1
#define PD6_INPUT_ENABLE		1

#define	KB_MAP_NUM		KB_MAP_NORMAL
#define	KB_MAP_FN		KB_MAP_NORMAL


#define  KB_DRIVE_PINS  {GPIO_PE0, GPIO_PD5, GPIO_PD7, GPIO_PE1, GPIO_PA1}
#define  KB_SCAN_PINS   {GPIO_PC7, GPIO_PA2, GPIO_PB1, GPIO_PA6, GPIO_PA5, GPIO_PD6}


#define KB_MAP_FOR_IR	{\
					{IR_TV_POWER,		IR_INPUT,		IR_DTA_POWER,	IR_BACK,  		IR_UPKEY,	},\
					{IR_LEFTKEY,		IR_OKKEY,		IR_RIGHTKEY,	IR_EXIT,  		IR_DOWNKEY,	}, \
					{IR_REWIND,			IR_PAUSEKEY,	IR_FORWARD,		IR_VOLUME_UP,  	IR_RETURN,	},\
					{IR_VOLUME_DOWN,	IR_MUTE, 		IR_CH_DOWN,    	IR_GUIDE,   	IR_MENU,	},\
					{IR_NUM_1,    		IR_NUM_2,		IR_NUM_3,    	IR_NUM_4,		IR_NUM_5,	},\
					{IR_NUM_7,      	IR_NUM_8,		IR_NUM_9,    	IR_SEARCH,  	IR_NUM_0, 	},\
					}

#define	KB_MAP_NORMAL	{\
						{VK_NONE_KEY,VK_TV,			VK_NONE_KEY,	VK_NONE_KEY,	VK_HOMEKEY,},	 \
						{VK_RECORED,	VK_NONE_KEY,	VK_NONE_KEY,	VK_CH_UP,		VK_CH_DOWN,},	 \
						{VK_NUM_2,	VK_RIGHTKEY,	VK_VOLUME_UP,	VK_NUM_3,		VK_NUM_1,},	 \
						{VK_NUM_5,	VK_OKKEY,		VK_VOLUME_DOWN,	VK_NUM_6,		VK_NUM_4,},	 \
						{VK_NUM_8,	VK_DOWNKEY,		VK_UPKEY,		VK_NUM_9,		VK_NUM_7,},	 \
						{VK_NUM_0,	VK_BACK,		VK_LEFTKEY,		VK_MUTE,		VK_MENU,},	 }


#define KB_ARRAY_ROW	5
#define KB_ARRAY_COL	6

#define	KB_LINE_HIGH_VALID	0


/*
 * LED PIN configuration
 *
 * */
	#define GREEN_LED_ON_LEVEL		1
	#define GREEN_LED_OFF_LEVEL		(GREEN_LED_ON_LEVEL^1)
	#define RED_LED_ON_LEVEL		1
	#define RED_LED_OFF_LEVEL		(RED_LED_ON_LEVEL^1)
	#define	ZRC_APP_GREEN_LED_GPIO           GPIO_PD1
	#define ZRC_APP_RED_LED_GPIO             GPIO_PD2

	#define	PD1_FUNC				AS_GPIO
	#define PD1_INPUT_ENABLE		0
	#define PD1_OUTPUT_ENABLE		1
	#define PD1_DATA_OUT			GREEN_LED_OFF_LEVEL


/*
 * ADC PIN configuration
 *
 * */
	#define ZRC_APP_ADC_PIN                  GPIO_PB7


/*
 * IR PIN configuration
 *
 * */
	#define IR_PWM_ID							PWM0
	#define GPIO_IR_OUT            		  		GPIO_PA0
	#define GPIO_IR_CTRL            		 	GPIO_PA3
	#define GPIO_IR_LEARN_IN					GPIO_PA4

	#define IR_PWM_FUNC							AS_PWM
	#define IR_PWM_PIN_CFG						PWM0_CFG_GPIO_A0()
	#define IR_CTRL_PIN_CFG						do										\
												{										\
												gpio_set_func(GPIO_IR_CTRL, AS_GPIO);	\
												gpio_set_input_en(GPIO_IR_CTRL, 0);		\
												gpio_set_output_en(GPIO_IR_CTRL, 1);	\
												gpio_set_data_strength(GPIO_IR_CTRL, 1);\
												gpio_write(GPIO_IR_CTRL, 1);			\
												}while(0);

	#define IR_LEARN_PIN_CFG					do											\
												{											\
												gpio_set_func(GPIO_IR_LEARN_IN, AS_GPIO);	\
												gpio_set_input_en(GPIO_IR_LEARN_IN, 1);		\
												gpio_set_output_en(GPIO_IR_LEARN_IN, 0);	\
												gpio_write(GPIO_IR_LEARN_IN, 0);			\
												}while(0);






/*
 * GPIO PIN configuration for printf DEBUG
 * DEBUG_INFO_TX_PIN: used in function "soft_uart_putc()"
 *
 * */
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_PB4//print
	#define PB4_OUTPUT_ENABLE		1
	#define PB4_INPUT_ENABLE		0
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
/*
 * USB PIN configuration(must be pulled up)
 *
 * */
#if (MODULE_USB_ENABLE)
	#define	PE2_FUNC				AS_USB
	#define	PE3_FUNC				AS_USB
	#define PE2_INPUT_ENABLE		1
	#define PE3_INPUT_ENABLE		1
	#define PULL_WAKEUP_SRC_PE2           PM_PIN_PULLUP_1M  //DM
	#define PULL_WAKEUP_SRC_PE3           PM_PIN_PULLUP_1M  //DP
#endif


/*
 *
 * AMIC pin nconfiguration
 *
 * */
#define APP_AMIC_PIN_CFG_ON			AMIC_PIN_CFG_ON(GPIO_PC4, GPIO_PC5, GPIO_PC6)
#define APP_AMIC_PIN_CFG_OFF		AMIC_PIN_CFG_OFF(GPIO_PC4, GPIO_PC5, GPIO_PC6)


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
