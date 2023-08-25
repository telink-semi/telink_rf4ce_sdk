/********************************************************************************************************
 * @file    callback_zrc.c
 *
 * @brief   This is the source file for callback_zrc.c
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

#include "../../proj/tl_common.h"
#if (RF4CE_ZRC2_ENABLE)


/**********************************************************************
 * INCLUDES
 */
#include "../../net/rf4ce/rf4ce_includes.h"
#include "../../net/rf4ce/profile/gdp/gdp.h"
#include "../../net/rf4ce/profile/zrc2/zrc_api.h"


#include "../../proj/drivers/keyboard.h"
#include "../../proj/drivers/drv_flash.h"

#include "../../proj/os/sys.h"

#if (MODULE_AUDIO_ENABLE)
#include "../../proj/drivers/audio/drv_audio.h"
#include "../../proj/drivers/ir/ir_learn.h"
#include "../common/TL_specificData/tl_specific_data.h"
#include "../common/TL_specificData/tl_specific_data_audio.h"
#include "../common/TL_specificData/tl_specific_data_ota.h"
#endif


#include "../common/TL_specificData/tl_specific_data.h"
//#include "../common/TL_ota/ota.h"
//#include "../common/TL_audio/audio.h"
#include "app_const.h"
#include "app_common.h"
#include "app_led.h"
//#include "app_data.h"
#include "app_config.h"
#include "rc_info.h"

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
void led_startBind(void *arg);
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

const zrc_userCbFunc_t zrc_cmdCallbacks =
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
	if(!rcu_appVars.ledBlinkTimer){
		SYSTEM_RESET();
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

	led_startSet(5, ZRC_APP_LED_BLINK_QUICK_TIME, 1);

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

	led_startSet(3, ZRC_APP_LED_OTA_RUN_TIME, 1);

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
	if ( is_timer_expired(rcu_appVars.ledBlinkTimer) ) {
        rcu_appVars.flags.bf.allowedDeep = 1;
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
	    rcu_appInfo.pairingRef = pairingRef;
	    /* move to verify state */
		app_setState(APP_VERIFY_PAIR_STATE);
		/* init keypressed tiem */
		rcu_appVars.keyPressedTime = clock_time();
	}
}


