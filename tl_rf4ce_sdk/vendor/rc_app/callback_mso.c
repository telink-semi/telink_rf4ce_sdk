/********************************************************************************************************
 * @file    callback_mso.c
 *
 * @brief   This is the source file for callback_mso.c
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
#if  (RF4CE_MSO_ENABLE)
/**********************************************************************
 * INCLUDES
 */
#include "../../net/rf4ce/rf4ce_includes.h"
#include "../../net/rf4ce/profile/mso/mso_api.h"


#include "../../proj/drivers/keyboard.h"
#include "../../proj/drivers/ir/ir_learn.h"
#include "../../proj/drivers/ir/ir.h"

#include "../common/TL_specificData/tl_specific_data.h"
//#include "../common/TL_ota/ota.h"
//#include "../common/TL_audio/audio.h"
#include "app_const.h"
#include "app_common.h"
#include "callback_mso.h"
#include "app_led.h"
#include "app_data.h"
#include "app_config.h"
#include "rc_info.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */
static const u16 CABLE_DEFAULT_BRAND[] = {
	1,
	2,
	3,
};

static const u16 TV_DEFAULT_BRAND = 258;

const appVendInfo_t app_vendor_info_list[APP_VID_SUPPPORT_MAX] = {
	/* Telink */
	{APP_VENDOR_ID_LIST, {'T', 'L', 0, 0, 0, 0, 0},	{'S','R','-','0','0','1','-','U',0,0,0,0,0,0,0},},
};

/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */
void mso_startCnfCb(u8 status);
void mso_pairCnfCb(u8 pairingRef, u8 status, u8 profileSize, u8 *profileList);
void mso_cmdSendCb(u8 pairingRef, u8 status, u8 keyCode);
void mso_unPairCnfCb(u8 pairingRef, u8 status);
int mso_doPair(void *arg);
s32 mso_doUnpair(void *arg);

//void mso_ckValiResp(u8 pairingRef, u8 status);;
void mso_bindCnf(u8 pairingRef, u8 status, u8 lqi);
void mso_setAttrResp(u8 pairingRef, u8 attrib, u8 index, u8 status);
void mso_startBindLed(void *arg);

void mso_startSendVerisonTimer(u8 index);

void mso_rfStatisticsSend(u8 lqi);
void mso_saveStackInfo(void *arg);
/**********************************************************************
 * GLOBAL VARIABLES
 */
ev_time_event_t *mso_sendRibTimer;
ev_time_event_t *mso_showNumTimer;
ev_time_event_t *mso_showRfStaTimer;

u8 uploadBattery;
/**********************************************************************
 * LOCAL VARIABLES
 */
const profile_cbFunc_t mso_profileCmdCallbacks =
{
    mso_startCnfCb,                     // start stack confirm command
    mso_pairCnfCb,
	mso_unPairCnfCb,
	NULL,
};


const mso_userCbFunc_t mso_cmdCallbacks =
{
	mso_ckValidResp,             //check validation resp
	NULL,                        //check validation ind
	mso_setAttrResp,             //set attr resp
	NULL,                        //set attr ind
	NULL,                        //get attr resp
	mso_bindCnf,                 //bind confirm
	NULL,                        //cmd recv
	mso_cmdSendCb,               //cmd send cb
	NULL,                        //validation code recv
	mso_appDataIndCb,			 //!< Function pointer to the received vendor specific data
};



int mso_waitForRespTimerCb(void *arg)
{
	mso_sendRibTimer = NULL;
	rcu_appVars.flags.bf.sendingRib = 0;
	rcu_appVars.flags.bf.allowedDeep = 1;
	app_restoreState();
	u8 value = 0;
	mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	return -1;
}

void mso_setAttrRespDone(void)
{
	/* allow suspend */
	rcu_appVars.flags.bf.sendingRib = 1;
	rcu_appVars.flags.bf.appInPair = 0;
	u8 value = 1;
	mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	/* send battery status */
	if ( mso_sendRibTimer ) {
		ev_unon_timer(&mso_sendRibTimer);
	}
	    mso_sendRibTimer = ev_on_timer(mso_waitForRespTimerCb, NULL, 120 * 1000);
}

/*******************Profile Callback Functions Implementation******************************/

/*********************************************************************
 * @fn      mso_setAttrResp
 *
 * @brief   Callback function for receiving set attribute response from target
 *
 * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
 *
 * @param[in]   attrid - attribute id
 *
 * @param[in]   index -  attribute index
 *
 * @param[in]   status - status
 *
 * @return  None
 */
