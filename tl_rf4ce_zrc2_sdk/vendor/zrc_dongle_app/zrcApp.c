/********************************************************************************************************
 * @file     zrcApp.c
 *
 * @brief    Initialization for target, some events need to be looped
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

#if __PROJECT_ZRC_2_DONGLE__

/**********************************************************************
 * INCLUDES
 */
#include "../../net/rf4ce/rf4ce_includes.h"
#include "../../net/rf4ce/profile/zrc2/zrc_api.h"
#include "../../net/rf4ce/profile/profile_api.h"

#include "../../net/rf4ce/profile/gdp/gdp.h"
#include "../../net/rf4ce/profile/gdp/gdp_const.h"

#include "../common/TH_api/TH_api.h"

#if (MODULE_AUDIO_ENABLE)
#include "../../proj/drivers/audio/drv_audio.h"
#endif
#include "zrc_const.h"
#include "dongle_info.h"
#include "zrcApp_data.h"

#if USB_KEYBOARD_ENABLE
#include "../../proj/drivers/keyboard.h"
#endif


/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */
u8 zrcApp_state;
u8 t_verifyTimes;
u8 validationCode[4];
u8 validationIndex;
/**********************************************************************
 * LOCAL FUNCTIONS
 */
void zrc_startCnfCb(u8 status);
void zrc_pairCnfCb(u8 pairingRef, u8 status, u8 profileSize, u8 *profileList);
void zrc_unPairIndCb(u8 pairingRef);
int zrc_doPair(void *arg);
void zrc_doUnpair(void *arg);
void zrc_saveFlash(void *arg);
void zrc_cmdRecvedCb(u8 pairingRef, u16 vendorId, u8 lqi, u8 actionNum, zrc_actionRecord_t *action);
void zrcApp_reset(void *arg);
void zrcApp_vendorInd(u8 pairingRef, u8 porfileId, u16 vendorId, u8 rxFlags, u8 lqi, u8 len, u8 *data);
u8 zrc_ckValiReq(u8 pairingRef);
void zrc_getAttrResp(u8 pairingRef, u8 attrib, u8 index, u8 status, u8 len, u8 *pData);
void zrc_bindCnf(u8 pairingRef, u8 status);
void zrc_validationKeyCb(u8 pairingRef, u16 vendorId, u8 lqi, u8 actionNum, zrc_actionRecord_t *action);

void gdp_pushIndCb(u8 pairingRef, u8 attrId, u8 *pData);
/**********************************************************************
 * GLOBAL VARIABLES
 */
extern u8 ota_enableFlag;

#if USB_KEYBOARD_ENABLE
const u8 rf4ce2hidKeyMapping[][2] = {
		{ZRCmdRC_Numpad0_or_10, VK_0},
		{ZRCmdRC_Numpad1, 		VK_1},
		{ZRCmdRC_Numpad2, 		VK_2},
		{ZRCmdRC_Numpad3, 		VK_3},
		{ZRCmdRC_Numpad4, 		VK_4},
		{ZRCmdRC_Numpad5, 		VK_5},
		{ZRCmdRC_Numpad6, 		VK_6},
		{ZRCmdRC_Numpad7, 		VK_7},
		{ZRCmdRC_Numpad8, 		VK_8},
		{ZRCmdRC_Numpad9, 		VK_9},
		{ZRCmdRC_Numpad8, 		VK_8},
		{ZRCmdRC_Mute,			0x7F},
		{ZRCmdRC_VolumeUp,		0x80},
		{ZRCmdRC_VolumeDown,	0x81},
};
#endif

/**********************************************************************
 * LOCAL VARIABLES
 */


static profile_cbFunc_t zrc_profileCmdCallbacks =
{
    zrc_startCnfCb,                     // start stack confirm command
    zrc_pairCnfCb,
	NULL,
	zrc_unPairIndCb,
};

static zrc_userCbFunc_t zrc_cmdCallbacks =
{
    NULL,               //!< Function pointer to the ZRC check validation resp command
    zrc_ckValiReq,               //!< Function pointer to the ZRC check validation req command
    NULL,               //!< Function pointer to the received ZRC set attribute resp command
    NULL,               //!< Function pointer to the received ZRC get attribute response command
    NULL,                //!< Function pointer to the received ZRC pull attribute response command
    NULL,                //!< Function pointer to the received ZRC push attribute response command
    zrc_bindCnf,               //!< Function pointer to the received ZRC bind confirm command
    zrc_cmdRecvedCb,               //!< Function pointer to handle the received ZRC command. Equal to command request indication.
    NULL,      //!< Function pointer to the received ZRC data sent confirm command
    zrc_validationKeyCb,               //!< Function pointer to the received ZRC validation key command
    NULL,
    zrc_appDataIndCb,
};


