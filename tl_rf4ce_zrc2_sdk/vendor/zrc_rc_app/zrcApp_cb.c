/********************************************************************************************************
 * @file     zrcApp_cb.c
 *
 * @brief    some call back funnctions for ZRC2 stack, key being pressed
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

#include "../../proj/tl_common.h"
#if (__PROJECT_ZRC_2_RC__ && !BOARD_UR2_211RF_8267)

/**********************************************************************
 * INCLUDES
 */
#include "../../net/rf4ce/rf4ce_includes.h"
#include "../../net/rf4ce/profile/gdp/gdp.h"
#if (RF4CE_ZRC2_ENABLE)
#include "../../net/rf4ce/profile/zrc2/zrc_api.h"
#else
#include "../../net/rf4ce/profile/zrc/zrc_api.h"
#endif

#include "../../proj/drivers/keyboard.h"
#include "../../proj/drivers/drv_flash.h"

#include "../../proj/os/sys.h"

#if (MODULE_AUDIO_ENABLE)
#include "../../proj/drivers/audio/drv_audio.h"
#include "../common/TL_specificData/TL_specific_data.h"
#include "../common/TL_specificData/TL_specific_data_audio.h"
#include "../common/TL_specificData/TL_specific_data_ota.h"
#endif
#include "zrc_const.h"
#include "zrcApp.h"
#include "zrcApp_cb.h"
#include "zrcApp_led.h"
#include "zrcApp_data.h"
#include "zrcApp_config.h"
#include "qsIR_Tx.h"
/**********************************************************************
 * LOCAL CONSTANTS
 */



/**********************************************************************
 * TYPEDEFS
 */

/**********************************************************************
 * LOCAL FUNCTIONS
 */
void zrc_startCnfCb(u8 status);
void zrc_pairCnfCb(u8 pairingRef, u8 status, u8 profileSize, u8 *profileList);
void zrc_checkValidationCb(u8 pairingRef, u8 status);
void zrc_cmdSendCb(u8 pairingRef, u8 status, u8 handle);
void zrc_unPairCnfCb(u8 pairingRef, u8 status);
int zrc_doPair(void *arg);
s32 zrc_doUnpair(void *arg);
void zrc_startBindLed(void *arg);
void zrc_bondCnfCb(u8 pairingRef, u8 status);
void zrc_attrPullRespCb(u8 pairingRef, u8 attrib, u16 entryId, u8 status, u8 len, u8 *val);
void zrc_attrPushRespCb(u8 pairingRef, u8 status);
void gdp_attrPushRespCb(u8 pairingRef, u8 status);
void gdp_afterBindingCb(u8 pairingRef, u8 status);
void zrc_appDataIndCb(u8 pairingRef, u8 *pd, u8 len);
/**********************************************************************
 * GLOBAL VARIABLES
 */

ev_time_event_t *zrc_showNumTimer;
ev_time_event_t *zrc_sendRibTimer;

/**********************************************************************
 * LOCAL VARIABLES
 */

const profile_cbFunc_t zrc_profileCmdCallbacks =
{
    zrc_startCnfCb,                     // start stack confirm command
    zrc_pairCnfCb,
	zrc_unPairCnfCb,
	NULL,
};

zrc_userCbFunc_t zrc_cmdCallbacks =
{
    zrc_checkValidationCb,               //!< Function pointer to the ZRC check validation resp command
    NULL,               //!< Function pointer to the ZRC check validation req command
    NULL,               //!< Function pointer to the received ZRC set attribute resp command
    NULL,               //!< Function pointer to the received ZRC get attribute response command
    NULL, //!< Function pointer to the received ZRC Pull attribute response command
    NULL, //!< Function pointer to the received ZRC Push attribute response command
    zrc_bondCnfCb,               //!< Function pointer to the received ZRC bind confirm command
    NULL,               //!< Function pointer to handle the received ZRC command. Equal to command request indication.
    zrc_cmdSendCb,      //!< Function pointer to the received ZRC data sent confirm command
    NULL,               //!< Function pointer to the received ZRC validation key command
    NULL,
    zrc_appDataIndCb,//NULL,				//!< Function pointer to the received vendor specific data
};

