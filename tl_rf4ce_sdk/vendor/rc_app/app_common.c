/********************************************************************************************************
 * @file    app_common.c
 *
 * @brief   This is the source file for app_common.c
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
#include "../../platform/platform_includes.h"
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
#include "../../net/rf4ce/profile/profile_api.h"
#include "../../proj/drivers/keyboard.h"
#include "../../proj/drivers/drv_flash.h"
#include "../../proj/drivers/ir/ir.h"
#include "../../proj/drivers/drv_adc.h"
#include "../../proj/drivers/drv_pm.h"
#include "../../proj/drivers/nv.h"
#include "../../proj/os/timer.h"
#include "../../proj/os/sys.h"

#include "app_const.h"
#include "app_common.h"
#include "app_data.h"

#include "app_config.h"
#include "rc_info.h"
#include "app_led.h"
#include "qsIR_Tx.h"
#if (MODULE_AUDIO_ENABLE)
#include "../../proj/drivers/audio/drv_audio.h"
#include "../common/TL_specificData/tl_specific_data_audio.h"
#endif
#include "../common/TL_specificData/tl_specific_data_ota.h"
/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */
extern u8 ota_enableFlag;
extern u32 sys_active_event_time;

/**********************************************************************
 * LOCAL FUNCTIONS
 */

void save_appInfo(void *arg);
void app_restoreState(void);
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

app_Control_t 	rcu_appVars;
app_Info_t   	rcu_appInfo;

cmdQ_t cmdQ;


tl_audioRecInfo_t g_audioRecInfo = {
		.sampleRate = MIC_SAMPLE_RATE,
		.resoutionBits = MIC_RESOLUTION_BIT,
		.micChlNum = MIC_CHANNLE_COUNT,
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
 * @fn      save_appInfo
 *
 * @brief   save application info to flash.
 *
 * @param   None
 *
 * @return  None
 */
void save_appInfo(void *arg)
{
	u8 r = irq_disable();
#if (MODULE_FLASH_ENABLE)
	if ( SUCCESS != nv_userSaveToFlash(APP_INFO_NV_ITEM, sizeof(app_Info_t), (u8*)&rcu_appInfo) ) {
		nv_resetModule(DS_USER_MODULS);
		nv_userSaveToFlash(APP_INFO_NV_ITEM, sizeof(app_Info_t), (u8*)&rcu_appInfo);
	}
#endif
	irq_restore(r);
}


/*********************************************************************
 * @fn      save_nwkpibInfo
 *
 * @brief   save nwk pib to flash (frame conuter).
 *
 * @param   None
 *
 * @return  None
 */
void save_nwkpibInfo(void *arg)
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
 * @fn      reset_allInfo
 *
 * @brief   this function is used to do the factory reset.
 *
 * @param   none
 *
 * @return  none
 */
void reset_allInfo(void* arg)
{
#if (MODULE_FLASH_ENABLE)
    nv_init(1);
#endif
    rcu_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
    rcu_appInfo.state = APP_UNCONNECT_STATE;
    app_setState(APP_UNCONNECT_STATE);

	pmFlag_t pmInfo;
	pmInfo.byte = analog_read(reg_mac_channel);
	analog_write(reg_mac_channel, pmInfo.byte);
	u8 value = 0;
    mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);

	/* voltage lower than 2.2V, not save flash */
	if ( checkBatteryBeforeSaveFlash(BAT_THRESHOLD_FLASH) ) {
		save_appInfo(NULL);
	}
	led_startSet(8, APP_LED_BLINK_QUICK_TIME, 1);
}


/*********************************************************************
 * @fn      init_IRModule
 *
 * @brief   this function is used to init IR SPWM.
 *
 * @param   none
 *
 * @return  none
 */
