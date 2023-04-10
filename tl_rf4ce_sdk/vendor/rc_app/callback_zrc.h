/********************************************************************************************************
 * @file    callback_zrc.h
 *
 * @brief   This is the header file for callback_zrc.h
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

#define    PROFILE_DEFAULT_KEY_TYPE			ZRC2_ACTION_TYPE_ATOMIC

#if  (RF4CE_ZRC2_ENABLE)
#define    profile_doUnpair					zrc_doUnpair
#define    profile_doPair					zrc_doPair
#define    profile_PowerStatusPush			zrc_PowerStatusPush
#endif


u8 profile_sendValidationKey(u8 pairingRef, u8 txOptions,u8 keycode);
u8  profile_startSendRFKeyCmd(u8 pairingRef, u8 txOptions, u8 keycode);
void profile_stopSendRFKeyCmd(u8 pairingRef);

int zrc_doPair(void *arg);
s32 zrc_doUnpair(void *arg);
int zrc_waitForNextKeyTimerCb(void *arg); 
void zrc_startWaitForNextKeyTimer(void); 
void zrc_stopWaitForNextKeyTimer(void);
int zrc_verifyTVBrand(u16 TVcode); 
void zrc_handleKeyPressed(void *arg);
void zrc_handleKeyReleased(void *arg); 

void zrc_restoreAppState(void);
void zrc_lightOnProgLed(void);
int zrc_ledShowVerInfo(void *arg);
void zrc_startSendCarrier(void *arg);
void zrc_sendCarrierOnNextCh(void);
void zrc_ckValidResp(u8 pairingRef, u8 status);
u8 ota_cmdCallback(u8 id, u8 status);
void zrc_numberRecall(void *arg);
int zrc_delayAllowDeep(void *arg);


zrc_sts_t zrc_PowerStatusPush(u8 status);
void stack_init(void);
void stack_saveProfileInfo(void *arg);
void zrcApp_initPib(void);