gdp_userCbFunc_t gdp_cmdCallbacks = {
	gdp_afterBindingCb,
	NULL,
	NULL,
	NULL,   //push attrbute indicate
	NULL,
};


#if RF4CE_SWITCHTO_BLE_EN
#define APP_ADDR_START_CODE     	0x7F000
#define APP_ADDR_BOOT_VALID_ADDR    0x08

u8 app_bootFromRf4ce(void){
	u8 bootFlag;
	flash_read(APP_ADDR_BOOT_VALID_ADDR, 1, &bootFlag);
	if(bootFlag == 0x4b){
		return 1;
	}
	return 0;
}

s32 zrc_sysReboot(void *arg){
	if(!zrc_appVars.ledBlinkTimer){
		mcu_reset();
		return -1;
	}else{
		return 0;
	}
}

static void bleSwitchToRf4ce(void){
	/* store the first 4KB code to 0x7f000, so that BLE can switch to rf4ce mode */
	flash_erase(0);
	u32 srcAddrOft = APP_ADDR_START_CODE;
	u32 dstAddrOft = 0;
	u32 blkLen = 32;
	u8 tmpBuf[32];
	for(u32 i = 0; i < 16; i++){
		for(u32 j = 0; j < 8; j++){
			flash_read(srcAddrOft, blkLen, tmpBuf);
			flash_write(dstAddrOft, blkLen, tmpBuf);
			srcAddrOft += blkLen;
			dstAddrOft += blkLen;
		}
	}

	zrc_startLed(5, ZRC_APP_LED_BLINK_QUICK_TIME, 1);

	ev_on_timer(zrc_sysReboot, NULL, 250*1000);
}

static void rf4ceSwitchToBle(void){
	/* store the first 4KB code to 0x7f000, so that BLE can switch to rf4ce mode */
	flash_erase(APP_ADDR_START_CODE);
	u32 srcAddrOft = 0;
	u32 dstAddrOft = APP_ADDR_START_CODE;
	u32 blkLen = 32;
	u8 tmpBuf[32];
	for(u32 i = 0; i < 16; i++){
		for(u32 j = 0; j < 8; j++){
			flash_read(srcAddrOft, blkLen, tmpBuf);
			flash_write(dstAddrOft, blkLen, tmpBuf);
			srcAddrOft += blkLen;
			dstAddrOft += blkLen;
		}
	}

	/* rewrite the value at the address of 0x08 to 0, so as to restart from 0x40000 */
	u8 value = 0;
	flash_write(APP_ADDR_BOOT_VALID_ADDR, 1, &value);

	zrc_startLed(3, ZRC_APP_LED_OTA_RUN_TIME, 1);

	ev_on_timer(zrc_sysReboot, NULL, 250*1000);
}
#endif

/*********************************************************************
 * @fn      zrc_startCnfCb
 *
 * @brief   Callback function for profile_startNwk.
 *
 * @param   status - Network start status, it is set by stack
 *
 * @return  None
 */
void zrc_startCnfCb(u8 status)
{
    u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	if ( is_timer_expired(zrc_appVars.ledBlinkTimer) ) {
        zrc_appVars.flags.bf.allowedDeep = 1;
    }
}



/*********************************************************************
 * @fn      zrc_pairCnfCb
 *
 * @brief   Callback function for pair .
 *
 * @param   status     - Network start status, it is set by stack
 *
 * @param   pairingRef - indicating the target stored in which pair table entry
 *
 * @param   profileSize - NOT used
 *
 * @param   profileList - NOT used
 *
 * @return  None
 */

void zrc_pairCnfCb(u8 pairingRef, u8 status, u8 profileSize, u8 *profileList)
{
	u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	if ( status == SUCCESS ) {
	    zrc_appInfo.pairingRef = pairingRef;
	    /* move to verify state */
		zrc_setAppState(ZRC_VERIFY_PAIR_STATE);
		/* init keypressed tiem */
		zrc_appVars.keyPressedTime = clock_time();
	}
}


void zrc_checkValidationCb(u8 pairingRef, u8 status){
	if(zrc_getAppState() == ZRC_VERIFY_PAIR_STATE){
		zrc_setAppState(ZRC_CHECK_VALIDATION_STATE);
	}
}

