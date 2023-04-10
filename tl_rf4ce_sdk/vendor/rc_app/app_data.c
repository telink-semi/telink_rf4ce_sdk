/********************************************************************************************************
 * @file    app_data.c
 *
 * @brief   This is the source file for app_data.c
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

#if (RF4CE_CONTROLLER)
/**********************************************************************
 * INCLUDES
 */
#include "../../net/rf4ce/rf4ce_includes.h"

#if (RF4CE_ZRC2_ENABLE)
#include "../../net/rf4ce/profile/zrc2/zrc_api.h"
#include "../../net/rf4ce/profile/zrc2/zrc.h"
#include "../../net/rf4ce/profile/gdp/gdp.h"
#include "callback_zrc.h"
#elif (RF4CE_MSO_ENABLE)
#include "../../net/rf4ce/profile/mso/mso_api.h"
#include "../../net/rf4ce/profile/mso/mso.h"
#include "callback_mso.h"
#endif

#include "../../net/rf4ce/profile/profile_attr.h"
#include "../../proj/drivers/ir/ir_database.h"
#include "../../proj/drivers/ir/ir.h"
#include "../../proj/drivers/keyboard.h"

#include "app_const.h"
#include "app_common.h"
#include "app_data.h"
#include "app_led.h"
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
extern app_Control_t 	rcu_appVars;
extern app_Info_t   	rcu_appInfo;




u8 rf_keyMappingTbl[KB_ARRAY_COL][KB_ARRAY_ROW] = KB_MAP_NORMAL;
u8 ir_keyMappingTbl[KB_ARRAY_COL][KB_ARRAY_ROW] = KB_MAP_FOR_IR;


int 	zrc_sendRepeateDataTimerCb(void *arg);
void 	zrc_startSendRepeateDataTimer(u32 rptTime);
u8 		zrc_sendZrcCmd(u8 zrcCode, u8 keyCode);
void 	zrc_sendIrCmd(void *arg);
void 	zrc_appSendData(u8 zrcCode, u8 keyCode, u8 fRepeat);
int 	zrc_sendDataTimerCb(void *arg);

u8 		zrcApp_cmdQPush(cmdQ_t *q, u8 item);
u8 		zrcApp_cmdQPop(cmdQ_t *q);
void 	zrc_sendDtvIrCmd(u8 keyCode, u8 repeat);
/*********************************************************************
 * @fn      zrc_sendRepeateDataTimerCb
 *
 * @brief   timer cb to auto send repeat zrc cmd.
 *
 * @param   None
 *
 * @return  None
 */

int app_sendRepeateDataTimerCb(void *arg)
{
	if(app_getState() == APP_SENDING_IR_STATE){
		if(rcu_appVars.flags.bf.repeat == 0){
			app_restoreState();
			return -1;
		}
		app_sendDtvIrCmd(rcu_appVars.keyCode, 1);
	}else{
		if(rcu_appVars.keyCode == 0xff){
			profile_stopSendRFKeyCmd(rcu_appInfo.pairingRef);
			rcu_appVars.flags.bf.inRfRpt = 0;
			rcu_appVars.flags.bf.repeat = 0;
			rcu_appVars.flags.bf.allowedDeep = 1;
			app_restoreState();

#if(MODULE_PM_ENABLE)
			sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif
			return -1;
		}
	}

    rcu_appVars.flags.bf.allowedDeep = 0;
	if ( rcu_appVars.coutchCnt-- == 0 ) {
		/* if send repeate data timer is on, stop it. */
		app_stopSendRepeateDataTimer();
		rcu_appVars.flags.bf.allowedDeep = 1;
		app_restoreState();
		rcu_appVars.flags.bf.repeat = 0;
        rcu_appVars.configTimer = NULL;
        app_validLevelForPm(0);
#if RF4CE_ZRC2_ENABLE
		if(zrc_getIrState())//stop qs
		{
			zrcStop();
		}
#endif
		return -1;
	}
	else {
		return 0;
	}
}


void app_startSendRepeateDataTimer(u32 rptTime)
{
    if( rcu_appVars.configTimer )
    {
        ev_unon_timer(&rcu_appVars.configTimer);
    }

    rcu_appVars.configTimer = ev_on_timer(app_sendRepeateDataTimerCb, NULL, rptTime);
    rcu_appVars.coutchCnt = 30*1000*1000 / rptTime;
}

