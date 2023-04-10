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

#if __PROJECT_MSO_DONGLE__

#include "../../proj/tl_common.h"
/**********************************************************************
 * INCLUDES
 */
#include "../../net/rf4ce/rf4ce_includes.h"
#include "../../net/rf4ce/profile/mso/mso_api.h"
#include "../../net/rf4ce/profile/profile_api.h"


#include "../common/TH_api/TH_api.h"

#if (MODULE_AUDIO_ENABLE)
#include "../../proj/drivers/audio/drv_audio.h"
#endif
#include "app_const.h"
#include "dongle_info.h"
#include "app_data.h"

#if USB_KEYBOARD_ENABLE
#include "../../proj/drivers/keyboard.h"
#endif


#include "../../proj/drivers/drv_flash.h"
#if POWER_DETECT_ENABLE
#include "../../proj/drivers/drv_adc.h"
#endif


/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */
u8 msoApp_state;
u8 t_verifyTimes;
u8 validationCode[4];
u8 validationIndex;
/**********************************************************************
 * LOCAL FUNCTIONS
 */
void mso_startCnfCb(u8 status);
void mso_pairCnfCb(u8 pairingRef, u8 status, u8 profileSize, u8 *profileList);
void mso_unPairIndCb(u8 pairingRef);
int mso_doPair(void *arg);
void mso_doUnpair(void *arg);
void mso_saveFlash(void *arg);
void mso_cmdRecvedCb(u8 pairingRef, u16 vendorId, u8 rxFlags, u8 lqi, u8 msoCode, u8 cmdCode, u8 cmdPayloadLen, u8 *payload);
void msoApp_reset(void *arg);
void msoApp_vendorInd(u8 pairingRef, u8 porfileId, u16 vendorId, u8 rxFlags, u8 lqi, u8 len, u8 *data);
u8 mso_ckValiReq(u8 pairingRef);
void mso_getAttrResp(u8 pairingRef, u8 attrib, u8 index, u8 status, u8 len, u8 *pData);
void mso_bindCnf(u8 pairingRef, u8 status,u8 lqi);
void mso_validationKeyCb(u8 pairingRef, u16 vendorId, u8 rxFlags, u8 lqi, u8 cmdCode);
void mso_setAttrInd(u8 pairingRef, u8 attrib, u8 index, u8 len, u8 *val, u8 lqi);
void mso_appDataIndCb(u8 pairingRef, u8 *pd, u8 len);
u32 app_powerDetect(void);
void checkWhenPowerOn(void);
void checkPowerDounce(void);
void voltage_detect(void);
void tl_audioProfileInit(u8 profileId);
/**********************************************************************
 * GLOBAL VARIABLES
 */
extern u8 ota_enableFlag;

/**********************************************************************
 * LOCAL VARIABLES
 */


const appVendInfo_t app_vendor_info_list[APP_VID_SUPPPORT_MAX] = {
	/* Telink */
	{APP_VENDOR_ID_LIST, {'T', 'L', 0, 0, 0, 0, 0},			  {'T', 'e', 'l', 'i', 'n', 'k', 0, 0, 0},},

};


/**********************************************************************
 * LOCAL VARIABLES
 */


static const profile_cbFunc_t mso_profileCmdCallbacks =
{
    mso_startCnfCb,                     // start stack confirm command
    mso_pairCnfCb,
	NULL,
	mso_unPairIndCb,
};

static const mso_userCbFunc_t mso_cmdCallbacks =
{
	NULL,             			//check validation resp
	mso_ckValiReq,              //check validation ind
	NULL,                       //set attr resp
	mso_setAttrInd,             //set attr ind
	mso_getAttrResp,            //get attr resp
	mso_bindCnf,                //bind confirm
	mso_cmdRecvedCb,            //cmd recv
	NULL,               		//cmd send cb
	mso_validationKeyCb,        //validation code recv
    mso_appDataIndCb,
};



/*******************Profile Callback Functions Implementation******************************/

/*********************************************************************
 * @fn      mso_ckValiReq
 *
 * @brief   Callback function for receive check validation request from controller.
 *
 * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
 *
 * @return  None
 */
