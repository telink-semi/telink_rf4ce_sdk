/********************************************************************************************************
 * @file    nwk_pib.h
 *
 * @brief   This is the header file for nwk_pib.h
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

/******************************************************************************
* nwk_pib.h
******************************************************************************
* Program Description:
*
******************************************************************************/

#pragma once

/** @addtogroup  TELINK_RF4CE_STACK TELINK RF4CE Stack
 *  @{
 */

/** @addtogroup  NWK_Module NWK
 *  @{
 */
 
/** @addtogroup  NWK_Constant NWK Constants
 *  @{
 */



/** @addtogroup nwk_pib_id NWK PIB
 * Standard NWK PIB Get and Set Attributes
 * @{
 */
#define NWK_ACTIVE_PERIOD                       0x60         //!< The active period of a device in MAC symbols   
#define NWK_BASE_CHANNEL                        0x61         //!< The logical channel that was chosen during device initialization
#define NWK_DISCOVERY_LQI_THRESHOLD             0x62         //!< The LQI threshold below which discovery requests will be rejected      
#define NWK_DISCOVERY_REPETITION_INTERVAL       0x63         //!< The interval, in MAC symbols, at which discovery attempts are made on all channels  
#define NWK_DUTY_CYCLE                          0x64         //!< The duty cycle of a device in MAC symbols 
#define NWK_FRAME_COUNTER                       0x65         //!< The frame counter added to the transmitted NPDU
#define NWK_INDICATE_DISCOVERY_REQUESTS         0x66         //!< Indicates whether the NLME indicates the reception of discovery request command frames to the application
#define NWK_IN_POWER_SAVE                       0x67         //!< The power save mode of the node
#define NWK_PAIRING_TABLE                       0x68         //!< NWK pair table
#define NWK_MAX_DISCOVERY_REPETITIONS           0x69         //!< The maximum number of discovery attempts made at the nwkDiscovery-RepetitionInterval rate
#define NWK_MAX_FIRST_ATTEMPT_CSMA_BACKOFFS     0x6a         //!< The maximum number of backoffs the MAC CSMA-CA algorithm will attempt before declaring a channel access failure for the first transmission attempt 
#define NWK_MAX_FIRST_ATTEMPT_FRAME_RETRIES     0x6b         //!< The maximum number of MAC retries allowed after a transmission failure for the first transmission attempt
#define NWK_MAX_REPORTED_NODE_DESCRIPTORS       0x6c         //!< The maximum number of node descriptors that can be obtained before reporting to the application    
#define NWK_RESPONSE_WAIT_TIME                  0x6d         //!< The maximum time in MAC symbols, a device shall wait for a response command frame following a request command frame
#define NWK_SCAN_DURATION                       0x6e         //!< A measure of the duration of a scanning operation
#define NWK_USER_STRING                         0x6f         //!< The user defined character string used to identify this node
#define NWK_NODE_CAPABILITIES                   0x70         //!< The node capabilities
#define NWK_PROFILE_IDENTIFIER                  0x71         //!< The profile ID    
#define NWK_VENDOR_IDENTIFIER                   0x72         //!< The vendor ID 
#define NWK_VENDOR_STRING                       0x73         //!< The vendor string
#define NWK_MAX_PAIR_ENTRY_NUM                  0x74         //!< The maximun pair table entry size

/** @} end of group nwk_pib_id */

/** @} end of group NWK_Constant */
  
/** @addtogroup  NWK_TYPE NWK Types
 *  @{
 */

/**
 *  @brief Structure of NWK information base
 */

typedef struct {
    u32    activePeriod;                            /*!< The active period of a device in MAC symbols */        
    u32    discoveryRepetitionInterval;             /*!< The interval, in MAC symbols, at which discovery attempts are made on all channels */                       
    u32    dutyCycle;                               /*!< The duty cycle of a device in MAC symbols */     
    u32    frameCounter;                            /*!< The frame counter added to the transmitted NPDU */        
    u32    responseWaitTime;                        /*!< The maximum time in MAC symbols, a device shall wait for a response command frame following a request command frame */            
	
    u8     baseChannel;                             /*!< The logical channel that was chosen during device initialization */       
    u8     discoveryLQIThreshold;                   /*!< The LQI threshold below which discovery requests will be rejected */                 
    u8     indicateDiscoveryRequests;               /*!< Indicates whether the NLME indicates the reception of discovery request command frames to the application */                     
    u8     inPowerSave;                             /*!< The power save mode of the node */       
	
    u8     maxDiscoveryRepetitions;                 /*!< The maximum number of discovery attempts made at the nwkDiscovery-RepetitionInterval rate */                   
    u8     maxFirstAttemptCSMABackoffs;             /*!< The maximum number of backoffs the MAC CSMA-CA algorithm will attempt before declaring a channel access failure for the first transmission attempt */                       
    u8     maxFirstAttemptFrameRetries;             /*!< The maximum number of MAC retries allowed after a transmission failure for the first transmission attempt */                       
    u8     maxReportedNodeDescriptors;              /*!< The maximum number of node descriptors that can be obtained before reporting to the application */                      
	
    u8     scanDuration;                            /*!< A measure of the duration of a scanning operation */        
    u8     nodeCapabilities;                        /*!< The node capabilities */             
    u16    vendorIdentifier;                        /*!< The vendor ID */             
	
    u8     userString[15];                          /*!< The user defined character string used to identify this node */          
    u8     profileIdentifier;                       /*!< The profile ID */             
	
    u8     vendorString[7];                         /*!< The vendor string */             
    u8     maxPairEntryNum;                         /*!< The maximun pair table entry size */               
} nwk_nib_t;


/** @} end of group NWK_TYPE */


/** @addtogroup  NWK_FUNCTIONS NWK API
 *  @{
 */

 /**
  * @brief       Reset the NWK PIB to default value
  *
  * @param       None
  *
  * @return      None
  */
void nwk_pibReset(void);

 /**
  * @brief       Call this function to set NWK layer pib value
  *
  * @param[in]   attr - The identifier of the NIB attribute to write @ref nwk_pib_id
  *
  * @param[in]   attrIndex - The index within the table or array of the specified NIB attribute to write
  *                          This parameter is valid only for NIB attributes that are tables or arrays
  *
  * @param[in]   value - The value of the indicated attribute to write
  *
  * @return      None
  */
u8 nwk_nlmeSetReq(u8 attr, u8 attrIndex, u8 *value);

 /**
  * @brief       Call this function to get NWK layer pib value
  *
  * @param[in]   attr - The identifier of the NIB attribute to read @ref nwk_pib_id
  *
  * @param[in]   attrIndex - The index within the table or array of the specified NIB attribute to read. 
  *                          This parameter is valid only for NIB attributes that are tables or arrays
  *
  * @param[in]   value - The value of the indicated attribute to write
  *
  * @return      None
  */
u8 nwk_nlmeGetReq(u8 attr, u8 attrIndex, u8 *value);

/**  @} end of group NWK_FUNCTIONS */

/**  @} end of group NWK_Module */

/**  @} end of group TELINK_RF4CE_STACK */
