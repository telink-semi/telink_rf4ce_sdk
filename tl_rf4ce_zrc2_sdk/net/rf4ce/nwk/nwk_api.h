/******************************************************************************
* nwk_api.h
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


/** @addtogroup nwk_primitive_id NWK primitive ID
 * Primitive ID of NWK request/response/indication/confirmation
 * @{
 */

#define RF4CE_NLDE_DATA_REQ                   0x70             //!< NLDE Data Request
#define RF4CE_NLDE_DATA_CONF                  0x71             //!< NLDE Data Confirm
#define RF4CE_NLDE_DATA_IND                   0x72             //!< NLDE Data Indication
#define RF4CE_NLME_START_REQ                  0x73             //!< NLME Start Request
#define RF4CE_NLME_START_CONF                 0x74             //!< NLME Start Confirm
#define RF4CE_NLME_PAIR_REQ                   0x75             //!< NLME Pair Request
#define RF4CE_NLME_PAIR_CONF                  0x76             //!< NLME Pair Confirm
#define RF4CE_NLME_PAIR_IND                   0x77             //!< NLME Pair Indication
#define RF4CE_NLME_PAIR_RESP                  0x78             //!< NLME Pair Response
#define RF4CE_NLME_UNPAIR_REQ                 0x79             //!< NLME Unpair Request
#define RF4CE_NLME_UNPAIR_CONF                0x7a             //!< NLME Unpair Confirm
#define RF4CE_NLME_UNPAIR_IND                 0x7b             //!< NLME Unpair Indication
#define RF4CE_NLME_UNPAIR_RESP                0x7c             //!< NLME Unpair Response
#define RF4CE_NLME_GET_REQ                    0x7d             //!< NLME Get Request
#define RF4CE_NLME_GET_CONF                   0x7e             //!< NLME Get Confirm
#define RF4CE_NLME_SET_REQ                    0x7f             //!< NLME Set Request
#define RF4CE_NLME_SET_CONF                   0x80             //!< NLME Set Confirm
#define RF4CE_NLME_DISCOVERY_REQ              0x81             //!< NLME Discovery Request
#define RF4CE_NLME_DISCOVERY_CONF             0x82             //!< NLME Discovery Confirm
#define RF4CE_NLME_DISCOVERY_IND              0x83             //!< NLME Discovery Indication
#define RF4CE_NLME_DISCOVERY_RESP             0x84             //!< NLME Discovery Response
#define RF4CE_NLME_AUTO_DISCOVERY_REQ         0x85             //!< NLME Auto Discovery Request
#define RF4CE_NLME_AUTO_DISCOVERY_CONF        0x86             //!< NLME Auto Discovery Confirm
#define RF4CE_NLME_RESET_REQ                  0x87             //!< NLME Reset Request
#define RF4CE_NLME_RESET_CONF                 0x88             //!< NLME Reset Confirm
#define RF4CE_NLME_COMM_STATUS_IND            0x89             //!< NLME Comm Status Indication
#define RF4CE_NLME_RX_ENABLE_REQ              0x8a             //!< NLME Rx Enable Request
#define RF4CE_NLME_RX_ENABLE_CONF             0x8b             //!< NLME Rx Enable Confirm
#define RF4CE_NLME_UPDATE_KEY_REQ             0x8c             //!< NLME Update Key Request
#define RF4CE_NLME_UPDATE_KEY_CONF            0x8d             //!< NLME Update Key Confirm
#define RF4CE_NLME_PING_REQ                   0x8e             //!< NLME Ping Request
#define RF4CE_NLME_PING_RESP                  0x8f             //!< NLME Ping Response

/** @} end of group nwk_primitive_id */


/** @addtogroup nwk_status_id NWK Status Code
 * Definition of NWK layer status
 * @{
 */
