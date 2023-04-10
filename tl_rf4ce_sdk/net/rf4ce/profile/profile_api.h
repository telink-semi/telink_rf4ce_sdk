/********************************************************************************************************
 * @file    profile_api.h
 *
 * @brief   This is the header file for profile_api.h
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
  @headerfile:    profile.h
  $Date:          2013-07-09 $
  Interface definition for general profile constant and APIs 
**************************************************************************************************/


/*! \mainpage Telink RF4CE SDK
 *
 * \section intro_sec Introduction
 *
 * \image html rf4ce_arch.png
 */


#pragma once


/** @addtogroup  TELINK_RF4CE_STACK TELINK RF4CE Stack
 *  @{
 */

/** @addtogroup  Profile_Module RF4CE Profile
 *  @{
 */
 
/** @defgroup Profile_Constant RF4CE Profile Constants
 *  @{
 */

/** @addtogroup profile_id_definition Profile IDs
 * Definition for Profile ID
 * @{
 */
#define PROFILE_GDP                               0x00              /*!< Profile ID for GDP */
#define PROFILE_ZRC                               0x01              /*!< Profile ID for ZRC (ZigBee Remote Control) */
#define PROFILE_ZID                               0x02              /*!< Profile ID for ZID (ZigBee Input Device) */
#define PROFILE_MSO                               0xc0              /*!< Profile ID for MSO */
#define PROFILE_ZRC2                              0x03              /*!< Profile ID for ZRC2.0 */
/** @} end of group profile_id_definition */

/**
 *  @brief  Definition for Profile Number, it is auto calculated
 *          according to different configuration
 */

#define PROFILE_NUM                           0x03

/**
 *  @brief  Definition for Profile Configuration Function Number
 */

#define PROFILE_CFG_FUNC_NUM                  (PROFILE_NUM)



/** @addtogroup profile_status_definition Profile Status
 * Definition for Profile status
 * @{
 */
typedef enum {
    PROFILE_SUCC,                                 /**< Success */
    PROFILE_BUSY,                                 /**< Error for the profile is in the TX busy state */
    PROFILE_INVALID_STATE,                   /**< Invalid state for the request to profile */
    PROFILE_INVALID_DEV_TYPE,                /**< Invalid device type for the request to profile */
} profile_sts_t;

/** @} end of group profile_status_definition */


// NWK Data handle for profile of ZRC
enum {
    RF4CE_ZRC_SPECIFIC_DATA_REQ_HANDLE	=	0xA0,
    RF4CE_ZRC_STANTARD_KEYCODE_HANDLE,
    RF4CE_ZRC_STANTARD_DATA_PRESS_HANDLE,
    RF4CE_ZRC_STANTARD_DATA_REPEAT_HANDLE,
    RF4CE_ZRC_STANTARD_DATA_RELEASE_HANDLE,
    RF4CE_ZRC_PULL_ATTR_REQ_HANDLE,
    RF4CE_ZRC_PUSH_ATTR_REQ_HANDLE,
    RF4CE_ZRC_SET_ATTR_REQ_HANDLE,
    RF4CE_ZRC_GET_ATTR_REQ_HANDLE,
    RF4CE_ZRC_CFG_COMPLETE_HANDLE,

    RF4CE_ZRC_SPECIFIC_DATA_RESP_HANDLE	=	0xB0,
    RF4CE_ZRC_STANTARD_DATA_RESP_HANDLE,
    RF4CE_ZRC_PULL_ATTR_RESP_HANDLE,
    RF4CE_ZRC_PUSH_ATTR_RESP_HANDLE,
    RF4CE_ZRC_SET_ATTR_RESP_HANDLE,
    RF4CE_ZRC_GET_ATTR_RESP_HANDLE,
    RF4CE_ZRC_GENERIC_RESP_HANDLE,
};


/** @} end of group Profile_Constant */

/** @defgroup Profile_Type RF4CE Profile Types
 *  @{
 */

/**
 *  @brief  Definition for Profile Configuration Function Type
 */
typedef void ( *profile_cfgFunc_t)( u8* pData);


