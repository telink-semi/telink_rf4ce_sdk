/********************************************************************************************************
 * @file    mso_api.h
 *
 * @brief   This is the header file for mso_api.h
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

/** @addtogroup  Profile_MSO MSO
 *  @{
 */
 
/** @defgroup MSO_Constant MSO Constants
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
/** @} end of group zrc_cmd_code */

 /** @addtogroup mso_cmd_code MSO Command Code
 * Definition for MSO command code
 * @{
 */
#define MSO_CMD_USER_CTRL_PRESSED                           0x01       //!< User control pressed 
#define MSO_CMD_USER_CTRL_REPEATED                          0x02       //!< User control repeated 
#define MSO_CMD_USER_CTRL_RELEASED                          0x03       //!< User control released
#define MSO_CMD_CHECK_VALIDATION_REQ                        0x20       //!< Check validation request 
#define MSO_CMD_CHECK_VALIDATION_RESP                       0x21       //!< Check validation response 
#define MSO_CMD_SET_ATTRIBUTE_REQ                           0x22       //!< Set attribute request  
#define MSO_CMD_SET_ATTRIBUTE_RESP                          0x23       //!< Set attribute response  
#define MSO_CMD_GET_ATTRIBUTE_REQ                           0x24       //!< Get attribute request 
#define MSO_CMD_GET_ATTRIBUTE_RESP                          0x25       //!< Get attribute response  

 /**  @} end of group mso_cmd_code */
 
 
 /** @addtogroup zid_error_code ZID Error Code
 * Definition for ZID Error Code
 * @{
 */
typedef enum {
    MSO_SUCC                                      = 0x00,        /**< MSO operation success */
	MSO_HAS_BOND                                  = 0x02,        /**< The MSO have binds */
	MSO_PENDING                                   = 0xC0,        /**< MSO time out */
    MSO_TIME_OUT                                  = 0xC1,        /**< MSO time out */
    MSO_COLLISION                                 = 0xC2,        /**< MSO configuration stage failure */
    MSO_FAILURE                                   = 0xC3,        /**< The MSO request is refused by the network layer */
    MSO_ABORT                                     = 0xC4,        /**< Invalid request to MSO layer */
    MSO_FULL_ABORT                                = 0xC5,        /**< MSO attribute is not found in registered attribute table */
    MSO_USER_NOT_PERMIT                           = 0x08,        /**< MSO attribute is not found in registered attribute table */
    MSO_BUSY                                      = 0x0a,        /**< MSO is under TX mode, and can't send another command before last request finish */
    MSO_INVALID_STATE                             = 0x0b,        /**< MSO is not in the proper state to do the request */
    MSO_NO_RESOURCES                              = 0x0c,        /**< MSO no resource */
	MSO_WAIT_RSP_TIMEOUT                          = 0x0d,        /**< MSO waiting response time out */	
	MSO_ATTR_NOT_COMPATIBLE                       = 0x0f,        /**< MSO attribute response is not meet user's requirement */
	MSO_ATTR_NOT_EXISTED                          = 0x10,        /**< The requested attribute is not existed */
	MSO_SEND_OUT_ERROR                            = 0x11,        /**< The requested attribute is not existed */
	MSO_USER_ABORT                                = 0x12,        /**< The requested attribute is not existed */
    MSO_INVALID_PARAM                             = 0x13,        /**< ZRC no resource */
    MSO_RESTORE                                   = 0x14,        /**< ZRC no resource */
} mso_sts_t;
 
 /**  @} end of group mso_error_code */


 /**  @} end of group MSO_Constant */

/** @defgroup MSO_Type MSO Types
 *  @{
 */
 
typedef struct {
	addrExt_t ieee;
	u8 state;
} mso_tgtRcTbl_t;


typedef	union {
	struct {
		u8 rfPredSepc : 1;
		u8 rfReptSepc : 1;
		u8 rfRelsSepc : 1;
		u8 irSepc     : 1;
		u8 reserved   : 2;
		u8 userDefault: 1;
		u8 permanent  : 1;
	} bf;
	u8 byte;
} irRf_dbFlags_t;