volatile u8 T_App_irSentCnt = 0;
void init_IRModule(void){

	ir_init(IR_PWM_ID);
	IR_PWM_PIN_CFG;
	IR_CTRL_PIN_CFG;

#if (MODULE_IR_LEARN_ENABLE)
	IR_LEARN_PIN_CFG;
#endif

#if	!IR_DMA_FIFO_EN
	hwTmr_init(TIMER_IDX_1, TIMER_MODE_SCLK);
#endif

#if (0 || IR_DEBUG)
	void ir_send_mn6014_c6d6(u8 addr, u8 cmd, u8 fRepeat);
	while(1){
		irq_enable();
		T_App_irSentCnt++;
		if(!ir_isBusy()){
			ir_send_mn6014_c6d6(0x29, 0x20, 0);
		}
		ev_process_timer();
	}
#endif
}


/*********************************************************************
 * @fn      init_rcuState
 *
 * @brief   Initialize the Lighting APP configuration
 *
 * @param   none
 *
 * @return  none
 */

void init_rcuState(void){
    /* Initialize profile APP global control variable */
    if ( SUCCESS != nv_userLoadFromFlash(APP_INFO_NV_ITEM, sizeof(app_Info_t), (u8*)&rcu_appInfo) ){
        rcu_appInfo.pairingRef = RF4CE_INVALID_PAIR_REF;
        rcu_appInfo.state = APP_UNCONNECT_STATE;
    	rcu_appVars.flags.byte = 0;
    	rcu_appVars.flags.bf.allowedDeep = 1;

		/* voltage lower than 2.2V, not save flash */
    	save_appInfo(NULL);
    }

    rcu_appVars.ledStopCnt = 0;
	rcu_appVars.blinkTimes = 0;
	rcu_appVars.flags.bf.ledStatus = 0;
	rcu_appVars.flags.bf.repeat = 0;
	//rcu_appVars.flags.bf.sendingRib = 0;
	rcu_appVars.flags.bf.ledIdleState = 0;
    rcu_appVars.flags.bf.allowedDeep = 1;
    app_setState(rcu_appInfo.state);

	/* int cmd Queue*/
    app_cmdQInit(&cmdQ);

#if(MODULE_PM_ENABLE)
	sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif

	rcu_appVars.lastKey = 0xff;
}


void app_cmdQInit(cmdQ_t *q)
{
	q->readPtr = q->writePtr = 0;
	memset(q->cmdQ, 0, APP_CMD_Q_SIZE);
}


u8 app_cmdQGetSize(cmdQ_t *q)
{
	return (q->writePtr + APP_CMD_Q_SIZE - q->readPtr) %  APP_CMD_Q_SIZE;
}


u8 app_cmdQPush(cmdQ_t *q, u8 item)
{
	if ( (q->writePtr + 1) % APP_CMD_Q_SIZE == q->readPtr ) {
		return 0;
	}
	q->cmdQ[q->writePtr] = item;
	q->writePtr = (q->writePtr + 1) % APP_CMD_Q_SIZE;
	return 1;
}


u8 app_cmdQPop(cmdQ_t *q)
{
	u8 item = q->cmdQ[q->readPtr];
	q->readPtr = (q->readPtr + 1) % APP_CMD_Q_SIZE;
	return item;
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
	init_IRModule();

    /* audio module Initialization */
#if (MODULE_AUDIO_ENABLE)
	APP_AMIC_PIN_CFG_OFF;
#if RF4CE_MSO_ENABLE
	tl_audioRecInit(PROFILE_MSO, &g_audioRecInfo);
#elif RF4CE_ZRC2_ENABLE
	tl_audioRecInit(PROFILE_ZRC2, &g_audioRecInfo);
#endif
#endif

	drv_adc_init();

	/* init ADC for battery detection */
	drv_adc_battery_detect_init();

    /* start battery detection */
    app_batteryDetect();

    checkBatteryPowerOn();

    /* Initialize stack */
    stack_init();

    /* Initialize Application layer modules */
    init_rcuState();

    app_loadInfo();

	irq_restore(r);

	app_validLevelForPm(1);

	/* start RF4CE network */
	if ( mac_getState() != 5 ) {
		profile_startNwk();
	}
}