void mso_setAttrResp(u8 pairingRef, u8 attrid, u8 index, u8 status)
{
	mso_setAttrRespDone();
}


/*********************************************************************
 * @fn      mso_ckValidResp
 *
 * @brief   Callback function for receiving check validation response from tagert
 *
 * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
 *
 * @param[in]   status - The status of the check validation response
 *
 * @return  None
 */
void mso_ckValidResp(u8 pairingRef, u8 status)
{
	if ( status == MSO_SUCC ) {

	} else if ( status != MSO_PENDING ) {

	} else {

	}
}


/*********************************************************************
 * @fn      mso_sendBatteryStatus
 *
 * @brief   send battery information to target timer callback function
 *
 * @param[in]   index - index of the attribute
 *
 * @return
 */
int mso_sendBatteryStatus(void *arg)
{
	u8 battery[11];
	u32 curBat = (u32) arg;

	memset(battery, 0 , 11);
	if ( curBat > BAT_THRESHOLD_PAIR ) {
		battery[0] = 0x04;
		battery[1] = battery[10] = 0x8c;
	}else if ( curBat >= BAT_THRESHOLD_FLASH ) {
		battery[0] = 0x00;
		battery[1] = battery[10] = 0x93;
	}else{
		battery[0] = 0x00;
		battery[1] = battery[10] = 0xbf;
	}

	if ( MSO_SUCC == mso_rcSendSetAttrCmd(rcu_appInfo.pairingRef, 0x03, 0, 11, battery) ) {
		rcu_appVars.flags.bf.sendingRib = 1;
		mso_showNumTimer = NULL;
		rcu_appVars.battStaUpload = 0;
		return -1;
	}
	else {
		return 10*100;
	}
}

/*********************************************************************
 * @fn      mso_startSendBatteryTimer
 *
 * @brief   start send battery information to target timer
 *
 * @param[in]   index - index of the attribute
 *
 * @return
 */
void mso_startSendBatteryTimer(u32 powerLow)
{
	if(rcu_appInfo.pairingRef == RF4CE_INVALID_PAIR_REF)
	{
		return;
	}
	/* send battery status */
    if ( mso_showNumTimer )
    {
        ev_unon_timer(&mso_showNumTimer);
    }
	mso_showNumTimer = ev_on_timer(mso_sendBatteryStatus, (void*)powerLow, 100 * 1000);
}


/*********************************************************************
 * @fn      mso_bindCnf
 *
 * @brief   Callback function for indication the bind result
 *
 * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
 *
 * @param[in]   status - the stauts of bind result.
 *
 *
 * @return  None
 */
u8 T_paring_doing = 0;
void mso_bindCnf(u8 pairingRef, u8 status, u8 lqi)
{
	u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
    rcu_appVars.flags.bf.appInPair = 0;

	if ( status == MSO_SUCC ) {
	    rcu_appInfo.pairingRef = pairingRef;
	    rcu_appInfo.state = APP_READY_STATE;

	    /* move to rf4ce ready state */
		app_setState(APP_READY_STATE);
		u32 blinkCnt = 3;
		led_startBind((void*)blinkCnt);
	}else if ( status == MSO_HAS_BOND ) {
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
        mso_saveStackInfo(NULL);
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
 * @fn      mso_startCnfCb
 *
 * @brief   Callback function for profile_startNwk.
 *
 * @param   status - Network start status, it is set by stack
 *
 * @return  None
 */
void mso_startCnfCb(u8 status)
{
    u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	if(is_timer_expired(rcu_appVars.ledBlinkTimer)) {
        rcu_appVars.flags.bf.allowedDeep = 1;
    }
}

/*********************************************************************
 * @fn      mso_pairCnfCb
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

void mso_pairCnfCb(u8 pairingRef, u8 status, u8 profileSize, u8 *profileList)
{
	u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	if ( status == SUCCESS ) {
		rcu_appInfo.pairingRef = pairingRef;
	    /* move to verify state */
		app_setState(APP_CHECK_VALIDATION_STATE);
		/* init keypressed tiem */
		rcu_appVars.keyPressedTime = clock_time();
	}

	/* after pair procedure, set the Tx power high */
//	rf_set_power_level_index(1);
}