typedef struct {
	union {
		struct {
			u8 minTxNum : 4;
			u8 keepTx   : 1;
			u8 shrtRetry: 1;
			u8 reserved : 2;
		} bf;
		u8 byte;
	} rfConfig;
	u8 rfTxOpt;
	u8 payloadLen;
	u8 payload[1];
} irRf_dbRfDesp_t;

typedef struct {
	union {
		struct {
			u8 minTxNum : 4;
			u8 keepTx   : 1;
			u8 reserved : 1;
			u8 tweakDb  : 1;
			u8 reserved2: 1;
		} bf;
		u8 byte;
	} irConfig;
	u8 irCodeLen;
	u8 irCode[1];
} irRf_dbIrDesp_t;


typedef struct {
	u8 major;
	u8 minor;
	u8 revision;
	u8 patch;
} sw_version_t;

typedef struct {
	union {
		struct {
			u8 model      : 4;
			u8 manufacture : 4;
		} bf;
		u8 byte;
	} vendor;
	u8 hwVer;
	union {
		struct {
			u8 lotCodeHigh  : 3;
			u8 reversed : 5;
		} bf;
		u8 byte;
	} lot;
	
	u8 lotCodeLow;	
} hw_version_t;

typedef struct {
	u8 major;
	u8 minor;
	u8 revision;
	u8 patch;
} irdb_version_t;

/**
 *  @brief  MSO target attributes
 */
typedef struct
{
    u32    aplKeyRepeatWaitTime;
    u16    aplValidationWaitTime ; 
    u16    aplValidationInitialWatchdogTime;
    u8     aplUserString[9];
} mso_tgtAttrTbl_t;


/**
 *  @brief  MSO controller attributes
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
} mso_rcAttrTbl_t;

typedef struct
{
	u32    peripheralIDs;
	u8     rfStatistics[16];
	u32    versioning[3];
	u32    shortRfRetryPeriod;
	u16    validationConfig[2];
	u8     batteryStatus[11];	
} mso_ribAttr_t;

/** @addtogroup mso_cb_definition MSO User Callback Function Types
 * Definition for MSO User callback function types
 * @{
 */

/**
 *  @brief  Definition user callback function type for MSO Check Validation Confirmation
 */
typedef void ( *mso_ckValidResp_cbFunc_t)(u8 pairingRef, u8 status);

/**
 *  @brief  Definition user callback function type for received MSO Set Attribute Response command
 */
typedef void ( *mso_setAttrResp_cbFunc_t)(u8 pairingRef, u8 attrib, u8 index, u8 status);


/**
 *  @brief  Definition user callback function type for received MSO Get Attribute Response command
 */
typedef void   ( *mso_getAttrResp_cbFunc_t)(u8 pairingRef, u8 attrib, u8 index, u8 status, u8 len, u8 *val);

/**
 *  @brief  Definition user callback function type for MSO data send confirmation
 */
typedef void ( *mso_sendDataCnf_cbFunc_t)(u8 pairingRef, u8 status, u8 cmd);

/**
 *  @brief  Definition user callback function type for MSO bind status
 */
typedef void ( *mso_bindCnf_cbFunc_t)(u8 pairingRef, u8 status, u8 lqi);

/**
 *  @brief  Definition user callback function type for MSO Command Request Indication
 */
typedef void ( *mso_cmdRecv_cbFunc_t)(u8 pairingRef, u16 vendorId, u8 rxFlags, u8 lqi, u8 msoCode, u8 cmdCode, u8 cmdPayloadLen, u8 *payload);

 /**  @} end of group zid_cb_definition */

/**
 *  @brief  Definition user callback function type for MSO Command Request Indication
 */
typedef u8 ( *mso_ckValidReq_cbFunc_t)(u8 pairingRef);


/**
 *  @brief  Definition user callback function type for MSO Command Request Confirmation
 */
typedef void ( *mso_cmdSend_cbFunc_t)(u8 pairingRef, u8 status, u8 keyCode);

/**
 *  @brief  Definition user callback function type for MSO Validation Key Indication
 */