u8 app_isOTAstate(void){
    return (app_getState() == APP_OTA_STATE);
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

    if ( ((app_getState() >= APP_PRE_PAIRING_STATE) && (app_getState() <= APP_CHECK_VALIDATION_STATE)) ||
    	 (app_getState() == APP_OTA_STATE) ||
		 (app_cmdQGetSize(&cmdQ) != 0)     ||
		 (app_isSendingIR())
#if MODULE_AUDIO_ENABLE
		 || (audio_recTaskStatusGet() == AUDIO_OPENED)
#endif
#if MODULE_IR_LEARN_ENABLE
		 || (app_getState() == APP_LEARNING_IR_STATE)
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
	return (app_getState() == APP_SENDING_IR_STATE || ir_isBusy()) ? 1 : 0;
}



/*********************************************************************
 * @fn      app_restoreState
 *
 * @brief   restore the application layer state.
 *
 * @param   None
 *
 * @return  None
 */
void app_restoreState(void){
	app_setState(rcu_appInfo.state);
}


/*********************************************************************
 * @fn      app_isNormalState
 *
 * @brief   return the application state (nomarl or temporary state)
 *
 * @param   None
 *
 * @return  None
 */
u8 app_isNormalState(void){
	return (app_getState() == APP_READY_STATE) | (app_getState() == APP_UNCONNECT_STATE);
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
    	return rcu_appVars.flags.bf.allowedDeep && !app_isSendingIR() && !rcu_appVars.flags.bf.repeat && !rcu_appVars.ledBlinkTimer && timer_event_idle() ;
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
	rcu_appVars.battStaUpload |= (rcu_appVars.battSta != pmInfo.bf.battSta) ? 1 : 0;
    /* save current paired target's channel */
    pairTable_t *pEntry = getPairingEntryByIndex(rcu_appInfo.pairingRef);
    if ( pEntry ) {
        pmInfo.bf.ch = pEntry->destChannel / 5 - 2;
    }
	pmInfo.bf.pmDeep = 1;

	pmInfo.bf.battSta = rcu_appVars.battSta;
	analog_write(reg_mac_channel, (u8)pmInfo.byte);
    u8 nwk_frameCnt[4];
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
    nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, nwk_frameCnt);
    foreach(i, 4) {
        analog_write(reg_nwk_seq_no+i, nwk_frameCnt[i]);
    }
    if(rcu_appVars.battSta >= BAT_LEVEL_CUTOFF){
    	ret = 0;
    	return ret;
    }
#endif

	if(rcu_appVars.battStaUpload){
		rcu_appVars.battStaUpload = 0;
		user_PowerStatusPush(rcu_appVars.battSta);//rcu_appVars.battSta
		return 2;
	}

    return ret;
}



/*********************************************************************
 * @fn      nwk_frameCountUpdate
 *
 * @brief   update frame count when power on
 *
 * @param   pairingRef
 *
 * @return  status
 */
