/********************************************************************************************************
 * @file    usbkeycode.h
 *
 * @brief   This is the header file for usbkeycode.h
 *
 * @author	Zigbee GROUP
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#pragma once

#define VK_NONE        		0x00

// function key bitmap
#define VK_MSK_CTRL			0x01
#define VK_MSK_SHIFT		0x02
#define VK_MSK_ALT			0x04
#define VK_MSK_WIN			0x08

#define VK_MSK_LCTRL		0x01
#define VK_MSK_LSHIFT		0x02
#define VK_MSK_LALT			0x04
#define VK_MSK_LWIN			0x08

#define VK_MSK_RCTRL		0x10
#define VK_MSK_RSHIFT		0x20
#define VK_MSK_RALT			0x40
#define VK_MSK_RWIN			0x80

// ordinary keys
#define VK_A				0x04
#define VK_B				0x05
#define VK_C				0x06
#define VK_D				0x07
#define VK_E				0x08
#define VK_F				0x09
#define VK_G				0x0a
#define VK_H				0x0b
#define VK_I				0x0c
#define VK_J				0x0d
#define VK_K				0x0e
#define VK_L				0x0f
#define VK_M				0x10
#define VK_N				0x11
#define VK_O				0x12
#define VK_P				0x13
#define VK_Q				0x14
#define VK_R				0x15
#define VK_S				0x16
#define VK_T				0x17
#define VK_U				0x18
#define VK_V				0x19
#define VK_W				0x1a
#define VK_X				0x1b
#define VK_Y				0x1c
#define VK_Z				0x1d
#define VK_1				0x1e
#define VK_2				0x1f
#define VK_3				0x20
#define VK_4				0x21
#define VK_5				0x22
#define VK_6				0x23
#define VK_7				0x24
#define VK_8				0x25
#define VK_9				0x26
#define VK_0				0x27
#define VK_ENTER			0x28
#define VK_ESC				0x29
#define VK_BACKSPACE		0x2a
#define VK_TAB				0x2b
#define VK_SPACE			0x2c	
#define VK_MINUS			0x2d
#define VK_EQUAL			0x2e
#define VK_LBRACE			0x2f
#define VK_RBRACE			0x30
#define VK_BACKSLASH		0x31
#define VK_NUMBER			0x32
#define VK_SEMICOLON		0x33
#define VK_QUOTE			0x34
#define VK_TILDE			0x35
#define VK_COMMA			0x36
#define VK_PERIOD			0x37
#define VK_SLASH			0x38
#define VK_CAPITAL			0x39
#define VK_F1				0x3a
#define VK_F2				0x3b
#define VK_F3				0x3c
#define VK_F4				0x3d
#define VK_F5				0x3e
#define VK_F6				0x3f
#define VK_F7				0x40
#define VK_F8				0x41
#define VK_F9				0x42
#define VK_F10				0x43
#define VK_F11				0x44
#define VK_F12				0x45
#define VK_PRINTSCREEN		0x46
#define VK_SCR_LOCK			0x47	
#define VK_PAUSE			0x48
#define VK_INSERT			0x49
#define VK_HOME				0x4a
#define VK_PAGE_UP			0x4b
#define VK_DELETE			0x4c
#define VK_END				0x4d
#define VK_PAGE_DOWN		0x4e
#define VK_RIGHT			0x4f
#define VK_LEFT				0x50
#define VK_DOWN				0x51
#define VK_UP				0x52
#define VK_NUM_LOCK			0x53
#define VKPAD_SLASH			0x54
#define VKPAD_ASTERIX		0x55
#define VKPAD_MINUS			0x56
#define VKPAD_PLUS			0x57
#define VKPAD_ENTER			0x58
#define VKPAD_1				0x59
#define VKPAD_2				0x5a
#define VKPAD_3				0x5b
#define VKPAD_4				0x5c
#define VKPAD_5				0x5d
#define VKPAD_6				0x5e
#define VKPAD_7				0x5f
#define VKPAD_8				0x60
#define VKPAD_9				0x61
#define VKPAD_0				0x62
#define VKPAD_PERIOD		0x63
#define VK_K45				0x64
#define VK_APP				0x65

//  NOT standard,  use these reserved code to distinguish ctrol keys
#define	VK_CTRL				0x90
#define	VK_SHIFT			0x91
#define	VK_ALT				0x92
#define VK_WIN				0x93
#define	VK_RCTRL			0x94
#define	VK_RSHIFT			0x95
#define	VK_RALT				0x96

enum{
    VK_EXT_START = 			0xa0,
    
    VK_SYS_START = 			VK_EXT_START,
    VK_POWER = 				VK_SYS_START,
    VK_SLEEP,
    VK_WAKEUP,
    VK_SYS_END,
	VK_SYS_CNT =			(VK_SYS_END - VK_SYS_START),

    VK_MEDIA_START = 		VK_SYS_END,
    VK_W_SRCH = 			VK_MEDIA_START,
    VK_WEB,
    VK_W_BACK,
    VK_W_FORWRD,
    VK_W_STOP,
    VK_W_REFRESH,
    VK_W_FAV,
    VK_MEDIA,
    VK_MAIL,
    VK_CAL,
    VK_MY_COMP,
    VK_NEXT_TRK,
    VK_PREV_TRK,
    VK_STOP,
    VK_PLAY_PAUSE,
    VK_W_MUTE,
    VK_VOL_UP,
    VK_VOL_DN,
    VK_MEDIA_END,
    //VK_EXT_END = 		VK_MEDIA_END,
	VK_MEDIA_CNT = 			(VK_MEDIA_END - VK_MEDIA_START),

    VK_SPEC_START = 		VK_MEDIA_END,//b7
    VK_PAD_SWITCH = 		VK_SPEC_START,//b7
    VK_SPEC_MIC_ON 	  = 		VK_SPEC_START,
    VK_SPEC_MIC_OFF,
	VK_SPEC_MODE_HAND,
	VK_SPEC_MODE_MOUSE,
	VK_SPEC_MODE_TV,
    VK_SPEC_END,//b8
    VK_SPEC_CNT = 			(VK_SPEC_END - VK_SPEC_START),
    VK_EXT_END = 			VK_SPEC_END,
};
#define	VK_FN				0xff

#define	VK_EXT_LEN           2
typedef struct{
	u8 val[VK_EXT_LEN];
}vk_ext_t;

// mulit-byte keycode for media keys, cannot used directly in c code..for reference
#define VK_POWER_V			0x01
#define VK_SLEEP_V			0x02
#define VK_WAKEUP_V			0x04

#define VK_W_SRCH_V			{0x21,0x02}
#define VK_WEB_V			{0x23,0x02}
#define VK_W_BACK_V			{0x24,0x02}
#define VK_W_FORWRD_V		{0x25,0x02}
#define VK_W_STOP_V			{0x26,0x02}
#define VK_W_REFRESH_V		{0x27,0x02}
// favorite
#define VK_W_FAV_V			{0x2a,0x02}
#define VK_MEDIA_V			{0x83,0x01}
#define VK_MAIL_V			{0x8a,0x01}
// calculator
#define VK_CAL_V			{0x92,0x01}
#define VK_MY_COMP_V		{0x94,0x01}
// next track -- 01(mosue-ep/USB_EDP_MOUSE) 05(len) 03(kb-report-id/USB_HID_KB_MEDIA) 
// b5(val) 00 00 00
#define VK_NEXT_TRK_V		{0xb5,0x00}
#define VK_PREV_TRK_V		{0xb6,0x00}
#define VK_STOP_V			{0xb7,0x00}
#define VK_PLAY_PAUSE_V		{0xcd,0x00}
#define VK_W_MUTE_V			{0xe2,0x00}
#define VK_VOL_UP_V			{0xe9,0x00}
#define VK_VOL_DN_V			{0xea,0x00}


#define VK_SPEC_MIC_ON_V		{0x72,0x01}
#define VK_SPEC_MIC_OFF_V		{0x73,0x01}
#define VK_SPEC_MODE_HAND_V		{0x74,0x01}
#define VK_SPEC_MODE_MOUSE_V	{0x75,0x01}
#define VK_SPEC_MODE_TV_V		{0x76,0x01}