#define RF4CE_NWK_SUCCESS                     0x00            //!< Operation successful
#define RF4CE_NWK_NO_ORG_CAPACITY             0xb0            //!< The originator's pairing table is full
#define RF4CE_NWK_NO_REC_CAPACITY             0xb1            //!< The recipient's pairing table is full
#define RF4CE_NWK_NO_PAIRING                  0xb2            //!< A pairing table entry could not be found
#define RF4CE_NWK_NO_RESPONSE                 0xb3            //!< A response frame was not received within nwkResponseWaitTime.
#define RF4CE_NWK_NOT_PERMITTED               0xb4            //!< A pairing request was denied
#define RF4CE_NWK_DUPLICATE_PAIRING           0xb5            //!< A duplicate pairing table entry was detected
#define RF4CE_NWK_FRAME_COUNTER_EXPIRED       0xb6            //!< The frame counter has reached its maximum value
#define RF4CE_NWK_DISCOVERY_ERROR             0xb7            //!< Too many unique matched discovery request or valid response command frames were received than requested
#define RF4CE_NWK_DISCOVERY_TIMEOUT           0xb8            //!< No discovery request or response command frames were received during discovery
#define RF4CE_NWK_SECURITY_TIMEOUT            0xb9            //!< The security link key exchange or recovery procedure did not complete within the required time
#define RF4CE_NWK_SECURITY_FAILURE            0xba            //!< A security link key was not successfully established between both ends of a pairing link
#define RF4CE_NWK_INVALID_PARAMETER           0xe8            //!< A parameter in the primitive is either not supported or is out of the valid range
#define RF4CE_NWK_UNSUPPORTED_ATTRIBUTE       0xf4            //!< A SET/GET request was issued with the identifier of a NIB attribute that is not supported
#define RF4CE_NWK_INVALID_INDEX               0xf9            //!< The specified table index was out of range

#define RF4CE_NWK_DISCOVERY_ABORT             0xc0            //!< Abort the discovery procedure by user
#define RF4CE_NWK_NO_BUFFER                   0xc1            //!< There is no buffer during allocate in NWK layer
#define RF4CE_NWK_INVALID_REQUEST             0xc2            //!< The invalid request since no security key is established. 
#define RF4CE_NWK_BAD_CCM_OUTPUT              0xce            //!< The encryption/decryption is error

/** @} end of group nwk_status_id */

/** @addtogroup nwk_tx_option NWK TX Options
 * Definition of NWK TX Option bit mask
 * @{
 */
#define  NWK_TXOPTION_BROADCAST               0x01            //!< Bit mask for broadcast transmission
#define  NWK_TXOPTION_IEEEADDR                0x02            //!< Bit mask for use destination IEEE address
#define  NWK_TXOPTION_ACK                     0x04            //!< Bit mask for acknowledged transmission
#define  NWK_TXOPTION_SECURITY                0x08            //!< Bit mask for transmit with security
#define  NWK_TXOPTION_SINGLE_CHANNEL          0x10            //!< Bit mask for use single channel operation
#define  NWK_TXOPTION_CHANNEL_NORMALIZATION   0x20            //!< Bit mask for specify channel designator
#define  NWK_TXOPTION_VENDOR_SPECIFIC         0x40            //!< Bit mask for data is vendor-specific

 /** @} end of group nwk_tx_option */

/**        
 *  @brief RF4CE max device type number 
 */
#define RF4CE_MAX_NUM_DEV_TYPES         3

/**        
 *  @brief RF4CE max profile ID number
 */
#define RF4CE_MAX_NUM_PROFILE_IDS       7

/**        
 *  @brief Length of reserved bytes for MAC
 */
#define RESERVED_MAC_BYTES_LEN         45  
 
/** @} end of group NWK_Constant */


/** @addtogroup  NWK_TYPE NWK Types
 *  @{
 */

 /**
 *  @brief  Definition for NWK layer callback function type
 */
typedef void ( *nwk_cb_t )( u8* pData );    
 


/**
 *  @brief Definition for pair table entry 
 */
typedef struct {   
    u8 destIEEEAddr[8];         //!< The IEEE address of the destination device
    u16 destNwkAddr;            //!< The network address of the destination device 
    u16 srcAddr;                //!< The network address to be assumed by the source device 
    u16 destPanId;              //!< The PAN identifier of the destination device 
    u16 destVendorId;              //!< The vendorId identifier of the destination device
    u8 linkKey[16];             //!< A valid 128-bit key The link key to be used to secure this pairing link
    u32 destFrameCounter;       //!< The frame counter last received from the recipient node
    u8 destChannel;             //!< The expected channel of the destination device
    u8 destNodeCap;             //!< The node capabilities of the recipient node 
} pairTable_t; 


