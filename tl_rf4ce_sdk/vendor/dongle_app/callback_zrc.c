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

#if __PROJECT_ZRC_2_DONGLE__

/**********************************************************************
 * INCLUDES
 */
#include "../../net/rf4ce/rf4ce_includes.h"

#if RF4CE_ZRC2_ENABLE
#include "../../net/rf4ce/profile/zrc2/zrc_api.h"
#elif RF4CE_MSO_ENABLE
#include "../../net/rf4ce/profile/mso/mso_api.h"
#endif

#include "../../net/rf4ce/profile/profile_api.h"

#include "../../net/rf4ce/profile/gdp/gdp.h"
#include "../../net/rf4ce/profile/gdp/gdp_const.h"

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
u32 app_powerDetect(void);
void checkWhenPowerOn(void);
void checkPowerDounce(void);
u8 checkPowerServiceLoop(u8 volThreshold);
//void voltage_detect(void);
void tl_audioProfileInit(u8 profileId);
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
		drv_generateRandomData(validationCode, 4);
		validationCode[0] = rand() % 10;
		validationCode[1] = rand() % 10;
		validationCode[2] = rand() % 10;


		usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
		if (!uartBuf) {
			while(1);
		}
		uartBuf->data[0] = 4;   //length
		uartBuf->data[1] = ZRC_APP_ID_PARING_KEYCODE; //MSO_APP_VALIDATION_CODE_IND;//msoCode;
		uartBuf->data[2] = validationCode[0];
		uartBuf->data[3] = validationCode[1];
		uartBuf->data[4] = validationCode[2];
		uartBuf->len = uartBuf->data[0] + 1;
		sendCmdToTH(uartBuf);

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

    ev_on_timer(zrc_doPair, 0, 100*1000);
   /* usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	uartBuf->data[0] = 4;// Pair OK;
	uartBuf->data[1] = ZRC_APP_VALIDATION_SUCC_IND;// Pair OK;
	uartBuf->data[2] = pairingRef;
	uartBuf->data[3] = status;
	uartBuf->data[4] = 0x00;
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);*/
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
        ev_on_timer(zrc_doPair, 0, 100*1000);
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
		ev_on_timer(zrc_doPair, 0, 100*1000);
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


	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf->data[0] = 6;
	uartBuf->data[1] = ZRC_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = ZRC_APP_CMD_CODE_IND;
	uartBuf->data[3] = cmdCode;
	uartBuf->data[4] = cmdCode - ZRCmdRC_Numpad0_or_10;
	uartBuf->data[5] = 1;
	uartBuf->data[6] = lqi;
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);
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
volatile int T_zrc_cmdRecvedCbCnt[1] = {0};
void zrc_cmdRecvedCb(u8 pairingRef, u16 vendorId, u8 lqi, u8 actionNum, zrc_actionRecord_t *action)
{
	if(actionNum == 0){
		return;
	}
	u8 len = 6;
	T_zrc_cmdRecvedCbCnt[0]++;
#if USB_CDC_ENABLE
	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf->data[0] = len;//msoCode;
	uartBuf->data[1] = ZRC_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = ZRC_APP_CMD_CODE_IND;//msoCode;
	uartBuf->data[3] = action->actionCode;
	uartBuf->data[4] = action->actionCode;
	uartBuf->data[5] = 1;
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
    /*usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	uartBuf->data[0] = 2;// un-Pair OK;
	uartBuf->data[1] = ZRC_APP_UNPIAR_IND;// un-Pair OK;
	uartBuf->data[2] = pairingRef;
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);*/
	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf->data[0] = 6;//msoCode;
	uartBuf->data[1] = ZRC_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = ZRC_APP_CMD_CODE_IND;//msoCode;
	uartBuf->data[3] = 0xff;
	uartBuf->data[4] = 0xff;
	uartBuf->data[5] = 1;
	uartBuf->data[6] = 0x00;
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);
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
 * @fn      zrc_saveFlash
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

volatile u8 uart_recv_flag = 0;
volatile u8 aaa_uart_buf[64] = {0};
volatile int aaa_uart_recv_num = 0;
s32 zrcApp_uartRecvCb(u8 *pdata){
	memcpy(aaa_uart_buf, pdata, 64);
	//ota_cmd_parsing(pdata);
	ev_buf_free(pdata);
	uart_recv_flag ^= 1;
	aaa_uart_recv_num++;

#if 1
	u32 *p_sent = (u32 *)aaa_uart_buf;
	usbcdc_txBuf_t* p = (usbcdc_txBuf_t *)ev_buf_allocate(LARGE_BUFFER);
	memcpy(p->data, aaa_uart_buf, 64);
	p->len = 66;
	sendCmdToTH(p);
#endif
	return -1;
}


/*********************************************************************
 * @fn      zrcApp_init
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

#if USB_CDC_ENABLE
	TH_trxInit(zrcApp_uartRecvCb);
#endif

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

	tl_audioProfileInit(PROFILE_ZRC2);

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

    /* Initialize zrc app */
    zrcApp_init();

    /* Register Profile Call back function */
    profile_registerUserCb(&zrc_profileCmdCallbacks);

    zrc_registerUserCb(&zrc_cmdCallbacks);

    gdp_registerUserCb(&gdp_cmdCallbacks);

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

void gdp_pushIndCb(u8 pairingRef, u8 attrId, u8 *pData){
	usbcdc_txBuf_t *uartBuf = (usbcdc_txBuf_t *)ev_buf_allocate(SMALL_BUFFER);
	if (!uartBuf) {
		while(1);
	}

	uartBuf->data[0] = 6;
	uartBuf->data[1] = ZRC_APP_ID_NORMAL_KEY;
	uartBuf->data[2] = ZRC_APP_CMD_CODE_IND;
	uartBuf->data[3] = 0xff;
	uartBuf->data[4] = 0xff;
	uartBuf->data[5] = attrId;
	uartBuf->data[6] = pData[0];
	uartBuf->len = uartBuf->data[0] + 1;
	sendCmdToTH(uartBuf);
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
#endif  /* __PROJECT_MSO_ADAPTOR_APP__ */