/*********************************************************************
 * @fn      zrc_pairCnfCb
 *
 * @brief   Callback function for pair .
 *
 * @param   status     - Network start status, it is set by stack
 *
 * @param   pairingRef - indicating the target stored in which pair table entry
 *
 * @param   profileSize - NOT used
 *
 * @param   profileList - NOT used
 *
 * @return  None
 */
void zrc_bondCnfCb(u8 pairingRef, u8 status)
{
	u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	zrc_appVars.flags.bf.appInPair = 0;

	if ( status == SUCCESS ) {
	    zrc_appInfo.pairingRef = pairingRef;
	    zrc_appInfo.state = ZRC_READY_STATE;

	    /* move to rf4ce ready state */
		zrc_setAppState(ZRC_READY_STATE);
		u32 blinkCnt = 3;
        zrc_startBindLed((void*)blinkCnt);
	}else if ( status == ZRC_RESTORE ) {
        zrc_appInfo.pairingRef = pairingRef;
        zrc_appInfo.state = ZRC_READY_STATE;
        zrc_setAppState(ZRC_READY_STATE);
        nv_userLoadFromFlash(ZRC_APP_NV_ITEM, sizeof(zrc_appInfo_t), (u8*)&zrc_appInfo);
        zrc_ledOff(1);
        zrc_stopLed(1);
		if ( zrc_appVars.flags.bf.ledIdleState == 0 ) {
			zrc_appVars.flags.bf.allowedDeep = 1;
		}
        return;
    }else {
		zrc_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
		zrc_appInfo.state = ZRC_UNCONNECT_STATE;
		zrc_setAppState(ZRC_UNCONNECT_STATE);
		zrc_stopLed(1);
		if ( zrc_appVars.flags.bf.ledIdleState == 0 ) {
			zrc_appVars.flags.bf.allowedDeep = 1;
		}
	}

	/* voltage lower than 2.2V, not save flash */
	if ( checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
        zrc_saveStackInfo(NULL);
        zrc_saveAppInfo(NULL);

        pmFlag_t pmInfo;
        pmInfo.byte = analog_read(reg_mac_channel);
        analog_write(reg_mac_channel, pmInfo.byte);
	}
	pmFlag_t pmInfo;
	pmInfo.byte = analog_read(reg_mac_channel);
	analog_write(reg_mac_channel, pmInfo.byte);
	zrc_ledOff(1);

}


/*********************************************************************
 * @fn      zrc_unPairCnfCb
 *
 * @brief   Callback function for unpair confirmation.
 *
 * @param   pairingRef  - the index of the pair entry
 *
 * @param   status  - the status of the command
 *
 * @return  None
 */
void zrc_unPairCnfCb(u8 pairingRef, u8 status)
{
    u32 blinkCnt = 2;
    zrc_setAppState(ZRC_UNCONNECT_STATE);

    zrc_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
    zrc_appInfo.state = ZRC_UNCONNECT_STATE;

	/* voltage lower than 2.2V, not save flash */
	if ( checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
		zrc_saveStackInfo(NULL);
        zrc_saveAppInfo(NULL);
	}
	else {
		blinkCnt = 4;
	}

    zrc_startBindLed((void*)blinkCnt);

    zrc_appVars.flags.bf.allowedDeep = 1;
}



/*********************************************************************
 * @fn      zrc_cmdSendCb
 *
 * @brief   Callback function for command sent confirmation.
 *
 * @param   status     - Indicating the command sent result
 *
 * @param   pairingRef - The pairingRef command sent to
 *  *
 * @param   handle 	   - The handle to notify the command type
 *
 * @return  None
 */