/**
 *  @brief Definition for application capability
 */
typedef struct {
    union {
        struct {
            u8 userStrSpecified : 1;                   //!< User string specified 
            u8 numDevType       : 2;                   //!< Number of supported device types 
            u8 reserved1        : 1;                   //!< Reserved bytes 
            u8 numProfile       : 3;                   //!< Number of supported profiles 
            u8 reserved2        : 1;                   //!< Reserved bytes 
        } bf;
        u8 val;        
    } appCap;
    u8 userStr[15];                                    //!< The user defined identification string of the responding node 
    u8 devTypeList[RF4CE_MAX_NUM_DEV_TYPES];           //!< The list of device types supported by the responding node 
    u8 profileIdList[RF4CE_MAX_NUM_PROFILE_IDS];       //!< The list of profile identifiers supported by the responding node 
} nwk_appInfo_t; 



/**
 *  @brief Definition for node descriptor
 */
typedef struct {
    u8 status;                                         //!< The status of the discovery request as reported by the responding device 
    u8 logicalChannel;                                 //!< The logical channel of the responding device 
    u16 panId;                                         //!< The PAN identifier of the responding device 
    u8 IEEEAddr[8];                                    //!< The IEEE address of the responding device 
    u8 nodeCapabilities;                               //!< The capabilities of the responding node 
    u16 vendorId;                                      //!< The vendor identifier of the responding node 
    u8 vendorString[7];                                //!< The vendor string of the responding node 
    nwk_appInfo_t appInfo;                             //!< Application parameters 
    u8 discReqLQI;                                     //!< The LQI of the discovery request command frame reported by the responding device 
} nwk_nodeDesc_t; 


/**
 *  @brief Definition for NLDE-Data.reqeust txOption
 */
typedef union {
    struct {
        u8      transMode           : 1;  //!< 0x01 for broadcast transmission, 0x00 for unicast transmission 
        u8      dstAddrMode         : 1;  //!< 0x01 for use destination IEEE address, 0x00 for use destination network address 
        u8      ackMode             : 1;  //!< 0x01 for acknowledged transmission, 0x00 for unacknowledged transmission 
        u8      securityMode        : 1;  //!< 0x01 for transmit with security, 0x00 for transmit without security 
        u8      agilityMode         : 1;  //!< 0x01 for use single channel operation, 0x00 for use multiple channel operation 
        u8      chNormalizationMode : 1;  //!< 0x01 for specify channel designator, 0x00 for do not specify channel designator 
        u8      payloadMode         : 1;  //!< 0x01 for data is vendor-specific, 0x00 for data is not vendor-specific 
    } bf;
    u8 byte;
} txOption_t;


/** @brief Defines a union for Destination/Source/Coordinator Address */
typedef union {    
    u16  shortAddr;                     //!< Short address     
    u8  ieeeAddr[8];                    //!< Extended address 
}Addr_t; 


/**
 *  @brief  Define Data request parameters type
 */    
typedef struct {
    u8     primitive;                                 //!< Primitive id of NLDE Data Request @ref nwk_primitive_id   
    u8     reserved[RESERVED_MAC_BYTES_LEN];          //!< Reserved bytes for MAC layer    
    u8     pairingRef;                                //!< Reference into the pairing table which contains the information required to transmit the NPDU. It is ignored if in a broadcast transmission 
    u8     profileId;                                 //!< The identifier of the profile indicating the format of the transmitted data @ref profile_id_definition 
    u16    vendorId;                                  //!< If the TxOptions parameter specifies that the data is vendor specific, it specifies the vendor identifier. Otherwise, it's ignored     
    u8     nsduLength;                                //!< The number of octets contained in the NPDU to be transmitted by the NLDE      
    u8     nsduHandle;                                //!< The handle associated with the NLDE to be transmitted       
    txOption_t   txOptions;                           //!< The transmission options for the NLDE        
    u8     nsdu[1];                                   //!< The set of octets forming the NPDU to be transmitted by the NLDE       
} nwk_dataReq_t;

/**
 *  @brief  Define Data indication parameters type
 */
