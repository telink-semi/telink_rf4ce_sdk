/**************************************************************************************************
  Filename:       mac_pib.c
  Revised:        $Date: 2013-05-27 $

  Description:    Implementation of MAC information base 
**************************************************************************************************/
#include "../../../proj/tl_common.h"
#include "../../../proj/common/types.h"
#include "mac_api.h"
#include "mac_const.h"
#include "mac_pib.h"
#include "mac_phy.h"

#if (MODULE_FLASH_ENABLE)
#include "../../../proj/drivers/nv.h"
#endif


/* Attribute index constants, based on attribute ID values from spec */
#define MAC_ATTR_SET1_START       0x40
#define MAC_ATTR_SET1_END         0x5D
#define MAC_ATTR_SET1_OFFSET      0
#define MAC_ATTR_SET2_START       0xE0
#define MAC_ATTR_SET2_END         0xE5
#define MAC_ATTR_SET2_OFFSET      (MAC_ATTR_SET1_END - MAC_ATTR_SET1_START + MAC_ATTR_SET1_OFFSET + 1)

/* frame response values */
#define MAC_MAX_FRAME_RESPONSE_MIN  143
#define MAC_MAX_FRAME_RESPONSE_MAX  25776

#define MAC_IEEE_ADDRESS_IN_FLASH         0x76000

#define MAC_IEEE_ADDRESS_IN_OTP           0x3f00

/* PIB access and min/max table type */
typedef struct
{
    u8     offset;
    u8     len;
    u8     min;
    u8     max;
} mac_pibTbl_t;

const u8 startIEEEAddr[] = {0x05, 0xd5, 0xb3, 0x70};

/**
 *  @brief PIB default values
 */
static const mac_pib_t macPibDefaults =
{
    54,                                         
    FALSE,                                      
    FALSE,                                      
    FALSE,                                      
    6,                                          

    NULL,                                       
    0,                                          
    MAC_BO_NON_BEACON,                          
    0,                                          
    0,                                          

    {{0}, ADDR_MODE_EXT},
    MAC_SHORT_ADDR_NONE,                        
    0,                                          
    FALSE,                                      
    4,                                          

    5,                                          
    0xFFFF,                                     
    FALSE,                                      
    TRUE,                                       
    MAC_SHORT_ADDR_NONE,                        

    MAC_SO_NONE,                                
    0x01F4,                                     
    FALSE,                                      
    8,                                          
    1220,                                       

    3,                                          
    32,                                         
    0,                                          
    TRUE,                                       
    FALSE,                                      

    /* Proprietary */
    PHY_TX_POWER_MAX,                           
    MAC_CHAN_11,                                
    {0xbb,0xbb,0xbb,0xbb,0xbb,0xbb,0xbb,0xbb},  
    1,                                          
    MAC_BO_NON_BEACON,                          
};



/* PIB access and min/max table.  min/max of 0/0 means not checked; if min/max are
 * equal, element is read-only
 */