void app_stopSendRepeateDataTimer(void)
{
	if ( rcu_appVars.configTimer ) {
		ev_unon_timer(&rcu_appVars.configTimer);
	}

	if ( rcu_appVars.flags.bf.inRfRpt ) {
//		zrc_rcStopSendActionCmd(rcu_appInfo.pairingRef, &zrc_rec);
		profile_stopSendRFKeyCmd(rcu_appInfo.pairingRef);
		rcu_appVars.flags.bf.inRfRpt = 0;
	}
	rcu_appVars.flags.bf.repeat = 0;

#if(MODULE_PM_ENABLE)
	sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif
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
u8 app_sendRFKeyCmd(u8 keyCode)
{
    /* check the current mode, if RF4CE mode  */
    u8 txOption;
#if (SECURITY_ENABLE)
    txOption = NWK_TXOPTION_ACK | NWK_TXOPTION_SECURITY;
#else
    txOption = NWK_TXOPTION_ACK;
#endif
    /* check the link status */

    if ( app_getState() == APP_CHECK_VALIDATION_STATE) {
    	return profile_sendValidationKey(rcu_appInfo.pairingRef, txOption, keyCode);
    }

    else if ( app_getState() != APP_READY_STATE || rcu_appInfo.pairingRef == RF4CE_INVALID_PAIR_REF ) {
        return FAILURE;
    }

	if(SUCCESS != profile_startSendRFKeyCmd(rcu_appInfo.pairingRef, txOption, keyCode)){
		return FAILURE;
    }else {
    		if(rcu_appVars.flags.bf.repeat == 1)
    		{
    		/* start the timer for repeat */
    		rcu_appVars.flags.bf.allowedDeep = 0;
    		rcu_appVars.flags.bf.inRfRpt = 1;
    		rcu_appVars.flags.bf.repeat = 1;
    		app_startSendRepeateDataTimer(50000);
#if(MODULE_PM_ENABLE)
    		sys_suspendTimerSet(55);
#endif
    		}
	}
	return SUCCESS;
}


u8 app_sendRFData(u8 keyCode)
{
	if (rcu_appVars.flags.bf.repeat) {
		return FAILURE;
	}
	rcu_appVars.flags.bf.inRfRpt = 0;

    /* if key code need to send, disable airmouse send data */
	rcu_appVars.keyCode = keyCode;

	return app_sendRFKeyCmd(keyCode);
}





volatile u8 T_zrc_sendDtvIrCmd = 0;
void app_sendDtvIrCmd(u8 keyCode, u8 repeat)
{
	u16 real_keyCode = 0;
	ir_bit16db_t* pIrDatabase = irdb16_getEntry(41);

	if(keyCode == 0xff){
		if(pIrDatabase && pIrDatabase->fn){
			pIrDatabase->fn(pIrDatabase->addr, keyCode, 0);
		}
		return;
	}

	for(int i= 0 ; i < KB_ARRAY_COL; i++)
	{
		for(int j= 0 ; j < KB_ARRAY_ROW; j++)
		{
			if(rf_keyMappingTbl[i][j] == keyCode)
			{
				real_keyCode = ir_keyMappingTbl[i][j];// ((ir_keyMappingTbl[i][j])<<8);
				if(pIrDatabase && pIrDatabase->fn)
				{
					T_zrc_sendDtvIrCmd++;
					pIrDatabase->fn(pIrDatabase->addr, real_keyCode, repeat);
				}
				return;
			}
		}
	}
}





u8 app_sendIRData(u8 profileId, u8 keyCode, u8 single_key)
{
	if ( app_getState() == APP_VERIFY_PAIR_STATE || rcu_appVars.flags.bf.repeat||app_isSendingIR()) {
		return FAILURE;
	}
	rcu_appVars.flags.bf.inRfRpt = 0;
	rcu_appVars.keyCode = keyCode;
	/* disable receiver to save power */
	u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);

    app_setState(APP_SENDING_IR_STATE);

	u8 repeat = 1;
	rcu_appVars.flags.bf.repeat = (single_key == 1) ? 0 : repeat;
	if(ir_isBusy()){
		rcu_appVars.flags.bf.repeat = 0;
		return FAILURE;
	}else{
		app_startSendRepeateDataTimer(150000);
		app_sendDtvIrCmd(keyCode, 0);
	}

	return SUCCESS;
}
#endif
