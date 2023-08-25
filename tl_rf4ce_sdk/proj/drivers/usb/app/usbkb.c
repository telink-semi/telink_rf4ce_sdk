/********************************************************************************************************
 * @file    usbkb.c
 *
 * @brief   This is the source file for usbkb.c
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

#include "../../../tl_common.h"
#include "../../airmouse.h"

#if(MODULE_USB_ENABLE && MODULE_KEYBOARD_ENABLE)



u32 debug_kbdata_report;
u32 debug_kbdata_report_normal;
u32 debug_kbdata_report_normal_ok;
u32 debug_kbdata_report_sys;
u32 debug_kbdata_report_sys_ok;
u32 debug_kbdata_report_media;
u32 debug_kbdata_report_media_ok;


#include "../os/sys.h"
#include "usbkb.h"
#include "../drivers/usb.h"
//#include "../drivers/usbhw.h"
//#include "../drivers/usbhw_i.h"
#include "../drivers/usbkeycode.h"
//#include "../os/ev.h"
#include "usbmouse.h"
#include "../../vendor/common/rf_frame.h"

int usbkb_hid_report_normal(u8 ctrl_key, u8 *keycode);

static u8 vk_sys_map[VK_SYS_CNT] = {
	VK_POWER_V, VK_SLEEP_V, VK_WAKEUP_V
};

static vk_ext_t vk_media_map[VK_MEDIA_CNT+VK_SPEC_CNT] = {
    {VK_W_SRCH_V},
    {VK_WEB_V},
    {VK_W_BACK_V},
    {VK_W_FORWRD_V},
    {VK_W_STOP_V},
    {VK_W_REFRESH_V},
    {VK_W_FAV_V},
    {VK_MEDIA_V},
    {VK_MAIL_V},
    {VK_CAL_V},
    {VK_MY_COMP_V},
    {VK_NEXT_TRK_V},
    {VK_PREV_TRK_V},
    {VK_STOP_V},
    {VK_PLAY_PAUSE_V},
    {VK_W_MUTE_V},
    {VK_VOL_UP_V},
    {VK_VOL_DN_V},

};

enum{
    KB_NORMAL_RELEASE_MASK = BIT(0),
    KB_SYS_RELEASE_MASK = BIT(1),
    KB_MEDIA_RELEASE_MASK = BIT(2),
    KB_SPECIAL_RELEASE_MASK = BIT(3),
};

static u32 usbkb_not_released;
static u32 usbkb_data_report_time;


//extern u16 device_duration[USB_INTF_MAX][USB_HID_MAX];
u8 repeat_ctrl_key = 0;
u8 repeat_keycode[KEYBOARD_REPORT_KEY_MAX] = {0};

int kb_report_process(void){
	usbkb_hid_report_normal(repeat_ctrl_key, repeat_keycode);
	return 0;
}
ev_time_event_t kb_report_timer = {kb_report_process};

static void usbkb_release_normal_key(void){
	if(usbkb_not_released & KB_NORMAL_RELEASE_MASK){
		u8 normal_keycode[KEYBOARD_REPORT_KEY_MAX] = {0};
		if(usbkb_hid_report_normal(0, normal_keycode)){
			BM_CLR(usbkb_not_released, KB_NORMAL_RELEASE_MASK);
		}
	}
	/*if(repeat_en){
		repeat_en = 0;
		ev_unon_timer(&kb_report_timer);
		foreach(i, KEYBOARD_REPORT_KEY_MAX){
			repeat_keycode[i] = 0;
		}
	}*/
	foreach(i, KEYBOARD_REPORT_KEY_MAX){
		repeat_keycode[i] = 0;
	}
}

static void usbkb_release_sys_key(void){
	if((usbkb_not_released & KB_SYS_RELEASE_MASK)){
		u32 release_data = 0;
		/*if(usbhotkey_hid_report(USB_HID_KB_SYS, (u8*)(&release_data), 1)){
			BM_CLR(usbkb_not_released, KB_SYS_RELEASE_MASK);
		}*/
	}
}