static const mac_pibTbl_t macPibTbl[] =
{
  {OFFSETOF(mac_pib_t, ackWaitDuration), sizeof(u8), 54, 54},                         
  {OFFSETOF(mac_pib_t, associationPermit), sizeof(bool), FALSE, TRUE},                
  {OFFSETOF(mac_pib_t, autoRequest), sizeof(bool), FALSE, TRUE},                      
  {OFFSETOF(mac_pib_t, battLifeExt), sizeof(bool), FALSE, TRUE},                      
  {OFFSETOF(mac_pib_t, battLifeExtPeriods), sizeof(u8), 6, 6},                        

  {OFFSETOF(mac_pib_t, pBeaconPayload), sizeof(u8 *), 0, 0},                          
  {OFFSETOF(mac_pib_t, beaconPayloadLength), sizeof(u8), 0, 52},                      
  {OFFSETOF(mac_pib_t, beaconOrder), sizeof(u8), 0, 15},                              
  {OFFSETOF(mac_pib_t, beaconTxTime), sizeof(u32), 1, 1},                             
  {OFFSETOF(mac_pib_t, bsn), sizeof(u8), 0x00, 0xFF},                                 

  {OFFSETOF(mac_pib_t, coordExtendedAddress.addr.extAddr), sizeof(addrExt_t), 0, 0},  
  {OFFSETOF(mac_pib_t, coordShortAddress), sizeof(u16), 0, 0},                        
  {OFFSETOF(mac_pib_t, dsn), sizeof(u8), 0x00, 0xFF},                                 
  {OFFSETOF(mac_pib_t, gtsPermit), sizeof(bool), FALSE, TRUE},                        

  {OFFSETOF(mac_pib_t, maxCsmaBackoffs), sizeof(u8), 0, 254},                   

  {OFFSETOF(mac_pib_t, minBe), sizeof(u8), 0, 8},                               
  {OFFSETOF(mac_pib_t, panId), sizeof(u16), 0, 0},                              
  {OFFSETOF(mac_pib_t, promiscuousMode), sizeof(bool), FALSE, TRUE},            
  {OFFSETOF(mac_pib_t, rxOnWhenIdle), sizeof(bool), FALSE, TRUE},               
  {OFFSETOF(mac_pib_t, shortAddress), sizeof(u16), 0, 0},                       

  {OFFSETOF(mac_pib_t, superframeOrder), sizeof(u8), 0, 15},                    
  {OFFSETOF(mac_pib_t, transactionPersistenceTime), sizeof(u16), 0, 0},         
  {OFFSETOF(mac_pib_t, associatedPanCoord), sizeof(bool), FALSE, TRUE},         

  {OFFSETOF(mac_pib_t, maxBe), sizeof(u8), 0, 8},                                     
  {OFFSETOF(mac_pib_t, maxFrameTotalWaitTime), sizeof(u16), 0x00, 0xFF},              

  {OFFSETOF(mac_pib_t, maxFrameRetries), sizeof(u8), 0, 7},                           
  {OFFSETOF(mac_pib_t, responseWaitTime), sizeof(u8), 2, 64},                         
  {OFFSETOF(mac_pib_t, syncSymbolOffset), sizeof(u8), 0, 0},                          
  {OFFSETOF(mac_pib_t, timeStampSupported), sizeof(bool), FALSE, TRUE},               
  {OFFSETOF(mac_pib_t, securityEnabled), sizeof(bool), FALSE, TRUE},                  

  /* Proprietary PIBs */
  {OFFSETOF(mac_pib_t, phyTransmitPower), sizeof(u8), 0, 0xFF},                       
  {OFFSETOF(mac_pib_t, logicalChannel), sizeof(u8), MAC_CHAN_11, MAC_CHAN_28},        
  {OFFSETOF(mac_pib_t, extendedAddress), sizeof(addrExt_t), 0, 0},                    
  {OFFSETOF(mac_pib_t, altBe), sizeof(u8), 0, 8},                                     
  {OFFSETOF(mac_pib_t, deviceBeaconOrder), sizeof(u8), 0, 15},                        
  {OFFSETOF(mac_pib_t, phyTransmitPower), sizeof(u8), 0, 0},                          
};

/* Invalid PIB table index used for error code */
#define MAC_PIB_INVALID     ((u8) (sizeof(macPibTbl) / sizeof(macPibTbl[0])))




/* MAC PIB */
mac_pib_t macPib;

/*********************************************************************
 * @fn      mac_pibIndex
 *
 * @brief   Takes an PIB attribute and returns the index in to macPibTbl for the attribute.
 *
 * @param   pibAttribute - PIB attribute to look up
 *
 * @return  Index in to macPibTbl for the attribute or MAC_PIB_INVALID.
 */
_CODE_MAC_ static u8 mac_pibIndex(u8 pibAttribute)
{
    if ((pibAttribute >= MAC_ATTR_SET1_START) && (pibAttribute <= MAC_ATTR_SET1_END))
    {
        return (pibAttribute - MAC_ATTR_SET1_START + MAC_ATTR_SET1_OFFSET);
    }
    else if ((pibAttribute >= MAC_ATTR_SET2_START) && (pibAttribute <= MAC_ATTR_SET2_END))
    {
        return (pibAttribute - MAC_ATTR_SET2_START + MAC_ATTR_SET2_OFFSET);
    }
    else
    {
        return MAC_PIB_INVALID;
    }
}