/** @addtogroup profile_cb_definition Profile User Callback Function Types
 * Definition for User callback function types
 * @{
 */

/**
 *  @brief  Definition user callback function type for Profile Start Confirmation
 */
typedef void ( *profile_startCnf_cbFunc_t)(u8 status);

/**
 *  @brief  Definition user callback function type for Profile Pair Confirmation
 */

typedef void ( *profile_pairCnf_cbFunc_t) (u8 pairingRef, u8 status, u8 profileSize, u8 *profileList);


/**
 *  @brief  Definition user callback function type for Profile Unpair Confirmation
 */
typedef void ( *profile_unpairCnf_cbFunc_t)(u8 pairingRef, u8 status);

/**
 *  @brief  Definition user callback function type for Profile Unpair Indication
 */
typedef void ( *profile_unpairInd_cbFunc_t)(u8 pairingRef);




/** @} end of group profile_cb_definition */


/**
 *  @brief  Structure Definition user callback functions, NULL for not use the callback
 */
typedef struct {
    profile_startCnf_cbFunc_t     pStartCnfFn;          //!< Function pointer to the start stack confirm command
    profile_pairCnf_cbFunc_t      pPairCnfFn;           //!< Function pointer to the pairing confirm command
    profile_unpairCnf_cbFunc_t    pUnpairCnfFn;         //!< Function pointer to the unpair confirm command
    profile_unpairInd_cbFunc_t    pUnpairIndFn;         //!< Function pointer to the unpair indication command
} profile_cbFunc_t;

/** @} end of group Profile_Type */




/** @defgroup profile_Public_Function Profile API
 *  @{
 */

 /**
  * @brief       Initialize the Profile module, which will also initialize the whole stack
  *              including MAC PHY NWK
  *
  * @param       None
  *
  * @return      None
  */
void  profile_init(void);

 /**
  * @brief       Reset the Profile module, which will also reset the whole stack
  *              including MAC PHY NWK
  *
  * @param       None
  *
  * @return      None
  */
void  profile_reset(u8 coldReset);

/**
  * @brief       Call this function to start the RF4CE network, for target, short address and panid
  *              will be generated in this stage. The start result can be returned in user callback 
  *              @ref profile_startCnf_cbFunc_t
  *
  * @param       None
  *
  * @return      None
  */
void  profile_startNwk(void);

/**
  * @brief       Call this function to start RF4CE pair procedure, for controller, short address and panid
  *              can be got in this stage. The pair result can be returned in user callback 
  *              @ref profile_pairCnf_cbFunc_t
  *
  * @param[in]   fUserSpecific  - boolean, 1 indicating if user string will be included, 0 indicating no user string
  * @param[in]   searchDevType  - The type of device the application is attempting to discovery @ref RF4CE_DEVICE_TYPE
  * @param[in]   keyCnt         - The key exchange transfer count. The default value is 0x24
  * @param[in]   pressKey       - The key exchange transfer count. The default value is 0x24
  *
  * @return      ERR_NONE: Success <BR>
  *              Other error code defined in @ref Error_Public_Constant <BR>
  */
profile_sts_t profile_startPair(u8 fUserSpecific, u8 searchDevType, u8 keyCnt, u8 pressKey);

/**
  * @brief       Call this function register the user callback function
  *
  * @param[in]   pUserCb - Pointer to user defined callback functions
  *
  * @return      None
  */
void  profile_registerUserCb( profile_cbFunc_t *pUserCb);


/**
  * @brief       Call this function to unpair a paired node.
  *
  * @param[in]   pairingRef  - boolean, 1 indicating if user string will be included, 0 indicating no user string
  *
  * @return      NONE
  */
void profile_unpairReq(u8 pairingRef);

/**
  * @brief       Call this function to get the profile layer tx status
  *
  * @param[in]   None
  *
  * @return      tx status
  */
u8 profile_getTxStatus(void);

/**  @} end of group profile_Public_Function */

/**  @} end of group Profile_Module */

/**  @} end of group TELINK_RF4CE_STACK */