static void usbkb_release_media_key(void){
	if(usbkb_not_released & KB_MEDIA_RELEASE_MASK){
		u8 ext_keycode[MEDIAKEY_REPORT_DATA_LEN] = {0};
		/*if(usbhotkey_hid_report(USB_HID_KB_MEDIA, ext_keycode, MEDIAKEY_REPORT_DATA_LEN)){
			BM_CLR(usbkb_not_released, KB_MEDIA_RELEASE_MASK);
		}*/
	} else if(usbkb_not_released & KB_SPECIAL_RELEASE_MASK){
		u8 ext_keycode[SPECIAL_REPORT_DATA_LEN] = {0};
		/*if(usbhotkey_hid_report(USB_HID_KB_SPEC, ext_keycode, SPECIAL_REPORT_DATA_LEN)){
			BM_CLR(usbkb_not_released, KB_SPECIAL_RELEASE_MASK);
		}*/
	}
}

static void usbkb_release_keys(void){
	// Ҫusbkb_release_sys_key usbkb_release_normal_key ˳
	// Ϊ usbkb_release_sys_key,  usbkb_release_media_key мһʱ
	//usbkb_release_sys_key();
	usbkb_release_normal_key();
	//usbkb_release_media_key();
}

void usbkb_release_check(){
	if(usbkb_not_released && clock_time_exceed(usbkb_data_report_time, USB_KEYBOARD_RELEASE_TIMEOUT)){
		usbkb_release_keys();	 //  release keys
	}

}

//  normal_keycode 淵ͨ
int usbkb_separate_key_types(u8 *keycode, u8 cnt, u8 *normal_key, u8 *ext_key){
    STATIC_ASSERT(KB_RETURN_KEY_MAX <= KEYBOARD_REPORT_KEY_MAX);
    assert(cnt <= KB_RETURN_KEY_MAX);

    int normal_cnt = 0;
    foreach(i, cnt){
        if(keycode[i] >= VK_EXT_START && keycode[i] < VK_EXT_END){
        	*ext_key = keycode[i];
        }else{
            normal_key[normal_cnt++] = keycode[i];
        }
    }
	return normal_cnt;
}

int usbkb_hid_report_normal(u8 ctrl_key, u8 *keycode){
	if(usb_has_suspend_irq || sys_stat_ctrl.host_suspend_mode){		//  do not report anything when in suspend. Maybe not unnecessary
		return 1;
	}
	if(usbhw_is_ep_busy(USB_EDP_KEYBOARD_IN))
		return 0;

	reg_usb_ep_ptr(USB_EDP_KEYBOARD_IN) = 0;

	// please refer to keyboard_report_desc
	reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = ctrl_key;
	reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = 0;//resv
	foreach(i, KEYBOARD_REPORT_KEY_MAX){
	    reg_usb_ep_dat(USB_EDP_KEYBOARD_IN) = keycode[i];
	}
	reg_usb_ep_ctrl(USB_EDP_KEYBOARD_IN) = FLD_EP_DAT_ACK;		// ACK
	return 1;
}


static inline void usbkb_report_normal_key(int ctrl_key, u8 *keycode, int cnt){
	int report_normal_key = 0;

	repeat_ctrl_key = report_normal_key;
	foreach(i, KEYBOARD_REPORT_KEY_MAX){
		repeat_keycode[i] = keycode[i];
	}

	if(cnt > 0){
		for(int i; i < cnt; i++){
			if(keycode[i] != 0){
				report_normal_key = 1;
				break;
			}
		}
		if(report_normal_key || (ctrl_key != 0)){
			if(usbkb_hid_report_normal(ctrl_key, keycode)){
				/*if(device_duration[USB_INTF_KEYBOARD][0] != 0xffff){
					ev_on_timer(&kb_report_timer, device_duration[USB_INTF_KEYBOARD][0] * 1000);
					repeat_en = 1;
				}*/
		    	BM_SET(usbkb_not_released, KB_NORMAL_RELEASE_MASK);
			}
			return;
		}/*else{
			ev_unon_timer(&kb_report_timer);
			repeat_en = 0;
		}*/
	}
	usbkb_release_normal_key();
}

static inline void usbkb_report_sys_key(u8 ext_key){
	if(ext_key >= VK_SYS_START && ext_key < VK_SYS_END){
		int idx = ext_key - VK_SYS_START;
		/*if(usbhotkey_hid_report(USB_HID_KB_SYS, (u8*)(&vk_sys_map[idx]), 1)){	// assert sys key len == 1, check descriptor
			BM_SET(usbkb_not_released, KB_SYS_RELEASE_MASK);
		}*/
	}else{
		usbkb_release_sys_key();
	}
}

