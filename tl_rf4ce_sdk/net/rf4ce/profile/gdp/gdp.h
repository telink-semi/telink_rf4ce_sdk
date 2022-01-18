/********************************************************************************************************
 * @file    gdp.h
 *
 * @brief   This is the header file for gdp.h
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

/**************************************************************************************************
  Filename:       gdp.h
  Revised:        $Date: 2013-07-09 $

  Description:    GDP interface definition file
**************************************************************************************************/

#pragma once

#include "../../../../proj/tl_common.h"
#include "../profile_attr.h"
#include "../profile_api.h"

#if (RF4CE_TARGET)
#include "../../../../proj/os/ev_queue.h"
#endif


typedef enum {
    GDP_SUCC,
    GDP_UNSUPPORTED_ATTR,
    GDP_ILLGEGAL_REQ,
    GDP_INVALID_ENTRY,
    GDP_INVALID_PARAMETER,
    GDP_TIME_OUT,
    GDP_BUSY,
    GDP_NO_RESOURCES,
    GDP_ENHANCE_KEY_ERR,
} gdp_sts_t;


typedef struct {
    u16 aplGDPVersion;
    u16 aplLinkLostWaitTime;
    u32 aplGDP_rcCapabilities; 
    u16 rcValidationWaitTime; 
    u16 aplAutoCheckValidationPeriod;
    u8  keyExtCnt;
    u8  maxPairingCand;
    u8  aplIdentifyCapa;
    u8  powerSts;
    u8  aplPollConfig[9];
} gdp_rcAttr_t;

typedef struct {
    u16 aplGDPVersion;
    u16 aplLinkLostWaitTime;
    u32 aplGDP_tgtCapabilities; 
    u16 aplAutoCheckValidationPeriod;
    u16 tgtValidationWaitTime; 
    u8  powerSts;
} gdp_tgtAttr_t;

typedef struct {
    u8 id;             /*!< Attribute ID */
    u8 len;            /*!< Attribute length */
    u8 offset;           /*!< Pointer to data field */
} gdp_attr_t;


typedef struct {
	union {
        struct {
            u16 dfltSecret   :1;
            u16 initVdSpcSec :1;
            u16 respVdSpcSec :1;
            u16 reserved     :5;
			u16 vendSpcPara  :8;
        } bf;
        u16 byte;
    } flags;	
} gdp_keyExcFlags_t;

typedef struct {
	u8 randomA[8];
	u8 randomB[8];
	u8 linkKey[16];
} gdp_keyExcInfo_t;

/**
 *  @brief  ZRC class device configuration event
 */
typedef struct gdp_evItem_tags {
    struct gdp_evItem_tags *next;
    u8 *pData;
    u8 evt;
} gdp_evItem_t;


/** @brief  ZRC state machine */
typedef struct
{
    u32            curState;            /*! The ZID State in which the event handler can be used */
    u32            event;               /*! The event for which the event handler is to be invoked */
    fn_ptr        evHandlerFunc;       /*! The corresponding event handler */
} gdp_stateMachine_t;

typedef struct {
	ev_time_event_t *waitRspTimer;
	gdp_keyExcInfo_t gdp_keyInfo;
	ev_time_event_t *nextCfgTimer;
	union {
		struct {
			u8 fTxing   : 1;     /* vendor sending packet */
			u8 fUseWaitingTimer  :1;
			u8 fNotUpdate  :1;
			u8 fJustEhSec  :1;
			u8 fJustPollCfg  :1;
			u8 fromApp  :1;
			u8 reversed : 2;
		} bf;
		u8 val;
	} flags;
	u8 state;
	u8 lastCmd;
	u8 pairingRef;
} gdp_vars_t;

typedef struct {
#if (RF4CE_TARGET)
	ev_queue_t *pendingQ;
#else 
	ev_time_event_t *pollingTimer;
	u8 keyPressCnt;
#endif
	union {
		struct {
			u8 fSupportPolling :1;
			u8 fSupportIdentify:1;
			u8 waitingRsp      :1;
			u8 tryChCnt        :2;
			u8 reserved        :3;
		} bf;
		u8 val;
	} flags;	
} gdp_poll_t;


typedef struct {
	u8 pollMethodId;
	union {
		struct {
			u16 timeBasePollCap   :1;
			u16 pollOnKeyPressCap :1;
			u16 pollOnPickupCap   :1;
			u16 pollOnResetCap    :1;
			u16 pollOnMicActCap   :1;
			u16 pollOnOthersCap   :1;
			u16 reserved          :10;
		} bf;
		u16 val;
	} flags;
	u8 minPollKeyCnt;
	u8 maxPollKeyCnt;
	u32 minPollTime;
	u32 maxPollTime;
} gdp_pollRecord_t;


typedef struct {
	u8 pollMethodId;
	union {
		struct {
			u16 timeBasePollEn   :1;
			u16 pollOnKeyPressEn :1;
			u16 pollOnPickupEn   :1;
			u16 pollOnResetEn    :1;
			u16 pollOnMicActEn   :1;
			u16 pollOnOthersEn   :1;
			u16 reserved          :10;
		} bf;
		u16 val;
	} flags;
	u8 pollKeyCnt;
	u32 pollInterval;
	u8 pollTimeOut;
} gdp_pollConfig_t;


