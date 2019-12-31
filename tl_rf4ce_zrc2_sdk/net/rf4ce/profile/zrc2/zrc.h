/**************************************************************************************************
  Filename:       zrc.h
  Revised:        $Date: 2014-07-02 $

  Description:    GDP interface definition file
**************************************************************************************************/

#pragma once

#include "../../../../proj/tl_common.h"
#include "../profile_api.h"
#include "../profile_attr.h"
#include "../gdp/gdp_const.h"
#include "../gdp/gdp.h"
#include "zrc_api.h"
#include "../../../../proj/common/types.h"
#include "../../../../proj/os/ev_queue.h"

//#define ZRC_aplcMaxKeyRepeatInterval                     120*1000  //ms
//#define ZRC_aplcMaxRIBAttributeSize                      92
//#define ZRC_aplcResponseIdleTime                         30
//#define ZRC_aplcBlackOutTime                             114*1000  //ms
//#define ZRC_aplcMinKeyExchangeTransferCount              3
  
#ifdef WIN32
#define ZRC2_aplcMaxConfigWaitTime                        4000  //ms
#else
#define ZRC2_aplcMaxConfigWaitTime                        100  //ms
#endif
#define ZRC2_aplcMaxActionRepeatTriggerInterval           200
#define ZRC2_aplcShortRetryDuration                       100
#define ZRC2_aplcBindWindowDuration                       15*1000  //15S
  
#define ZRC2_CHECK_BINDING_PROCEDURE_TIME                 30*1000  //30S


#define MAX_ZRC_ATTR_NUM                       7
#define MAX_ZRC_IR_RF_KEY_DATABASE_NUM         40


#define ZRC_USE_NODE_IF_DUPLICATE                 0
#define ZRC_RECLASSIFY_IF_DUPLICATE               1
#define ZRC_ABORT_BINDING_IF_DUPLICATE            2
#define ZRC_RESERVED_BINDING_IF_DUPLICATE         3


#define ZRC_CLASS_NUM_PRE_COMMISSIONED            0
#define ZRC_CLASS_NUM_BUTTON_PRESS_IND            1
#define ZRC_CLASS_NUM_BUTTON_RESERVED             2
#define ZRC_CLASS_NUM_DISCOVERABLE_ONLY           0xf
  
  
#define ZRC_PRIMARY_CLASS_DESCRIPTOR             0
#define ZRC_SECONDARY_CLASS_DESCRIPTOR           1
#define ZRC_TERTIARY_CLASS_DESCRIPTOR            2



typedef enum {
    ZRC2_DORMANT,
    ZRC2_UNBOND,
    ZRC2_BINDING,
    ZRC2_VALIDATING,
    ZRC2_VALIDATED,
    ZRC2_BOUND,
    /* if the peer node is zrc 1.0 */
    ZRC_BOUND,
} zrc_State_t;


typedef struct {
	union {
        struct {
            u8 classNum         : 4;     /* vendor sending packet */
            u8 duplicateHandle  : 2;
			u8 reserved         : 2;
        } bf;
        u8 val;
    } flags; 
} zrc_classDesc_t;

typedef struct {
    union {
        struct {
            u8 minClassNum : 4;
            u8 maxClassNum : 4;
        }bf;
        u8 val; 
    }flags; 
} zrc_classFilter_t;

typedef struct {
	u8 zrc_userStr[8];
	u8 null_byte;
	u16 reserved;
	zrc_classDesc_t teriaryCD;
	zrc_classDesc_t secCD;
	zrc_classDesc_t priCD;
	u8 LqiTh;
} zrc_discRespUserStr_t;

typedef struct {
    u8 zrc_userStr[8];
    u8 null_byte;
    u16 vendorFilter;
    zrc_classFilter_t classFilter;
    u8 minLqi;    
    u8 reserved[2];
} zrc_discReqUserStr_t;


    
typedef struct {
	addrExt_t ieee;
	u8 classNum[3];
    union {
        struct {
            u8 ch   : 3;     /* vendor sending packet */
            u8 isZRC2 : 1;
            u8 reversed : 4;
        } bf;
        u8 val;
    } flags;    
    
    u16 panId;
} zrc_nodeInfo_t;

/**
 *  @brief  ZRC class device configuration event
 */
typedef struct zrc_evItem_t{
    struct zrc_evItem_t *next;
    u8 *pData;
    u8 evt;
} zrc_evItem_t;



/** @brief  ZRC state machine */
typedef struct
{
    u16            curState;            /*! The ZID State in which the event handler can be used */
    u16            event;               /*! The event for which the event handler is to be invoked */
    fn_ptr        evHandlerFunc;       /*! The corresponding event handler */
} zrc_stateMachine_t;


typedef struct {
    u16               zrc2ProfileVersion;
    u16               zrc2ZRCActionBanksVersion;     
    u32               zrc2ProfileCapabilities;
    u8                zrc2ActionBanksSupportedTX[32];
    u8                zrc2ActionCodesSupportedTX[32];
    u16                zrc2ActionRepeatTriggerInterval;
} zrc2_rcPib_t;

typedef struct {
    u16               zrc2ProfileVersion;
    u16               zrc2ZRCActionBanksVersion;
    u32               zrc2ProfileCapabilities;
    u8                zrc2ActionBanksSupportedRX[32];
    //u8                zrc2ActionCodesSupportedRX[32];
    u16                zrc2ActionRepeatWaitTime;
} zrc2_tgtPib_t;