void zrc_checkValidationCb(u8 pairingRef, u8 status){
	if(app_getState() == APP_VERIFY_PAIR_STATE){
		app_setState(APP_CHECK_VALIDATION_STATE);
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
	rcu_appVars.flags.bf.appInPair = 0;

	if ( status == SUCCESS ) {
	    rcu_appInfo.pairingRef = pairingRef;
	    rcu_appInfo.state = APP_READY_STATE;

	    /* move to rf4ce ready state */
		app_setState(APP_READY_STATE);
		u32 blinkCnt = 3;
        led_startBind((void*)blinkCnt);
	}else if ( status == ZRC_RESTORE ) {
        rcu_appInfo.pairingRef = pairingRef;
        rcu_appInfo.state = APP_READY_STATE;
        app_setState(APP_READY_STATE);
        nv_userLoadFromFlash(APP_INFO_NV_ITEM, sizeof(app_Info_t), (u8*)&rcu_appInfo);
        led_Off(1);
        led_stopAll(1);
		if ( rcu_appVars.flags.bf.ledIdleState == 0 ) {
			rcu_appVars.flags.bf.allowedDeep = 1;
		}
        return;
    }else {
		rcu_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
		rcu_appInfo.state = APP_UNCONNECT_STATE;
		app_setState(APP_UNCONNECT_STATE);
		led_stopAll(1);
		if ( rcu_appVars.flags.bf.ledIdleState == 0 ) {
			rcu_appVars.flags.bf.allowedDeep = 1;
		}
	}

	/* voltage lower than 2.2V, not save flash */
	if ( checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
        zrc_saveStackInfo(NULL);
        save_appInfo(NULL);

        pmFlag_t pmInfo;
        pmInfo.byte = analog_read(reg_mac_channel);
        analog_write(reg_mac_channel, pmInfo.byte);
	}
	pmFlag_t pmInfo;
	pmInfo.byte = analog_read(reg_mac_channel);
	analog_write(reg_mac_channel, pmInfo.byte);
	led_Off(1);

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
    app_setState(APP_UNCONNECT_STATE);

    rcu_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
    rcu_appInfo.state = APP_UNCONNECT_STATE;

	/* voltage lower than 2.2V, not save flash */
	if ( checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
        zrc_saveStackInfo(NULL);
        save_appInfo(NULL);
	}
	else {
		blinkCnt = 4;
	}

    led_startBind((void*)blinkCnt);

    rcu_appVars.flags.bf.allowedDeep = 1;
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
		if(rcu_appVars.flags.bf.repeat && handle == RF4CE_ZRC_STANTARD_KEYCODE_HANDLE){
			/* if send repeate data timer is on, stop it. */
			app_restoreState();
			//zrc_stopSendRepeateData();
			zrc_timerForReleaseKeyStop();
			zrc_rcForceStopSendCmd(rcu_appInfo.pairingRef);
			rcu_appVars.flags.bf.allowedDeep = 1;
			rcu_appVars.flags.bf.inRfRpt = 0;
			app_stopSendRepeateDataTimer();
			rcu_appVars.flags.bf.repeat = 0;
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
	if ( !checkBatteryBeforeSaveFlash(BAT_THRESHOLD_PAIR) ) {
		app_restoreState();
		rcu_appVars.flags.bf.allowedDeep = 1;
		led_startSet(4, APP_LED_BLINK_QUICK_TIME, 1);
		return -1;
	}
	if ( rcu_appVars.flags.bf.appInPair == 0 ) {
	    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	    if ( SUCCESS == profile_startPair(1, DEVICE_ALL, 0x3, 0) ) {
	        app_setState(APP_PAIRING_STATE);
			/* if pair procedure, set the Tx power low */
			rcu_appVars.flags.bf.appInPair = 1;
			rcu_appVars.flags.bf.allowedDeep = 0;

			app_ledFlushInfo_t  ledAction;
			ledAction.onInterval = 200 * 1000;
			ledAction.modeNum = 3;
			ledAction.offIntervalList[0] = 150 * 1000;
			ledAction.offIntervalList[1] = 600 * 1000;
			ledAction.offIntervalList[2] = 600 * 1000;
			Led_startSpecial(0, &ledAction, 1);

            return -1;
	    }else {
            ev_on_timer(zrc_doPair, NULL, 1);
	        return -1;
	    }
	}else{
	    return -1;
	}
}









volatile u8 T_zrc_attrPushRespCbCnt = 0;
void zrc_attrPushRespCb(u8 pairingRef, u8 status){
	T_zrc_attrPushRespCbCnt++;
}

s32 zrc_userHandlerAfterBound(void *arg){
	if(app_getState() == APP_PRE_CONFIGURE_STATE){
		rcu_appVars.flags.bf.allowedDeep = 1;
		app_setState(APP_READY_STATE);
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
    profile_unpairReq(rcu_appInfo.pairingRef);
    rcu_appVars.flags.bf.allowedDeep = 0;

    return -1;
}


/*********************************************************************
 * @fn      zrcApp_initPib
 *
 * @brief   Initialize the necessary Network PIB
 *
 * @param   none
 *
 * @return  none
 */
void zrcApp_initPib(void)
{
	u8 value;
#if SECURITY_ENABLE
	value = 0x0c;
#else
	value = 0x08;
#endif
	nwk_nlmeSetReq(NWK_NODE_CAPABILITIES, 0, &value);

	/* Default discovery reception interval, 1 second. (0xf424 * 16)us */
	u32 respWaitTime = 0xF424;
	nwk_nlmeSetReq(NWK_DISCOVERY_REPETITION_INTERVAL, 0, (u8*)&respWaitTime);

	/* Default response waiting timer, 1 second. (0xf424 * 16)us */
	respWaitTime = 0xF424;
	nwk_nlmeSetReq(NWK_RESPONSE_WAIT_TIME, 0, (u8*)&respWaitTime);

	/* Default discovery repetitions */
	value = 0x5;
	nwk_nlmeSetReq(NWK_MAX_DISCOVERY_REPETITIONS, 0, &value);

	/* disable rx */
	value = 0;
	mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);

	value = 54;
	mac_mlmeSetReq(MAC_ACK_WAIT_DURATION, &value);

	u32 dutyCycle = RF4CE_NWK_MAX_DUTY_CYCLE;  //RF4CE_NWK_MAX_DUTY_CYCLE; //960 * 3;
	nwk_nlmeSetReq(NWK_DUTY_CYCLE, 0, (u8*)&dutyCycle);

	/* vendor identifier */
	u16 vendId = APP_VENDOR_ID_LIST;
	nwk_nlmeSetReq(NWK_VENDOR_IDENTIFIER, 2, (u8 *)&vendId);

	u8 *userStr = (u8 *)ZRC2_APP_USER_STRING;
	nwk_nlmeSetReq(NWK_USER_STRING, 0, userStr);
}




void gdp_afterBindingCb(u8 pairingRef, u8 status){
	if ( status == SUCCESS && app_getState() == APP_READY_STATE ){
		zrc_stopSendRepeateDataTimer();
		app_setState(APP_PRE_CONFIGURE_STATE);
		ev_on_timer(zrc_userHandlerAfterBound, NULL, 150 * 1000);
	}
}


zrc_sts_t zrc_PowerStatusPush(u8 status)
{
	zrc_sts_t ret = ZRC_SUCC;
	attr_t attr;
	attr.id = GDP_ATTR_aplPowerStatus;           /*!< Attribute ID */
	attr.len = 1;                                /*!< Attribute length */
	attr.val = &status;
	ret = gdp_sendPushAttrCmd(rcu_appInfo.pairingRef, &attr, 1);
	return ret;
}


/*********************************************************************
 * @fn      stack_init
 *
 * @brief   This function initialize the RF4CE stack and related profile. If ZRC/ZID/ZRC profile is
 *          enabled in this application, related profile should be registered here.
 *
 * @param   None
 *
 * @return  None
 */
void stack_init(void)
{
    profile_init();
	zrcApp_initPib();

    /* Register Profile Call back function */
    profile_registerUserCb((profile_cbFunc_t *)&zrc_profileCmdCallbacks);

	zrc_registerUserCb((zrc_userCbFunc_t *)&zrc_cmdCallbacks);
}

/*********************************************************************
 * @fn      zrc_saveStackInfo
 *
 * @brief   save stack pib to flash.
 *
 * @param   None
 *
 * @return  None
 */
void zrc_saveStackInfo(void *arg)
{
	u8 r = irq_disable();
#if (MODULE_FLASH_ENABLE)
	if ( SUCCESS != mac_savePibToFlash() ) {
		nv_resetModule(DS_MAC_PHY_MODULE);
		mac_savePibToFlash();
	}

	if ( SUCCESS != nwk_savePibToFlash() || SUCCESS != nwk_savePairTblToFlash() ) {
		nv_resetModule(DS_NWK_MODULE);
		nwk_savePairTblToFlash();
		nwk_savePibToFlash();
	}
	if ( (SUCCESS != zrc_saveTgtRcTblToFlash())
		|| (SUCCESS != zrc_saveRibToFlash())
		|| (SUCCESS != zrc_saveAttrToFlash()) ){
		nv_resetModule(DS_PROFILE_MODULE);
		zrc_saveTgtRcTblToFlash();
		zrc_saveRibToFlash();
		zrc_saveAttrToFlash();
	}
#endif
	irq_restore(r);
}


/*****************************KEY API*********************************/
static zrc_actionRecord_t zrc_rec = {{{0,0,0}},0,0,0,{0}};
//validation
u8 profile_sendValidationKey(u8 pairingRef, u8 txOptions,u8 keycode)
{
    zrc_rec.actionControl.bf.actionType = ZRC2_ACTION_TYPE_ATOMIC;
	zrc_rec.actionCode = keycode;
    return zrc_rcSendValidCodeReq(pairingRef, txOptions, &zrc_rec);
}



//send key
u8  profile_startSendRFKeyCmd(u8 pairingRef, u8 txOptions, u8 keycode)
{

		zrc_rec.actionCode = keycode;
//		if(zrc_rec.actionCode == VK_TV)//VK_TV send ZRC2_ACTION_TYPE_ATOMIC KEY
//		zrc_rec.actionControl.bf.actionType = ZRC2_ACTION_TYPE_ATOMIC;
//		else
		zrc_rec.actionControl.bf.actionType = ZRC2_ACTION_TYPE_START;

	if(SUCCESS != zrc_rcSendActionCmdReq(pairingRef, txOptions, &zrc_rec)){
		rcu_appVars.flags.bf.repeat = 0;
		return FAILURE;
    }
	else
	{
		if(zrc_rec.actionControl.bf.actionType == ZRC2_ACTION_TYPE_START)
		rcu_appVars.flags.bf.repeat = 1;
		return SUCCESS;
	}
}


void profile_stopSendRFKeyCmd(u8 pairingRef)
{
	zrc_rcStopSendActionCmd(pairingRef, &zrc_rec);
}

#endif