volatile u8 T_DBG_appCmdCnf[8] = {0};
void zrc_cmdSendCb(u8 pairingRef, u8 status, u8 handle)
{
	/* frame counter exceed the maximum one, reest the frame counter and unpair*/
	if ( status == RF4CE_NWK_FRAME_COUNTER_EXPIRED ) {
		u32 curFrameCnt = NWK_FRAME_COUNTER_DEFAULT_VALUE;
		nwk_nlmeSetReq(NWK_FRAME_COUNTER, 0, (u8*)&curFrameCnt);
		zrc_doUnpair(NULL);
	}
	
	if(status != SUCCESS){
		if(zrc_appVars.flags.bf.repeat && handle == RF4CE_ZRC_STANTARD_KEYCODE_HANDLE){
			/* if send repeate data timer is on, stop it. */
			zrc_restoreAppState();
			//zrc_stopSendRepeateData();
			zrc_timerForReleaseKeyStop();
			zrc_rcForceStopSendCmd(zrc_appInfo.pairingRef);
			zrc_appVars.flags.bf.allowedDeep = 1;
			zrc_appVars.flags.bf.inRfRpt = 0;
			zrc_stopSendRepeateDataTimer();
			zrc_appVars.flags.bf.repeat = 0;
		}
		T_DBG_appCmdCnf[0]++;
	}else{
		if(handle == RF4CE_ZRC_SPECIFIC_DATA_REQ_HANDLE){
			T_DBG_appCmdCnf[1]++;
		}else if(handle == RF4CE_ZRC_STANTARD_KEYCODE_HANDLE){
			T_DBG_appCmdCnf[2]++;
		}else if(handle == RF4CE_ZRC_PULL_ATTR_REQ_HANDLE){
			T_DBG_appCmdCnf[3]++;
		}else if(handle == RF4CE_ZRC_PUSH_ATTR_REQ_HANDLE){
			T_DBG_appCmdCnf[4]++;
		}else if(handle == RF4CE_ZRC_SET_ATTR_REQ_HANDLE){
			T_DBG_appCmdCnf[5]++;
		}else if(handle == RF4CE_ZRC_GET_ATTR_REQ_HANDLE){
			T_DBG_appCmdCnf[6]++;
		}
	}
}

void zrc_appDataIndCb(u8 pairingRef, u8 *pd, u8 len){
	tl_appDataIndicate( pd, len);
}

volatile u8 T_DBG_appAudioCb[8] = {0};
void zrc_appAudioCb(u8 state, u8 status){
	T_DBG_appAudioCb[0]++;
	if(state == AUDIO_OPENED && status == SUCCESS){
		zrc_setAppState(ZRC_AUDIO_STATE);
		zrc_ledOn(1);
		T_DBG_appAudioCb[1]++;
	}else if(state == AUDIO_IDLE){
		zrc_restoreAppState();
		zrc_ledOff(1);
		T_DBG_appAudioCb[2]++;
	}
	T_DBG_appAudioCb[4] = state;
	T_DBG_appAudioCb[5] = status;
}
/*********************************************************************
 * @fn      zrc_doPair
 *
 * @brief   Start pair procedure.
 *
 * @param   None
 *
 * @return  None
 */
int zrc_doPair(void *arg)
{
    u8 value = 1;
	/* voltage lower than 2.2V, can't initail the pairing  */
	if ( !checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
		zrc_startLed(4, ZRC_APP_LED_BLINK_QUICK_TIME, 1);
		return -1;
	}
	if ( zrc_appVars.flags.bf.appInPair == 0 ) {
	    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	    if ( SUCCESS == profile_startPair(1, DEVICE_ALL, 0x3, 0) ) {
	        zrc_setAppState(ZRC_PAIRING_STATE);
			/* if pair procedure, set the Tx power low */
			zrc_appVars.flags.bf.appInPair = 1;
			zrc_appVars.flags.bf.allowedDeep = 0;

			app_ledFlushInfo_t  ledAction;
			ledAction.onInterval = 200 * 1000;
			ledAction.modeNum = 3;
			ledAction.offIntervalList[0] = 150 * 1000;
			ledAction.offIntervalList[1] = 600 * 1000;
			ledAction.offIntervalList[2] = 600 * 1000;
			zrc_startLedSpecial(0, &ledAction, 1);

            return -1;
	    }else {
            ev_on_timer(zrc_doPair, NULL, 1);
	        return -1;
	    }
	}else{
	    return -1;
	}
}

void ota_mcuReboot(void);
void zrc_otaCb(u8 status){

	if(status == OTA_STA_SUCCESS){
		/* reboot */
		ota_mcuReboot();
	}
	else
	{
		u32 blinkCnt = 3;
		zrc_startLed(blinkCnt, 100*1000, 1);
	}
	zrc_setAppState(ZRC_READY_STATE);
	u8 value = 0;
	mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
}