extern nv_sts_t nwk_savePibFrameCountToFlash(void);
extern nv_sts_t nwk_loadPibFrameCountFromFlash(u32 *frameCount);
void nwk_frameCountUpdate(u8 pairingRef, u32 *nwk_frameCnt){
	nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, (u8 *)nwk_frameCnt);
	if ( RF4CE_INVALID_PAIR_REF != pairingRef ) {
		*nwk_frameCnt += NWK_ADD_FRAME_COUNTER;
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
	pairTable_t *pEntry = getPairingEntryByIndex(rcu_appInfo.pairingRef);
	pmInfo.byte = analog_read(reg_mac_channel);

	/* recover from deep sleep or real power on*/
	if(pmInfo.bf.pmDeep){  /* from deep sleep */
		if ( pmInfo.bf.ch != 0 && pEntry ) {
			pEntry->destChannel = 5 * (pmInfo.bf.ch + 2);
		}
		rcu_appVars.battSta = pmInfo.bf.battSta;

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
		rcu_appVars.battStaUpload |= 1;
		nwk_frameCountUpdate(rcu_appInfo.pairingRef, &curPibFrameCnt);
		foreach(i, 4) {
			analog_write(reg_nwk_seq_no+i, 0);
		}
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

	app_lightOnProgLed();

	if(app_isSendingIR()){
		ev_rf4ceStackEnable(0);
		return;
	}else{
		ev_rf4ceStackEnable(1);
	}

	if((APP_READY_STATE == app_getState())
	   ||(APP_OTA_STATE == app_getState())
	   ||(APP_AUDIO_STATE == app_getState())
	   ) {
		u32 nwk_frameCnt;
		static u32 nwk_frameCntInFlash = 0;
		nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, (u8*)&nwk_frameCnt);
		if(nwk_frameCntInFlash == 0 || nwk_frameCntInFlash + NWK_ADD_FRAME_COUNTER < nwk_frameCnt){
			nwk_loadPibFrameCountFromFlash(&nwk_frameCntInFlash);
			nwk_savePibFrameCountToFlash();
		}
	}
#if POWER_DETECT_ENABLE
	extern u32 tick_usb_enum;
	if((!audio_recTaskStatusGet())&&(clock_time_exceed(tick_usb_enum,10*1000)))
	{
		tick_usb_enum = clock_time ();
		voltage_detect();
	}
#endif
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

volatile u16 batteryCap=0;
u32 app_batteryDetect(void){
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
	rcu_appVars.battSta = i;
	irq_restore(r);
#endif
	return ret;
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
	//printf("checkBatteryBeforeSaveFlash \n\r");
#if POWER_DETECT_ENABLE
	if(rcu_appVars.battSta>volThreshold)
	return 0;
	else
	return 1;
#else
	return 1;
#endif
}

/*********************************************************************
 * @fn      checkBattery when power on
 *
 * @brief   check the battery level when power on
 *
 * @param   None
 *
 * @return  1:battery level is great than or equal to BAT_LEVEL_CUTOFF
 * 		    0:battery level is less than BAT_LEVEL_CUTOFF
 */
#define BatteryDounceNum      100
void checkBatteryPowerOn(void){
	//printf("checkBatteryPowerOn \n\r");
#if POWER_DETECT_ENABLE
	if(pm_get_mcu_status()==MCU_STATUS_BOOT)//deep
	{
		voltage_detect();
	}
#endif
#if (!FLASH_PROTECT)
		flash_unlock();
#endif
}


void voltage_detect(void)
{
#if POWER_DETECT_ENABLE
	//TLSR8 series has only one ADC.
	//so the battery voltage cannot be detected when other ADC channel are running.
	if(audio_recTaskStatusGet())
		return;
	u16 voltage = 0;
	u32 curTick = clock_time();
	u32 battMatrix =  sizeof(batteryVoltage)/sizeof(u16);
	u8  i=0;
	//printf("VDD: %d\n", voltage);
	while(1)
	{
		voltage = drv_get_adc_data();
		for (i=0; i<battMatrix; i++ ) {
			if ( voltage > batteryVoltage[i]) {
				break;
			}
		}

		if(i<=BAT_THRESHOLD_FLASH)
			break;

		if(clock_time_exceed(curTick, 1000 * 1000))
		{
#if MODULE_PM_ENABLE
			rcu_appVars.flags.bf.allowedDeep = 1;
			platform_lowpower_enter(PLATFORM_MODE_DEEPSLEEP, 0, 0);
#else
			SYSTEM_RESET();
#endif
		}
	}
	batteryCap = voltage;
	rcu_appVars.battSta = i;
#endif
}




void user_PowerStatusPush(u8 status)
{
	if((rcu_appInfo.pairingRef != RF4CE_INVALID_PAIR_REF)&&
	   (APP_READY_STATE == app_getState()))
	profile_PowerStatusPush(status);
}




app_deepSleepInfo_t app_deepSleepInfo;

void app_boundInfoSave(void){
	u8 *nwk_frameCnt = (u8 *)&app_deepSleepInfo.nwkFrameCount;
	nwk_nlmeGetReq(NWK_FRAME_COUNTER, 0, nwk_frameCnt);
	app_deepSleepInfo.flag = analog_read(reg_mac_channel);
}

void zrc_boundInfoRestore(void){
	u8 *nwk_frameCnt = (u8 *)&app_deepSleepInfo.nwkFrameCount;
	foreach(i, 4) {
		analog_write(reg_nwk_seq_no+i, nwk_frameCnt[i]);
	}
	analog_write(reg_mac_channel, app_deepSleepInfo.flag);
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


/***********************************process key*************************************/
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
		app_stopSendRepeateDataTimer();
	}

	if(keyCode == 0xff){
		return;
	}

	if(rcu_appVars.battSta >= BAT_THRESHOLD_WARNING){
		led_startSet(3, APP_LED_BLINK_SEND_DATA_WARNING_TIME, 1);
	}else{
		led_startSet(1, APP_LED_BLINK_SEND_DATA_TIME, 1);
	}

	if(rcu_appInfo.pairingRef != RF4CE_INVALID_PAIR_REF){
		if(APP_READY_STATE == app_getState()){
#if MODULE_AUDIO_ENABLE
			if(keyCode == VK_RECORED){
#if 0
				/* send audio data directly */
				if (audio_recTaskStatusGet() == AUDIO_IDLE){
					audio_recTaskStart();
					led_On(1);
				}else{
					audio_recTaskStop();
					led_Off(1);
				}
#else
				/* send audio start command to target, open audio once receiving the audio open response command
				 * need to work with TELINK dongle
				 * */
				if (audio_recTaskStatusGet() == AUDIO_OPENED){
#if RF4CE_MSO_ENABLE
					tl_audio_stop(PROFILE_MSO);
#elif RF4CE_ZRC2_ENABLE
					tl_audio_stop(PROFILE_ZRC2);
#endif

				}else{
					app_audioStart(3,10,audio_stateSettingCb);
				}
#endif
			}else{
				if (audio_recTaskStatusGet() != AUDIO_OPENED){

					if(keyCode == VK_TV){
						app_startOta();
						return;
					}
					else
					{
						app_sendRFData(keyCode);
					}

				}
			}
#else
			app_sendRFData(keyCode);
#endif
		}
	}else{
#if (RF4CE_ZRC2_ENABLE)
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
#endif
#if (MODULE_IR_LEARN_ENABLE)
		void LearningStaCallBack(u8 st);
		if(keyCode==ZRCmdRC_SetupMenu)//sw38
		{
			setlearningState(LearningStaCallBack);
			ir_start_learn(ZRCmdRC_Mute);
			return;
		}

		if(keyCode==ZRCmdRC_Mute)//sw35
		{
			app_setState(ZRC_SENDING_IR_STATE);
			if(ir_learn_send_nv(keyCode)==SUCCESS)
					return;
		}
#endif
		app_sendIRData(APP_KEY_FOR_DTA, keyCode, 0);
		T_press_key_handler++;
	}
}