gdp_userCbFunc_t gdp_cmdCallbacks = {
	NULL,
	NULL,
	NULL,
	gdp_pushIndCb,   //push attrbute indicate
	NULL,
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
u8 zrc_ckValiReq(u8 pairingRef){
	if ( zrcApp_state == ZRC_APP_START_VALIDATION )
	{
		//generateRandomData(validationCode, 4);
		validationCode[0] = rand() % 10;
		validationCode[1] = rand() % 10;
		validationCode[2] = rand() % 10;


		u8 *uartBuf = ev_buf_allocate(SMALL_BUFFER);
		if (!uartBuf) {
			while(1);
		}
		uartBuf[0] = 4;   //length
		uartBuf[1] = ZRC_APP_ID_PARING_KEYCODE; //MSO_APP_VALIDATION_CODE_IND;//msoCode;
		uartBuf[2] = validationCode[0];
		uartBuf[3] = validationCode[1];
		uartBuf[4] = validationCode[2];

		sendCmdToTH(uartBuf, uartBuf[0] + 1);

		zrcApp_state = ZRC_APP_VALIDATING_STATE;
		validationIndex = 0;
		t_verifyTimes = 0;



	}

	if ( t_verifyTimes++ >= 20 ) {
		return ZRC_FAILURE;
	}

	if ( zrcApp_state == ZRC_APP_VALIDATING_STATE ) {
		return ZRC_PENDING;
	} else {
		return ZRC_SUCC;
	}
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
void zrc_bindCnf(u8 pairingRef, u8 status)
{
	if ( status == ZRC_SUCC ) {
#if (__DEBUG__ )
        T_succCnt++;
#endif
		zrc_saveFlash(NULL);
#if (__DEBUG__ )
	}
    else {
        T_failCnt++;
#endif
	}

    ev_on_timer(zrc_doPair, 0, 1);
   /* u8 *uartBuf = ev_buf_allocate(SMALL_BUFFER);
	uartBuf[0] = 4;// Pair OK;
	uartBuf[1] = ZRC_APP_VALIDATION_SUCC_IND;// Pair OK;
	uartBuf[2] = pairingRef;
	uartBuf[3] = status;
	uartBuf[4] = 0x00;

	sendCmdToTH(uartBuf, 4);*/
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
void zrc_startCnfCb(u8 status)
{
	if ( status == SUCCESS ) {
		/* Force start in channel 25 */
        ev_on_timer(zrc_doPair, 0, 1);
	}
	u8 value = 20;// 20;
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
void zrc_pairCnfCb(u8 pairingRef, u8 status, u8 profileSize, u8 *profileList)
{
	if ( status == SUCCESS ) {
		zrcApp_state = ZRC_APP_START_VALIDATION;
	} else {
		ev_on_timer(zrc_doPair, 0, 1);
	}
}


/*********************************************************************
 * @fn      zrc_validationKeyCb
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
void zrc_validationKeyCb(u8 pairingRef, u16 vendorId, u8 lqi, u8 actionNum, zrc_actionRecord_t *action)
{

	u8 cmdCode = action->actionCode;
	if ( validationCode[validationIndex] == cmdCode - 0x20 ) {
		validationIndex++;
	} else {
		validationIndex = 0;
	}
	if ( validationIndex == 3 ) {
		zrcApp_state = ZRC_APP_VALIDATED_STATE;
	}


	u8 *uartBuf = ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf[0] = 6;
	uartBuf[1] = ZRC_APP_ID_NORMAL_KEY;
	uartBuf[2] = ZRC_APP_CMD_CODE_IND;
	uartBuf[3] = cmdCode;
	uartBuf[4] = cmdCode - ZRCmdRC_Numpad0_or_10;
	uartBuf[5] = 1;
	uartBuf[6] = lqi;
	sendCmdToTH(uartBuf, uartBuf[0] + 1);
}



/*********************************************************************
 * @fn      zrc_cmdRecvedCb
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
volatile int T_zrc_cmdRecvedCbCnt = 0;
void zrc_cmdRecvedCb(u8 pairingRef, u16 vendorId, u8 lqi, u8 actionNum, zrc_actionRecord_t *action)
{
	if(actionNum == 0){
		return;
	}
	u8 len = 6;
	T_zrc_cmdRecvedCbCnt++;

#if USB_CDC_ENABLE
	//return;
	u8 *uartBuf = ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf[0] = len;//msoCode;
	uartBuf[1] = ZRC_APP_ID_NORMAL_KEY;
	uartBuf[2] = ZRC_APP_CMD_CODE_IND;//msoCode;
	uartBuf[3] = action->actionCode;
	uartBuf[4] = action->actionCode;
	uartBuf[5] = 1;
	uartBuf[6] = lqi;
	sendCmdToTH(uartBuf, uartBuf[0] + 1);
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
 * @fn      zrc_unPairIndCb
 *
 * @brief   Callback function for receive unpair request from controller.
 *
 * @param   pairingRef  - the index of the pair entry
 *
 * @return  None
 */
void zrc_unPairIndCb(u8 pairingRef)
{
    /*u8 *uartBuf = ev_buf_allocate(SMALL_BUFFER);
	uartBuf[0] = 2;// un-Pair OK;
	uartBuf[1] = ZRC_APP_UNPIAR_IND;// un-Pair OK;
	uartBuf[2] = pairingRef;


	sendCmdToTH(uartBuf, uartBuf[0] + 1);*/
	u8 *uartBuf = ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf[0] = 6;//msoCode;
	uartBuf[1] = ZRC_APP_ID_NORMAL_KEY;
	uartBuf[2] = ZRC_APP_CMD_CODE_IND;//msoCode;
	uartBuf[3] = 0xff;
	uartBuf[4] = 0xff;
	uartBuf[5] = 1;
	uartBuf[6] = 0x00;
	sendCmdToTH(uartBuf, uartBuf[0] + 1);
	ev_on_timer(zrc_doPair, 0, 100*1000);
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
int zrc_doPair(void *arg)
{

    if ( SUCCESS != profile_startPair(1, 0xff, 0x3, 0) ) {
		return 0;
	}
    return -1;
}




void zrcApp_reset(void *arg)
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
void zrc_saveFlash(void *arg)
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
void zrcApp_initPib(void)
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
    value = 0x2;
    nwk_nlmeSetReq(NWK_MAX_DISCOVERY_REPETITIONS, 0, &value);
    value = 0x01;
    nwk_nlmeSetReq(NWK_MAX_REPORTED_NODE_DESCRIPTORS, 0, &value);

	u16 vendorId = APP_VENDOR_ID_LIST;  //0x10ef;//APP_VENDOR_ID_LIST[0];
	nwk_nlmeSetReq(NWK_VENDOR_IDENTIFIER, 0, (u8*)&vendorId);
}

u8 uart_recv_flag = 0;
u8 aaa_uart_buf[64] = {0};
volatile int aaa_uart_recv_num = 0;
s32 zrcApp_uartRecvCb(u8 *pdata){
	memcpy(aaa_uart_buf, pdata, 64);
	//ota_cmd_parsing(pdata);
	ev_buf_free(pdata);
	uart_recv_flag = 1;

	/*u32 *p_sent = (u32 *)aaa_uart_buf;
	*p_sent = aaa_uart_recv_num++;
	u8* p = ev_buf_allocate(LARGE_BUFFER);
	memcpy(p, aaa_uart_buf, 64);
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
void zrcApp_init(void)
{
	/* Initialize NWK PIB */
	zrcApp_initPib();

	TH_trxInit(zrcApp_uartRecvCb);

	zrcApp_state = ZRC_APP_INVALID_STATE;
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

void user_init(void){
#if (USB_MIC_ENABLE)
	audio_decInit();
#endif

    /* Initialize stack */
    profile_init();

    /* Initialize mso app */
    zrcApp_init();

    /* Register Profile Call back function */
    profile_registerUserCb(&zrc_profileCmdCallbacks);

    zrc_registerUserCb(&zrc_cmdCallbacks);

    gdp_registerUserCb(&gdp_cmdCallbacks);

    /* start RF4CE network */
    profile_startNwk();

}

void app_idle_handler(void)
{
#if USB_KEYBOARD_ENABLE
	extern void usbkb_release_check(void);
	usbkb_release_check();
#endif
}

void gpio_user_irq_handler(void){

}

void gdp_pushIndCb(u8 pairingRef, u8 attrId, u8 *pData){
	u8 *uartBuf = ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf[0] = 6;
	uartBuf[1] = ZRC_APP_ID_NORMAL_KEY;
	uartBuf[2] = ZRC_APP_CMD_CODE_IND;
	uartBuf[3] = 0xff;
	uartBuf[4] = 0xff;
	uartBuf[5] = attrId;
	uartBuf[6] = pData[0];
	sendCmdToTH(uartBuf, uartBuf[0] + 1);
}

#endif  /* __PROJECT_MSO_ADAPTOR_APP__ */
