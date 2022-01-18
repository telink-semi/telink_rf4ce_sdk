/********************************************************************************************************
 * @file    zrc_api.h
 *
 * @brief   This is the header file for zrc_api.h
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

#pragma once

/** @addtogroup  TELINK_RF4CE_STACK TELINK RF4CE Stack
 *  @{
 */

/** @addtogroup  Profile_ZRC ZRC
 *  @{
 */
 
/** @defgroup ZRC_Constant ZRC Constants
 *  @{
 */
 /** @addtogroup zrc_cmd_code ZRC Command Code
 * Definition for ZRC command code
 * @{
 */
#define ZRC_USER_CTRL_PRED              0x01    //!< User control pressed
#define ZRC_USER_CTRL_REPT              0x02    //!< User control repeated
#define ZRC_USER_CTRL_RELS              0x03    //!< User control released
#define ZRC_CMD_DISC_REQ                0x04    //!< Command discovery request
#define ZRC_CMD_DISC_RESP               0x05    //!< Command discovery response

//#define 

/** @} end of group zrc_cmd_code */

 /** @addtogroup zrc_cmd_code ZRC Command Code
 * Definition for ZRC command code
 * @{
 */
#define ZRC_CMD_GENERIC_RESP								           0x40
#define ZRC_CMD_CFG_COMPLETE                                       0x41 
#define ZRC_CMD_HEARBET                                                    0x42
#define ZRC_CMD_GET_ATTRIBUTE_REQ                            0x43       //!< Get attribute request
#define ZRC_CMD_GET_ATTRIBUTE_RESP                           0x44       //!< Get attribute response 
#define ZRC_CMD_PUSH_ATTRIBUTE_REQ                          0x45
#define ZRC_CMD_SET_ATTRIBUTE_REQ                             0x46       //!< Set attribute request 
#define ZRC_CMD_PULL_ATTRIBUTE_REQ                          0x47
#define ZRC_CMD_PULL_ATTRIBUTE_RESP                         0x48
#define ZRC_CMD_CHECK_VALIDATION                           0x49       //!< Check validation request  
#define ZRC_CMD_SET_ATTRIBUTE_RESP                            0x4d 

#define ZRC_CMD_GENERIC_RSP_LEN                0x02

 /**  @} end of group zrc_cmd_code */
 
 
 /** @addtogroup zid_error_code ZID Error Code
 * Definition for ZID Error Code
 * @{
 */
typedef enum {
    ZRC_SUCC                                      = 0x00,        /**< ZRC operation success */
	ZRC_PENDING                                   = 0x01,        /**< ZRC  */
    ZRC_TIME_OUT                                  = 0x02,        /**< ZRC time out */
    ZRC_FAILURE                                   = 0x03,        /**< The ZRC request is refused by the network layer */
    ZRC_ABORT                                     = 0xC4,        /**< Invalid request to ZRC layer */
    ZRC_FULL_ABORT                                = 0xC5,        /**< ZRC attribute is not found in registered attribute table */
    ZRC_USER_NOT_PERMIT                           = 0x08,        /**< ZRC attribute is not found in registered attribute table */
    ZRC_BUSY                                      = 0x0a,        /**< ZRC is under TX mode, and can't send another command before last request finish */
    ZRC_INVALID_STATE                             = 0x0b,        /**< ZRC is not in the proper state to do the request */
    ZRC_NO_RESOURCES                              = 0x0c,        /**< ZRC no resource */
    ZRC_WAIT_RSP_TIMEOUT                          = 0x0d,        /**< ZRC waiting response time out */  
    ZRC_ATTR_NOT_COMPATIBLE                       = 0x0f,        /**< ZRC attribute response is not meet user's requirement */
    ZRC_ATTR_NOT_EXISTED                          = 0x10,        /**< The requested attribute is not existed */
    ZRC_SEND_OUT_ERROR                            = 0x11,        /**< The requested attribute is not existed */
    ZRC_USER_ABORT                                = 0x12,        /**< The requested attribute is not existed */
    ZRC_INVALID_PARAM                           = 0x13,        /**< ZRC no resource */
    ZRC_RESTORE                                   = 0x14,        /**< ZRC no resource */    
}zrc_sts_t;