/* push attribute command format*/
typedef struct{
	u8 frmCtrl;
	u8 attrId;
	u16 entryId;
	u8 attrLen;
}gdp_pushAttrCmd_t;


#define SET_GDP_CONFIG_STATE(newState)   ( gdp_v->state = (newState) )
#define GET_GDP_CONFIG_STATE()   		( gdp_v->state )


#define SET_GDP_NORMAL_STATE(newState)   ( gdp_v->state = (newState) )
#define GET_GDP_NORMAL_STATE()   		( gdp_v->state )

#define set_gpdTxStatus()  do { gdp_v->flags.bf.fTxing = 1; } while (0)
#define get_gpdTxStatus()  ( gdp_v->flags.bf.fTxing )

typedef enum {
    GDP_CONFIG_STATE = 1,
   
    GDP_EXI_WAIT_GENERIC_RESP,  
    GDP_EXI_WAIT_GET_ATTR_RESP, 
    GDP_EXI_WAIT_PULL_ATTR_RESP,
    GDP_CC_WAIT_GENERIC_RESP,   

    GDP_EXI_WAIT_PUSH_ATTR_REQ, 
    GDP_EXI_WAIT_GET_ATTR_REQ,  
    GDP_EXI_WAIT_PULL_ATTR_REQ, 
    GDP_CC_WAIT_CONFIG_COMPLETE,
  
	GDP_AFTER_BINDING_CONFIG = 0x20,
    GDP_KEY_EXC_WAIT_CHALLENGE_REQ,
	GDP_KEY_EXC_WAIT_RESP,
    GDP_KEY_EXC_WAIT_CHALLENGE_RESP,
	GDP_KEY_EXC_WAIT_CONF,

	GDP_NEXT_PROCEDURE_1,

	GDP_PNP_WAIT_PUSH_ATTR_REQ,
	GDP_PNP_WAIT_PULL_ATTR_REQ,

	GDP_PNP_SEND_PUSH_ATTR_REQ,
    GDP_PNP_WAIT_GENERIC_RESP,
	GDP_PNP_WAIT_PULL_ATTR_RESP,

	GDP_NEXT_PROCEDURE_2,
	
	GDP_ICAP_WAIT_PUSH_ATTR_REQ,
	GDP_ICAP_WAIT_GENERIC_RESP,

	GDP_AFTER_BINDING_CONFIG_DONE,

	GDP_SENDING_POLLING_REQ,
	GDP_SENDING_PENDING_DATA,

	GDP_CHECK_VALIDATION,
	
	GDP_STA_NORMAL_PUSH_REQ,
	GDP_STA_NORMAL_PUSH_RSP,

} gpd_state_t;


/**
 *  @brief  Definition user callback function type for ZID Configuration Confirmation
 */
typedef void ( *gdp_afterBindconfig_cbFunc_t)(u8 pairingRef, u8 status);

/**
 *  @brief  Definition user callback function type for received ZID Heart Beat command
 */
typedef void ( *gdp_identify_cbFunc_t)(u8 pairingRef, u8 identifyFlags, u16 identifyTime);


/**
 *  @brief  Definition user callback function type for received Heart Beat command
 */
typedef void ( *gdp_heartBeat_RespCbFunc_t)(u8 pairingRef, u8 *pData);

/**
 *  @brief  Definition user callback function type for received push attribute command
 */
typedef void ( *gdp_pushAttrIndFunc_t)(u8 pairingRef, u8 attrId, u8 *pData);

/**
 *  @brief  Definition user callback function type for received push attribute command
 */
typedef void ( *gdp_pushAttrCnfFunc_t)(u8 pairingRef, u8 status);

/**
 *  @brief  Structure Definition for ZRC user callback functions, NULL for not use the callback
 */
typedef struct {
	gdp_afterBindconfig_cbFunc_t          	 pGdpAfterBindCfgCb;         //!< Function pointer to the received ZRC data sent confirm command
	gdp_identify_cbFunc_t   	             pGdpIdendtifyCb;            //!< Function pointer to the received ZRC validation key command
	gdp_heartBeat_RespCbFunc_t               pGdpHBRespCb;
	gdp_pushAttrIndFunc_t					 pPushAttrIndCb;
	gdp_pushAttrCnfFunc_t					 pPushAttrCnfCb;
} gdp_userCbFunc_t;


/**
 *  @brief  Definition for GDP profile attribute ID
 */
#define GDP_ATTR_aplGDPVersion                     0x80
#define GDP_ATTR_aplGDPCapabilities                0x81
#define GDP_ATTR_aplKeyExchangeTransferCount       0x82
#define GDP_ATTR_aplPowerStatus                    0x83