u8 mso_ckValiReq(u8 pairingRef)
{
#if DONGLE_TEST_MODE
	if ( msoApp_state == MSO_APP_START_VALIDATION ) {
		t_verifyTimes = 0;
		msoApp_state = MSO_APP_VALIDATING_STATE;
	}
	if ( msoApp_state == MSO_APP_VALIDATING_STATE ) {
		if(t_verifyTimes++ >= 3){
			msoApp_state = MSO_APP_VALIDATED_STATE;
		}
		return MSO_PENDING;
	} else {
		return MSO_SUCC;
	}
#else
	//return MSO_SUCC;
	if ( msoApp_state == MSO_APP_START_VALIDATION ) {
		//generateRandomData(validationCode, 4);

		validationCode[0] = rand() % 10;
		validationCode[1] = rand() % 10;
		validationCode[2] = rand() % 10;

		usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
		if (!uartBuf) {
			while(1);
		}

		uartBuf->data[0] = 4;//msoCode;
		uartBuf->data[1] = MSO_APP_ID_PARING_KEYCODE;
		uartBuf->data[2] = validationCode[0];//msoCode;
		uartBuf->data[3] = validationCode[1];
		uartBuf->data[4] = validationCode[2];
		uartBuf->len = uartBuf->data[0] + 1;
		sendCmdToTH(uartBuf);

		msoApp_state = MSO_APP_VALIDATING_STATE;
		validationIndex = 0;
		t_verifyTimes = 0;
	}

	if ( t_verifyTimes >= 10 ) {
		/*u8 *uartBuf = ev_buf_allocate(SMALL_BUFFER);
		if (!uartBuf) {
			while(1);
		}
		uartBuf[0] = 3;
		uartBuf[1] = OTA_CMD_VALID_KEYCODE;//msoCode;
		uartBuf[2] = MSO_APP_VALIDATION_FAIL_IND;//msoCode;
		uartBuf[3] = MSO_COLLISION;

		sendCmdToTH(uartBuf, uartBuf[0] + 1);*/
		return MSO_FAILURE;
	}

	if ( msoApp_state == MSO_APP_VALIDATING_STATE ) {
		return MSO_PENDING;
	} else {
		return MSO_SUCC;
	}
#endif
}

/*********************************************************************
 * @fn      mso_getAttrResp
 *
 * @brief   Callback function for get attribute response.
 *
 * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
 *
 * @param[in]   attrId - The attribute Id
 *
 * @param[in]   index - The attribute index
 *
 * @param[in]   status - The status of the response
 *
 * @param[in]   len - The attribute length
 *
 * @param[in]   pData - The attribute value
 *
 * @return  None
 */

void mso_getAttrResp(u8 pairingRef, u8 attrib, u8 index, u8 status, u8 len, u8 *pData)
{

}


/*********************************************************************
  * @fn      mso_setAttrInd
  *
  * @brief       Callback function for receive set attribute reqquest command from controller.
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   attrId - The attribute Id
  *
  * @param[in]   index - The attribute index
  *
  * @param[in]   valLen - The attribute length
  *
  * @param[in]   val - The attribute value
  *
  */
void mso_setAttrInd(u8 pairingRef, u8 attrib, u8 index, u8 len, u8 *val, u8 lqi)
{
	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}
	uartBuf->data[0] = len + 7;
	uartBuf->data[1] = MSO_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = MSO_APP_SET_ATTR_IND;
	uartBuf->data[3] = pairingRef;
	uartBuf->data[4] = attrib;
	uartBuf->data[5] = index;
	uartBuf->data[6] = len;
	uartBuf->data[7] = lqi;
	memcpy(uartBuf+8, val, len);
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);
#if 0
	if ( mso_otaTimer == NULL ) {
		mso_otaTimer = (ev_time_event_t *)ev_buf_allocate(sizeof(ev_time_event_t));
		if ( mso_otaTimer == NULL ) {
			while(1);
		}
	}
	mso_otaTimer->cb = mso_startOTA;
	mso_otaTimer->data = (void*)pairingRef;
	ev_on_timer(mso_otaTimer,NULL, 80 * 1000);