void release_key_handler(u8 keyCode)
{
#if (RF4CE_ZRC2_ENABLE)
	if(zrc_getIrState())
	{
		zrcStop();
	}
#endif
}

#if (MODULE_IR_LEARN_ENABLE)
void LearningStaCallBack(u8 st)
{
	if(st==IR_LEARN_START)//start learning
	{
		led_startSet(0,500*1000,1);
		app_setState(APP_LEARNING_IR_STATE);
	}
	else if(st==IR_LEARN_SUCCESS)//stop learning
	{
		led_startSet(3,100*1000,1);
		app_restoreAppState();
	}
	else if(st==IR_LEARN_FAILED)//stop learning
	{
		led_Off(1);
		app_restoreAppState();
	}
}
#endif

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

	if(rcu_appVars.battSta >= BAT_LEVEL_CUTOFF){
		rcu_appVars.flags.bf.allowedDeep = 1;
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

    if(app_isNormalState() || APP_SENDING_IR_STATE == app_getState()){
    	if(kbEvt->cnt == 2){
    		keyCode = 0xff;
    	}
    	press_key_handler(keyCode, lastKeyCode);
    	lastKeyCode = keyCode;
    	T_keyScan_keyPressedCB[0]++;

#if RF4CE_SWITCHTO_BLE_EN
    	if((key1 == VK_VOLUME_DOWN && key2 == VK_VOLUME_UP) || (key1 == VK_VOLUME_UP && key2 == VK_VOLUME_DOWN)){
    		/* switch to BLE mode */
    		app_setState(ZRC_PRE_MODE_SWITCH);
    		zrc_appVars.keyPressedTime = clock_time();
    		zrc_appVars.flags.bf.allowedDeep = 0;
    	}
#endif

    	/*
		 * check if need to do pairing
		 *
		 * */
		//if((key1 == VK_HOMEKEY && key2 == VK_MENU) || (key1 == VK_MENU && key2 == VK_HOMEKEY)){
    	if(keyCode == VK_HOMEKEY)
    	{
			/* enter Programming mode, stop repeat timer if need it */
			app_restoreState();
			app_stopSendRepeateDataTimer();
			release_key_handler(NULL);

			rcu_appVars.keyPressedTime = clock_time();
			app_setState(APP_PRE_PAIRING_STATE);
			rcu_appVars.flags.bf.allowedDeep = 0;
			return;
		}

		if(rcu_appInfo.pairingRef != RF4CE_INVALID_PAIR_REF
				&&(APP_READY_STATE == app_getState())
				&&(audio_recTaskStatusGet() != AUDIO_OPENED)){

			/*
			 * check if need to do un-pairing
			 *
			 * */
			if((key1 == VK_MENU && key2 == VK_BACK) || (key1 == VK_BACK && key2 == VK_MENU)){
				ev_on_timer(profile_doUnpair, NULL, 5*100*1000);
				return;
			}
		}
	}
#if MODULE_AUDIO_ENABLE
    else if(APP_AUDIO_STATE == app_getState()){
		if(keyCode == VK_RECORED){
#if RF4CE_MSO_ENABLE
					tl_audio_stop(PROFILE_MSO);
#elif RF4CE_ZRC2_ENABLE
					tl_audio_stop(PROFILE_ZRC2);
#endif
		}
		lastKeyCode = keyCode;
	}
#endif
    else if(APP_CHECK_VALIDATION_STATE == app_getState()){
		T_keyScan_keyPressedCB[1]++;
		if(keyCode >= VK_NUM_0 && keyCode <= VK_NUM_9){
			app_sendRFData(keyCode);
		}
	}

    if(toNormal == 1){
		app_restoreState();
		rcu_appVars.flags.bf.allowedDeep = 1;
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

	if(app_isNormalState() || APP_SENDING_IR_STATE == app_getState()) {
#if(MODULE_PM_ENABLE)
		sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif
		 /* if send repeate data timer is on, stop it. */
		rcu_appVars.flags.bf.allowedDeep = 1;
		app_restoreState();
		//zrc_stopSendRepeateData();
		app_stopSendRepeateDataTimer();
		rcu_appVars.flags.bf.repeat = 0;
		release_key_handler(keyCode);
	}
	else if((APP_PRE_PAIRING_STATE == app_getState()) || (APP_PRE_MODE_SWITCH == app_getState()))
	{
		toNormal = 1;
	}


	if(toNormal == 1)
	{
		app_restoreState();
		rcu_appVars.flags.bf.allowedDeep = 1;
	}
}

void user_keyEventHandle(void){
	static u8 valid_keyCode = 0xff;

#if(MODULE_PM_ENABLE)
	sys_suspendTimerSet(PM_SUSPEND_WAKEUP_TIME);
#endif

    sys_active_event_time = clock_time();
    if(kb_event.cnt)
    {
    	keyScan_keyPressedCB(&kb_event);
    	if(kb_event.cnt == 1){
    		valid_keyCode = kb_event.keycode[0];
    	}
    	app_validLevelForPm(0);
	}
    else
	{
		keyScan_keyReleasedCB(valid_keyCode);
		valid_keyCode = 0xff;
		app_validLevelForPm(1);
	}
}



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
 * @fn      app_lightOnProgLed
 *
 * @brief   function for handle LED status
 *
 * @param   None
 *
 * @return  None
 */
void app_lightOnProgLed(void)
{
    if ( (APP_PRE_PAIRING_STATE == app_getState() || APP_PRE_MODE_SWITCH == app_getState()) &&
         (clock_time_exceed(rcu_appVars.keyPressedTime, KEY_PRESSED_TIME_FOR_PAIRING)) ) {
    	if(APP_PRE_PAIRING_STATE == app_getState()){
			/* start pairing when released */
			led_Off(1);
			app_setState(APP_PAIRING_STATE);
			app_boundInfoSave();
			profile_doPair(NULL);
    	}else{
#if RF4CE_SWITCHTO_BLE_EN
    		rf4ceSwitchToBle();
    		/*if(app_bootFromRf4ce()){
    			rf4ceSwitchToBle();
    		}else{
    			bleSwitchToRf4ce();
    		}*/
    		app_setState(ZRC_MODE_SWITCH);
#endif
    	}
    }
}




volatile u8 T_DBG_appAudioCb[8] = {0};
extern u8 TIMER_FOR_USER;
void audio_stateSettingCb(u8 state, u8 status){
	T_DBG_appAudioCb[0]++;
	if(state == AUDIO_OPENED && status == TL_AUDIO_STA_START){
		app_setState(APP_AUDIO_STATE);
		led_On(1);
		T_DBG_appAudioCb[1]++;
	}else if(state == AUDIO_IDLE){
		app_restoreState();
		led_Off(1);
		TIMER_STOP(TIMER_FOR_USER);
		SetAudioTxState(0);
		T_DBG_appAudioCb[2]++;
	}
	T_DBG_appAudioCb[4] = state;
	T_DBG_appAudioCb[5] = status;
}




void ota_mcuReboot(void);
void app_otaStateCb(u8 status){

	if(status == OTA_STA_SUCCESS){
		/* reboot */
		ota_mcuReboot();
	}
	else
	{
		u32 blinkCnt = 3;
		led_startSet(blinkCnt, 100*1000, 1);
	}
	app_setState(APP_READY_STATE);
	u8 value = 0;
	mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
}

void app_startOta(void){
	if ( (rcu_appInfo.pairingRef != RF4CE_INVALID_PAIR_REF) &&
		(profile_getTxStatus() == 0)&&(checkBatteryBeforeSaveFlash(BAT_THRESHOLD_OTA)))
	{
		led_startSet(0,2*1000*1000,1);
		app_setState(APP_OTA_STATE);
		u8 value = 1;
		mac_mlmeSetReq(MAC_RX_ON_WHEN_IDLE, &value);
		ota_startReq(rcu_appInfo.pairingRef,app_otaStateCb);
	}else{
		rcu_appVars.flags.bf.allowedDeep = 1;
		app_restoreState();
	}
}


void led_startBind(void *arg)
{
    u32 blinkCnt = (u32)arg;
    led_startSet(blinkCnt, APP_LED_BLINK_QUICK_TIME, 1);
}
#endif  /* __PROJECT_ZRC_CLASS_DEV_APP__ */
