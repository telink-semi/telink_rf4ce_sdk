/********************************************************************************************************
 * @file     zrcApp.c
 *
 * @brief    Initialization for RCU, some events need to be looped
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
#else
#include "../../net/rf4ce/profile/zrc/zrc_api.h"
#endif
#include "../../net/rf4ce/profile/profile_api.h"

#include "../../proj/drivers/keyboard.h"
#include "../../proj/drivers/drv_flash.h"
#include "../../proj/drivers/ir/ir.h"
#include "../../proj/drivers/drv_adc.h"
#include "../../proj/drivers/drv_pm.h"
#include "../../proj/os/timer.h"
#include "../../proj/os/sys.h"

#include "zrc_const.h"
#include "zrcApp.h"
#include "zrcApp_data.h"
#include "zrcApp_cb.h"
#include "zrcApp_config.h"
#include "rc_info.h"
#include "zrcApp_led.h"
#include "qsIR_Tx.h"
#if (MODULE_AUDIO_ENABLE)
#include "../../proj/drivers/audio/drv_audio.h"
#include "../common/TL_specificData/TL_specific_data_audio.h"
#endif

/**********************************************************************
 * LOCAL CONSTANTS
 */

extern profile_cbFunc_t zrc_profileCmdCallbacks;

extern zrc_userCbFunc_t zrc_cmdCallbacks;
extern const u8 ZRC2_APP_USER_STRING[];
/**********************************************************************
 * TYPEDEFS
 */
extern u8 ota_enableFlag;
extern u32 sys_active_event_time;

/**********************************************************************
 * LOCAL FUNCTIONS
 */

void zrc_saveStackInfo(void *arg);
void zrc_saveAppInfo(void *arg);
void zrc_restoreAppState(void);
void app_idle_handler(void);
u32 app_batteryDetect(void);
/**********************************************************************
 * GLOBAL VARIABLES
 */
//const u32 firmwareVersion = (MAJOR_VER<<24) | (MINOR_VER<<16) | (PATCH_VER<<8) | BUILD_VER;

/**********************************************************************
 * LOCAL VARIABLES
 */
u32 app_wakeupPins[] = KB_DRIVE_PINS;
u8 app_wakeupLevel[KB_ARRAY_ROW] = {1};

zrc_app_t zrc_appVars;

zrc_appInfo_t zrc_appInfo;

cmdQ_t cmdQ;

u8 zrc_batteryLowCnt;

tl_audioRecInfo_t g_audioRecInfo = {
		.sampleRate = MIC_SAMPLE_RATE,
		.resoutionBits = MIC_RESOLUTION_BIT,
		.channelNum = MIC_CHANNLE_COUNT,
		.codecType = TL_AUDIO_CODEC_TYPE_ADPCM,
		.pktSize = MIC_ADPCM_FRAME_SIZE,
		.interval = 10,
};

u8 app_validLevelForPm(u8 reset){
	for(s32 i = 0; i < sizeof(app_wakeupPins)/sizeof(s32); i++){
		if(reset){
			app_wakeupLevel[i] = 1;
		}else{
			app_wakeupLevel[i] = gpio_read(app_wakeupPins[i]) ? 0 : 1;
		}
	}
	return 0;
}

/*********************************************************************
 * @fn      zrc_saveAppInfo
 *
 * @brief   save application info to flash.
 *
 * @param   None
 *
 * @return  None
 */