#endif
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
#if (__DEBUG__ )
u32 T_succCnt = 0;
u32 T_failCnt = 0;
#endif
void mso_bindCnf(u8 pairingRef, u8 status,u8 lqi)
{
	lqi = lqi;
	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}
	uartBuf->data[0] = 3;
	uartBuf->data[1] = MSO_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = MSO_APP_BOND_IND;//msoCode;
	uartBuf->data[3] = status;
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);

	msoApp_state = MSO_APP_NORMAL_STATE;

	ev_on_timer(mso_doPair, 0, 500*1000);

	if ( status == RF4CE_NWK_DISCOVERY_ABORT ) {
		return;
	}
	if ( status == MSO_SUCC ) {
		mso_saveFlash(NULL);
		T_succCnt++;
	}else{
		T_failCnt++;
	}
}

/*********************************************************************
 * @fn      mso_startCnfCb
 *
 * @brief   Callback function for profile_startNwk.
 *          Target (Light): After network is started successfully, pair procedure be started automatically
 *          Controller (Switch): After network is started successfully, do noting, waiting push button to start pair procedure
 *
 * @param   status - Network start status, it is set by stack
 *
 * @return  None
 */
void mso_startCnfCb(u8 status)
{
	if ( status == SUCCESS ) {
		/* Force start in channel 25 */
		//EV_SCHEDULE_TASK(mso_doPair, 0);
		//mso_doPair(0);
#if 1 || !DONGLE_TEST_MODE
		ev_on_timer(mso_doPair,0, 500*1000);
#endif
	}
	u8 value = 15;
	nwk_nlmeSetReq(NWK_BASE_CHANNEL, 0, &value);
}


/*********************************************************************
 * @fn      mso_pairCnfCb
 *
 * @brief   Callback function for profile_startPair.
 *          Target(Light): set LED_CONNECT_STATUS on when pair success
 *          Controller(Switch): set LED_PORT[i] on when pair success with light i
 *
 * @param   status     - Network start status, it is set by stack
 * @param   pairingRef - indicating the light stored in which pair table entry
 *
 * @return  None
 */
volatile int T_mso_pairCnfErrCnt = 0;
volatile int T_mso_pairCnfOkCnt = 0;
void mso_pairCnfCb(u8 pairingRef, u8 status, u8 profileSize, u8 *profileList)
{
	if ( status == RF4CE_NWK_DISCOVERY_ABORT ) {
		return;
	}
	if ( status == SUCCESS ) {
		T_mso_pairCnfOkCnt++;
		msoApp_state = MSO_APP_START_VALIDATION;
	} else {
		//EV_SCHEDULE_TASK(mso_doPair, 0);
        //mso_doPair(0);

#if !DONGLE_TEST_MODE
        ev_on_timer(mso_doPair,0, 500);
#else
        T_mso_pairCnfErrCnt++;
        ev_on_timer(mso_doPair,0, 500);
        msoApp_state = MSO_APP_INVALID_STATE;
#endif
	}
}


/*********************************************************************
 * @fn      mso_validationKeyCb
 *
 * @brief   Callback function for receive validation key from controller.
 *
 * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
 *
 * @param[in]   vendorId - The vendor Id to put in the RF4CE data request frame
 *
 * @param[in]   rxFlags - rx options for this frame
 *
 * @param[in]   lqi - The lqi value of this packet
 *
 * @param[in]   cmdCode - RC command code
 *
 * @return  None
 */
void mso_validationKeyCb(u8 pairingRef, u16 vendorId, u8 rxFlags, u8 lqi, u8 cmdCode)
{
#if DONGLE_TEST_MODE
	return;
#endif

	if ( validationCode[validationIndex] == cmdCode - ZRCmdRC_Numpad0_or_10 ) {
		validationIndex++;
	} else {
		validationIndex = 0;
	}
	if ( validationIndex == 3 ) {
		msoApp_state = MSO_APP_VALIDATED_STATE;

#if 0
		uartBuf[0] = 5;// Pair OK;
		uartBuf[1] = OTA_CMD_VALID_KEYCODE;
		uartBuf[2] = MSO_APP_VALIDATION_SUCC_IND;// Pair OK;
		uartBuf[3] = msoApp_state;
		uartBuf[4] = lqi;
		uartBuf[5] = 0x00;

		sendCmdToTH(uartBuf, uartBuf[0] + 1);
		return;
#endif
	}
	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf->data[0] = 6;
	uartBuf->data[1] = MSO_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = MSO_APP_CMD_CODE_IND;
	uartBuf->data[3] = cmdCode;
	uartBuf->data[4] = cmdCode - ZRCmdRC_Numpad0_or_10;
	uartBuf->data[5] = 1;
	uartBuf->data[6] = lqi;
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);
}