typedef struct {
    u8     primitive;                                //!< Primitive id of NLDE Data Request @ref nwk_primitive_id              
    u8     pairingRef;                               //!< Reference into the pairing table which matched the information contained in the received NPDU                 
    u8     profileId;                                //!< The identifier of the profile indicating the format of the received data       
    u16    vendorId;                                 //!< If the RxFlags parameter specifies that the data is vendor specific, it specifies the vendor identifier. Otherwise, it's ignored       
    u8     nsduLength;                               //!< The number of octets contained in the NSDU received by the NLDE     
    u8     rxLinkQuality;                            //!< LQI value measured during the reception of the NPDU.  Lower values represent lower LQI       
    u8     rxFlags;                                  //!< Reception indication flags for this NSDU @ref nwk_rx_flags    
    u8     *nsdu;                                    //!< The set of octets forming the NSDU received by the NLDE 
} nwk_dataInd_t;


/**
 *  @brief  Define Data confirm parameters type
 */
typedef struct {
   u8 primitive;                                   //!< Primitive id of NLDE Data Request @ref nwk_primitive_id      
   u8 nsduhandle;                                  //!< The handle associated with the NPDU being confirmed      
   u8 pairingRef;                                  //!< Reference into the pairing table which matched the information contained in the received NPDU       
   u8 status;                                      //!< The status of the last NSDU transmission   
} nwk_dataCnf_t;                                      

/**
 *  @brief  Define Auto discovery request parameters type
 */
typedef struct {
    u8 primitive;                                 //!< Primitive id of NLDE Data Request @ref nwk_primitive_id 
    nwk_appInfo_t recAppCapa;                     //!< The application info parameter             
    u32 autoDiscDuration;                         //!< The maximum number of MAC symbols NLME will be in auto discovery response mode         
} nwk_autoDiscReq_t;
    
/**
 *  @brief  Define Rx on enable request parameters type
 */
typedef struct {
    u8 primitive;                                 //!< Primitive id of NLME Request @ref nwk_primitive_id 
    u32 rxOnDuration;                             //!< The maximum number of MAC symbols in rx On.          
} nwk_rxOnEnableReq_t;


/**
 *  @brief  Define Rx on enable request parameters type
 */
typedef struct {
    u8 primitive;                                 //!< Primitive id of NLME Request @ref nwk_primitive_id 
    u8 status;                                    //!< The status of rx on enable primitvie.          
} nwk_rxOnEnableCnf_t;
        
/**
 *  @brief  Define Auto discovery confirm parameters type
 */
typedef struct {
    u8 primitive;                                 //!< Primitive id of NLDE Data Request @ref nwk_primitive_id    
    u8 status;                                    //!< The status of the auto discovery response mode  
    u8 srcIEEEAddr[8];                            //!< The IEEE address to which the discovery response was sent          
} nwk_autoDiscCnf_t;



/**
 *  @brief  Define Discovery request parameters type
 */
typedef struct {
    u8 primitive;                                       //!< Primitive id of NLDE Data Request @ref nwk_primitive_id                     
    u8 reserved[RESERVED_MAC_BYTES_LEN];                //!< Reserved bytes for MAC layer                                            
    u16 dstPanId;                                       //!< The PAN identifier of the destination device for the discovery                     
    u16 dstNwkAddr;                                     //!< The address of the destination device for the discovery                       
    u8 searchDevType;                                   //!< The device type to discover. This value can be set to 0xff to indicate a wildcard                        
    u8 discProfileIdListSize;                           //!< The number of profile identifiers contained in the DiscProfileIdList parameter                                 
    u8 discProfileIdList[RF4CE_MAX_NUM_PROFILE_IDS];    //!< The list of profile identifiers                                                        
    u32 discDuration;                                   //!< The maximum number of MAC symbols to wait for discovery responses on each channel                          
    nwk_appInfo_t orgAppCap;                            //!< The application info parameter                                 
} nwk_discReq_t;


/**
 *  @brief  Define Discovery indication parameters type
 */
typedef struct {
    u8 primitive;                               //!< Primitive id of NLDE Data Request @ref nwk_primitive_id      
    u8 status;                                  //!< The status of the pairing table       
    u8 srcIEEEAddr[8];                          //!< The IEEE address of the device requesting the discovery               
    u8 orgNodeCap;                              //!< The capabilities of the originator of the discovery request           
    u16 orgVendorId;                            //!< The vendor identifier of the originator of the discovery request          
    u8 orgVendorStr[7];                         //!< The vendor string of the originator of the discovery request               
    nwk_appInfo_t orgAppCap;                    //!< The application info parameter                   
    u8 searchDevType;                           //!< The device type being discovered           
    u8 rxLinkQuality;                           //!< LQI value          
} nwk_discInd_t; 