/*********************************************************************
 * @fn      mso_unPairCnfCb
 *
 * @brief   Callback function for unpair confirmation.
 *
 * @param   pairingRef  - the index of the pair entry
 *
 * @param   status  - the status of the command
 *
 * @return  None
 */
void mso_unPairCnfCb(u8 pairingRef, u8 status)
{
    u32 blinkCnt = 2;
    app_setState(APP_UNCONNECT_STATE);
    rcu_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
    rcu_appInfo.state = APP_UNCONNECT_STATE;

	/* voltage lower than 2.2V, not save flash */
	if (checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
	    mso_saveStackInfo(NULL);
	    save_appInfo(NULL);
	}
	else {
		blinkCnt = 4;
	}

	led_startBind((void *)blinkCnt);
	rcu_appVars.flags.bf.allowedDeep = 1;
}



void mso_appDataIndCb(u8 pairingRef, u8 *pd, u8 len){
	tl_appDataIndicate( pd, len);
}


/*********************************************************************
 * @fn      mso_cmdSendCb
 *
 * @brief   Callback function for command sent confirmation.
 *
 * @param   status     - Indicating the command sent result
 *
 * @param   pairingRef - The pairingRef command sent to
 *
 * @return  None
 */
void mso_cmdSendCb(u8 pairingRef, u8 status, u8 keyCode)
{
	/* frame counter exceed the maximum one, reest the frame counter and unpair*/
	if ( status == RF4CE_NWK_FRAME_COUNTER_EXPIRED && keyCode == ZRC_USER_CTRL_PRED ) {
		u32 curFrameCnt = NWK_FRAME_COUNTER_DEFAULT_VALUE;
		nwk_nlmeSetReq(NWK_FRAME_COUNTER, 0, (u8*)&curFrameCnt);
        mso_doUnpair(NULL);
	}
}

/*
 * now support three vendor
 * */
void mso_setVendorInfo(u8 idx){
	if (idx >= APP_VID_SUPPPORT_MAX){
		return;
	}
	u8 vendStr[7];
	u8 userStr[15];
	u16 vendId = app_vendor_info_list[idx].vendId;
	memcpy(vendStr, app_vendor_info_list[idx].vendString, 7);
	memcpy(userStr, app_vendor_info_list[idx].userString, 15);

	nwk_nlmeSetReq(NWK_VENDOR_IDENTIFIER, 0, (u8*)&vendId);
	nwk_nlmeSetReq(NWK_VENDOR_STRING, 0, (u8*)vendStr);
	nwk_nlmeSetReq(NWK_USER_STRING, 0, (u8*)userStr);
}

/*********************************************************************
 * @fn      profile_doPair
 *
 * @brief   Start pair procedure.
 *
 * @param   None
 *
 * @return  None
 */
int mso_doPair(void *arg)
{
	u8 value = 1;
	T_paring_doing = 1;

	/* voltage lower than 2.2V, can't initail the pairing  */
    if (!checkBatteryBeforeSaveFlash(BAT_THRESHOLD_PAIR) ){
		app_restoreState();
		rcu_appVars.flags.bf.allowedDeep = 1;
		led_startSet(4, APP_LED_BLINK_QUICK_TIME, 1);
		return -1;
	}
	if ( rcu_appVars.flags.bf.appInPair == 0 ) {
	    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
	    if ( SUCCESS == profile_startPair(1, 0x09, 0x3, 0) ) {
			/* if in send rib, stop it. */
			if ( mso_sendRibTimer ) {
				ev_unon_timer(&mso_sendRibTimer);
			}

			app_ledFlushInfo_t  ledAction;
			ledAction.onInterval = 200 * 1000;
			ledAction.modeNum = 3;
			ledAction.offIntervalList[0] = 150 * 1000;
			ledAction.offIntervalList[1] = 600 * 1000;
			ledAction.offIntervalList[2] = 600 * 1000;
			Led_startSpecial(0, &ledAction, 1);

	        app_setState(APP_PAIRING_STATE);
			/* if pair procedure, set the Tx power low */
			//RF_TX_PA_POWER_LEVEL(0);
			rcu_appVars.flags.bf.appInPair = 1;
			rcu_appVars.flags.bf.allowedDeep = 0;
//			rf_set_power_level_index(2);
	    }
		else {
            ev_on_timer(mso_doPair, NULL, 1);
	        return -1;
	    }
	}
    return -1;
}


/*********************************************************************
 * @fn      mso_doUnpair
 *
 * @brief   Start unpair procedure.
 *
 * @param   None
 *
 * @return  None
 */