//ZRC2 profile attribute id
// Reserved						   						0x00-0x7f
// Reserved for GDP				   						0x80-0x9f
#define ZRC2_ATTR_aplZRCProfileVersion        				0xa0         //!< 
#define ZRC2_ATTR_aplZRCProfileCapabilities               	0xa1         //!< 
#define ZRC2_ATTR_aplActionRepeatTriggerInterval       		0xa2         //!< 
#define ZRC2_ATTR_aplActionRepeatWaitTime            		0xa3         //!< 
#define ZRC2_ATTR_aplActionBanksSupportedRX                	0xa4         //!< 
#define ZRC2_ATTR_aplActionBanksSupportedTX               	0xa5         //!< 
#define ZRC2_ATTR_aplIRDBVendorSupport         				0xa6		 //!< 
#define ZRC2_ATTR_aplZRCActionBanksVersion                  0xa7         //!< 
//Reserved for scalar profile attributes   				0xa8-0xbf
#define ZRC2_ATTR_aplActionCodesSupportedRX                 0xc0         //!< 
#define ZRC2_ATTR_aplActionCodesSupportedTX                 0xc1         //!< 
#define ZRC2_ATTR_aplMappableActions                 		0xc2         //!< 
#define ZRC2_ATTR_aplActionMappings                 		0xc3         //!< 
#define ZRC2_ATTR_aplHomeAutomation              			0xc4         //!< 
//Reserved for arrayed profile attributes          		0xc6-0xdf
//Reserved for scalar profile attributes          		0xe0-0xff
 
 
#define ZRC2_ACTION_TYPE_RESERVED                    0
#define ZRC2_ACTION_TYPE_START                       1
#define ZRC2_ACTION_TYPE_REPEAT                      2
#define ZRC2_ACTION_TYPE_ATOMIC                      3

#define ZRC2_ACTION_CMD_HEADER                       0x06

/** @defgroup ZRC_Type ZRC Types
 *  @{
 */


typedef struct {
	addrExt_t ieee;
	u8 state;
} zrc_tgtRcTbl_t;

typedef struct {
	u8 appSpecUsrString[9];
	u8 str_null;
	u8 advance_pair_supp;
	u8 enhance_secu;
	u8 reserved[3];
} zrc2_pair_reqresp_usrStr;


typedef struct {
	union {
		struct {
			u8 actionType :2;
			u8 reserved   :2;
			u8 modifier   :4;
		} bf;
		u8 byte;
	} actionControl;
	u8  actionPayloadLen;
	u8  actionBank;
	u8  actionCode;
	//u16 actionVendor;
	u8 	actionPayload[1];
} zrc_actionRecord_t;

/**
 *  @brief  ZRC target attributes
 */
typedef struct
{
    u32    aplKeyRepeatWaitTime;
    u16    aplValidationWaitTime ; 
    u16    aplValidationInitialWatchdogTime;
    u8     aplUserString[9];
} zrc_tgtAttrTbl_t;

/**
 *  @brief  ZRC controller attributes
 */
typedef struct
{
    u32    aplKeyRepeatInterval;
    u32    aplResponseWaitTime;
    u16    aplLinkLostWaitTime;
    u16    aplAutoCheckValidationPeriod;
    u8     aplKeyExchangeTransferCount;
    u8     aplMaxPairingCandidates; 
    u8     aplUserString[9];	
} zrc_rcAttrTbl_t;






typedef struct{
	u8 	rfSpecified:1;
	u8 	irSpecified:1;
	u8	rfDescriptorFirst:1;
	u8 	resved:3;
	u8	useDefault:1;
	u8	permanent:1;
}zrc2_actionMappingFlag_t;

/*
 * RF Description
 * */
typedef struct{
	u8	transNumMin:4;	//!< Minimum number of transmissions
	u8	transKeep:1;	//!< Keep transmitting until key release
	u8	retry:1;		//!< Short RF Retry
	u8	atomicAction:1;	//!Atomic Action
	u8	resved:1;
}zrc_rfMappingCfg_t;