/**
 *  @brief  Define Discovery response parameters type
 */
typedef struct {
    u8 primitive;                              //!< Primitive id of NLDE Data Request @ref nwk_primitive_id     
    u8 reserved[RESERVED_MAC_BYTES_LEN];       //!< Reserved bytes for MAC layer                           
    u8 status;                                 //!< The status of the discovery request 
    u8 dstIEEEAddr[8];                         //!< The IEEE address of the device requesting discovery          
    nwk_appInfo_t recAppCap;                   //!< The application info parameter                
    u8 discReqLQI;                             //!< The LQI value from the associated NLME-DISCOVERY.indication primitive     
} nwk_discResp_t; 

/**
 *  @brief  Define Data request parameters type
 */
typedef struct {
    u8 primitive;                                            //!< Primitive id of NLDE Data Request @ref nwk_primitive_id 
    u8 status;                                               //!< The status of the network discovery attempt 
    u8 numNodes;                                             //!< The number of discovered nodes in the NodeDescList parameter  
    nwk_nodeDesc_t nodeDesc[RF4CE_MAX_NODE_DESC_LIST_NUM];   //!< The list of node descriptors discovered                                              
} nwk_discCnf_t; 



/**
 *  @brief  Define Pair request parameters type
 */
typedef struct {
    u8 primitive;                             //!< Primitive id of NLDE Data Request @ref nwk_primitive_id  
    u8 reserved[RESERVED_MAC_BYTES_LEN];      //!< Reserved bytes for MAC layer                         
    u8 logicalChannel;                        //!< The logical channel of the device with which to pair        
    u8 dstIEEEAddr[8];                        //!< The IEEE address of the device with which to pair         
    u8 keyExTransferCount;                    //!< The number of transfers the target should use to exchange the link key with the pairing originator            
    u16 dstPanId;                             //!< The PAN identifier of the device with which to pair     
    nwk_appInfo_t orgAppCap;                  //!< The application info parameter              
} nwk_pairReq_t; 


/**
 *  @brief  Define Pair response parameters type
 */
typedef struct {
    u8 primitive;                           //!< Primitive id of NLDE Data Request @ref nwk_primitive_id   
    u8 reserved[RESERVED_MAC_BYTES_LEN];    //!< Reserved bytes for MAC layer                           
    nwk_appInfo_t recAppCap;                //!< The application info parameter                
    u8 status;                              //!< The status of the pairing request 
    u16 dstPanId;                           //!< The PAN identifier of the device requesting the pair    
    u8 dstIEEEAddr[8];                      //!< The IEEE address of the device requesting the pair        
    u8 provPairingRef;                      //!< The reference to the provisional pairing entry if the pair was accepted or 0xff otherwise          
} nwk_pairResp_t;



/**
 *  @brief  Define Pair indication parameters type
 */
typedef struct {
    u8 primitive;                          //!< Primitive id of NLDE Data Request @ref nwk_primitive_id   
    u8 status;                             //!< The status of the provisional pairing 
    u16 srcPanId;                          //!< The PAN identifier of the device requesting the pair    
    u8 srcIEEEAddr[8];                     //!< The IEEE address of the device requesting the pair          
    u8 orgNodeCap;                         //!< The capabilities of the originator of the pair request     
    u16 orgVendorId;                       //!< The vendor identifier of the originator of the pair request     
    u8 orgVendorString[7];                 //!< The vendor string of the originator of the pair request             
    u8 provPairingRef;                     //!< The pairing reference that will be used           
    u8 keyExTransferCount;                 //!< The number of transfers the target should use to exchange the link key with the pairing originator                  
    nwk_appInfo_t orgAppCap;               //!< The application info parameter                
} nwk_pairInd_t;


/**
 *  @brief  Define Pair confirm parameters type
 */