void zrc_saveAppInfo(void *arg)
{
	u8 r = irq_disable();
#if (MODULE_FLASH_ENABLE)
	if ( SUCCESS != nv_userSaveToFlash(ZRC_APP_NV_ITEM, sizeof(zrc_appInfo_t), (u8*)&zrc_appInfo) ) {
		nv_resetModule(DS_USER_MODULS);
		nv_userSaveToFlash(ZRC_APP_NV_ITEM, sizeof(zrc_appInfo_t), (u8*)&zrc_appInfo);
	}
#endif
	irq_restore(r);
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

/*********************************************************************
 * @fn      zrc_saveStackInfo
 *
 * @brief   save nwk pib to flash (frame conuter).
 *
 * @param   None
 *
 * @return  None
 */
void zrc_savePibInfo(void *arg)
{
	/* voltage lower than 2.2V, not save flash */
	if ( !checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
		return;
	}
	u8 r = irq_disable();
#if (MODULE_FLASH_ENABLE)
	if ( SUCCESS != nwk_savePibToFlash() ) {
		u32 nwk_frameCnt;
	    nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, (u8 *)&nwk_frameCnt);
		nwk_frameCnt++;
		nwk_nlmeSetReq(NWK_FRAME_COUNTER, 0, (u8 *)&nwk_frameCnt);
	    nwk_savePibToFlash();
	}
#endif
	irq_restore(r);
}


/*******************Initialization Functions Implementation******************************/

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

	u32 dutyCycle = 1250*3;  //RF4CE_NWK_MAX_DUTY_CYCLE; //960 * 3;
	nwk_nlmeSetReq(NWK_DUTY_CYCLE, 0, (u8*)&dutyCycle);

	/* vendor identifier */
	u16 vendId = APP_VENDOR_ID_LIST;
	nwk_nlmeSetReq(NWK_VENDOR_IDENTIFIER, 2, (u8 *)&vendId);

	u8 *userStr = (u8 *)ZRC2_APP_USER_STRING;
	nwk_nlmeSetReq(NWK_USER_STRING, 0, userStr);
}


/*********************************************************************
 * @fn      zrc_resetAll
 *
 * @brief   this function is used to do the factory reset.
 *
 * @param   none
 *
 * @return  none
 */
void zrc_resetAll(void* arg)
{
#if (MODULE_FLASH_ENABLE)
    nv_init(1);
#endif
    zrc_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
    zrc_appInfo.state = ZRC_UNCONNECT_STATE;
    zrc_setAppState(ZRC_UNCONNECT_STATE);

	pmFlag_t pmInfo;
	pmInfo.byte = analog_read(reg_mac_channel);
	analog_write(reg_mac_channel, pmInfo.byte);
	u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);

	/* voltage lower than 2.2V, not save flash */
	if ( checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
		zrc_saveAppInfo(NULL);
	}
	zrc_startLed(8, ZRC_APP_LED_BLINK_QUICK_TIME, 1);
}


/*********************************************************************
 * @fn      zrcApp_initIR
 *
 * @brief   this function is used to init IR SPWM.
 *
 * @param   none
 *
 * @return  none
 */