#define GDP_ATTR_aplPollConstraints                0x84
#define GDP_ATTR_aplPollConfiguration              0x85
#define GDP_ATTR_aplMaxPairingCandidates           0x86
#define GDP_ATTR_aplAutoCheckValidationPeriod      0x87
#define GDP_ATTR_aplTargetValidationWaitTime       0x88
#define GDP_ATTR_aplControllerValidationWaitTime   0x89
#define GDP_ATTR_aplLinkLostWaitTime               0x8a
#define GDP_ATTR_aplIdentificationCapabilities     0x8b


#define GDP_SUPPORT_EX_VALIDATION(gdpCap)        ( ((gdpCap) & 0x01) != 0 )
#define GDP_SUPPORT_POLL_SERVER(gdpCap)          ( ((gdpCap) & 0x02) != 0 )
#define GDP_SUPPORT_POLL_CLIENT(gdpCap)          ( ((gdpCap) & 0x04) != 0 )
#define GDP_SUPPORT_ID_SERVER(gdpCap)            ( ((gdpCap) & 0x08) != 0 )
#define GDP_SUPPORT_ID_CLIENT(gdpCap)            ( ((gdpCap) & 0x10) != 0 )
#define GDP_SUPPORT_ENHANCE_SECURITY(gdpCap)     ( ((gdpCap) & 0x20) != 0 )
#define GDP_SUPPORT_SHARED_LOCAL_SEC(gdpCap)     ( ((gdpCap) & 0x40) != 0 )
#define GDP_SUPPORT_SHARED_REMOTE_SEC(gdpCap)    ( ((gdpCap) & 0x80) != 0 )




u8 gdp_nldeIndHandler(u8* pData);
u8 gdp_nldeCnfHandler(u8 *pData);

gdp_sts_t gdp_sendGetAttrCmd(u8 pairingRef, u8 *attrList, u8 attrNum);
gdp_sts_t gdp_sendPushAttrCmd(u8 pairingRef, attr_t* pAttrList, u8 attrNum);
void gdp_Init(void);
void gdp_genericRespHandler(u8 *pData);
void gdp_getAttrRespHandler(u8 *pData);
void gdp_pullAttrRespHandler(u8 *pData);
void gdp_genericRespHandler(u8 *pData);
void gdp_startWaitRspTimer(void *arg);
int gdp_waitRspTimerCB(void *arg);
void gdp_stopWaitRspTimer(void);
int gdp_configurationFail(void *arg);
gdp_sts_t gdp_task(u8 evt, u8 *pData);
void gdp_pairCnfHandler(u8 status, u8 pairingRef);
void gdp_setVendorSpecInKeyExchange(u16 vendorSpecPara);
void gdp_pushAttrReqHandler(u8 *pData);
void gdp_getAttrReqHandler(u8 *pData);
void gdp_getAttrReqHandler(u8 *pData);
void gdp_getAttrReqHandler(u8 *pData);
void gdp_pullAttrReqHandler(u8 *pData);
void gdp_cfgCompleteHandler(u8* pData);
void gdp_pressKeyTriggerPolling(u8 pairingRef);

void gdp_doConfiguration(u8 *pData);
void gdp_clearBuffer(void);
u8 gdp_getTxStatus(void);
void gdp_reset(u8 clodReset);
u8 gdp_startEnhanceSecurity(u8 pairingRef);
void gdp_startPollingCfg(u8 pairingRef);
extern void zrc_rcConfigurationFail(u8 status);
extern void zrc_tgt_finishCfg(void *arg);
void gdp_keyExchangeCmdHandler(u8 *pData);
void gdp_startNextAfterBindingConfig(void *arg);
gdp_sts_t gdp_sendKeyExcChallegeReq(u8 pairingRef);
nv_sts_t gdp_loadPollConstrain(u8 pairingRef, u8 len, u8 *val);
void gdp_afterBingding(void *arg);
gdp_sts_t gdp_sendCheckValiReq(u8 pairingRef);
void gdp_savePollConstrainTask(void *arg);
void gdp_registerUserCb(gdp_userCbFunc_t* pGdpCb);

u8 aes_cmac(u8 *key, u16 msgLen, u8 *msg, u8 macLen, u8 *mac);
int gdp_afterBindConfigDone(void *arg);
void gdp_handleClientNotify(u8 *pData);
void gdp_handleHeartbeatCmd(u8 *pData);
extern void zrc_setTxStatus(void);
void gdp_stopPollingTask(void *arg);
void gdp_handleSpecialCmdInNormalState(u8 cmd, u8 *pData);
nv_sts_t gdp_saveRcAttrToFlash(void);
nv_sts_t gdp_saveTgtAttrToFlash(void);
nv_sts_t gdp_savePollVarsToFlash(void);
nv_sts_t gdp_loadRcAttrFromFlash(void);
nv_sts_t gdp_loadTgtAttrFromFlash(void);
nv_sts_t gdp_loadPollVarsFromFlash(void);
gdp_sts_t gdp_sendCheckValiResp(u8 pairingRef, u8 status);
void gdp_enterValidation(void);
void gdp_updatePollingState(void);