typedef struct {
    u8 primitive;                          //!< Primitive id of NLDE Data Request @ref nwk_primitive_id   
    u8 status;                             //!< The status of the pair attempt 
    u8 pairingRef;                         //!< The pairing table reference for this pairing link     
    u16 recVendorId;                       //!< The vendor identifier of the originator of the pair response      
    u8 recVendorStr[7];                    //!< The vendor string of the originator of the pair response         
    nwk_appInfo_t recAppCap;               //!< The application info parameter               
} nwk_pairCnf_t; 


/**
 *  @brief  Define Ping request parameters type
 */
typedef struct {
    u8 primitive;                          //!< Primitive id of NLDE Data Request @ref nwk_primitive_id    
    u8 reserved[RESERVED_MAC_BYTES_LEN];   //!< Reserved bytes for MAC layer                           
    u8 dstIEEEAddr[8];                     //!< The IEEE address of the device requesting the ping       
    u8 options;                            //!< The ping requset option  
    u8 pingPayloadLen;                     //!< The payload length of ping request           
    u8 pingPayload[1];                     //!< The payload of the ping request              
} nwk_pingReq_t; 


/**
 *  @brief  Define Rx-enable request parameters type
 */
typedef struct {
    u8 primitive;                          //!< Primitive id of NLDE Data Request @ref nwk_primitive_id   
    u8 reserved[RESERVED_MAC_BYTES_LEN];   //!< Reserved bytes for MAC layer                            
    u32 rxOnDuration;                      //!< The number of MAC symbols for which the receiver is to be enabled         
} nwk_rxEnableReq_t;


/**
 *  @brief  Define Rx-enable confirm parameters type
 */
typedef struct {
    u8 primitive;                         //!< Primitive id of NLDE Data Request @ref nwk_primitive_id     
    u8 status;                            //!< The result of the request to enable or disable the receiver
} nwk_rxEnableCnf_t;



/**
 *  @brief  Define Start request parameters type
 */
typedef struct {
    u8 primitive;                         //!< Primitive id of NLDE Data Request @ref nwk_primitive_id     
} nwk_startReq_t;


/**
 *  @brief  Define Start confirm parameters type
 */
typedef struct {
    u8 primitive;                        //!< Primitive id of NLDE Data Request @ref nwk_primitive_id 
    u8 status;                           //!< The status of the start attempt  
} nwk_startCnf_t;


/**
 *  @brief  Define Reset request parameters type
 */
typedef struct {
    u8 primitive;                        //!< Primitive id of NLDE Data Request @ref nwk_primitive_id 
    u8 setDefaultNIB;                    //!< If TRUE, reset all NIB attributes to their default values     
} nwk_resetReq_t;


/**
 *  @brief  Define Reset confirm parameters type
 */
typedef struct {
    u8 primitive;                        //!< Primitive id of NLDE Data Request @ref nwk_primitive_id      
    u8 status;                           //!< The status of the reset request   
} nwk_resetCnf_t;


/**
 *  @brief  Define Unpair request parameters type
 */
typedef struct {
    u8 primitive;                       //!< Primitive id of NLDE Data Request @ref nwk_primitive_id 
    u8 pairingRef;                      //!< The reference into the local pairing table of the entry that is to be removed 
} nwk_unpairReq_t;

/**
 *  @brief  Define Unpair response parameters type
 */
typedef struct {
    u8 primitive;                      //!< Primitive id of NLDE Data Request @ref nwk_primitive_id  
    u8 pairingRef;                     //!< The reference into the local pairing table of the entry that is to be removed 
} nwk_unpairResp_t;


/**
 *  @brief  Define Unpair indication parameters type
 */
typedef struct {
    u8 primitive;                      //!< Primitive id of NLDE Data Request @ref nwk_primitive_id 
    u8 pairingRef;                     //!< The pairing table reference that has been removed from the pairing table 
} nwk_unpairInd_t;


/**
 *  @brief  Define Unpair confirm parameters type
 */
typedef struct {
    u8 primitive;                     //!< Primitive id of NLDE Data Request @ref nwk_primitive_id     
    u8 status;                        //!< The status of the unpair attempt  
    u8 pairingRef;                    //!< The pairing table reference for this pairing link      
} nwk_unpairCnf_t;


/**
 *  @brief  Define Update key request parameters type
 */