volatile u8 T_zrcApp_irSentCnt = 0;
void zrcApp_initIR(void){

	ir_init(IR_PWN_ID);
	IR_PWM_PIN_CFG;
	hwTmr_init(TIMER_IDX_1, TIMER_MODE_SCLK);


#if (0 || IR_DEBUG)
	void ir_send_mn6014_c6d6(u8 addr, u8 cmd, u8 fRepeat);
	while(1){
		irq_enable();
		T_zrcApp_irSentCnt++;
		if(!ir_isBusy()){
			ir_send_mn6014_c6d6(0x29, 0x20, 0);
		}
		ev_process_timer();
	}
#endif
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

void zrcApp_init(void){
    /* Initialize ZRC APP global control variable */
    if ( SUCCESS != nv_userLoadFromFlash(ZRC_APP_NV_ITEM, sizeof(zrc_appInfo_t), (u8*)&zrc_appInfo) ){
        zrc_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
        zrc_appInfo.state = ZRC_UNCONNECT_STATE;
    	zrc_appVars.flags.byte = 0;
    	zrc_appVars.flags.bf.allowedDeep = 1;

		/* voltage lower than 2.2V, not save flash */
		zrc_saveAppInfo(NULL);
    }

    zrc_appVars.ledStopCnt = 0;
	zrc_appVars.blinkTimes = 0;
	zrc_appVars.flags.bf.ledStatus = 0;
	zrc_appVars.flags.bf.repeat = 0;
	//zrc_appVars.flags.bf.sendingRib = 0;
	zrc_appVars.flags.bf.ledIdleState = 0;
    zrc_appVars.flags.bf.allowedDeep = 1;
    zrc_setAppState(zrc_appInfo.state);

	/* int cmd Queue*/
	zrcApp_cmdQInit(&cmdQ);

#if(MODULE_PM_ENABLE)
	sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif

	zrc_appVars.lastKey = 0xff;
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
    profile_registerUserCb(&zrc_profileCmdCallbacks);

	zrc_registerUserCb(&zrc_cmdCallbacks);
}


/*********************************************************************
 * @fn      user_init
 *
 * @brief   User level initialization code.
 *
 * @param   None
 *
 * @return  None
 */
void user_init(void){
	/* during init, disable irq */
	u8 r = irq_disable();

	/* init IR */
	zrcApp_initIR();

    /* audio module Initialization */
#if (MODULE_AUDIO_ENABLE)
	APP_AMIC_PIN_CFG_OFF;
	tl_audioRecInit(PROFILE_ZRC2, &g_audioRecInfo, zrc_appAudioCb);
#endif

	drv_adc_init();
    /* Initialize stack */
    stack_init();

	/* init ADC for battery detection */
	drv_adc_battery_detect_init();

    /* start battery detection */
    app_batteryDetect();

    /* Initialize Application layer modules */
    zrcApp_init();

    app_loadInfo();

	irq_restore(r);

#if (!FLASH_PROTECT)
    flash_write_status(FLASH_PROTECT_NONE);
#endif

	app_validLevelForPm(1);

	/* start RF4CE network */
	if ( mac_getState() != 5 ) {
		profile_startNwk();
	}
}

u8 app_isOTAstate(void){
    return (zrc_getAppState() == ZRC_OTA_STATE);
}

/*********************************************************************
 * @fn      app_isIdle
 *
 * @brief   return the application layer idle status, if yes, power save will enter.
 *
 * @param   None
 *
 * @return  status
 */
u8 app_isIdle(void){
	u8 idle = 1;

    if ( ((zrc_getAppState() >= ZRC_PRE_PAIRING_STATE) && (zrc_getAppState() <= ZRC_CHECK_VALIDATION_STATE)) ||
    	 (zrc_getAppState() == ZRC_OTA_STATE) ||
		 (zrcApp_cmdQGetSize(&cmdQ) != 0)     ||
		 (app_isSendingIR())
#if MODULE_AUDIO_ENABLE
		 || (audio_recTaskStatusGet() == AUDIO_OPENED)
#endif
		) {
    	idle &= 0;

	}
	return idle;
}

/*********************************************************************
 * @fn      app_isSendingIR
 *
 * @brief   return the application layer IR status (sending IR or not).
 *
 * @param   None
 *
 * @return  status
 */
u8 app_isSendingIR(void){
	return (zrc_getAppState() == ZRC_SENDING_IR_STATE || ir_isBusy()||zrcIrGetState()) ? 1 : 0;
}



/*********************************************************************
 * @fn      zrc_restoreAppState
 *
 * @brief   restore the application layer state.
 *
 * @param   None
 *
 * @return  None
 */
void zrc_restoreAppState(void){
	zrc_setAppState(zrc_appInfo.state);
}


/*********************************************************************
 * @fn      zrc_isNormalState
 *
 * @brief   return the application state (nomarl or temporary state)
 *
 * @param   None
 *
 * @return  None
 */
u8 zrc_isNormalState(void){
	return (zrc_getAppState() == ZRC_READY_STATE) | (zrc_getAppState() == ZRC_UNCONNECT_STATE);
}

/*********************************************************************
 * @fn      app_allowedDeep
 *
 * @brief   check application can go into deep sleep or not
 *
 * @param   None
 *
 * @return  status
 */
u8 app_allowedDeep(void){
	/* if sys time is idle more than 60s, force to deep sleep mode. */
	if ( clock_time_exceed(sys_active_event_time, 60 * 1000 * 1000) ) {
		return 1;
	}

	else {
    	return zrc_appVars.flags.bf.allowedDeep && !app_isSendingIR() && !zrc_appVars.flags.bf.repeat && !zrc_appVars.ledBlinkTimer && timer_event_idle() ;
	}
}


/*********************************************************************
 * @fn      app_saveInfoPm
 *
 * @brief   save necessary info to analog register before goes into deep sleep
 *
 * @param   None
 *
 * @return  status
 */
int app_saveInfoPm(void){
	int ret = 1;

#if (MODULE_PM_ENABLE)
	pmFlag_t pmInfo;
	pmInfo.byte = analog_read(reg_mac_channel);
	zrc_appVars.battStaUpload |= (zrc_appVars.battSta != pmInfo.bf.battSta) ? 1 : 0;
    /* save current paired target's channel */
    pairTable_t *pEntry = getPairingEntryByIndex(zrc_appInfo.pairingRef);
    if ( pEntry ) {
        pmInfo.bf.ch = pEntry->destChannel / 5 - 2;
    }
	pmInfo.bf.pmDeep = 1;

	pmInfo.bf.battSta = zrc_appVars.battSta;
	analog_write(reg_mac_channel, (u8)pmInfo.byte);
    u8 nwk_frameCnt[4];
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
    nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, nwk_frameCnt);
    foreach(i, 4) {
        analog_write(reg_nwk_seq_no+i, nwk_frameCnt[i]);
    }
    if(zrc_appVars.battSta >= BAT_LEVEL_CUTOFF){
    	ret = 0;
    	return ret;
    }
#endif

	if(zrc_appVars.battStaUpload){
		zrc_appVars.battStaUpload = 0;
		user_ZrcPowerStatusPush(zrc_appVars.battSta);
		return 2;
	}

    return ret;
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


u32 app_batteryDetect(void){
	u32 ret = 0;

#if BATTERY_DETECT_ENABLE
	u32 battMatrix = sizeof(batteryVoltage)/sizeof(u16);
	u16 battThres[5] = {0};
	u8 r = irq_disable();
	u32 i = 0;
	u32 battery_value1 = 0;
	u16 battBuf[12] = {0};

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

	zrc_appVars.battSta = i;
	irq_restore(r);
#endif
	return ret;
}

/*********************************************************************
 * @fn      zrc_frameCountUpdate
 *
 * @brief   update frame count when power on
 *
 * @param   pairingRef
 *
 * @return  status
 */
extern nv_sts_t nwk_savePibFrameCountToFlash(void);
extern nv_sts_t nwk_loadPibFrameCountFromFlash(u32 *frameCount);
void zrc_frameCountUpdate(u8 pairingRef, u32 *nwk_frameCnt){
	nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, (u8 *)nwk_frameCnt);
	if ( RF4CE_INVALID_PAIR_REF != pairingRef ) {
		*nwk_frameCnt += ZRC_ADD_FRAME_COUNTER;
		nwk_nlmeSetReq(NWK_FRAME_COUNTER, 0, (u8 *)nwk_frameCnt);
		if (checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH)) {
			/* wait 200 ms for stable */
			nwk_savePibFrameCountToFlash();
		}
	}
}
/*********************************************************************
 * @fn      app_loadInfo
 *
 * @brief   load necessary info from analog register when wakeup from deep sleep
 *
 * @param   None
 *
 * @return  status
 */
