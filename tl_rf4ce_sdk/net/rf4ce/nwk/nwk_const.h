/********************************************************************************************************
 * @file    nwk_const.h
 *
 * @brief   This is the header file for nwk_const.h
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
* nwk_const.h
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

/** @addtogroup nwk_channel NWK Channels
 * Definition of NWK Channels
 * @{
 */
#define RF4CE_MAC_CHANNEL_1        MAC_CHAN_15
#define RF4CE_MAC_CHANNEL_2        MAC_CHAN_20
#define RF4CE_MAC_CHANNEL_3        MAC_CHAN_25
/** @} end of group nwk_channel */

/**
 *  @brief Define the total channel in RF4CE
 */
#define RF4CE_TOTAL_MAC_CHANNELS   3



/**
 *  @brief Define the RF4CE protocol info
 */
#define RF4CE_NWK_PROTOCOL_VERSION                    0x01
#define RF4CE_NWK_PROTOCOL_IDENTIFIER                 0xce


/**
 *  @brief Define maximun node descriptor number in RF4CE
 */
#define RF4CE_MAX_NODE_DESC_LIST_NUM    3

/**
 *  @brief Define then invalid key value
 */
#define RF4CE_INVALID_KEY_VALUE         0xFF

/**
 *  @brief Define the invalid pair reference value
 */
#define RF4CE_INVALID_PAIR_REF         0xFF



/** @addtogroup nwk_rx_flags NWK RX Flags
 *  Rx Flags
 * @{
 */
#define RF4CE_RX_FLAGS_BROADCAST                    0x01            //!< Broadcast or Unicast flags
#define RF4CE_RX_FLAGS_SECURITY                     0x02            //!< Security mode flags
#define RF4CE_RX_FLAGS_VENDOR_SPECIFIC              0x04            //!< Vendor specific frame flags
/** @} end of group nwk_rx_flags */

/**
 *  @brief Define the default channel value
 */
#define NWK_BASE_CHANNEL_DEF_VALUE          0x0f


/**
 *  @brief Define the maximum number of entries supported in the node descriptors list generated through the discovery process
 */
#define RF4CE_NWK_MAX_NODE_DISC_LIST_SIZE     3

/**
 *  @brief Define the maximum duty cycle in MAC symbols, permitted for a power saving device
 */
#define RF4CE_NWK_MAX_DUTY_CYCLE            15625


/** @addtogroup nwk_default_nib NWK Default NIB Value
 *  Default NIB Value
 * @{
 */
#define NWK_ACTIVE_PERIOD_DEFAULT_VALUE                     0x00041a                          //!< The active period of a device in MAC symbols
#define NWK_BASE_CHANNEL_DEFAULT_VALUE                      15                                //!< The logical channel that was chosen during device initialization
#define NWK_DISCOVERY_LQI_THRESHOLD_DEFAULT_VALUE           0xff                              //!< The LQI threshold below which discovery requests will be rejected
#define NWK_DISCOVERY_REPETITION_INTERVAL_DEFAULT_VALUE     0x0030d4                          //!< The interval, in MAC symbols, at which discovery attempts are made on all channels
#define NWK_DUTY_CYCLE_DEFAULT_VALUE                        RF4CE_NWK_MAX_DUTY_CYCLE          //!< The duty cycle of a device in MAC symbols
#define NWK_FRAME_COUNTER_DEFAULT_VALUE                     0x00000001                        //!< The frame counter added to the transmitted NPDU
#define NWK_INDICATE_DISCOVERY_REQUESTS_DEFAULT_VALUE       FALSE                             //!< Indicates whether the NLME indicates the reception of discovery request command frames to the application
#define NWK_IN_POWER_SAVE_DEFAULT_VALUE                     FALSE                             //!< The power save mode of the node
#define NWK_MAX_DISCOVERY_REPETITIONS_DEFAULT_VALUE         0x01                              //!< The maximum number of discovery attempts made at the nwkDiscovery-RepetitionInterval rate
#define NWK_MAX_FIRST_ATTEMPT_CSMA_BACKOFFS_DEFAULT_VALUE   4                                 //!< The maximum number of backoffs the MAC CSMA-CA algorithm will attempt before declaring a channel access failure for the first transmission attempt
#define NWK_MAC_FIRST_ATTEMPT_FRAME_RETRIES_DEFAULT_VALUE   3                                 //!< The maximum number of MAC retries allowed after a transmission failure for the first transmission attempt
#define NWK_MAX_REPORTED_NODE_DESCRIPTORS_DEFAULT_VALUE     0x03                              //!< The maximum number of node descriptors that can be obtained before reporting to the application
#define NWK_RESPONSE_WAIT_TIME_DEFAULT_VALUE                0x00186a                          //!< The maximum time in MAC symbols, a device shall wait for a response command frame following a request command frame
#define NWK_SCAN_DURATION_DEFAULT_VALUE                     6                                 //!< A measure of the duration of a scanning operation
#define NWK_USER_STRING_DEFAULT_VALUE                       {'T','e','l','i','n','k',0,0,0,0,0,0,0,0,0}   //!< The user defined character string used to identify this node
#define NWK_NODE_CAPABILITIES_DEFAULT_VALUE                 0x1                               //!< The default node capabilities
#define NWK_PROFILE_ID_DEFAULT_VALUE                        0x1                               //!< The default profile ID
#define NWK_VENDOR_ID_DEFAULT_VALUE                         0x10d0                            //!< The default vendor ID
//#define NWK_VENDOR_STRING_DEFAULT_VALUE                     {0x54, 0x2E, 0x57, 0, 0, 0, 0}       //!< The default vendor string
#define NWK_VENDOR_STRING_DEFAULT_VALUE                     {'T', 'L', 0, 0, 0, 0, 0}       //!< The default vendor string
#define NWK_PAIR_TABLE_ENTRIES_DEFAULT_VALUE                5                                 //!< The default valid pair table entry size
#define NWK_MAX_TABLE_ENTRIES_DEFAULT_VALUE                 5                                 //!< The default maximun pair table entry size
/** @} end of group nwk_default_nib */

/**  @} end of group NWK_Constant */

/**  @} end of group NWK_Module */

/**  @} end of group TELINK_RF4CE_STACK */