typedef struct {
    u8 primitive;                     //!< Primitive id of NLDE Data Request @ref nwk_primitive_id 
    u8 pairingRef;                    //!< The reference into the local pairing table of the entry whose key is to be updated  
    u8 newLinkKey[16];                //!< The security link key to replace the key in the pairing table     
} nwk_updateKeyReq_t;

/**
 *  @brief  Define Update key confirm parameters type
 */
typedef struct {
    u8 primitive;                    //!< Primitive id of NLDE Data Request @ref nwk_primitive_id     
    u8 status;                       //!< The status of the request to update the security link key  
    u8 pairingRef;                   //!< The reference into the local pairing table of the entry whose key is to be updated      
} nwk_updateKeyCnf_t;


/**
 *  @brief  Define Comm status indication parameters type
 */
typedef struct {
    u8 primitive;                  //!< Primitive id of NLDE Data Request @ref nwk_primitive_id     
    u8 pairingRef;                 //!< Reference into the pairing table indicating the recipient node      
    u16 dstPanId;                  //!< The PAN identifier of the destination device     
    u8 dstAddrMode;                //!< The addressing mode used in the DstAddr parameter      
    Addr_t dstAddr;                //!< The address of the destination device       
    u8 status;                     //!< The status of the transmission  
} nwk_commStatusInd_t;

/** @} end of group NWK_TYPE */

/** @addtogroup  NWK_FUNCTIONS NWK API
 *  @{
 */
 
 
 /**
  * @brief       Reset NWK layer
  *
  * @param[in]   fSetDfltPib - 1 indicating to reset MAC pib to default value
  *
  * @return      None
  */
void nwk_reset(u8 fSetDfltPib);

 /**
  * @brief       Initialize NWK layer
  *
  * @param[in]   cbFunc - Specified NWK layer callback function
  *
  * @return      None
  */
void nwk_init(nwk_cb_t cbFunc);

 /**
  * @brief       Call this function to send a NWK layer data request
  *
  * @param[in]   pDataReq - Parameters of the data request
  *
  * @return      None
  */
void nwk_nldeDataReq(nwk_dataReq_t *pDataReq);

 /**
  * @brief       Call this function to send a NWK layer start request
  *
  * @param[in]   pStartReq - Parameters of the start request
  *
  * @return      None
  */
void nwk_nlmeStartReq(nwk_startReq_t *pStartReq);

 /**
  * @brief       Call this function to send a NWK layer pair request
  *
  * @param[in]   pPairReq - Parameters of the pair request
  *
  * @return      None
  */
void nwk_nlmePairReq(nwk_pairReq_t *pPairReq);

 /**
  * @brief       Call this function to send a NWK layer pair response
  *
  * @param[in]   pPairResp - Parameters of the pair response
  *
  * @return      None
  */
void nwk_nlmePairResp(nwk_pairResp_t *pPairResp);


 /**
  * @brief       Call this function to send a NWK layer unpair request
  *
  * @param[in]   pUnpairReq - Parameters of the unpair request
  *
  * @return      None
  */
void nwk_nlmeUnpairReq(nwk_unpairReq_t *pUnpairReq);

 /**
  * @brief       Call this function to send a NWK layer unpair response
  *
  * @param[in]   pUnpairResp - Parameters of the unpair response
  *
  * @return      None
  */
void nwk_nlmeUnpairResp(nwk_unpairResp_t *pUnpairResp);

 /**
  * @brief       Call this function to send a NWK layer discovery request
  *
  * @param[in]   pDiscReq - Parameters of the discovery request
  *
  * @return      None
  */
void nwk_nlmeDiscReq(nwk_discReq_t *pDiscReq);

 /**
  * @brief       Call this function to send a NWK layer discovery response
  *
  * @param[in]   pDiscResp - Parameters of the discovery response
  *
  * @return      None
  */
void nwk_nlmeDiscResp(nwk_discResp_t *pDiscResp);

 /**
  * @brief       Call this function to send a NWK layer auto discovery request
  *
  * @param[in]   pAutoDiscReq - Parameters of the auto discovery request
  *
  * @return      None
  */
void nwk_nlmeAutoDiscReq(nwk_autoDiscReq_t *pAutoDiscReq);



 /**
  * @brief       Call this function to send a NWK layer updaate key request
  *
  * @param[in]   pUpdateKeyReq - Parameters of the updaate key request
  *
  * @return      None
  */