typedef void ( *mso_validationRecv_cbFunc_t)(u8 pairingRef, u16 vendorId, u8 rxFlags, u8 lqi, u8 cmdCode);

/**
 *  @brief  Definition user callback function type for received MSO Set Attribute Requeste command
 */
typedef void   ( *mso_setAttrInd_cbFunc_t)(u8 pairingRef, u8 attrib, u8 index, u8 len, u8 *val, u8 lqi);


/**
 *  @brief  Definition user callback function type for vendor specific command
 */
typedef void ( *mso_vendorSpecificDataIndCb_t)(u8 pairingRef, u8 *pData, u8 len);


 /**  @} end of group zid_cb_definition */
 
/**
 *  @brief  Structure Definition for MSO user callback functions, NULL for not use the callback
 */
typedef struct {
	mso_ckValidResp_cbFunc_t       pMsockValidRespFn;     //!< Function pointer to the MSO check validation resp command
	mso_ckValidReq_cbFunc_t        pMsockValidReqFn;      //!< Function pointer to the MSO check validation req command
	mso_setAttrResp_cbFunc_t       pMsoSetAttrRespFn;     //!< Function pointer to the received MSO set attribute resp command 
	mso_setAttrInd_cbFunc_t        pMsoSetAttrIndFn;      //!< Function pointer to the received MSO set attribute indication command 
	mso_getAttrResp_cbFunc_t       pMsoGetAttrRespFn;     //!< Function pointer to the received MSO get attribute response command
	mso_bindCnf_cbFunc_t           pMsoBindCnfFn;         //!< Function pointer to the received MSO bind confirm command
	mso_cmdRecv_cbFunc_t           pMsoCmdRecvFn;         //!< Function pointer to handle the received MSO command. Equal to command request indication.
	mso_cmdSend_cbFunc_t           pMsoCmdSendCb;         //!< Function pointer to the received MSO data sent confirm command
	mso_validationRecv_cbFunc_t    pMsoValidationKeyCb;   //!< Function pointer to the received MSO validation key command
	mso_vendorSpecificDataIndCb_t  pMsoVendSpecDataIndCb; //!< Function pointer to the received vendor specific data
} mso_userCbFunc_t;

 /**  @} end of group MSO_Type */

 
/** @defgroup MSO_FUNCTIONS ZID API
 *  @{
 */

/**
  * @brief       Call this function to register the async command callback function
  *
  * @param[in]   pZidCb - User defined command callback function set.
  *
  * @return      None
  */
void mso_registerUserCb(mso_userCbFunc_t* pMsoCb);

/**
  * @brief       Call this function to send validation code in Buttonless validation
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   txOptions - Transmission options for this frame @ref nwk_tx_option
  *
  * @param[in]   rcCommand - RC command code
  *
  * @return      Status @ref mso_status_id
  */

mso_sts_t mso_rcSendValidCodeReq(u8 pairingRef, u8 txOptions, u8 rcCommand);
	
/**
  * @brief       Call this function to transmit a MSO command to a paired device
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
  * @return      Status @ref mso_status_id
  */
mso_sts_t mso_rcSendCmdReq(u8 pairingRef, u8 txOptions, u8 cmdCode, u8 rcCommand,  u8 cmdPayloadLen, u8 *cmdPayload, bool repeat);

/**
  * @brief       Call this function to stop send repeat command
  *
  * @param[in]   None
  *
  * @return      None
  */
void mso_rcStopSendRepeatCmd(void);

/**
  * @brief       Call this function to transmit a MSO get attribute command to a paired device
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   attrId - The attribute Id
  *
  * @param[in]   index - The attribute index
  *
  * @param[in]   valLen - The attribute length
  *
  * @return      Status @ref mso_status_id
  */
mso_sts_t mso_rcSendGetAttrCmd(u8 pairingRef, u8 attrId, u8 index, u8 valLen);

/**
  * @brief       Call this function to transmit a MSO set attribute command to a paired device
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
  * @return      Status @ref mso_status_id
  */
mso_sts_t mso_rcSendSetAttrCmd(u8 pairingRef, u8 attrId, u8 index, u8 valLen, u8 *val);

