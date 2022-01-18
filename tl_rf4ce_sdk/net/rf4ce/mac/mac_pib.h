/********************************************************************************************************
 * @file    mac_pib.h
 *
 * @brief   This is the header file for mac_pib.h
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
  Filename:       mac_pib.h
  Revised:        $Date: 2013-05-27 $

  Description:    Interface of MAC information base 
**************************************************************************************************/

#pragma once

#include "../../../proj/common/types.h"

/** @addtogroup  TELINK_RF4CE_STACK TELINK RF4CE Stack
 *  @{
 */

/** @addtogroup  MAC_Module MAC
 *  @{
 */
 
/** @addtogroup  MAC_Constant MAC Constants
 *  @{
 */



/** @addtogroup mac_pib_id MAC PIB
 * Standard MAC PIB Get and Set Attributes
 * @{
 */
#define MAC_ACK_WAIT_DURATION             0x40  
#define MAC_ASSOCIATION_PERMIT            0x41  
#define MAC_AUTO_REQUEST                  0x42  
#define MAC_BATT_LIFE_EXT                 0x43  
#define MAC_BATT_LIFE_EXT_PERIODS         0x44  
#define MAC_BEACON_PAYLOAD                0x45  
#define MAC_BEACON_PAYLOAD_LENGTH         0x46  
#define MAC_BEACON_ORDER                  0x47  
#define MAC_BEACON_TX_TIME                0x48  
#define MAC_BSN                           0x49  
#define MAC_COORD_EXTENDED_ADDRESS        0x4A  
#define MAC_COORD_SHORT_ADDRESS           0x4B  
#define MAC_DSN                           0x4C  
#define MAC_GTS_PERMIT                    0x4D  
#define MAC_MAX_CSMA_BACKOFFS             0x4E  
#define MAC_MIN_BE                        0x4F  
#define MAC_PAN_ID                        0x50  
#define MAC_PROMISCUOUS_MODE              0x51  
#define MAC_RX_ON_WHEN_IDLE               0x52  
#define MAC_SHORT_ADDRESS                 0x53  
#define MAC_SUPERFRAME_ORDER              0x54  
#define MAC_TRANSACTION_PERSISTENCE_TIME  0x55  
#define MAC_ASSOCIATED_PAN_COORD          0x56  
#define MAC_MAX_BE                        0x57  
#define MAC_MAX_FRAME_TOTAL_WAIT_TIME     0x58  
#define MAC_MAX_FRAME_RETRIES             0x59  
#define MAC_RESPONSE_WAIT_TIME            0x5A  
#define MAC_SYNC_SYMBOL_OFFSET            0x5B  
#define MAC_TIMESTAMP_SUPPORTED           0x5C  
#define MAC_SECURITY_ENABLED              0x5D  



#define MAC_PHY_TRANSMIT_POWER            0xE0  
#define MAC_LOGICAL_CHANNEL               0xE1  
#define MAC_EXTENDED_ADDRESS              0xE2  
#define MAC_ALT_BE                        0xE3  
#define MAC_DEVICE_BEACON_ORDER           0xE4  
#define MAC_PHY_TRANSMIT_POWER_SIGNED     0xE5  
                                                 
/** @} end of group mac_pib_id */

/** @} end of group MAC_Constant */

/** @addtogroup  MAC_TYPE MAC Types 
 *  @{
 */

typedef enum {
    MAC_PIB_NV_ID,
} mac_nv_id_t;

/**
 *  @brief Structure of MAC information base
 */
typedef struct
{
    u8             ackWaitDuration;
    bool           associationPermit;
    bool           autoRequest;
    bool           battLifeExt;
    u8             battLifeExtPeriods;

    u8             *pBeaconPayload;
    u8             beaconPayloadLength;
    u8             beaconOrder;
    u32            beaconTxTime;
    u8             bsn;

    addr_t         coordExtendedAddress;
    u16            coordShortAddress;
    u8             dsn;
    bool           gtsPermit;
    u8             maxCsmaBackoffs;

    u8             minBe;
    u16            panId;
    bool           promiscuousMode;
    bool           rxOnWhenIdle;
    u16            shortAddress;

    u8             superframeOrder;
    u16            transactionPersistenceTime;
    bool           associatedPanCoord;
    u8             maxBe;
    u16            maxFrameTotalWaitTime;

    u8             maxFrameRetries;
    u8             responseWaitTime;
    u8             syncSymbolOffset;
    bool           timeStampSupported;
    bool           securityEnabled;

    /* Proprietary */
    u8             phyTransmitPower;
    u8             logicalChannel;
    addrExt_t      extendedAddress;
    u8             altBe;
    u8             deviceBeaconOrder;

} mac_pib_t;

typedef struct
{
	u8              primitive;            //!< Primitive id of MLME Reset Confirm @ref mac_primitive_id
	u8              status;               //!< The status of the reset @ref mac_status_id
	u8				pibAttribute;
} mac_setCnf_t;

typedef struct
{
	u8              primitive;
	u8             	pibAttribute;
	u64				pibAttributeValue;
} mac_setReq_t;

typedef struct
{
	u8              primitive;
	u8             	pibAttribute;
} mac_getReq_t;

typedef struct
{
	u8              primitive;
	u8              status;
	u8				pibAttribute;
	u64				pibAttributeValue;
} mac_getCnf_t;

/**
 *  @brief MAC PIB variable for external use
 */
extern mac_pib_t macPib;


/** @} end of group MAC_TYPE */


/** @addtogroup  MAC_FUNCTIONS MAC API
 *  @{
 */


 /**
  * @brief       Reset the MAC PIB to default value
  *
  * @param       None
  *
  * @return      None
  */
void mac_pibReset(void);


 /**
  * @brief       Set an attribute value in the MAC PIB
  *
  * @param[in]   pibAttribute - The attribute identifier @ref mac_pib_id
  * @param[in]   pValue       - Pointer to the attribute value
  *
  * @return      MAC Status @ref mac_status_id
  */
mac_sts_t mac_mlmeSetReq(u8 pibAttribute, void* pValue);


 /**
  * @brief       Get an attribute value in the MAC PIB
  *
  * @param[in]   pibAttribute - The attribute identifier @ref mac_pib_id
  * @param[out]  pValue       - Pointer to the attribute value
  *
  * @return      MAC Status @ref mac_status_id
  */
mac_sts_t mac_mlmeGetReq(u8 pibAttribute, void* pValue);

/**
  * @brief       Get specified attribute length
  *
  * @param[in]   pibAttribute - The attribute identifier @ref mac_pib_id
  *
  * @return      Length of the specified attribute
  */
u8 mac_getAttrLen(u8 pibAttribute);


/**
  * @brief       Get current phy channel
  *
  * @param[in]   None
  *
  * @return      current phy channel
  */
u8 rf_getChannel(void);

/**
  * @brief       Save mac pib to flash
  *
  * @param[in]   None
  *
  * @return      status
  */
nv_sts_t mac_savePibToFlash(void);

/**
  * @brief       Load mac pib from flash
  *
  * @param[in]   None
  *
  * @return      status
  */
nv_sts_t mac_loadPibFromFlash(void);


/**  @} end of group MAC_FUNCTIONS */

/**  @} end of group MAC_Module */

/**  @} end of group TELINK_RF4CE_STACK */