typedef struct{
	zrc_rfMappingCfg_t	cfg;
	u8	txOpt;
	u8	length;
	u8	actionData[0];
}zrc2_actionMappingRfDesc_t;

/*
 * IR Description
 * */
typedef struct{
	u8	vendorSpec:1;	//!< Vendor Specific
	u8	resved:7;
}zrc_irMappingCfg_t;


typedef struct{
	zrc_irMappingCfg_t	cfg;
	u16	vendorId;
	u8	length;
	u8	*irCode;
}zrc2_actionMappingIrDesc_t;


typedef struct{
	zrc2_actionMappingFlag_t mappingFlag;
	u8	*rfDesc;
	u8	*irDesc;
}zrc2_actionMappingAttrEntry_t;



/** @addtogroup zrc_cb_definition ZRC User Callback Function Types
 * Definition for ZRC User callback function types
 * @{
 */

/**
 *  @brief  Definition user callback function type for ZRC Check Validation Confirmation
 */
typedef void ( *zrc_ckValidResp_cbFunc_t)(u8 pairingRef, u8 status);

/**
 *  @brief  Definition user callback function type for received ZRC Set Attribute Response command
 */
typedef void ( *zrc_setAttrResp_cbFunc_t)(u8 pairingRef, u8 attrib, u8 index, u8 status);


/**
 *  @brief  Definition user callback function type for received ZRC Get Attribute Response command
 */
typedef void   ( *zrc_getAttrResp_cbFunc_t)(u8 pairingRef, u8 attrib, u8 index, u8 status, u8 len, u8 *val);

/**
 *  @brief  Definition user callback function type for received ZRC Pull Attribute Response command
 */
typedef void   ( *zrc_pullAttrResp_cbFunc_t)(u8 pairingRef, u8 attrib, u16 entryId, u8 status, u8 len, u8 *val);

/**
 *  @brief  Definition user callback function type for received ZID Generic Response command
 */
typedef void ( *zrc_genericResp_cbFunc_t)(u8 pairingRef, u8 status);


/**
 *  @brief  Definition user callback function type for ZRC bind status
 */
typedef void ( *zrc_bindCnf_cbFunc_t)(u8 pairingRef, u8 status);

/**
 *  @brief  Definition user callback function type for ZRC Command Request Indication
 */
typedef void ( *zrc_cmdRecv_cbFunc_t)(u8 pairingRef, u16 vendorId, u8 lqi, u8 actionNum, zrc_actionRecord_t *action);

 /**  @} end of group zid_cb_definition */

/**
 *  @brief  Definition user callback function type for ZRC Command Request Indication
 */
typedef u8 ( *zrc_ckValidReq_cbFunc_t)(u8 pairingRef);


/**
 *  @brief  Definition user callback function type for ZRC Command Request Confirmation
 */
//typedef void ( *zrc_cmdSend_cbFunc_t)(u8 pairingRef, u8 status, u8 actionNum, zrc_actionRecord_t *action);
typedef void ( *zrc_cmdSend_cbFunc_t)(u8 pairingRef, u8 status, u8 handle);

/**
 *  @brief  Definition user callback function type for ZRC Validation Key Indication
 */
typedef void ( *zrc_validationRecv_cbFunc_t)(u8 pairingRef, u16 vendorId, u8 lqi, u8 actionNum, zrc_actionRecord_t *action);

/**
 *  @brief  Definition user callback function type for received ZRC Set Attribute Requeste command
 */
typedef void   ( *zrc_setAttrInd_cbFunc_t)(u8 pairingRef, u8 attrib, u8 index, u8 len, u8 *val);

/**
 *  @brief  Definition user callback function type for received Heart Beat command
 */
typedef void ( *zrc_heartBeat_RespCbFunc_t)(u8 pairingRef, u8 *pData);

/**
 *  @brief  Definition user callback function type for vendor specific command
 */
typedef void ( *zrc_vendorSpecificDataIndCb_t)(u8 pairingRef, u8 *pData, u8 len);