void zrc_doOta(void){
	if ( (zrc_appInfo.pairingRef != RF4CE_INVALID_PAIR_REF) &&
		(zrc_getTxStatus() == 0)) {
//		if(ota_reqeustCmd(zrc_appInfo.pairingRef)){
//			zrc_appVars.flags.bf.allowedDeep = 1;
//			zrc_restoreAppState();
//            return;
//		}
	    zrc_startLed(0,2*1000*1000,1);
		zrc_setAppState(ZRC_OTA_STATE);
		u8 value = 1;
		mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
		ota_startReq(zrc_appInfo.pairingRef,zrc_otaCb);
	}else{
		zrc_appVars.flags.bf.allowedDeep = 1;
		zrc_restoreAppState();
	}
}



volatile u8 T_zrc_attrPushRespCbCnt = 0;
void zrc_attrPushRespCb(u8 pairingRef, u8 status){
	T_zrc_attrPushRespCbCnt++;
}

s32 zrc_userHandlerAfterBound(void *arg){
	if(zrc_getAppState() == ZRC_PRE_CONFIGURE_STATE){
		zrc_appVars.flags.bf.allowedDeep = 1;
		zrc_setAppState(ZRC_READY_STATE);
		return -1;
	}
	return 0;
}


/*********************************************************************
 * @fn      zrc_doUnpair
 *
 * @brief   Start unpair procedure.
 *
 * @param   None
 *
 * @return  None
 */
s32 zrc_doUnpair(void *arg)
{
    profile_unpairReq(zrc_appInfo.pairingRef);
    zrc_appVars.flags.bf.allowedDeep = 0;

    return -1;
}


/*********************************************************************
 * @fn      press_key_handler
 *
 * @brief   handle key pressed
 *
 * @param   none
 *
 * @return  none
 */

void keyScan_keyReleasedCB(u8 keyCode);
volatile u8 T_press_key_handler = 0;
void press_key_handler(u8 keyCode, u8 validKey){
	pmFlag_t pmInfo;
	pmInfo.byte = analog_read(reg_mac_channel);

	if((keyCode == 0xff) || (keyCode != validKey)){
		zrc_stopSendRepeateDataTimer();
	}

	if(keyCode == 0xff){
		return;
	}

	if(zrc_appVars.battSta >= BAT_THRESHOLD_WARNING){
		zrc_startLed(3, ZRC_APP_LED_BLINK_SEND_DATA_WARNING_TIME, 1);
	}else{
		zrc_startLed(1, ZRC_APP_LED_BLINK_SEND_DATA_TIME, 1);
	}

//	if(keyCode == VK_BACK){
//		defaultActionmappingRecovery();
//	}
//	if(keyCode == VK_VOLUME_DOWN || keyCode == VK_VOLUME_UP || keyCode == VK_MUTE){
//		zrc_volume_keyHandler(keyCode);
//		return;
//	}

	if(zrc_appInfo.pairingRef != RF4CE_INVALID_PAIR_REF){
		if(ZRC_READY_STATE == zrc_getAppState()){
#if MODULE_AUDIO_ENABLE
			if(keyCode == VK_RECORED){
#if 0
				/* send audio data directly */
				if (audio_recTaskStatusGet() == AUDIO_IDLE){
					audio_recTaskStart();
					zrc_ledOn(1);
				}else{
					audio_recTaskStop();
					zrc_ledOff(1);
				}
#else
				/* send audio start command to target, open audio once receiving the audio open response command
				 * need to work with TELINK dongle
				 * */
				if (audio_recTaskStatusGet() == AUDIO_OPENED){
					tl_audio_stop(PROFILE_ZRC2, zrc_appInfo.pairingRef);
				}else{
					tl_audio_start(PROFILE_ZRC2, zrc_appInfo.pairingRef);
				}
#endif
			}else{
				if (audio_recTaskStatusGet() != AUDIO_OPENED){

					if(keyCode == VK_TV){
						zrc_doOta();
						return;
					}
					else
					{
						zrc_sendRFData(PROFILE_ZRC, keyCode);
					}

				}
			}
#else
			zrc_sendRFData(PROFILE_ZRC, keyCode);
#endif
		}
	}else{

		if(keyCode==VK_NUM_0)
		{
			extern u8 volumnDown[];
			zrcIrInit(volumnDown);
			return;
		}

		if(keyCode==VK_NUM_1)
		{
			extern u8 volumnUp[];
			zrcIrInit(volumnUp);
			return;
		}

		zrc_sendIRData(ZRC_KEY_FOR_DTA, keyCode, 0);
		T_press_key_handler++;
	}
}