/**
  * @brief       Call this function to transmit a MSO check validation request command to a paired device
  *
  * @param[in]   pairingRef - Reference into the pairing table which contains the information required to transmit the command
  *
  * @param[in]   autoVali - If the bit is set to '1 ', it indicate that the controller requests to be validated automatically
  *
  * @return      Status @ref mso_status_id
  */
mso_sts_t mso_rcSendCheckValiReq(u8 pairingRef, u8 autoVali);


#if (RF4CE_TARGET)
/**
  * @brief       Call this function to get the target side attribute list
  *
  * @param[in]   None
  *
  * @return      the pointer to target attribute list
  */
mso_tgtAttrTbl_t *mso_tgtGetAttrInfo(void);
#endif

#if (RF4CE_CONTROLLER)
/**
  * @brief       Call this function to get the remote side attibute list
  *
  * @param[in]   None
  *
  * @return      the pointer to controller attribute list
  */
mso_rcAttrTbl_t *mso_rcGetAttrInfo(void);

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
  * @return      Status @ref mso_status_id
  */
mso_sts_t mso_setAttrReq(u8 pairingRef, u8 attrId, u8 index, u8 len, u8 *value);

/**
  * @brief       Call this function to transmit a MSO get attribute command to a paired device
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
  * @return      Status @ref mso_status_id
  */
mso_sts_t mso_getAttrReq(u8 pairingRef, u8 attrId, u8 index, u8 len, u8 *value);


#if (MODULE_FLASH_ENABLE)
/**
  * @brief       Call this function to save the pair pioneer state table to flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t mso_saveTgtRcTblToFlash(void);
/**
  * @brief       Call this function to save the RIB to flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t mso_saveRibToFlash(void);
/**
  * @brief       Call this function to save the attribute to flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t mso_saveAttrToFlash(void);
/**
  * @brief       Call this function to load the RIB from flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t mso_loadRibFromFlash(void);
/**
  * @brief       Call this function to load the attribute from flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t mso_loadAttrFromFlash(void);
/**
  * @brief       Call this function to load the pair pioneer state table from flash
  *
  * @param[in]   None
  *
  * @return      Status 
  */
nv_sts_t mso_loadTgtRcTblToFlash(void);

/**
  * @brief       Call this function to set the IR-RF database entry
  *
  * @param[in]   pairingRef - which node's IR-RF database to set
  *
  * @param[in]   key - The key index
  *
  * @param[in]   len - The item length
  *
  * @param[in]   val - The item value
  *
  * @return      Status 
  */
nv_sts_t mso_setIrRfDbEntry(u8 pairingRef, u8 key, u8 len, u8 *val);
/**
  * @brief       Call this function to get the IR-RF database entry
  *
  * @param[in]   pairingRef - which node's IR-RF database to set
  *
  * @param[in]   key - The key index
  *
  * @param[in]   len - The item length
  *
  * @param[in]   val - The item value
  *
  * @return      Status 
  */
nv_sts_t mso_getIrRfDbEntry(u8 pairingRef, u8 key, u8 len, u8 *val);

/**
  * @brief       Call this function to set the general purpose entry
  *
  * @param[in]   pairingRef - which node's IR-RF database to set
  *
  * @param[in]   key - The key index
  *
  * @param[in]   val - The item value
  *
  * @return      Status 
  */
nv_sts_t mso_setGeneralPurposeEntry(u8 pairingRef, u8 index, u8 *val);

/**
  * @brief       Call this function to get the general purpose entry
  *
  * @param[in]   pairingRef - which node's IR-RF database to set
  *
  * @param[in]   key - The key index
  *
  * @param[in]   val - The item value
  *
  * @return      Status 
  */
nv_sts_t mso_getGeneralPurposeEntry(u8 pairingRef, u8 index, u8 *val);
#endif // MODULE_FLASH_ENABLE



/**  @} end of group MSO_FUNCTIONS */

/**  @} end of group Profile_MSO */

/**  @} end of group TELINK_RF4CE_STACK */
