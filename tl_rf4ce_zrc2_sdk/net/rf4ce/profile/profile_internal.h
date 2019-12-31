#pragma once

#include "profile_attr.h"
#include "profile_api.h"

// internal use


/** @addtogroup profile_nv_id_definition Profile NV items
 * Definition for Profile NV ID
 * @{
 */
#define NV_ZRC_SUPPORT_CMD_ID                     0x00              /*!< NV id for Support cmd */
#define NV_ZRC_PIB_ID                             0x01              /*!< NV id for ZRC PIB */

#define NV_ZID_PIB_ID                             0x02              /*!< NV id for ZID PIB */
#define NV_ZID_PROXY_TBL_ID                       0x03              /*!< NV id for ZID proxy table */
#define NV_ZID_STATE_ID                           0x04              /*!< NV id for ZID state */

#define NV_MSO_RIB_ID                             0x05              /*!< NV id for MSO RIB */
#define NV_MSO_TBL_ID                             0x06              /*!< NV id for MSO table */
#define NV_MSO_ATTR_ID                            0x07              /*!< NV id for MSO attribute */
#define NV_MSO_RIB_GEN_PURPOSE_ID                 0x08              /*!< NV id for MSO attribute */

#define NV_ZRC2_TGT_RC_ID                          0x09   
#define NV_ZRC2_TGT_RIB_ID                         0x0a    
#define NV_ZRC2_RIB_ID                            0x0b 
#define NV_ZRC2_RC_RIB_ID                          0x0c   


#define NV_GDP_RC_ATTR_ID                          0x10              /*!< NV id for GDP RIB */
#define NV_GDP_TGT_ATTR_ID                         0x11              /*!< NV id for GDP table */
#define NV_GDP_POLL_ATTR_ID                        0x12              /*!< NV id for GDP attribute */

/** @} end of group profile_nv_id_definition */

typedef struct attrList_t {
	u8 profileID;
	u8 attrNum;
	attr_t *attrTable;
} attrList_t;

#define IS_INDEXED_ATTRIBUTE(id)                  ((id >=0xc0) && (id <=0x80))

#define RF4CE_GDP_PROFILE_BITMASK                 0x01
#define RF4CE_ZRC_PROFILE_BITMASK                 0x02
#define RF4CE_ZID_PROFILE_BITMASK                 0x04
#define RF4CE_MSO_PROFILE_BITMASK                 0x08
// internal use
u8 profile_processGetAttrCmd(u8 profileID, u8 *reqAttrID, u8 len, u8* pBuf, u8 *handleResult);
u8            profile_processPushAttrCmd(u8 profileID, u8* pRequestedAttr, s8 len);
profile_sts_t profile_sendNldeReq(u8 profileID, u8 pairingRef, u8 len, u8* pData, u8 txOption);


void          profile_getAttrCmdHandler(u8 profileID, u8* pData);
void          profile_pushAttrCmdHandler(u8 profileID, u8 *pData);
int           profile_doNextProfileConfiguration(void *arg);
attr_t*       profile_findAttrEntry_1(u8 profileID, u8 attrID);
void          profile_startNextProfileConfig(void);
void          profile_registerAttrList(u8 profileID,attr_t* pAttrTable, u8 attrNum);
nv_sts_t         profile_savePibToFlash(u8 id, u16 len, u8* buf);
nv_sts_t         profile_loadPibFromFlash(u8 id, u16 len, u8* buf);
u8 profile_buildPushAttrCmd(u8 profileID, u8* pRequestedAttr, u8 *attrIdList, u16 *entryId, u8 attrNum);
attrList_t* profile_findAttrList(u8 profileID);
attr_t* profile_findAttrEntry(attrList_t* pAttrList, u8 attrId);
profile_sts_t profile_interalStartPair(u8 userSepcific, u8 searchDevType, u8 keyCnt, u8 firstBind);
void profile_stopProfileConfig(void);