void release_key_handler(u8 keyCode){
	if(zrcIrGetState())
	{
		zrcStop();
	}
}

/**********************************************keyScan_keyPressedCB***********************
 * @fn      keyScan_keyPressedCB
 *
 * @brief   this function is called when key is presed
 *
 * @param   keyCnt number of keys are presed
 *
 * @param   keyIndex
 *
 * @return  none
 */
volatile u8 T_keyScan_keyPressedCB[4] = {0};
u8 key1=0xff, key2=0xff;
void keyScan_keyPressedCB(kb_data_t *kbEvt){
	u8 toNormal = 0;
	u8 keyCode = kbEvt->keycode[0];
	static u8 lastKeyCode = 0xff;

	if(zrc_appVars.battSta >= BAT_LEVEL_CUTOFF){
		zrc_appVars.flags.bf.allowedDeep = 1;
		return;
	}


    //u8
    key1=0xff;
    key2=0xff;
    if(kbEvt->cnt >= 2){
		if(kbEvt->cnt == 2){
			key1 = kb_event.keycode[0];
			key2 = kb_event.keycode[1];
		}
		keyCode = 0xff;
    }

    if(zrc_isNormalState() || ZRC_SENDING_IR_STATE == zrc_getAppState()){
    	if(kbEvt->cnt == 2){
    		keyCode = 0xff;
    	}
    	press_key_handler(keyCode, lastKeyCode);
    	lastKeyCode = keyCode;
    	T_keyScan_keyPressedCB[0]++;

#if RF4CE_SWITCHTO_BLE_EN
    	if((key1 == VK_VOLUME_DOWN && key2 == VK_VOLUME_UP) || (key1 == VK_VOLUME_UP && key2 == VK_VOLUME_DOWN)){
    		/* switch to BLE mode */
    		zrc_setAppState(ZRC_PRE_MODE_SWITCH);
    		zrc_appVars.keyPressedTime = clock_time();
    		zrc_appVars.flags.bf.allowedDeep = 0;
    	}
#endif

    	/*
		 * check if need to do pairing
		 *
		 * */
		//if((key1 == VK_HOMEKEY && key2 == VK_MENU) || (key1 == VK_MENU && key2 == VK_HOMEKEY)){
#if (MCU_CORE_8278)
    	if(keyCode == VK_OKKEY)
#else
    	if(keyCode == VK_HOMEKEY)
#endif
    	{
			/* enter Programming mode, stop repeat timer if need it */
			zrc_restoreAppState();
			zrc_stopSendRepeateDataTimer();
			release_key_handler(NULL);

			zrc_appVars.keyPressedTime = clock_time();
			zrc_setAppState(ZRC_PRE_PAIRING_STATE);
			zrc_appVars.flags.bf.allowedDeep = 0;
			return;
		}

		if(zrc_appInfo.pairingRef != RF4CE_INVALID_PAIR_REF
				&&(ZRC_READY_STATE == zrc_getAppState())
				&&(audio_recTaskStatusGet() != AUDIO_OPENED)){

			/*
			 * check if need to do un-pairing
			 *
			 * */
			if((key1 == VK_MENU && key2 == VK_BACK) || (key1 == VK_BACK && key2 == VK_MENU)){
				ev_on_timer(zrc_doUnpair, NULL, 5*100*1000);
				return;
			}
		}
	}
#if MODULE_AUDIO_ENABLE
    else if(ZRC_AUDIO_STATE == zrc_getAppState()){
		if(keyCode == VK_RECORED){
			tl_audio_stop(PROFILE_ZRC2, zrc_appInfo.pairingRef);
		}
		lastKeyCode = keyCode;
	}
#endif
    else if(ZRC_CHECK_VALIDATION_STATE == zrc_getAppState()){
		T_keyScan_keyPressedCB[1]++;
		if(keyCode >= VK_NUM_0 && keyCode <= VK_NUM_9){
			zrc_sendRFData(PROFILE_ZRC2, keyCode);
		}
	}

    if(toNormal == 1){
		zrc_restoreAppState();
		zrc_stopWaitForNextKeyTimer();
		zrc_appVars.flags.bf.allowedDeep = 1;
    }
}


