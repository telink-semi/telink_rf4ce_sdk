/********************************************************************************************************
 * @file     zrcApp_data.c
 *
 * @brief    ZRC2 RCU data handler includes IR/RF code, audio data
 *
 * @author
 * @date     July. 2, 2019
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

#if __PROJECT_ZRC_2_RC__
/**********************************************************************
 * INCLUDES
 */
#include "../../net/rf4ce/rf4ce_includes.h"

#if (RF4CE_ZRC2_ENABLE)
#include "../../net/rf4ce/profile/zrc2/zrc_api.h"
#include "../../net/rf4ce/profile/zrc2/zrc.h"
#include "../../net/rf4ce/profile/gdp/gdp.h"
#else
#include "../../net/rf4ce/profile/zrc/zrc_api.h"
#endif
#include "../../net/rf4ce/profile/profile_attr.h"

#include "../../proj/drivers/ir/ir_database.h"
#include "../../proj/drivers/ir/ir.h"
#include "../../proj/drivers/keyboard.h"

#include "zrc_const.h"
#include "zrcApp.h"


#include "zrcApp_data.h"
#include "zrcApp_cb.h"
#include "zrcApp_led.h"
#include "qsIR_Tx.h"
#include "../common/TL_specificData/tl_specific_data_audio.h"
#include "../../proj/os/sys.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */

/**********************************************************************
 * TYPEDEFS
 */
extern cmdQ_t cmdQ;

/**********************************************************************
 * GLOBAL VARIABLES
 */


/**********************************************************************
 * LOCAL VARIABLES
 */


extern zrc_app_t zrc_appVars;

extern zrc_appInfo_t zrc_appInfo;

ev_time_event_t *zrc_handleQTimer;


u8 rf_keyMappingTbl[KB_ARRAY_COL][KB_ARRAY_ROW] = KB_MAP_NORMAL;
u8 ir_keyMappingTbl[KB_ARRAY_COL][KB_ARRAY_ROW] = KB_MAP_FOR_IR;


int zrc_sendRepeateDataTimerCb(void *arg);
void zrc_startSendRepeateDataTimer(u32 rptTime);
u8 zrc_sendZrcCmd(u8 zrcCode, u8 keyCode);
void zrc_sendIrCmd(void *arg);
void zrc_appSendData(u8 zrcCode, u8 keyCode, u8 fRepeat);
int zrc_sendDataTimerCb(void *arg);

u8 zrcApp_cmdQPush(cmdQ_t *q, u8 item);
u8 zrcApp_cmdQPop(cmdQ_t *q);
void zrc_sendDtvIrCmd(u8 keyCode, u8 repeat);
/*********************************************************************
 * @fn      zrc_sendRepeateDataTimerCb
 *
 * @brief   timer cb to auto send repeat zrc cmd.
 *
 * @param   None
 *
 * @return  None
 */
static zrc_actionRecord_t zrc_rec = {{{0,0,0}},0,0,0,{0}};
int zrc_sendRepeateDataTimerCb(void *arg)
{
	if(zrc_getAppState() == ZRC_SENDING_IR_STATE){
		if(zrc_appVars.flags.bf.repeat == 0){
			zrc_restoreAppState();
			return -1;
		}
		zrc_sendDtvIrCmd(zrc_appVars.keyCode, 1);
	}else{
		if(zrc_appVars.keyCode == 0xff){
			zrc_rcStopSendActionCmd(zrc_appInfo.pairingRef, &zrc_rec);
			zrc_appVars.flags.bf.inRfRpt = 0;
			zrc_appVars.flags.bf.repeat = 0;
			zrc_appVars.flags.bf.allowedDeep = 1;
			zrc_restoreAppState();

#if(MODULE_PM_ENABLE)
			sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif
			return -1;
		}
	}

    zrc_appVars.flags.bf.allowedDeep = 0;
	if ( zrc_appVars.coutchCnt-- == 0 ) {
		/* if send repeate data timer is on, stop it. */
		zrc_stopSendRepeateDataTimer();
		zrc_appVars.flags.bf.allowedDeep = 1;
		zrc_restoreAppState();
		zrc_appVars.flags.bf.repeat = 0;
        zrc_appVars.configTimer = NULL;
        app_validLevelForPm(0);
		if(zrcIrGetState())//stop qs
		{
			zrcStop();
		}
		return -1;
	}
	else {
		return 0;
	}
}


