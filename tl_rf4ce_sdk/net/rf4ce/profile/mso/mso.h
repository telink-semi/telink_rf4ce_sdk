/********************************************************************************************************
 * @file    mso.h
 *
 * @brief   This is the header file for mso.h
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
#include "../profile_api.h"
#include "../profile_attr.h"
#include "mso_api.h"
#include "../../../../proj/common/types.h"

#define MSO_aplcMaxKeyRepeatInterval                     120  //ms
#define MSO_aplcMaxRIBAttributeSize                      92
#define MSO_aplcResponseIdleTime                         30
#define MSO_aplcBlackOutTime                             114  //ms
#define MSO_aplcMinKeyExchangeTransferCount              3

#define MAX_MSO_ATTR_NUM                       			 7
#define MAX_MSO_IR_RF_KEY_DATABASE_NUM         			 40


#define MSO_USE_NODE_IF_DUPLICATE              			 0
#define MSO_REMOVE_NODE_IF_DUPLICATE           			 1
#define MSO_RECLASSIFY_IF_DUPLICATE            			 2
#define MSO_ABORT_BINDING_IF_DUPLICATE         			 3

#define MSO_PRIMARY_CLASS_DESCRIPTOR           			 0
#define MSO_SECONDARY_CLASS_DESCRIPTOR         			 1
#define MSO_TERTIARY_CLASS_DESCRIPTOR          			 2



typedef struct {
	union {
        struct {
            u8 classNum         : 4;     /* vendor sending packet */
            u8 duplicateHandle  : 2;
			u8 applyStrictLqiTh : 1;
			u8 enableReqAutoVali: 1;
        } bf;
        u8 val;
    } flags; 
} mso_classDesc_t;

typedef struct {
	u8 mso_userStr[9];
	u8 reserved;
	mso_classDesc_t teriaryCD;
	mso_classDesc_t secCD;
	mso_classDesc_t priCD;
	u8 strictLqiTh;
	u8 basicLqiTh;
} mso_userStr_t;


typedef struct {
	addrExt_t ieee;
	u16 panId;
	u8 ch;
	u8 autoReq;
} mso_nodeInfo_t;

/**
 *  @brief  MSO class device configuration event
 */
typedef struct mso_evItem_t{
    struct mso_evItem_t *next;
    u8 evt;
    u8 *pData;
} mso_evItem_t;

/**
 *  @brief  Definition for attribute record format
 */
typedef struct {
    u8 id;             /*!< Attribute ID */
    u8 len;            /*!< Attribute length */
    u8 accessCtl;      /*!< Attribute access control, Get/Push/Pull/Set - bit field */
    u8 offset;           /*!< Pointer to data field */
} mso_attr_t;

/** @brief  MSO state machine */
typedef struct
{
    u8            curState;            /*! The ZID State in which the event handler can be used */
    u8            event;               /*! The event for which the event handler is to be invoked */
    fn_ptr        evHandlerFunc;       /*! The corresponding event handler */
} mso_stateMachine_t;

extern mso_userCbFunc_t *mso_userCb;
extern mso_tgtRcTbl_t *mso_tgtRcTbl;
extern mso_tgtRcTbl_t mso_tgtRcTable[];
extern mso_ribAttr_t *mso_ribAttr;
extern const u8 mso_maxGeneralPurposeCnt;
extern const u8 mso_maxKeyCnt;
extern const mso_ribAttr_t defalutRibAttrVal;

u8 mso_nldeIndHandler(u8* pData);
u8 mso_nldeCnfHandler(u8* pData);
mso_sts_t mso_sendGetAttrCmd(u8 pairingRef, u8 attrId, u8 index, u8 valLen);
mso_sts_t mso_sendPushAttrCmd(u8 pairingRef, attr_t* pAttrList, u8 attrNum);
void mso_getAttrCmdHandler(u8* pData);
u8 mso_attrVerify(u8 attrId, u8 index, u8 valLen, u8 write);
void mso_init(void);
u8 mso_getTxStatus(void);
mso_sts_t mso_sendCheckValiResp(u8 pairingRef, u8 status);
mso_sts_t mso_setAttrCmdHandler(u8 *pData);
void mso_setTxStatus(void);
void mso_pairCnfHandler(u8 status, u8 pairingRef);
void mso_doConfiguration(u8 *pData);
void mso_reset(u8 coldReset);
void mso_tgtUpdateUserString(u8 *buf, u8 *userStr, u8 tertiaryCD, u8 secCD, u8 priCD, u8 strictLqiTh, u8 basicLqiTh);
void mso_rcUpdateUserString(u8 *buf, u8 *userStr, u8 bindInd);
u8 mso_setState(u8 pairingRef, u8 state, u8 create);
u8 mso_getState(u8 pairingRef);
mso_sts_t mso_sendNldeReq(u8 pairingRef, u8 txOptions, u8 len, u8* pData);
mso_sts_t mso_sendSetAttrCmd(u8 pairingRef, u8 attrId, u8 index, u8 valLen, u8 *val);
nv_sts_t mso_loadTgtRcTblToFlash(void);
mso_sts_t mso_setAttr(u8 pairingRef, u8 attrId, u8 index, u8 len, u8 *value);
mso_sts_t mso_vendorSpecficDataSend(u8 pairingRef, u8* payload, u8 len, u8 fUseWaitingTimer);
void mso_setNeedWaitTimer(u8 needTimer);

u8 mso_tgtGetBackupStats(void);
void mso_setBackupStats(u8 status);
u8 mso_getBackupStats(void);
void mso_setBackupStats(u8 status);
void mso_backupPairEntry(void);
u8 mso_restorePairEntry(void);
u8 mso_bondWithTargetIndex(void);
nv_sts_t mso_savePairEntry(u8 len, u8 *val);
nv_sts_t mso_loadPairEntry(u8 len, u8 *val);