/*********************************************************************
 * @fn      keyScan_keyReleasedCB
 *
 * @brief   this function is called when key is released
 *
 * @param   keyCnt number of keys are released
 *
 * @param   keyIndex
 *
 * @return  none
 */

void keyScan_keyReleasedCB(u8 keyCode){
	u32 toNormal = 0;

	if(zrc_isNormalState() || ZRC_SENDING_IR_STATE == zrc_getAppState()) {
#if(MODULE_PM_ENABLE)
		sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif
		 /* if send repeate data timer is on, stop it. */
		zrc_appVars.flags.bf.allowedDeep = 1;
		zrc_restoreAppState();
		//zrc_stopSendRepeateData();
		zrc_stopSendRepeateDataTimer();
		zrc_appVars.flags.bf.repeat = 0;
		release_key_handler(keyCode);

//		if(keyCode == VK_OKKEY){
//			tempActionmappingClear();
//			zrc_appVars.actionMappingIdx = 0;
//			ev_on_timer(actionMappingStart, NULL, 200 * 1000);
//			return;
//		}

	}else if(ZRC_PRE_PAIRING_STATE == zrc_getAppState() ||
			 ZRC_PRE_MODE_SWITCH == zrc_getAppState()){
		toNormal = 1;
	}



	if(toNormal == 1){
		zrc_restoreAppState();
		//zrc_stopWaitForNextKeyTimer();
		zrc_appVars.flags.bf.allowedDeep = 1;
	}
}

void user_keyEventHandle(void){
	static u8 valid_keyCode = 0xff;

#if(MODULE_PM_ENABLE)
	sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif

    sys_active_event_time = clock_time();
    if(kb_event.cnt){
    	keyScan_keyPressedCB(&kb_event);
    	if(kb_event.cnt == 1){
    		valid_keyCode = kb_event.keycode[0];
    	}
    	app_validLevelForPm(0);
	}else{
		keyScan_keyReleasedCB(valid_keyCode);
		valid_keyCode = 0xff;
		app_validLevelForPm(1);
	}
    return;

}


/*********************************************************************
 * @fn      zrc_startBindLed
 *
 * @brief   this function is used to start led blink to indicate the
 *          bind result.
 *
 * @param   None
 *
 * @return  None
 */
void zrc_startBindLed(void *arg)
{
    u32 blinkCnt = (u32)arg;
    zrc_startLed(blinkCnt, ZRC_APP_LED_BLINK_QUICK_TIME, 1);
}

/*********************************************************************
 * @fn      zrc_lightOnProgLed
 *
 * @brief   function for handle LED status
 *
 * @param   None
 *
 * @return  None
 */
void zrc_lightOnProgLed(void)
{
    if ( (ZRC_PRE_PAIRING_STATE == zrc_getAppState() || ZRC_PRE_MODE_SWITCH == zrc_getAppState()) &&
         (clock_time_exceed(zrc_appVars.keyPressedTime, ZRC_KEY_PRESSED_TIME_FOR_PAIRING)) ) {
    	if(ZRC_PRE_PAIRING_STATE == zrc_getAppState()){
			/* start pairing when released */
			zrc_ledOff(1);
			zrc_setAppState(ZRC_PAIRING_STATE);
			zrc_boundInfoSave();
			zrc_doPair(NULL);
    	}else{
#if RF4CE_SWITCHTO_BLE_EN
    		rf4ceSwitchToBle();
    		/*if(app_bootFromRf4ce()){
    			rf4ceSwitchToBle();
    		}else{
    			bleSwitchToRf4ce();
    		}*/
    		zrc_setAppState(ZRC_MODE_SWITCH);
#endif
    	}
    }
}



void gdp_afterBindingCb(u8 pairingRef, u8 status){
	if ( status == SUCCESS && zrc_getAppState() == ZRC_READY_STATE ){
		zrc_stopSendRepeateDataTimer();
		zrc_setAppState(ZRC_PRE_CONFIGURE_STATE);
		ev_on_timer(zrc_userHandlerAfterBound, NULL, 150 * 1000);
	}
}


#endif