void zrc_startSendRepeateDataTimer(u32 rptTime)
{
    if ( zrc_appVars.configTimer ) {
        ev_unon_timer(&zrc_appVars.configTimer);
    }
    zrc_appVars.configTimer = ev_on_timer(zrc_sendRepeateDataTimerCb, NULL, rptTime);
    zrc_appVars.coutchCnt = 30*1000*1000 / rptTime;
}

void zrc_stopSendRepeateDataTimer(void)
{
	if ( zrc_appVars.configTimer ) {
		ev_unon_timer(&zrc_appVars.configTimer);
	}

	if ( zrc_appVars.flags.bf.inRfRpt ) {
		zrc_rcStopSendActionCmd(zrc_appInfo.pairingRef, &zrc_rec);
		zrc_appVars.flags.bf.inRfRpt = 0;
	}
	zrc_appVars.flags.bf.repeat = 0;

#if(MODULE_PM_ENABLE)
	sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif
}

volatile int aaa_zrc_app_cmd[2] = {0};
int zrc_appSendQCmd(void *arg)
{
	if ( zrcApp_cmdQGetSize(&cmdQ) > 0 ) {
		aaa_zrc_app_cmd[0]++;
		if ( profile_getTxStatus() ) {
			return 0;
		}
		u8 txOption;
		u8 keyCode = zrcApp_cmdQPop(&cmdQ);
		if ( nwk_isSecEnable() ) {
			txOption = NWK_TXOPTION_ACK | NWK_TXOPTION_SECURITY;
		}
		else {
			txOption = NWK_TXOPTION_ACK;
#if (RF4CE_ZRC2_ENABLE)
			zrc_rec.actionControl.bf.actionType = ZRC_USER_CTRL_RELS;//PRED;
			zrc_rec.actionCode = keyCode;
			zrc_rcSendActionCmdReq(zrc_appInfo.pairingRef, txOption, &zrc_rec);
#endif
			//zrc_sendCmdRequest(zrc_appInfo.pairingRef, ID_VENDOR, txOption, ZRC_USER_CTRL_PRED, keyCode, 0, NULL);
		}
	}

	if (zrcApp_cmdQGetSize(&cmdQ) > 0) {
		return 0;
	}
	else {
		zrc_handleQTimer = NULL;
		return -1;
	}
}


/*********************************************************************
 * @fn      zrc_sendZrcCmd
 *
 * @brief   Send zrc command to specified light
 *
 * @param   pairingRef - Specify which light to send command to
 * @param   cmd        - Sample light command code
 *
 * @return  None
 */
u8 zrc_sendZrcCmd(u8 zrcCode, u8 keyCode)
{
    /* check the current mode, if RF4CE mode  */
    u8 txOption;
#if (SECURITY_ENABLE)
    txOption = NWK_TXOPTION_ACK | NWK_TXOPTION_SECURITY;
#else
    txOption = NWK_TXOPTION_ACK;
#endif
    /* check the link status */

    if ( zrc_getAppState() == ZRC_CHECK_VALIDATION_STATE) {
	    zrc_rec.actionControl.bf.actionType = zrcCode;
    	zrc_rec.actionCode = keyCode;
        return zrc_rcSendValidCodeReq(zrc_appInfo.pairingRef, txOption, &zrc_rec);
    }

    else if ( zrc_getAppState() != ZRC_READY_STATE || zrc_appInfo.pairingRef == RF4CE_INVALID_PAIR_REF ) {
        return FAILURE;
    }

	zrc_rec.actionCode = keyCode;
	zrc_rec.actionControl.bf.actionType = zrcCode;
	/* if repeat key */
	if(ZRC2_ACTION_TYPE_ATOMIC == zrcCode){
		zrc_rec.actionControl.bf.actionType = ZRC2_ACTION_TYPE_START;
#if(MODULE_PM_ENABLE)
		sys_suspendTimerSet(55);
#endif
	}

	aaa_zrc_app_cmd[1]++;
	if(SUCCESS != zrc_rcSendActionCmdReq(zrc_appInfo.pairingRef, txOption, &zrc_rec)){
		return FAILURE;
    }else {
    	if(ZRC2_ACTION_TYPE_START == zrc_rec.actionControl.bf.actionType){
    		/* start the timer for repeat */
    		zrc_appVars.flags.bf.allowedDeep = 0;
    		zrc_appVars.flags.bf.inRfRpt = 1;
    		zrc_appVars.flags.bf.repeat = 1;
    		zrc_startSendRepeateDataTimer(50000);
    	}
	}

	return SUCCESS;
}

