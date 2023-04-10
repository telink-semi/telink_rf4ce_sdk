/********************************************************************************************************
 * @file    app_const.h
 *
 * @brief   This is the header file for app_const.h
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

#pragma  once

/**
 *  @brief Definition for the report data interval, 1s
 */

/**
 *  @brief Definition for the zrc app cmd
 */
enum {
    ZRC_APP_ID_PARING_KEYCODE =  0x20,
    ZRC_APP_ID_NORMAL_KEY =  0x21,
};

enum {
    ZRC_APP_CMD_CODE_IND =  0xF1,
    ZRC_APP_VALIDATION_KEY_IND =  0xF2,
    ZRC_APP_VALIDATION_CODE_IND =  0xF3,
    ZRC_APP_SET_ATTR_IND =  0xF4,
    ZRC_APP_VALIDATION_SUCC_IND =  0xF5,
    ZRC_APP_UNPIAR_IND =  0xF6,
};

/**
 *  @brief Definition for the mso app validation state 
 */

enum {
    ZRC_APP_INVALID_STATE,
    ZRC_APP_START_VALIDATION,
    ZRC_APP_VALIDATING_STATE,
    ZRC_APP_VALIDATED_STATE,
    ZRC_APP_NORMAL_STATE,
};

/**
 *  @brief Definition for the mso app cmd
 */
enum {
    MSO_APP_ID_PARING_KEYCODE =  0x20,
    MSO_APP_ID_NORMAL_KEY =  0x21,
};

static const u16 batteryVoltage[] = {
		3000,
		2600,
		2400,
		2000,
};

enum{
	PWR_LEVEL_FULL = 0,
	PWR_LEVEL_2P6_UP,
	PWR_LEVEL_2P4_6,
	PWR_LEVEL_CUTOFF  // <2.0v
};

#define 	PWR_THRESHOLD_NORMAL	PWR_LEVEL_2P6_UP
#define 	PWR_THRESHOLD_RESET		PWR_LEVEL_2P4_6


#if  RF4CE_MSO_ENABLE
/**
 *  @brief Definition for the mso app cmd
 */

typedef enum {
    MSO_APP_CMD_CODE_IND =  0xF1,
    MSO_APP_VALIDATION_KEY_IND =  0xF2,
    MSO_APP_VALIDATION_CODE_IND =  0xF3,
    MSO_APP_SET_ATTR_IND =  0xF4,
    MSO_APP_VALIDATION_SUCC_IND =  0xF5,
    MSO_APP_UNPIAR_IND =  0xF6,
    MSO_APP_BOND_IND =  0xF7,
    MSO_APP_VALIDATION_FAIL_IND =  0xF8,
}mso_indication;

/**
 *  @brief Definition for the mso app validation state
 */

typedef enum {
    MSO_APP_INVALID_STATE,
    MSO_APP_START_VALIDATION,
    MSO_APP_VALIDATING_STATE,
    MSO_APP_VALIDATED_STATE,
    MSO_APP_NORMAL_STATE,
    MSO_APP_START_BINDING_STATE,
}mso_target_state;

#define APP_VID_SUPPPORT_MAX	3

typedef struct{
	u16 vendId;
	u8  vendString[7];
	u8  userString[15];
}appVendInfo_t;
extern const appVendInfo_t app_vendor_info_list[];
#endif