/*********************************************************************
 * @fn      mso_cmdRecvedCb
 *
 * @brief   Callback function for receive command from controller. In this
 *          sample, only command code is used.
 *
 * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
 *
 * @param[in]   vendorId - The vendor Id to put in the RF4CE data request frame
 *
 * @param[in]   rxFlags - rx options for this frame
 *
 * @param[in]   msoCode - The ZRC command code @ref zrc_cmd_code
 *
 * @param[in]   cmdCode - RC command code
 *
 * @param[in]   cmdPayloadLen - The RC command payload length
 *
 * @param[in]   cmdPayload - cmdPayload
 *
 * @return  None
 */
volatile int T_mso_cmdRecvedCbCnt;
void mso_cmdRecvedCb(u8 pairingRef, u16 vendorId, u8 rxFlags, u8 lqi, u8 msoCode, u8 cmdCode, u8 cmdPayloadLen, u8 *payload)
{
	u8 len = 6;
	T_mso_cmdRecvedCbCnt++;

#if USB_CDC_ENABLE
	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf->data[0] = len;//msoCode;
	uartBuf->data[1] = MSO_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = MSO_APP_CMD_CODE_IND;//msoCode;
	uartBuf->data[3] = msoCode;
	uartBuf->data[4] = cmdCode;
	uartBuf->data[5] = cmdPayloadLen;
	uartBuf->data[6] = lqi;
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);
#elif USB_KEYBOARD_ENABLE
	kb_data_t keyData;
	keyData.cnt = 1;
	keyData.ctrl_key = 0;
	keyData.keycode[0] = 0xff;

	if(action->actionControl.byte == 0){
		keyData.cnt = 0;
	}

	for(s32 i = 0; i < sizeof(rf4ce2hidKeyMapping)/2; i++){
		if(action->actionCode == rf4ce2hidKeyMapping[i][0]){
			keyData.keycode[0] = rf4ce2hidKeyMapping[i][1];
			break;
		}
	}

	if(keyData.keycode[0] != 0xff){
		usbkb_hid_report(&keyData);
	}
#endif
}


/*********************************************************************
 * @fn      mso_unPairIndCb
 *
 * @brief   Callback function for receive unpair request from controller.
 *
 * @param   pairingRef  - the index of the pair entry
 *
 * @return  None
 */
void mso_unPairIndCb(u8 pairingRef)
{
	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}
	uartBuf->data[0] = 3;
	uartBuf->data[1] = MSO_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = MSO_APP_UNPIAR_IND;
	uartBuf->data[3] = pairingRef;
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);

	//EV_SCHEDULE_TASK(mso_doPair, NULL);
	//mso_doPair(NULL);
#if !DONGLE_TEST_MODE
	ev_on_timer(mso_doPair,0, 500);
#else
	 msoApp_state = MSO_APP_INVALID_STATE;
#endif
}



/*********************************************************************
 * @fn      mso_doPair
 *
 * @brief   Start pair procedure.
 *
 * @param   None
 *
 * @return  None
 */
volatile int T_mso_doPairNum[2] = {0};
int mso_doPair(void *arg)
{
	T_mso_doPairNum[0]++;
    if ( SUCCESS != profile_startPair(1, 0xff, 3, 0) ) {
		//EV_SCHEDULE_TASK(mso_doPair, NULL);
		//mso_doPair(NULL);
    	T_mso_doPairNum[1]++;
    	msoApp_state = MSO_APP_INVALID_STATE;
		return 0;
	}

    msoApp_state = MSO_APP_START_BINDING_STATE;
    return -1;
}




void msoApp_reset(void *arg)
{
    //nv_init(1);
}
/*********************************************************************
 * @fn      mso_saveFlash
 *
 * @brief   save all pib to flash.
 *
 * @param   None
 *
 * @return  None
 */