static inline void usbkb_report_media_key(u8 ext_key){
	if(ext_key >= VK_MEDIA_START && ext_key < VK_MEDIA_END){
		STATIC_ASSERT(VK_EXT_LEN <= MOUSE_REPORT_DATA_LEN);

		u8 ext_keycode[MEDIAKEY_REPORT_DATA_LEN] = {0};

		int idx = ext_key - VK_MEDIA_START;
		foreach(i, VK_EXT_LEN){
			ext_keycode[i] = vk_media_map[idx].val[i];
		}
		/*if(usbhotkey_hid_report(USB_HID_KB_MEDIA, ext_keycode, MEDIAKEY_REPORT_DATA_LEN)){
			BM_SET(usbkb_not_released, KB_MEDIA_RELEASE_MASK);
		}*/
    }else if(ext_key >= VK_SPEC_START && ext_key < VK_SPEC_END){
		STATIC_ASSERT(VK_EXT_LEN <= SPECIAL_REPORT_DATA_LEN);

		u8 ext_keycode[SPECIAL_REPORT_DATA_LEN] = {0};

		int idx = ext_key - VK_SPEC_START + VK_MEDIA_CNT;
		foreach(i, VK_EXT_LEN){
			ext_keycode[i] = vk_media_map[idx].val[i];
		}
		/*if(usbhotkey_hid_report(USB_HID_KB_SPEC, ext_keycode, SPECIAL_REPORT_DATA_LEN)){
			BM_SET(usbkb_not_released, KB_SPECIAL_RELEASE_MASK);
		}*/
    }else{
		usbkb_release_media_key();
	}
}

static inline int usbkb_check_repeat_and_save(kb_data_t *data){
    assert(data);
	static kb_data_t last_data;
	int same = kb_is_data_same(&last_data, data);
	if(!same){
		STATIC_ASSERT(sizeof(last_data) == 8);
	    ((u32*) (&last_data))[0] = ((u32*) (data))[0];
	    ((u32*) (&last_data))[1] = ((u32*) (data))[1];
	}
	return same;
}

int usbkb_hid_report(kb_data_t *data){
	if(usb_has_suspend_irq || sys_stat_ctrl.host_suspend_mode){		//  do not report anything when in suspend. Maybe not unnecessary
		BM_CLR(usbkb_not_released, KB_SYS_RELEASE_MASK);
		BM_CLR(usbkb_not_released, KB_MEDIA_RELEASE_MASK);
		BM_CLR(usbkb_not_released, KB_SPECIAL_RELEASE_MASK);
		return 0;
	}
    assert(data);
    u8 ext_key = VK_EXT_END, normal_key_cnt = 0;
    u8 normal_keycode[KEYBOARD_REPORT_KEY_MAX] = {0};

	if(data->cnt > KB_RETURN_KEY_MAX){	// must,   in case bad packets
		return 1;
	}


	normal_keycode[0] = data->keycode[0];

	/*	http://msdn.microsoft.com/en-us/windows/hardware/gg462991.aspx
		It is also important to notice that any re-triggering of events should be done by software timers in the host system and not by hardware timers in the device itself.
		For example, if the user keeps pressing the Volume Increment button, the device should only generate one input report with this state information
	*/
	if(data->cnt > 0 && usbkb_check_repeat_and_save(data)){
		if(usbkb_not_released){
			usbkb_data_report_time = clock_time();
			return 1;
		}
	}

	//usbkb_report_normal_key(data->ctrl_key, normal_keycode, 1);
	//return;

#if 1
	if(data->cnt > 0){
		//  keycodeദ
	    //normal_key_cnt = usbkb_separate_key_types(data->keycode, data->cnt, normal_keycode, &ext_key);
	}

	normal_keycode[0] = data->keycode[0];
	normal_key_cnt = 1;
	usbkb_report_normal_key(data->ctrl_key, normal_keycode, normal_key_cnt);
	usbkb_data_report_time = clock_time();
	return 1;
	usbkb_report_sys_key(ext_key);
	usbkb_report_media_key(ext_key);
#endif

    usbkb_data_report_time = clock_time();
	return 1;
}

void usbkb_init(){
	//ev_on_poll(EV_POLL_KEYBOARD_RELEASE_CHECK, usbkb_release_check);
}

#endif