void nwk_nlmeUpdateKeyReq(nwk_updateKeyReq_t *pUpdateKeyReq);



 /**
  * @brief       Call this function to send a NWK layer reset request
  *
  * @param[in]   pResetReq - Parameters of the reset request
  *
  * @return      None
  */
void nwk_nlmeResetReq(nwk_resetReq_t *pResetReq);


 /**
  * @brief       Call this function to get the pair entry from pair table
  *
  * @param[in]   pairRef - The pair entry index 
  *
  * @return      Pair entry
  */
pairTable_t *getPairingEntryByIndex(u8 pairingRef);


 /**
  * @brief       Call this function to reset the pair entry
  *
  * @param[in]   pEntry - The pair entry need to reset 
  *
  * @return      None
  */
void resetPairTableEntry(pairTable_t *pEntry);

 /**
  * @brief       Call this function to get next valid pair entry in pair table
  *
  * @param[in]   pairRef - The current valid pair entry index
  *
  * @return      The next valid pair entry index in pair table
  */
u8 getNextPairingEntryIndex(u8 pairingRef) ;


 /**
  * @brief       Call this function to get pair entry by IEEE address 
  *
  * @param[in]   extAddr - The IEEE address 
  *
  * @return      The  pair entry in pair table
  */
pairTable_t *getPairingEntryFromExtAddr(addrExt_t extAddr);


 /**
  * @brief       Call this function to get the pair entry index by pair entry
  *
  * @param[in]   pEntry - The pair entry 
  *
  * @return      Pair entry index in pair table
  */
u8 getPairingEntryIndex(pairTable_t *pEntry);

 /**
  * @brief       Call this function to save the current nwk pib to flash
  *
  * @param[in]   None
  *
  * @return      the status of save pib
  */
nv_sts_t nwk_savePibToFlash(void);

 /**
  * @brief       Call this function to load the nwk pib from flash
  *
  * @param[in]   pEntry - The pair entry 
  *
  * @return      the status of load pib
  */
nv_sts_t nwk_loadPibFromFlash(void);

 /**
  * @brief       Call this function to save the current nwk pair table info to flash
  *
  * @param[in]   pEntry - The pair entry 
  *
  * @return      the status of save pair table
  */

nv_sts_t nwk_savePairTblToFlash(void);

 /**
  * @brief       Call this function to load the current nwk pair table info from flash
  *
  * @param[in]   pEntry - The pair entry 
  *
  * @return      the status of load pair table
  */
nv_sts_t nwk_loadPairTblFromFlash(void);

/**
 * @brief       Call this function to get the pair table size
 *
 * @param[in]   None
 *
 * @return      the size of pair table
 */

u8 nwk_getPairTblSize(void);

/**
 * @brief       Call this function to check the security enable status
 *
 * @param[in]   None
 *
 * @return      security enable status
 */
u8 nwk_isSecEnable(void);

/**
 * @brief       Call this function to check the whether stack allow to suspend
 *
 * @param[in]   None
 *
 * @return      status
 */
u8 stack_allow_suspend(void);

/**
 * @brief       Call this function to abort the discovery procedure
 *
 * @param[in]   None
 *
 * @return      status
 */
void nwk_abortNwkDiscoveryProcedure(void);

/**
 * @brief       Call this function to start frequency agility
 *
 * @param[in]   start - 1 for start, 0 for stop
 *
 * @return      status
 */
void nwk_startFrequencyAgility(u8 start);


/**
 * @brief       Call this function to start send audio data from profile layer
 *
 * @param[in]   data - the data buffer will be sent
 *
 * @param[in]   len - the data length
 *
 * @param[in]   profileId - profile identifier
 *
 * @return      status
 */
u8 nwk_profileAudioDataSend(u8 *data, u8 len, u8 profileId);



/**
 * @brief      Call this function to filter the packets that format is data
 * 			   and insecurity and filtering flag is true
 *
 * @param[in]   flag - filtering flag, if it is set to ture the packet will be filter
 * 					   or won't be filter
 *
 * @return      none
 */
void nwk_setDataPktFilterFlag(bool flag);

/**  @} end of group NWK_FUNCTIONS */

/**  @} end of group NWK_Module */

/**  @} end of group TELINK_RF4CE_STACK */