typedef struct {
    union {
        struct {
            u32 supportActionsOriginator          :1;
            u32 supportActionsRecipient           :1;
            u32 supportHAActionsOriginator        :1;
            u32 supportHAActionsRecipient         :1;
            u32 supportActionMappingClient        :1;
            u32 supportActionMappingServer        :1;
            u32 supportVendorSpecificIRDBFormats  :1;
            u32 informAboutSupportedActions       :1;
            u32 reserved                          :24;
        }bf;
        u32 byte32;
    } profileCapabilities;
} zrc2_profileCap_t;

/**
 *  @brief  Definition for attribute record format
 */
typedef struct {
    u8 id;             /*!< Attribute ID */
    u8 len;            /*!< Attribute length */
    u8 offset;           /*!< Pointer to data field */
} zrc_attr_t;

typedef struct {
    u8 bankHasSent;
    u8 bankNeedSent;
    u8 bankPos[1];
} bankInfo_t;

typedef struct {
	u8 pairingRef;
	u8 attrId;
	u16 entryId;
	u8 value[32];
} zrc_attrInfo_t;


extern zrc_userCbFunc_t *zrc_userCb;
//extern zrc_tgtRcTbl_t *zrc_tgtRcTbl;
extern zrc_tgtRcTbl_t zrc_tgtRcTable[];
extern zrc2_tgtPib_t zrc2_tgtRibAttrTbl[RF4CE_MAX_PAIR_ENTRY_NUM];
extern zrc2_rcPib_t zrc2_rcRibAttrTbl[RF4CE_MAX_PAIR_ENTRY_NUM];     
extern u8 zrc_maxKeyCnt;
extern bankInfo_t *actionBankInfo;
#if (RF4CE_CONTROLLER)   
extern gdp_rcAttr_t gdp_rcAttrVal;
extern gdp_tgtAttr_t gdp_tgtAttrValTbl[];
#else 
extern gdp_tgtAttr_t gdp_tgtAttrVal;
extern gdp_rcAttr_t gdp_rcAttrValTbl[];
#endif




u8 zrc2_nldeIndHandler(u8* pData);
u8 zrc2_nldeCnfHandler(u8* pData);
zrc_sts_t zrc_sendGetAttrCmd(u8 pairingRef, u8* attrIdList, u8 attrNum, u8* attrLenList,u8* indexList );
zrc_sts_t zrc_sendPushAttrCmd(u8 pairingRef, attr_t* pAttrList, u8 attrNum);

u8 zrc_attrVerify(u8 attrId, u8 index, u8 valLen, u8 write);
void zrc2_init(void);
zrc_sts_t zrc_setAttrCmdHandler(u8 *pData);
void zrc_setTxStatus(void);
void zrc2_pairCnfHandler(u8 status, u8 pairingRef);
void zrc2_doConfiguration(u8 *pData);
void zrc2_reset(u8 coldReset);

u8 zrc_setState(u8 pairingRef, u8 state, u8 create);
u8 zrc_getState(u8 pairingRef);
u8 zrc_getStateByAddr(u8 *addr);
zrc_sts_t zrc_sendNldeReq(u8 pairingRef, u8 txOptions, u8 len, u8* pData);
zrc_sts_t zrc_sendSetAttrCmd(u8 pairingRef, u8 attrId, u8 index, u8 valLen, u8 *val);
nv_sts_t zrc_loadTgtRcTblFromFlash(void);
zrc_sts_t zrc_setAttr(u8 pairingRef, u8 attrId, u8 index, u8 len, u8 *value);
zrc_sts_t zrc2_rcSendPushAttrCmd(u8 pairingRef, u8* pAttrList, u8 len, u8 fUseWaitingTimer);
zrc_sts_t zrc2_rcSendPullAttrCmd(u8 pairingRef, u8* pAttrList, u8 len, u8 fUseWaitingTimer);
bankInfo_t *zrc_getBitOnePosition(u8 *bitMap);
u8 profile_buildGetAttrCmd(u8 profileID, u8* pRequestedAttr, u8 *attrIdList, u16 *entryId, u8 attrNum);
zrc_sts_t zrc2_rcSendGetAttrCmd(u8 pairingRef, u8* attrIdList, u8 attrNum, u8 fUseWaitingTimer);
zrc_attr_t *zrc_findAttrInfo(u8 id);
bool zrc_rcIsSupportedZRC2(u8 *profileIdList, u8 num);
u8 zrc_bondWithTargetIndex(void);
nv_sts_t zrc_savePairEntry(u8 len, u8 *val);
nv_sts_t zrc_loadPairEntry(u8 len, u8 *val);
void zrc_backupPairEntry(void);
u8 zrc_restorePairEntry(void);
nv_sts_t zrc_setInfo(u8 pairingRef, u8 attrid, u8 index, u8 len, u8 *val);
nv_sts_t zrc_getInfo(u8 pairingRef, u8 attrid, u8 index, u8 len, u8 *val);
void zrc_saveInfoTask(void *arg);

_CODE_ZRC_ void zrc_pairForceStop(void);
void zrc_setNeedWaitTimer(u8 needTimer);

zrc_sts_t zrc2_vendorSpecficDataSend(u8 pairingRef, u8* payload, u8 len, u8 fUseWaitingTimer);
