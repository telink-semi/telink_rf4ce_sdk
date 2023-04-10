/********************************************************************************************************
 * @file    callback_mso.h
 *
 * @brief   This is the header file for callback_mso.h
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

#include "../../proj/drivers/keyboard.h"

#define    PROFILE_DEFAULT_KEY_TYPE			ZRC_USER_CTRL_PRED

#if  (RF4CE_MSO_ENABLE)
#define    profile_doUnpair					mso_doUnpair
#define    profile_doPair					mso_doPair
#define    profile_PowerStatusPush			mso_PowerStatusPush
#endif

extern const profile_cbFunc_t mso_profileCmdCallbacks;

//extern const mso_userCbFunc_t mso_cmdCallbacks;

#define isSetElem(e,e0,e1) ((e == e0)||(e == e1))
#define isCombKey(k0,k1,e0,e1) (isSetElem(k0,e0,e1)&&isSetElem(k1,e0,e1))

int mso_doPair(void *arg);
//void mso_doUnpair(void *arg);
int mso_waitForNextKeyTimerCb(void *arg); 
void mso_startWaitForNextKeyTimer(void); 
void mso_stopWaitForNextKeyTimer(void);
int mso_verifyTVBrand(u16 TVcode); 
void mso_handleKeyPressed(void *arg);
void mso_handleKeyReleased(void *arg); 
void keyScan_keyReleasedCB(u8 keyCode);//Cnt, u8 *keyIndex);
//void keyScan_keyPressedCB(u8 keyCOde);//Cnt, u8 *keyIndex);
void keyScan_keyPressedCB(kb_data_t *kbEvt);
void mso_restoreAppState(void);
void mso_lightOnProgLed(void);
//void mso_ledShowBattery(mso_batteryLevel_t level);
int mso_ledShowVerInfo(void *arg);
void mso_startSendCarrier(void *arg);
void mso_sendCarrierOnNextCh(void);
void mso_ckValidResp(u8 pairingRef, u8 status);
u8 ota_cmdCallback(u8 id, u8 status);
void mso_numberRecall(void *arg);
int mso_delayAllowDeep(void *arg);
//void mso_showBatteryStatus(mso_batteryLevel_t level);




void stack_saveProfileInfo(void *arg);

//void profile_PowerStatusPush(mso_batteryLevel_t level);

void stack_init(void);
s32 mso_doUnpair(void *arg);
void mso_appDataIndCb(u8 pairingRef, u8 *pd, u8 len);
void mso_PowerStatusPush(u8 level);
u8 profile_sendValidationKey(u8 pairingRef, u8 txOptions,u8 keycode);
u8  profile_startSendRFKeyCmd(u8 pairingRef, u8 txOptions, u8 keycode);
void profile_stopSendRFKeyCmd(u8 pairingRef);



