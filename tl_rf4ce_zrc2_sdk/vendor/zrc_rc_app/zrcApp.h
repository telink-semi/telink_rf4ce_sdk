
#pragma  once


#define ZRC_KEY_PRESSED_TIME_FOR_QUICK_SETUP     (5 * 1000 * 1000)
#define ZRC_KEY_PRESSED_TIME_FOR_CONFIGURE     (2 * 1000 * 1000)
#define ZRC_KEY_PRESSED_TIME_FOR_PAIRING     (3 * 1000 * 1000)
#define ZRC_KEY_PRESSED_TIME_FOR_FACTORY_RESET  (5 * 1000 * 1000)
#define ZRC_KEY_PRESSED_TIME_FOR_SHOW_MODE  (5 * 1000 * 1000)

#define ZRC_WAIT_FOR_NEXT_KEY_TIME        (30 * 1000 * 1000)
#define ZRC_SEND_RF_REPEATE_KEY_INTERVAL  (300 * 1000)
#define ZRC_AUTO_SEARCH_TV_INTERVAL       (1200 * 1000)

#define ZRC_SEND_IR_REPEATE_KEY_INTERVAL  (1000 * 1000)

/* the blink time must be times of PM_ACTIVE_SUSPEND_WAKEUP_TIME */
#define ZRC_APP_LED_BLINK_QUICK_TIME           		(100*1000)
#define ZRC_APP_LED_BLINK_SLOW_TIME            		(250*1000)
#define ZRC_APP_LED_OTA_RUN_TIME            		(500*1000)
#define ZRC_APP_LED_BLINK_SEND_DATA_TIME       		(150*1000)
#define ZRC_APP_LED_BLINK_SEND_DATA_WARNING_TIME    (50*1000)
#define ZRC_APP_LED_BLINK_KEY_PRED_TIME        		(100*1000)

#define ZRC_KEY_PRESSED_INTERVAL_IN_VERIFY     (200 * 1000)

#define ZRC_APP_CMD_Q_SIZE          10

#define zrc_getAppState()    (zrc_appVars.state)
#define zrc_setAppState(newState) (zrc_appVars.state = (newState))
u8 zrc_isNormalState(void);



enum{
	ZRC_TESTING_STATE		= 0x01,
	ZRC_UNCONNECT_STATE,
	ZRC_READY_STATE,

	ZRC_PRE_PAIRING_STATE	= 0x10,
	ZRC_PAIRING_STATE,
	ZRC_VERIFY_PAIR_STATE,
	ZRC_CHECK_VALIDATION_STATE,

	ZRC_SENDING_IR_STATE    = 0x20,
	ZRC_LEARNING_IR_STATE    = 0x21,
	ZRC_OTA_STATE			= 0x30,
	ZRC_AUDIO_STATE			= 0x40,
	ZRC_CONFIGURE_STATE_1,
	ZRC_PRE_CONFIGURE_STATE,
	ZRC_QUICK_SETUP_STATE,

	ZRC_PRE_MODE_SWITCH		= 0x50,
	ZRC_MODE_SWITCH			,
};

enum {
	ZRC_IR_MODE,
	ZRC_RF4CE_MODE,
};

enum {
	ZRC_KEY_FOR_DTA,
	ZRC_KEY_FOR_TV,
};

typedef enum {
	ZRC_NOT_USED = 0,
	ZRC_BATTERY_HIGH,
	ZRC_BATTERY_GOOD,
	ZRC_BATTERY_LOW,
	ZRC_BATTERY_CRITICAL,
} zrc_batteryLevel_t;

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
        } bf;
        u8 byte;
    } flags;
	u8 ledStopCnt;
	u8 blinkTimes;
	u8 battSta;
	u8 battStaUpload;
	u8 state;
	u8 actionMappingIdx;
} zrc_app_t;


typedef struct {
	u8 pairingRef;              //!< Indicating current light to control
	u8 state;
} zrc_appInfo_t;

typedef struct {
    u8 keyCnt;
    u8 keyIndex[3];
} zrc_keyInfo_t;

typedef struct {
	u8 readPtr;
	u8 writePtr;
	u8 cmdQ[ZRC_APP_CMD_Q_SIZE];
} cmdQ_t;

typedef union {
	struct {
		u8 ch       : 2;
		u8 pmDeep   : 1;
		u8 battSta  : 5;
	} bf;
	u8 byte;
} pmFlag_t;

enum{
	ZRC_POWER_KEY_TRY_NONE = 0,
	ZRC_POWER_KEY_TRY_TV = 1,
	ZRC_POWER_KEY_TRY_AUDIO = 2,
};

enum{
	ZRC_POWER_TOGGLE_KEY_TV = 0,
	ZRC_POWER_TOGGLE_KEY_TRY_TV = 1,
	ZRC_POWER_TOGGLE_KEY_TRY_AUDIO = 2,
};

extern zrc_app_t zrc_appVars;
extern zrc_appInfo_t zrc_appInfo;

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
void zrc_appAudioCb(u8 state, u8 status);