void app_loadInfo(void){
	pmFlag_t pmInfo;
	u32 curFrameCnt;
	u32 curPibFrameCnt;
	u8 *nwk_frameCnt = (u8 *)&curPibFrameCnt;
	pairTable_t *pEntry = getPairingEntryByIndex(zrc_appInfo.pairingRef);
	pmInfo.byte = analog_read(reg_mac_channel);

	/* recover from deep sleep or real power on*/
	if(pmInfo.bf.pmDeep){  /* from deep sleep */
		if ( pmInfo.bf.ch != 0 && pEntry ) {
			pEntry->destChannel = 5 * (pmInfo.bf.ch + 2);
		}
		zrc_appVars.battSta = pmInfo.bf.battSta;

		/*
		 * check if the frame count is full, if it's full, the frame count will be updated
		 *
		 * */
		nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, (u8*)&curFrameCnt);
		foreach(i, 4) {
			nwk_frameCnt[i] = analog_read(reg_nwk_seq_no+i);
		}
	}else{
		WaitMs(100);
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		app_batteryDetect();
		zrc_appVars.battStaUpload |= 1;
		zrc_frameCountUpdate(zrc_appInfo.pairingRef, &curPibFrameCnt);
	}
	pmInfo.bf.pmDeep = 0;
	analog_write(reg_mac_channel, (u8)pmInfo.byte);
	nwk_nlmeSetReq(NWK_FRAME_COUNTER, 0, nwk_frameCnt);
}


