/********************************************************************************************************
 * @file    app_common.h
 *
 * @brief   This is the header file for app_common.h
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

#pragma  once


#define  KEY_PRESSED_TIME_FOR_QUICK_SETUP     			(5 * 1000 * 1000)
#define  KEY_PRESSED_TIME_FOR_CONFIGURE     			(2 * 1000 * 1000)
#define  KEY_PRESSED_TIME_FOR_PAIRING     				(3 * 1000 * 1000)
#define  KEY_PRESSED_TIME_FOR_FACTORY_RESET  			(5 * 1000 * 1000)
#define  KEY_PRESSED_TIME_FOR_SHOW_MODE  				(5 * 1000 * 1000)

#define  WAIT_FOR_NEXT_KEY_TIME        					(30 * 1000 * 1000)
#define  SEND_RF_REPEATE_KEY_INTERVAL  					(300 * 1000)
#define  AUTO_SEARCH_TV_INTERVAL       					(1200 * 1000)
#define  SEND_IR_REPEATE_KEY_INTERVAL  					(1000 * 1000)



//#define ZRC_KEY_PRESSED_TIME_FOR_QUICK_SETUP     		(5 * 1000 * 1000)
//#define ZRC_KEY_PRESSED_TIME_FOR_CONFIGURE     		(2 * 1000 * 1000)
//#define ZRC_KEY_PRESSED_TIME_FOR_PAIRING     			(3 * 1000 * 1000)
//#define ZRC_KEY_PRESSED_TIME_FOR_FACTORY_RESET  		(5 * 1000 * 1000)
//#define ZRC_KEY_PRESSED_TIME_FOR_SHOW_MODE  			(5 * 1000 * 1000)
//
//#define ZRC_WAIT_FOR_NEXT_KEY_TIME        			(30 * 1000 * 1000)
//#define ZRC_SEND_RF_REPEATE_KEY_INTERVAL  			(300 * 1000)
//#define ZRC_AUTO_SEARCH_TV_INTERVAL       			(1200 * 1000)
//#define ZRC_SEND_IR_REPEATE_KEY_INTERVAL  			(1000 * 1000)


/* the blink time must be times of PM_ACTIVE_SUSPEND_WAKEUP_TIME */
#define APP_LED_BLINK_QUICK_TIME           				(100*1000)
#define APP_LED_BLINK_SLOW_TIME            				(250*1000)
#define APP_LED_OTA_RUN_TIME            				(500*1000)
#define APP_LED_BLINK_SEND_DATA_TIME       				(150*1000)
#define APP_LED_BLINK_SEND_DATA_WARNING_TIME    		(50*1000)
#define APP_LED_BLINK_KEY_PRED_TIME        				(100*1000)

//#define ZRC_APP_LED_BLINK_QUICK_TIME           		(100*1000)
//#define ZRC_APP_LED_BLINK_SLOW_TIME            		(250*1000)
//#define ZRC_APP_LED_OTA_RUN_TIME            			(500*1000)
//#define ZRC_APP_LED_BLINK_SEND_DATA_TIME       		(150*1000)
//#define ZRC_APP_LED_BLINK_SEND_DATA_WARNING_TIME    	(50*1000)
//#define ZRC_APP_LED_BLINK_KEY_PRED_TIME        		(100*1000)



#define ZRC_KEY_PRESSED_INTERVAL_IN_VERIFY     (200 * 1000)

#define APP_CMD_Q_SIZE          10

#define app_getState()    		(rcu_appVars.state)
#define app_setState(newState)  (rcu_appVars.state = (newState))
u8 app_isNormalState(void);



enum{
	APP_TESTING_STATE		= 0x01,
	APP_UNCONNECT_STATE,
	APP_READY_STATE,

	APP_PRE_PAIRING_STATE	= 0x10,
	APP_PAIRING_STATE,
	APP_VERIFY_PAIR_STATE,
	APP_CHECK_VALIDATION_STATE,

	APP_SENDING_IR_STATE    = 0x20,
	APP_LEARNING_IR_STATE   = 0x21,
	APP_OTA_STATE			= 0x30,
	APP_AUDIO_STATE			= 0x40,
	APP_CONFIGURE_STATE_1,
	APP_PRE_CONFIGURE_STATE,
	APP_QUICK_SETUP_STATE,