s32 mso_doUnpair(void *arg)
{
    profile_unpairReq(rcu_appInfo.pairingRef);
    rcu_appVars.flags.bf.allowedDeep = 0;
    return -1;
}

/*********************************************************************
 * @fn      mso_waitForNextKeyTimerCb
 *
 * @brief   timer callback function for wait for the next key during the program state.
 *
 * @param   None
 *
 * @return  None
 */
int mso_waitForNextKeyTimerCb(void *arg)
{
    /* program mode end, turn off the LED, and move out of the configure state */
    led_Off(1);
	app_restoreState();
	rcu_appVars.flags.bf.allowedDeep = 1;
    rcu_appVars.configTimer = NULL;
    return -1;
}

/*********************************************************************
 * @fn      mso_startWaitForNextKeyTimer
 *
 * @brief   Start a timer to wait for the next key during the program state.
 *
 * @param   None
 *
 * @return  None
 */
void mso_startWaitForNextKeyTimer(void)
{
    if ( rcu_appVars.configTimer ) {
        ev_unon_timer(&rcu_appVars.configTimer);
    }
	rcu_appVars.flags.bf.allowedDeep = 0;
	rcu_appVars.configTimer = ev_on_timer(mso_waitForNextKeyTimerCb, NULL, WAIT_FOR_NEXT_KEY_TIME);
}

/*********************************************************************
 * @fn      mso_stopWaitForNextKeyTimer
 *
 * @brief   stop a timer to wait for the next key during the program state.
 *			It's called when next key is pressed
 *
 * @param   None
 *
 * @return  None
 */
void mso_stopWaitForNextKeyTimer(void)
{
    if ( rcu_appVars.configTimer ) {
        ev_unon_timer(&rcu_appVars.configTimer);
    }
}

/*********************************************************************
 * @fn      mso_autoSendTvPower
 *
 * @brief   auto send tv power function, it's used in tv auto search
 *
 * @param   None
 *
 * @return  None
 */
void mso_sendTvPower(void *arg)
{

}

/*********************************************************************
 * @fn      mso_autoSearchPrevManualTvTimerCb
 *
 * @brief   manual send tv power function
 *
 * @param   None
 *
 * @return  None
 */
int mso_autoSearchPrevManualTvTimerCb(void *arg)
{

	return -1;
}


/*********************************************************************
 * @fn      mso_stopAutoSearchTvTimer
 *
 * @brief   stop auto send TV power timer
 *
 * @param   None
 *
 * @return  None
 */
void mso_stopAutoSearchTimer(void)
{
//    if ( rcu_appVars.autoScanTimer ) {
//        ev_unon_timer(&rcu_appVars.autoScanTimer);
//    }
}




/*********************************************************************
 * @fn      mso_PowerStatusPush
 *
 * @brief   this function is used to show battery level
 *
 * @param   level battery level
 *
 * @return  none
 */
void mso_PowerStatusPush(u8 level)
{
	uploadBattery = rcu_appVars.battSta;
	if ( rcu_appInfo.pairingRef != RF4CE_INVALID_PAIR_REF ) {
		rcu_appVars.flags.bf.allowedDeep = 0;
		rcu_appVars.flags.bf.appInPair = 0;
		u32 pwr = 0;
		if ( level > 3 ) {
			pwr = 1;
			mso_startSendBatteryTimer(pwr);
		} else {
			pwr = 0;
			mso_startSendBatteryTimer(pwr);
		}
	}
}



/*********************************************************************
 * @fn      mso_sendCarrierOnNextCh
 *
 * @brief   this function is used to send carrier on next channel
 *
 * @param   None
 *
 * @return  None
 */
void mso_sendCarrierOnNextCh(void)
{

}

/*********************************************************************
 * @fn      mso_startSendCarrier
 *
 * @brief   this function is used to send carrier
 *
 * @param   None
 *
 * @return  None
 */
void mso_startSendCarrier(void *arg)
{

}





int mso_delayAllowDeep(void *arg)
{
	rcu_appVars.flags.bf.allowedDeep = 1;
	mso_showNumTimer = NULL;
	return -1;
}

//int mso_rfStaInfo(void *arg){
//	u8 rfSta[16] = {0};
//	rfSta[0] = (u8)arg;
//	memcpy(&rfSta[1], macPib.extendedAddress, 8);
//
//	if ( MSO_SUCC == mso_rcSendSetAttrCmd(mso_appInfo.pairingRef, 0x01, 0, 16, rfSta) ) {
//		rcu_appVars.flags.bf.sendingRib = 1;
//		return -1;
//	}else {
//		return 10*100;
//	}
//}