void mso_saveFlash(void *arg)
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

		if ( (SUCCESS != mso_saveTgtRcTblToFlash())
			//|| (SUCCESS != mso_saveRibToFlash())
			//|| (SUCCESS != mso_saveAttrToFlash())
			)
		{
			nv_resetModule(DS_PROFILE_MODULE);
			mso_saveTgtRcTblToFlash();
			mso_saveRibToFlash();
			mso_saveAttrToFlash();
		}
#endif
	irq_restore(r);

}





/*******************Initialization Functions Implementation******************************/

/*********************************************************************
 * @fn      msoApp_initPib
 *
 * @brief   Initialize the necessary Network PIB
 *
 * @param   none
 *
 * @return  none
 */
void msoApp_initPib(void)
{

	/* Set the LQI threshold to 1 means a very low value */
    u8 value = 0x1;
    nwk_nlmeSetReq(NWK_DISCOVERY_LQI_THRESHOLD, 0, &value);

    if ( nwk_isSecEnable() ) {
        value = 7;
    } else {
        value = 3;
    }
    nwk_nlmeSetReq(NWK_NODE_CAPABILITIES, 0, &value);

	/* Default discovery reception interval, 1 second. (0xf424 * 16)us */
    u32 respWaitTime = 0xF424;
    nwk_nlmeSetReq(NWK_DISCOVERY_REPETITION_INTERVAL, 0, (u8*)&respWaitTime);

	/* Default response waiting timer, 100 millsecond. (0x186a * 16)us */
    respWaitTime = 0x186a;
    nwk_nlmeSetReq(NWK_RESPONSE_WAIT_TIME, 0, (u8*)&respWaitTime);

	/* Default discovery repetitions */
    value = 0x1E;
    nwk_nlmeSetReq(NWK_MAX_DISCOVERY_REPETITIONS, 0, &value);

    value = 0x01;
    nwk_nlmeSetReq(NWK_MAX_REPORTED_NODE_DESCRIPTORS, 0, &value);

	u16 vendId = app_vendor_info_list[0].vendId;
	nwk_nlmeSetReq(NWK_VENDOR_IDENTIFIER, 0, (u8*)&vendId);
	nwk_nlmeSetReq(NWK_VENDOR_STRING, 0, (u8 *)app_vendor_info_list[0].vendString);

	nwk_nlmeSetReq(NWK_USER_STRING, 0, (u8 *)app_vendor_info_list[0].userString);
}



u8 uart_recv_flag = 0;
u8 aaa_uart_buf[64] = {0};
volatile int aaa_uart_recv_num = 0;
s32 msoApp_uartRecvCb(u8 *pdata){
	memcpy(aaa_uart_buf, pdata, 64);
	ev_buf_free(pdata);
	uart_recv_flag = 1;

	/*u32 *p_sent = (u32 *)aaa_uart_buf;
	*p_sent = aaa_uart_recv_num++;
	usbcdc_txBuf_t* p = (usbcdc_txBuf_t *)ev_buf_allocate(LARGE_BUFFER);
	memcpy(p->data, aaa_uart_buf, 64);
	p->len = 64;
	//sendCmdToTH(p, 64);
	extern void ota_cmd_parsing(u8 *p_ota);*/
	return -1;
}


/*********************************************************************
 * @fn      msoApp_init
 *
 * @brief   Initialize the Lighting APP configuration
 *
 * @param   none
 *
 * @return  none
 */
void msoApp_init(void)
{
	/* Initialize NWK PIB */
	msoApp_initPib();

#if USB_CDC_ENABLE
	TH_trxInit(msoApp_uartRecvCb);//mso_uartRecv
#endif

	msoApp_state = MSO_APP_INVALID_STATE;
}

/*********************************************************************
 * @fn      user_init
 *
 * @brief   user application level initialization
 *
 * @param   none
 *
 * @return  none
 */
