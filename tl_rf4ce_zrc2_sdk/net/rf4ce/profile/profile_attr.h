#pragma once

/** @addtogroup  TELINK_RF4CE_STACK TELINK RF4CE Stack
 *  @{
 */

/** @addtogroup  Profile_Module RF4CE Profile
 *  @{
 */
 
/** @addtogroup Profile_Constant RF4CE Profile Constants
 *  @{
 */

/** @addtogroup profile_attr_type Profile Attribute Type
 * Definition for Attribute Type
 * @{
 */
#define ATTR_TYPE_U8                              0x00        //!< attribute type for 8 bit 
#define ATTR_TYPE_U16                             0x01        //!< attribute type for 16 bit 
#define ATTR_TYPE_ARRAY                           0x02        //!< attribute type for array 

/** @} end of group profile_attr_type */


/** @addtogroup profile_attr_access Profile Attribute Access Bit Field
 * Definition for Attribute Access Control Bit Mask
 * @{
 */
#define ATTR_ACCESS_GET                           0x01        //!< attribute access type for get command 
#define ATTR_ACCESS_PUSH                          0x02        //!< attribute access type for push command 
#define ATTR_ACCESS_SET                           0x04        //!< attribute access type for set command 
#define ATTR_ACCESS_PULL                          0x08        //!< attribute access type for pull command 
#define ATTR_ACCESS_ALL                           0x0F        //!< attribute access type for all command 
#define ATTR_ACCESS_NONE                          0x00        //!< attribute access type for none command 

/** @} end of group profile_attr_access */

/**
 *  @brief  Definition for Attribute Status Code
 */
/** @addtogroup profile_attr_status Profile Attribute Status
 * Definition for Attribute Status Code
 * @{
 */
#define ATTR_SUCC                                 0x00        //!< Attribute operation success 
#define ATTR_UNSUPPORTED                          0x01        //!< Not supported attribute 
#define ATTR_ILLEGAL_REQ                          0x02        //!< Illegal attribute operation 
#define ATTR_INVALID_INDEX                        0x03        //!< Invalid attribute index 
#define ATTR_INVALID_PARAMETER                    0x04        //!< Invalid attribute operation parameter 
#define ATTR_NOT_EXISTED                          0x05        //!< Specified attribute not found 

/** @} end of group profile_attr_status */

/** @} end of group Profile_Constant */


/** @addtogroup Profile_Type RF4CE Profile Types
 *  @{
 */

/**
 *  @brief  Definition for attribute record format
 */
typedef struct {
    u8 id;             /*!< Attribute ID */
    u8 len;            /*!< Attribute length */
    u16 accessCtl;      /*!< Attribute access control, Get/Push/Pull/Set - bit field */
    u8 *val;           /*!< Pointer to data field */
} attr_t;

/** @} end of group Profile_Type */

/** @addtogroup profile_Public_Function Profile API
 *  @brief Function declaration of profile module
 *  @{
 */

 /**
  * @brief       Get specified attribute value.
  *
  * @param[in]   profileID   - Specified profile ID. @ref profile_id_definition
  * @param[in]   attributeID - Specified attribute ID.
  * @param[out]  pVal        - Returned the attribute value.
  *
  * @return      Status of the operation. @ref profile_attr_status
  */
u8 profile_getAttr(u8 profileID, u8 attributeID, u8 *pVal);

 /**
  * @brief       Set specified attribute value.
  *
  * @param[in]   profileID   - Specified profile ID. @ref profile_id_definition
  * @param[in]   attributeID - Specified attribute ID.
  * @param[in]   pVal        - Value to be set to the attribute table.
  *
  * @return      Status of the operation. @ref profile_attr_status
  */
u8 profile_setAttr (u8 profileID, u8 attributeID, u8 *pVal);

/**  @} end of group profile_Public_Function */

/**  @} end of group Profile_Module */

/**  @} end of group TELINK_RF4CE_STACK */