/*********************************************************************
 * @fn      mac_pibReset
 *
 * @brief   Reset the MAC PIB to default value
 *
 * @param   None
 *
 * @return  None
 */
		
_CODE_MAC_ void mac_pibReset(void)
{
    /* copy PIB defaults */
    macPib = macPibDefaults;

    /* initialize random sequence numbers */
    macPib.dsn = (u8)rand();
    macPib.bsn = (u8)rand();

	const u8 invalidIEEEAddr[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	
	u8 extAddr[8] = {0};
	flash_read_page(MAC_IEEE_ADDRESS_IN_FLASH, 8, extAddr);
	
	/* check the ieee address is valid or not */
	if (memcmp(invalidIEEEAddr, extAddr, 8) == 0 ) {
		memcpy(extAddr+4, startIEEEAddr, 4);
		u8 randomAddr[4];
    	generateRandomData(randomAddr, 4);
		randomAddr[3] &= 0x0f;
        randomAddr[3] |= 0x80;
		memcpy(extAddr, randomAddr, 4);
	}else{
		u8 addrL[2] = {0};
		memcpy(addrL, &extAddr[6], 2);
		for(u32 i = 7; i > 1; i--){
			extAddr[i] = extAddr[i - 2];
		}
		memcpy(&extAddr[0], addrL, 2);
	}
	
    mac_mlmeSetReq(MAC_EXTENDED_ADDRESS, extAddr);
}

/*********************************************************************
 * @fn      mac_mlmeSetReq
 *
 * @brief   Set an attribute value in the MAC PIB
 *
 * @param   pibAttribute - The attribute identifier
 * @param   pValue - Pointer to the attribute value
 *
 * @return  MAC Status
 */
_CODE_MAC_ mac_sts_t mac_mlmeSetReq(u8 pibAttribute, void* pValue)
{
    u8 i, r;
	
#if (HOST_ROLE)
    u8 type;
    u8 dataBuf[100];
#endif


    if (pibAttribute == MAC_BEACON_PAYLOAD) {
        memcpy(macPib.pBeaconPayload, pValue, macPib.beaconPayloadLength);
        return MAC_SUCCESS;
    }

    if ((i = mac_pibIndex(pibAttribute)) == MAC_PIB_INVALID) {
        return MAC_UNSUPPORTED_ATTRIBUTE;
    }

    if ((macPibTbl[i].min != 0) || (macPibTbl[i].max != 0)) {
        if (pibAttribute == MAC_MAX_FRAME_TOTAL_WAIT_TIME){
            if ((*((u16 *) pValue) < MAC_MAX_FRAME_RESPONSE_MIN) ||
                (*((u16 *) pValue) > MAC_MAX_FRAME_RESPONSE_MAX)){
                return MAC_INVALID_PARAMETER;
            }
        }

        /* range check */
        if ((*((u8 *) pValue) < macPibTbl[i].min) || (*((u8 *) pValue) > macPibTbl[i].max)){
            return MAC_INVALID_PARAMETER;
        }

    }

    /* set value in PIB */
    r = irq_disable();
    memcpy((u8 *) &macPib + macPibTbl[i].offset, pValue, macPibTbl[i].len);
    irq_restore(r);


    switch (pibAttribute)
    {
    case MAC_PAN_ID:
        rf_set(RF_ID_MAC_PANID, (u8*)&macPib.panId, 2);
        break;

    case MAC_SHORT_ADDRESS:
        rf_set(RF_ID_MAC_SHORT_ADDR, (u8*)&macPib.shortAddress, 2);
        break;

    case MAC_RX_ON_WHEN_IDLE:
        /* turn rx on or off */
        rf_set(RF_ID_RX_ONOFF, (u8*)&macPib.rxOnWhenIdle, 1);
        break;

    case MAC_LOGICAL_CHANNEL:
		rf_set(RF_ID_CHANNEL, (u8*)&macPib.logicalChannel, 1);
        break;

    case MAC_EXTENDED_ADDRESS:
        rf_set(RF_ID_MAC_EXT_ADDR, (u8*)&macPib.extendedAddress, 8);
        break;

    

    case MAC_PHY_TRANSMIT_POWER:
        /* Legacy transmit power attribute */
        macPib.phyTransmitPower = (u8)(-(s8)macPib.phyTransmitPower);


    case MAC_PHY_TRANSMIT_POWER_SIGNED:
        /* Set the transmit power */
		rf_set(RF_ID_TX_POWER, (u8*)&macPib.phyTransmitPower, 1);
        break;

    default:
        break;
    }

    return MAC_SUCCESS;
}

/*********************************************************************
 * @fn      mac_mlmeGetReq
 *
 * @brief   Get an attribute value in the MAC PIB
 *
 * @param   pibAttribute - The attribute identifier
 * @param   pValue - Pointer to the attribute value
 *
 * @return  MAC Status
 */
_CODE_MAC_ mac_sts_t mac_mlmeGetReq(u8 pibAttribute, void* pValue)
{
    u8 i,r;

    if (pibAttribute == MAC_BEACON_PAYLOAD) {
        memcpy(pValue, macPib.pBeaconPayload, macPib.beaconPayloadLength);
        return MAC_SUCCESS;
    }

    if ((i = mac_pibIndex(pibAttribute)) == MAC_PIB_INVALID)
    {
        return MAC_UNSUPPORTED_ATTRIBUTE;
    }

    r = irq_disable();
    memcpy(pValue, (u8 *) &macPib + macPibTbl[i].offset, macPibTbl[i].len);
    irq_restore(r);

    return MAC_SUCCESS;
}

_CODE_MAC_ mac_sts_t mac_mlmeSetReqWithCnf(mac_setReq_t *pSetReq)
{
    u8 i, r;
    u8 pibAttribute;
    void* pValue;

    pibAttribute=pSetReq->pibAttribute;
    pValue=&(pSetReq->pibAttributeValue);
    mac_setCnf_t *mac_setCnf= (mac_setCnf_t *)pSetReq;

    if (pibAttribute == MAC_BEACON_PAYLOAD) {
        memcpy(macPib.pBeaconPayload, pValue, macPib.beaconPayloadLength);
        mac_setCnf->pibAttribute=pibAttribute;
      	mac_setCnf->primitive=MAC_MLME_SET_CNF;
        mac_setCnf->status=MAC_SUCCESS;
        mac_sendConfirm((u8*)mac_setCnf);
        return MAC_SUCCESS;
    }

    if ((i = mac_pibIndex(pibAttribute)) == MAC_PIB_INVALID) {

        return MAC_UNSUPPORTED_ATTRIBUTE;
    }

    if ((macPibTbl[i].min != 0) || (macPibTbl[i].max != 0)) {
        if (pibAttribute == MAC_MAX_FRAME_TOTAL_WAIT_TIME){
            if ((*((u16 *) pValue) < MAC_MAX_FRAME_RESPONSE_MIN) ||
                (*((u16 *) pValue) > MAC_MAX_FRAME_RESPONSE_MAX)){
                return MAC_INVALID_PARAMETER;
            }
        }

        /* range check */
        if ((*((u8 *) pValue) < macPibTbl[i].min) || (*((u8 *) pValue) > macPibTbl[i].max)){
            return MAC_INVALID_PARAMETER;
        }
    }

    r = irq_disable();
    memcpy((u8 *) &macPib + macPibTbl[i].offset, pValue, macPibTbl[i].len);
    irq_restore(r);

    switch (pibAttribute)
    {
    case MAC_PAN_ID:
        rf_set(RF_ID_MAC_PANID, (u8*)&macPib.panId, 2);
        break;

    case MAC_SHORT_ADDRESS:
        rf_set(RF_ID_MAC_SHORT_ADDR, (u8*)&macPib.shortAddress, 2);
        break;

    case MAC_RX_ON_WHEN_IDLE:
        rf_set(RF_ID_RX_ONOFF, (u8*)&macPib.rxOnWhenIdle, 1);
        break;

    case MAC_LOGICAL_CHANNEL:
		rf_set(RF_ID_CHANNEL, (u8*)&macPib.logicalChannel, 1);
        break;

    case MAC_EXTENDED_ADDRESS:
        rf_set(RF_ID_MAC_EXT_ADDR, (u8*)&macPib.extendedAddress, 8);

        break;

    case MAC_PHY_TRANSMIT_POWER:
        /* Legacy transmit power attribute */
        macPib.phyTransmitPower = (u8)(-(s8)macPib.phyTransmitPower);


    case MAC_PHY_TRANSMIT_POWER_SIGNED:
		rf_set(RF_ID_TX_POWER, (u8*)&macPib.phyTransmitPower, 1);
        break;

    default:
        break;
    }
    mac_setCnf->pibAttribute=pibAttribute;
	mac_setCnf->primitive=MAC_MLME_SET_CNF;
    mac_setCnf->status=MAC_SUCCESS;
    mac_sendConfirm((u8*)mac_setCnf);
    return MAC_SUCCESS;
}


_CODE_MAC_ mac_sts_t mac_mlmeGetReqWithCnf(mac_getReq_t *pGetReq)
{
    u8 i,r;
    u8 pibAttribute;
    void* pValue;

    pibAttribute=pGetReq->pibAttribute;
    pValue=&(pGetReq->primitive)+3;
	mac_getCnf_t *mac_getCnf= (mac_getCnf_t *)pGetReq;
	mac_getCnf->pibAttribute=pibAttribute;
	mac_getCnf->primitive=MAC_MLME_GET_CNF;
	//mac_getCnf->pibAttributeValue=(*(u64*)pValue);
    if (pibAttribute == MAC_BEACON_PAYLOAD) {
        memcpy(pValue, macPib.pBeaconPayload, macPib.beaconPayloadLength);
        mac_getCnf->status=MAC_SUCCESS;
        for(i=8;i<0;i++)
		{
        	mac_getCnf->pibAttributeValue<<=8;
        	mac_getCnf->pibAttributeValue |= (*((u8*)pValue+i));
		}
        mac_sendConfirm((u8*)mac_getCnf);
        return MAC_SUCCESS;
    }

    if ((i = mac_pibIndex(pibAttribute)) == MAC_PIB_INVALID)
    {
        return MAC_UNSUPPORTED_ATTRIBUTE;
    }

    r = irq_disable();
    memcpy(pValue, (u8 *) &macPib + macPibTbl[i].offset, macPibTbl[i].len);
    irq_restore(r);
    mac_getCnf->status=MAC_SUCCESS;
    for(i=8;i<0;i++)
	{
		mac_getCnf->pibAttributeValue<<=8;
		mac_getCnf->pibAttributeValue |= (*((u8*)pValue+i));
	}
    mac_sendConfirm((u8*)mac_getCnf);
    return MAC_SUCCESS;
}


_CODE_MAC_ u8 mac_getAttrLen(u8 pibAttribute)
{
    u8 i;

    if (pibAttribute == MAC_BEACON_PAYLOAD) {
        return macPib.beaconPayloadLength;
    }

    if ((i = mac_pibIndex(pibAttribute)) == MAC_PIB_INVALID)
    {
        return 0;
    }

    return macPibTbl[i].len;
}


inline u8 rf_getChannel(void){
	return	macPib.logicalChannel;
}

#if (MODULE_FLASH_ENABLE)
_CODE_MAC_ nv_sts_t mac_savePibToFlash(void)
{
    return nv_write(DS_MAC_PHY_MODULE, MAC_PIB_NV_ID, sizeof(mac_pib_t), (u8*)&macPib);
}

nv_sts_t mac_loadPibFromFlash(void)
{
	return nv_read(DS_MAC_PHY_MODULE, MAC_PIB_NV_ID, sizeof(mac_pib_t), (u8*)&macPib);
}

#endif