extern const profile_cbFunc_t mso_profileCmdCallbacks;
extern const mso_userCbFunc_t mso_cmdCallbacks;
void user_init(void){
#if (USB_MIC_ENABLE)
	audio_decInit();
#endif

	tl_audioProfileInit(PROFILE_MSO);

#if (POWER_DETECT_ENABLE)
	drv_adc_init();

	/* init ADC for battery detection */
	drv_adc_battery_detect_init();

    /* start battery detection */
    app_powerDetect();

    checkWhenPowerOn();
#endif
	/* Initialize stack */
    profile_init();

    /* Initialize mso app */
    msoApp_init();

    /* Register Profile Call back function */
    profile_registerUserCb((profile_cbFunc_t *)&mso_profileCmdCallbacks);

    mso_registerUserCb((mso_userCbFunc_t *)&mso_cmdCallbacks);

    /* start RF4CE network */
    profile_startNwk();

}
extern u32 tick_usb_enum;
void app_idle_handler(void)
{
#if USB_KEYBOARD_ENABLE
	extern void usbkb_release_check(void);
	usbkb_release_check();
#endif

#if POWER_DETECT_ENABLE
	if(ev_isTaskDone()&&(clock_time_exceed(tick_usb_enum,10*1000)))
	{
		tick_usb_enum = clock_time ();
		voltage_detect();
	}
#endif

}

void gpio_user_irq_handler(void){

}




void insert(u16 *a,int n){
	int i,j,temp;
	for(i = 1; i < n; i++) {
		temp = a[i];
		j = i - 1;

		while(j >= 0 && temp < a[j]) {
			a[j+1] = a[j];
			j--;
		}
		a[j+1] = temp;
	}
}

u8 battSta = 0;
volatile u16 batteryCap=0;
u32 app_powerDetect(void){
	u32 ret = 0;

#if POWER_DETECT_ENABLE
	u32 battMatrix = sizeof(batteryVoltage)/sizeof(u16);
	u16 battThres[5] = {0};
	u8 r = irq_disable();
	u32 i = 0;
	u32 battery_value1 = 0;
	static u16 battBuf[12] = {0};

	for(i = 0; i < 12; i++){
		battBuf[i] = drv_get_adc_data();
		WaitUs(5);
	}
	insert(battBuf, 12);

	battery_value1 += battBuf[4];
	for(i = 5; i < 8; i++){
		if(abs(battBuf[i] - battBuf[i-1]) > 30){
			irq_restore(r);
			return 0;
		}
		battery_value1 += battBuf[i];
	}
	battery_value1 /= 4;

	for (i=0; i<battMatrix; i++ ) {
		if ( battery_value1 > (batteryVoltage[i] + battThres[i]) ) {
			break;
		}
	}

	batteryCap = battery_value1;
	battSta = i;
	irq_restore(r);
#endif
	return ret;
}


/*********************************************************************
 * @fn      checkBatteryBeforeSaveFlash
 *
 * @brief   check the battery level
 *
 * @param   None
 *
 * @return  1:battery level is great than or equal to volThreshold
 * 		    0:battery level is less than volThreshold
 */
u8 checkPowerServiceLoop(u8 volThreshold){
#if POWER_DETECT_ENABLE
	if(battSta>=volThreshold)
	return 0;
	else
	return 1;
#else
	return 1;
#endif
}






void voltage_detect(void)
{
#if POWER_DETECT_ENABLE
	u16 voltage = 0;
	u32 curTick = clock_time();
	u32 battMatrix =  sizeof(batteryVoltage)/sizeof(u16);
	u8  i=0;
	//printf("VDD: %d\n", voltage);
	while(1)
	{
		voltage = drv_get_adc_data();
		for (i=0; i<battMatrix; i++ )
		{
			if ( voltage > batteryVoltage[i])
			{
				break;
			}
		}

		if(i<=PWR_THRESHOLD_RESET)
			break;

		if(clock_time_exceed(curTick, 1000 * 1000))
		{
			SYSTEM_RESET();
		}
	}
	batteryCap = voltage;
	battSta = i;
#endif
}

/*********************************************************************
 * @fn      check power  when power on
 *
 * @brief   check the power level when power on
 *
 * @param   None
 *
 * @return  1:power level is great than or equal to BAT_LEVEL_CUTOFF
 * 		    0:power level is less than BAT_LEVEL_CUTOFF
 */
void checkWhenPowerOn(void){
#if POWER_DETECT_ENABLE
	voltage_detect();
#endif
#if (!FLASH_PROTECT)
	flash_unlock();
#endif
}


#endif  /* __PROJECT_MSO_ADAPTOR_APP__ */