	APP_PRE_MODE_SWITCH		= 0x50,
	APP_MODE_SWITCH			,
};

enum {
	APP_IR_MODE,
	APP_RF4CE_MODE,
};

enum {
	APP_KEY_FOR_DTA,
	APP_KEY_FOR_TV,
};

typedef enum {
	APP_NOT_USED = 0,
	APP_BATTERY_HIGH,
	APP_BATTERY_GOOD,
	APP_BATTERY_LOW,
	APP_BATTERY_CRITICAL,
} app_batteryLevel_t;

enum{
	RF_LINK_TRY,
	RF_LINK_SUCC,
	RF_LINK_FAIL,
};

typedef struct {
    ev_time_event_t *ledBlinkTimer;
    ev_time_event_t *configTimer;
    u32 keyPressedTime;
	u16 coutchCnt;
	u8 keyCode;
	u8 lastKey;
	union {
        struct {
            u8 ledStatus    :1;            
            u8 ledIdleState :1;
			u8 repeat       :1;
			u8 appInPair    :1;
			u8 ledIndex     :2;
			u8 allowedDeep  :1;
			u8 inRfRpt      :1;
			u8 sendingRib   :1;
			u8 reserved     :7;
        } bf;
        u16 byte;
    } flags;
	u8 ledStopCnt;
	u8 blinkTimes;
	u8 battSta;
	u8 battStaUpload;
	u8 state;
	u8 actionMappingIdx;
} app_Control_t;


typedef struct {
	u8 pairingRef;              //!< Indicating current light to control
	u8 state;
} app_Info_t;

typedef struct {
    u8 keyCnt;
    u8 keyIndex[3];
} app_keyInfo_t;

typedef struct {
	u8 readPtr;
	u8 writePtr;
	u8 cmdQ[APP_CMD_Q_SIZE];
} cmdQ_t;

typedef union {
	struct {
		u8 ch       : 3;
		u8 pmDeep   : 1;
		u8 battSta  : 4;
	} bf;
	u8 byte;
} pmFlag_t;

enum{
	RCU_POWER_KEY_TRY_NONE = 0,
	RCU_POWER_KEY_TRY_TV = 1,
	RCU_POWER_KEY_TRY_AUDIO = 2,
};

enum{
	RCU_POWER_TOGGLE_KEY_TV = 0,
	RCU_POWER_TOGGLE_KEY_TRY_TV = 1,
	RCU_POWER_TOGGLE_KEY_TRY_AUDIO = 2,
};

typedef struct{
	u32 nwkFrameCount;
	u8 flag;
}app_deepSleepInfo_t;



extern app_Control_t  rcu_appVars;
extern app_Info_t 	  rcu_appInfo;

extern u32 sys_active_event_time;
void app_loadInfo(void);
u8 app_isSendingIR(void);
u8 zrc_isNormalState(void);
void zrc_boundInfoSave(void);
u8 get_batterySatus (u8 chn);
u8 checkBatteryBeforeSaveFlash(u16 volThreshold);
void checkBatteryPowerOn(void);
u8 get_avgBattery(u8 chn);
void zrc_resetAll(void* arg);
void zrc_saveStackInfo(void *arg);
void zrc_saveAppInfo(void *arg);
void zrc_savePibInfo(void *arg);
void zrc_restoreAppState(void);
void app_idle_handler(void);
u8 app_validLevelForPm(u8 reset);
void audio_stateSettingCb(u8 state, u8 status);

void save_nwkpibInfo(void *arg);

void app_restoreState(void);
void led_startBind(void *arg);
void app_startOta(void);
void app_lightOnProgLed(void);
void save_appInfo(void *arg);
void user_PowerStatusPush(u8 status);
void led_startBind(void *arg);
void app_cmdQInit(cmdQ_t *q);
u8 app_cmdQGetSize(cmdQ_t *q);
u8 app_cmdQPush(cmdQ_t *q, u8 item);
u8 app_cmdQPop(cmdQ_t *q);
void voltage_detect(void);