/*********************************************************************
 * @fn      app_idle_handler
 *
 * @brief   idle function for system
 *
 * @param   None
 *
 * @return  status
 */

extern void user_keyEventHandle(void);
void app_idle_handler(void){
	static u32 keyScanStartTime = 0;
	if(clock_time_exceed(keyScanStartTime, 10*1000)){
		if(kb_scan_key(0, 1)){
			user_keyEventHandle();
		}
		keyScanStartTime = clock_time();
	}

	zrc_lightOnProgLed();

	if(app_isSendingIR()){
		ev_rf4ceStackEnable(0);
		return;
	}else{
		ev_rf4ceStackEnable(1);
	}

	if ( (ZRC_READY_STATE == zrc_getAppState())||
		 (ZRC_OTA_STATE == zrc_getAppState())||
		 (ZRC_AUDIO_STATE == zrc_getAppState())) {
		u32 nwk_frameCnt;
		static u32 nwk_frameCntInFlash = 0;
		nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, (u8*)&nwk_frameCnt);
		if(nwk_frameCntInFlash == 0 || nwk_frameCntInFlash + ZRC_ADD_FRAME_COUNTER < nwk_frameCnt){
			nwk_loadPibFrameCountFromFlash(&nwk_frameCntInFlash);
			nwk_savePibFrameCountToFlash();
		}
	}

	if(stack_allow_suspend() && app_isIdle() && ev_isTaskDone() &&
	   !app_isSendingIR() && !zrc_appVars.ledBlinkTimer){
		app_batteryDetect();
	}
}


/*********************************************************************
 * @fn      checkBatteryBeforeSaveFlash
 *
 * @brief   check the battery level before save flash
 *
 * @param   None
 *
 * @return  battery level
 */
u8 checkBatteryBeforeSaveFlash(u16 volThreshold){
	return 1;
}


typedef struct{
	u32 nwkFrameCount;
	u8 flag;
}zrc_deepSleepInfo_t;
zrc_deepSleepInfo_t zrc_deepSleepInfo;

void zrc_boundInfoSave(void){
	u8 *nwk_frameCnt = (u8 *)&zrc_deepSleepInfo.nwkFrameCount;
	nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, nwk_frameCnt);
	zrc_deepSleepInfo.flag = analog_read(reg_mac_channel);
}

void zrc_boundInfoRestore(void){
	u8 *nwk_frameCnt = (u8 *)&zrc_deepSleepInfo.nwkFrameCount;
	foreach(i, 4) {
		analog_write(reg_nwk_seq_no+i, nwk_frameCnt[i]);
	}
	analog_write(reg_mac_channel, zrc_deepSleepInfo.flag);
}

void enter_deepsleep_exit_bind(void){
	for(s32 i = 0; i < 5; i++){
		if((gpio_read(app_wakeupPins[i]) && app_wakeupLevel[i]) ||
			(!gpio_read(app_wakeupPins[i]) && !app_wakeupLevel[i])){
			return;
		}
		platform_wakeup_pad_cfg(app_wakeupPins[i], app_wakeupLevel[i], 1);
	}
	app_validLevelForPm(0);
}

#endif  /* __PROJECT_ZRC_CLASS_DEV_APP__ */