typedef void ( *zrc_pushAttrResp_cbFunc_t)(u8 pairingRef, u8 status, u8 rspCode);



 /**  @} end of group zid_cb_definition */
 
/**
 *  @brief  Structure Definition for ZRC user callback functions, NULL for not use the callback
 */
typedef struct {
	zrc_ckValidResp_cbFunc_t       	pZrcckValidRespFn;     //!< Function pointer to the ZRC check validation resp command
	zrc_ckValidReq_cbFunc_t        	pZrcckValidReqFn;      //!< Function pointer to the ZRC check validation req command
	zrc_setAttrResp_cbFunc_t       	pZrcSetAttrRespFn;     //!< Function pointer to the received ZRC set attribute resp command 
	zrc_getAttrResp_cbFunc_t       	pZrcGetAttrRespFn;     //!< Function pointer to the received ZRC get attribute response command
	zrc_pullAttrResp_cbFunc_t      	pZrcPullAttrRespFn;     //!< Function pointer to the received ZRC pull attribute resp command
	zrc_pushAttrResp_cbFunc_t      	pZrcPushAttrRespFn;     //!< Function pointer to the received ZRC Generic response after push attribute
	zrc_bindCnf_cbFunc_t           	pZrcBindCnfFn;         //!< Function pointer to the received ZRC bind confirm command
	zrc_cmdRecv_cbFunc_t           	pZrcCmdRecvFn;         //!< Function pointer to handle the received ZRC command. Equal to command request indication.
	zrc_cmdSend_cbFunc_t          	pZrcCmdSendCb;         //!< Function pointer to the received ZRC data sent confirm command
	zrc_validationRecv_cbFunc_t   	pZrcValidationKeyCb;   //!< Function pointer to the received ZRC validation key command
	zrc_heartBeat_RespCbFunc_t      pZrcHBRespCb;
	zrc_vendorSpecificDataIndCb_t   pZrcVendSpecDataIndCb; //!< Function pointer to the received vendor specific data
} zrc_userCbFunc_t;

 /**  @} end of group ZRC_Type */

 
/** @defgroup ZRC_FUNCTIONS ZID API
 *  @{
 */
/**
  * @brief       get the tx status of the ZRC
  * @param[in]   None
  *
  * @return      None
  */
u8 zrc_getTxStatus(void);

/**
  * @brief       Call this function to register the async command callback function
  *
  * @param[in]   pZidCb - User defined command callback function set.
  *
  * @return      None
  */
void zrc_registerUserCb(zrc_userCbFunc_t* pZrcCb);

/**
  * @brief       Call this function to send validation code in Buttonless validation
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   txOptions - Transmission options for this frame @ref nwk_tx_option
  *
  * @param[in]   rcCommand - RC command code
  *
  * @return      Status @ref zrc_status_id
  */

zrc_sts_t zrc_rcSendValidCodeReq(u8 pairingRef, u8 txOptions, zrc_actionRecord_t *rcCommand);
	
/**
  * @brief       Call this function to transmit a ZRC command to a paired device
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   vendorId - The vendor Id to put in the RF4CE data request frame
  *
  * @param[in]   txOptions - Transmission options for this frame @ref nwk_tx_option
  *
  * @param[in]   cmdCode - The ZRC command code @ref zrc_cmd_code
  *
  * @param[in]   rcCommand - RC command code
  *
  * @param[in]   cmdPayloadLen - The RC command payload length
  *
  * @param[in]   cmdPayload - cmdPayload
  *
  * @param[in]   repeat - this cmd is repeated or not
  *
  * @return      Status @ref zrc_status_id
  */
zrc_sts_t zrc_rcSendActionCmdReq(u8 pairingRef, u8 txOption, zrc_actionRecord_t *record);

/**
  * @brief       Call this function to stop send repeat command
  *
  * @param[in]   None
  *
  * @return      None
  */
zrc_sts_t zrc_rcStopSendActionCmd(u8 pairingRef, zrc_actionRecord_t *record);

/**
  * @brief       stop the send timer for repeat key
  *
  * @param[in]   None
  *
  * @return      None
  */