volatile u8 T_zrc_sendDtvIrCmd = 0;
void zrc_sendDtvIrCmd(u8 keyCode, u8 repeat)
{
	u16 real_keyCode = 0;
	ir_bit16db_t* pIrDatabase = irdb16_getEntry(41);

	if(keyCode == 0xff){
		if(pIrDatabase && pIrDatabase->fn){
			pIrDatabase->fn(pIrDatabase->addr, keyCode, 0);
		}
		return;
	}

	for(int i= 0 ; i < KB_ARRAY_COL; i++){
		for(int j= 0 ; j < KB_ARRAY_ROW; j++){
			if(rf_keyMappingTbl[i][j] == keyCode){
				real_keyCode = ir_keyMappingTbl[i][j];// ((ir_keyMappingTbl[i][j])<<8);
				if(pIrDatabase && pIrDatabase->fn){
					T_zrc_sendDtvIrCmd++;
					pIrDatabase->fn(pIrDatabase->addr, real_keyCode, repeat);
				}
				return;
			}
		}
	}
}


u8 zrc_sendRFData(u8 profileId, u8 keyCode)
{
	if (zrc_appVars.flags.bf.repeat) {
		return FAILURE;
	}
	zrc_appVars.flags.bf.inRfRpt = 0;

    /* if key code need to send, disable airmouse send data */
	zrc_appVars.keyCode = keyCode;

	return zrc_sendZrcCmd(ZRC2_ACTION_TYPE_ATOMIC, keyCode);
}


u8 zrc_sendIRData(u8 profileId, u8 keyCode, u8 single_key)
{
	if ( zrc_getAppState() == ZRC_VERIFY_PAIR_STATE || zrc_appVars.flags.bf.repeat||app_isSendingIR()) {
		return FAILURE;
	}
	zrc_appVars.flags.bf.inRfRpt = 0;
	zrc_appVars.keyCode = keyCode;
	/* disable receiver to save power */
	u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);

	zrc_setAppState(ZRC_SENDING_IR_STATE);

	u8 repeat = 1;
	zrc_appVars.flags.bf.repeat = (single_key == 1) ? 0 : repeat;
	if(ir_isBusy()){
		zrc_appVars.flags.bf.repeat = 0;
		return FAILURE;
	}else{
		zrc_startSendRepeateDataTimer(150000);
		zrc_sendDtvIrCmd(keyCode, 0);
	}

	return SUCCESS;
}


void zrcApp_cmdQInit(cmdQ_t *q)
{
	q->readPtr = q->writePtr = 0;
	memset(q->cmdQ, 0, ZRC_APP_CMD_Q_SIZE);
}

u8 zrcApp_cmdQGetSize(cmdQ_t *q) {
	return (q->writePtr + ZRC_APP_CMD_Q_SIZE - q->readPtr) %  ZRC_APP_CMD_Q_SIZE;
}

u8 zrcApp_cmdQPush(cmdQ_t *q, u8 item) {
	if ( (q->writePtr + 1) % ZRC_APP_CMD_Q_SIZE == q->readPtr ) {
		return 0;
	}
	q->cmdQ[q->writePtr] = item;
	q->writePtr = (q->writePtr + 1) % ZRC_APP_CMD_Q_SIZE;
	return 1;
}

u8 zrcApp_cmdQPop(cmdQ_t *q) {
	u8 item = q->cmdQ[q->readPtr];
	q->readPtr = (q->readPtr + 1) % ZRC_APP_CMD_Q_SIZE;
	return item;
}




zrc_sts_t user_ZrcPowerStatusPush(u8 status){
	zrc_sts_t ret = ZRC_SUCC;

	attr_t attr;
	attr.id = GDP_ATTR_aplPowerStatus;           /*!< Attribute ID */
	attr.len = 1;                                /*!< Attribute length */
	attr.val = &status;

	ret = gdp_sendPushAttrCmd(zrc_appInfo.pairingRef, &attr, 1);

	return ret;
}
#endif