volatile int T_lqi = 0xff;
void mso_rfStatisticsSend(u8 lqi)
{
	T_lqi = lqi;
//	mso_showRfStaTimer = ev_on_timer(mso_rfStaInfo, (void*)lqi, 200 * 1000);
}




/*********************************************************************
 * @fn      msoApp_initPib
 *
 * @brief   Initialize the necessary Network PIB
 *
 * @param   none
 *
 * @return  none
 */
//void msoApp_initPib(void)
//{
//	u8 value;
//#if SECURITY_ENABLE
//    value = 0x0c;
//#else
//    value = 0x08;
//#endif
//    nwk_nlmeSetReq(NWK_NODE_CAPABILITIES, 0, &value);
//
//
//	mso_rcAttrTbl_t *attrPtr = mso_rcGetAttrInfo();
//	attrPtr->aplKeyRepeatInterval = 110;
//}

/*********************************************************************
 * @fn      init_NwkPib
 *
 * @brief   Initialize the necessary Network PIB
 *
 * @param   none
 *
 * @return  none
 */
void init_msoPib(void)
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

	u32 dutyCycle = RF4CE_NWK_MAX_DUTY_CYCLE; //960 * 3;
	nwk_nlmeSetReq(NWK_DUTY_CYCLE, 0, (u8*)&dutyCycle);

	/* vendor identifier */
	u16 vendId = APP_VENDOR_ID_LIST;
	nwk_nlmeSetReq(NWK_VENDOR_IDENTIFIER, 2, (u8 *)&vendId);

	u8 *vendorStr = (u8 *)MSO_APP_VENDOR_STRING;
	nwk_nlmeSetReq(NWK_VENDOR_STRING, 0, vendorStr);

	u8 *userStr = (u8 *)MSO_APP_USER_STRING;
	nwk_nlmeSetReq(NWK_USER_STRING, 0, userStr);
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

    init_msoPib();

    /* disable rx */
    u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);

    /* Register Profile Call back function */
    profile_registerUserCb((profile_cbFunc_t *)&mso_profileCmdCallbacks);
	mso_registerUserCb((mso_userCbFunc_t *)&mso_cmdCallbacks);
}


/*********************************************************************
 * @fn      mso_saveStackInfo
 *
 * @brief   save stack pib to flash.
 *
 * @param   None
 *
 * @return  None
 */
void mso_saveStackInfo(void *arg)
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

	if ( SUCCESS != mso_saveTgtRcTblToFlash() ) {
		nv_resetModule(DS_PROFILE_MODULE);
	    mso_saveTgtRcTblToFlash();
	}
#endif
	irq_restore(r);
}





/*********************************************************************
 * @fn      stack_saveProfileInfo
 *
 * @brief   app call this funstion to save info
 *
 * @param   None
 *
 * @return  None
 */
void stack_saveProfileInfo(void *arg)
{
	mso_saveStackInfo((void *)arg);
}

/*****************************KEY API*********************************/
static u8 mso_keyCode = 0;
//validation
u8 profile_sendValidationKey(u8 pairingRef, u8 txOptions,u8 keycode)
{
	if(rcu_appVars.flags.bf.sendingRib == 1)
			return FAILURE;
	mso_keyCode = keycode;
    return mso_rcSendValidCodeReq(pairingRef, txOptions, mso_keyCode);
}



//send key
u8  profile_startSendRFKeyCmd(u8 pairingRef, u8 txOptions, u8 keycode)
{
	bool repeat = FALSE;
	mso_keyCode = keycode;
	if (mso_keyCode == VK_VOLUME_DOWN || mso_keyCode == VK_VOLUME_UP) {//volume key can send repeat key
			repeat = TRUE;
	}

		rcu_appVars.flags.bf.repeat = 0;
	if(SUCCESS != mso_rcSendCmdReq(pairingRef, txOptions, ZRC_USER_CTRL_PRED, mso_keyCode, 0, NULL, repeat))
	{
		return FAILURE;
	}
	else
	{
		if(repeat==TRUE)
		rcu_appVars.flags.bf.repeat = 1;
		return SUCCESS;
	}

}


void profile_stopSendRFKeyCmd(u8 pairingRef)
{
	mso_rcStopSendRepeatCmd();
}


#endif