void zrc_timerForReleaseKeyStop(void);

/**
  * @brief       Call this function to force to stop send command
  *
  * @param[in]   pairingRef
  *
  * @return      None
  */
_CODE_ZRC_ zrc_sts_t zrc_rcForceStopSendCmd(u8 pairingRef);

/**
  * @brief       Call this function to transmit a ZRC get attribute command to a paired device
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   attrId - The attribute Id
  *
  * @param[in]   index - The attribute index
  *
  * @param[in]   valLen - The attribute length
  *
  * @return      Status @ref zrc_status_id
  */
zrc_sts_t zrc_rcSendGetAttrCmd(u8 pairingRef, u8* attrIdList, u8 attrNum, u8* attrLenList, u8* indexList);

/**
  * @brief       Call this function to transmit a ZRC set attribute command to a paired device
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   attrId - The attribute Id
  *
  * @param[in]   index - The attribute index
  *
  * @param[in]   valLen - The attribute length
  *
  * @param[in]   val - The attribute value
  *
  * @return      Status @ref zrc_status_id
  */
zrc_sts_t zrc_rcSendSetAttrCmd(u8 pairingRef, u8 attrId, u8 index, u8 valLen, u8 *val);

/**
  * @brief       Call this function to transmit a ZRC check validation request command to a paired device
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   autoVali - If the bit is set to '1 ', it indicate that the controller requests to be validated automatically
  *
  * @return      Status @ref zrc_status_id
  */
zrc_sts_t zrc_rcSendCheckValiReq(u8 pairingRef, u8 autoVali);


#if (RF4CE_TARGET)
/**
  * @brief       Call this function to get the target side attribute list
  *
  * @param[in]   None
  *
  * @return      the pointer to target attribute list
  */
zrc_tgtAttrTbl_t *zrc_tgtGetAttrInfo(void);
void zrc_tgtBindButtonPressed(void);
void zrc_tgtBindWithIV(void);
#endif

#if (RF4CE_CONTROLLER)
/**
  * @brief       Call this function to get the remote side attibute list
  *
  * @param[in]   None
  *
  * @return      the pointer to controller attribute list
  */
zrc_rcAttrTbl_t *zrc_rcGetAttrInfo(void);

#endif
/**
  * @brief       Call this function to set the attribute
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   attrId - The attribute Id
  *
  * @param[in]   index - The attribute index
  *
  * @param[in]   len - The attribute length
  *
  * @param[in]   value - The attribute value
  *
  * @return      Status @ref zrc_status_id
  */
zrc_sts_t zrc_setAttrReq(u8 pairingRef, u8 attrId, u8 index, u8 len, u8 *value);

/**
  * @brief       Call this function to transmit a ZRC get attribute command to a paired device
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   attrId - The attribute Id
  *
  * @param[in]   index - The attribute index
  *
  * @param[in]   len - The attribute length
  *
  * @param[in]   value - The attribute value
  *
  * @return      Status @ref zrc_status_id
  */
zrc_sts_t zrc_getAttrReq(u8 pairingRef, u8 attrId, u8 index, u8 len, u8 *value);


#if (MODULE_FLASH_ENABLE)
/**
  * @brief       Call this function to save the pair pioneer state table to flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t zrc_saveTgtRcTblToFlash(void);
/**
  * @brief       Call this function to save the RIB to flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t zrc_saveRibToFlash(void);
/**
  * @brief       Call this function to save the attribute to flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t zrc_saveAttrToFlash(void);
/**
  * @brief       Call this function to load the RIB from flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t zrc_loadRibFromFlash(void);
/**
  * @brief       Call this function to load the attribute from flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t zrc_loadAttrFromFlash(void);
/**
  * @brief       Call this function to load the pair pioneer state table from flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t zrc_loadTgtRcTblFromFlash(void);


#endif // MODULE_FLASH_ENABLE



/**  @} end of group ZRC_FUNCTIONS */

/**  @} end of group Profile_ZRC */

/**  @} end of group TELINK_RF4CE_STACK */
