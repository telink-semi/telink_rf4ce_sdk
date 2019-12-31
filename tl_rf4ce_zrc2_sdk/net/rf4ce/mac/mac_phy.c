#include "../../../proj/tl_common.h"

/**********************************************************************
 * INCLUDES
 */
#include "mac_api.h"
#include "mac_trx.h"
#include "mac_const.h"
#include "mac_task.h"
#include "mac_phy.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */
#define MAC_FCF_ACK_REQ_BIT    0x20
#define MAC_FCF_INTRA_PAN_BIT  0x40
#define MAC_FCF_DST_ADDR_BIT   0x0c
#define MAC_FCF_FRAME_TYPE     0x07


#define RF_DROP_REASON_INVALID_CRC        0x01
#define RF_DROP_REASON_RF_BUSY            0x02
#define RF_DROP_REASON_EXPECT_ACK         0x03
#define RF_DROP_REASON_INVALIC_BEACON     0x04
#define RF_DROP_REASON_FILTER_PANID       0x05
#define RF_DROP_REASON_FILTER_DSTADDR     0x06
#define RF_DROP_REASON_FILTER_LEN         0x07
#define RF_DROP_REASON_INVALIC_FRAME_TYPE         0x08

u8 rf_busyFlag = 0;
_attribute_aligned_(4) u8 rf_tx_buf[ZB_RADIO_TX_HDR_LEN + 127];
/**********************************************************************
 * LOCAL TYPES
 */
#if RF_STATISTIC_ENABLE
typedef struct {
    u32 rxCnt;
    u32 rxGarbageCnt;
	u32 txAckCnt;
    u32 txFinishCnt;
} rf_statistic_t;
#endif  /* RF_STATISTIC_ENABLE */


/**********************************************************************
 * LOCAL VARIABLES
 */
u8 rf_lastRssi;
volatile s8 soft_rssi;
volatile s32 sum_rssi, cnt_rssi = 1;
u8 rfMode =  RF_STATE_TX;


u8 rf_txPower;

rf_txCB_t  rf_txCbFunc;
rf_ackCB_t rf_ackCbFunc;

rf_specificFunc_t rf_specificFuns;

u8*        rf_rxBuf;

/* Variables defined in the rf driver */
extern u8*        rf_rxBuf;
extern rf_txCB_t  rf_txCbFunc;
extern u8         rf_txPower;
extern u8         rf_lastRssi;
extern rf_ackCB_t rf_ackCbFunc;
extern ev_poll_callback_t rf_edDetect_ptr;
//extern u8         fPaEn;
//extern u32        rf_pa_txen_pin;
//extern u32        rf_pa_rxen_pin;
u8 _attribute_aligned_(4) rf_ack_buf[12];

/*
 * Parameters for 802.15.4 RF filter
 */
u16 rf_shortAddr;
u8  rf_extAddr[8];
u16 rf_panID;
u8  rf_framePending;
u8  rf_isAck;
u8  rf_isCoord;

/* Used in the upper layer stack */
extern volatile s8 soft_rssi;
u8 rx_irq_busy = 0;

#if RF_STATISTIC_ENABLE
rf_statistic_t rf_statistic;
#endif

u8 T_drop;


/*********************************************************************
 * @fn      rf_regProtocolSpecific
 *
 * @brief   Register protocol specific RF functions.
 *            Note: This function must be called before rf_init() and rf_reset()
 *
 * @param   txCbFunc - tx done callback function
 * @param   rxCbFunc - rx done callback function
 *
 * @return  none
 */
 _CODE_MAC_ void rf_regProtocolSpecific(rf_specificFunc_t* funcs)
{
    rf_specificFuns.initFunc  = funcs->initFunc;
    rf_specificFuns.resetFunc = funcs->resetFunc;
    rf_specificFuns.setFunc   = funcs->setFunc;
}



/*********************************************************************
 * @fn      rf_reset
 *
 * @brief   Reset RF module and do the RF power up.
 *
 * @param   none
 *
 * @return  none
 */
 _CODE_MAC_ void rf_reset(void)
{
#if 0
	rf_txPower = 0;
	rf_lastRssi = 0;
#ifndef WIN32
    //trf_drv_init(0);
    ZB_RADIO_INIT();
#endif
    /* Add real RF HW initialize function */
    rf_setTxPower(PHY_TX_POWER_MAX);
    rf_setChannel(DFLT_LOGIC_CHANNEL);
    rf_setTrxState(RF_STATE_TX);

    if (rf_specificFuns.resetFunc) {
        rf_specificFuns.resetFunc();
    }
#endif
}


/*********************************************************************
 * @fn      rf_init
 *
 * @brief   Initialize RF module and do the RF power up.
 *
 * @param   none
 *
 * @return  none
 */
 _CODE_MAC_ void rf_init(void)
{
    rf_reset();

    /* Protocol specific initialization */
    if (rf_specificFuns.initFunc) {
        rf_specificFuns.initFunc();
    }

    ZB_RADIO_TRX_CFG((RF_PKT_BUFF_LEN));

	ZB_RADIO_RX_ENABLE;
	ZB_RADIO_TX_ENABLE;

	rf_setTxPower(PHY_TX_POWER_MAX);

   // rf_start();   //TODO by jingzhi
}


/*********************************************************************
 * @fn      rf_paInit
 *
 * @brief   Initialize PA.
 *
 * @param   none
 *
 * @param   none
 *
 * @return  none
 */
 _CODE_MAC_ void rf_paInit(u32 TXEN_pin, u32 RXEN_pin)
{

}


/*********************************************************************
 * @fn      rf_setCBFunc
 *
 * @brief   Initialize Interrupt callback function.
 *
 * @param   txCbFunc - tx done callback function
 * @param   rxCbFunc - rx done callback function
 *
 * @return  none
 */
 _CODE_MAC_ void rf_setCBFunc(rf_txCB_t txCbFunc)
{
    rf_txCbFunc = txCbFunc;
}

/*********************************************************************
 * @fn      rf_setAckCB
 *
 * @brief   Initialize ZigBee ACK packet Callback function.
 *
 * @param   ackCbFunc - Pointer to the ack callback function
 *
 * @return  none
 */
 _CODE_MAC_ void rf_setAckCB(rf_ackCB_t ackCbFunc)
{
	rf_ackCbFunc = ackCbFunc;
}

 _CODE_MAC_ u8 *rf_getRxBuf(void){
	 u8 *buf = (u8 *)ev_buf_allocate(LARGE_BUFFER);
	 if(buf){
		 return (buf + RF_RX_BUFFER_OFFSET);
	 }

	 return NULL;
}


/*********************************************************************
 * @fn      rf_set
 *
 * @brief   Set rf parameter according to specified RF ID
 *
 * @param   id     - The specified RF ID
 *          pValue - The detailed rf parameter
 *          len    - The length will be set to
 *
 * @return  none
 */
 _CODE_MAC_ void rf_set(u8 id, u8 *pValue, u8 len)
{
    if (id >RF_ID_GENERAL_END) {
        if (rf_specificFuns.setFunc) {
        	rf_specificFuns.setFunc(id, pValue, len);
        }
        return;
    }

    switch (id) {
    case RF_ID_CHANNEL:
        //rf_setChannel(*pValue);
        break;

    case RF_ID_TX_POWER:
        rf_setTxPower(*pValue);
        break;

    case RF_ID_RX_ONOFF:
        if (*pValue) {
            rf_setTrxState(RF_STATE_RX);
        } else {
            rf_setTrxState(RF_STATE_TX);
        }
        break;

    default:
        break;
    }
}

/**********************************************************************
 * LOCAL FUNCTIONS
 */
_CODE_MAC_ void rf_802154_reset(void)
{
    /* Reset the parameters using in the filter */
    rf_shortAddr = 0xFFFE;
    rf_panID = 0;
    rf_framePending = 0;
    rf_isAck = 0;
    memset(rf_extAddr, 0, 8);

    /* Reset ack buf */
    memset(rf_ack_buf, 0, 12);
    rf_ack_buf[0] = 4;
    rf_ack_buf[4] = 5;
    rf_ack_buf[5] = 0x02;
    rf_ack_buf[6] = 0x00;

#if RF_STATISTIC_ENABLE
    memset(&rf_statistic, 0, sizeof(rf_statistic_t));
#endif

}

_CODE_MAC_ void rf_802154_init(void)
{
    rf_802154_reset();
}


_CODE_MAC_ void rf_802154_set(u8 id, u8 *pValue, u8 len)
{
    switch(id) {

    case RF_ID_MAC_SHORT_ADDR:
        memcpy((u8*)&rf_shortAddr, pValue, 2);
        break;

    case RF_ID_MAC_EXT_ADDR:
        memcpy(rf_extAddr, pValue, 8);
        break;

    case RF_ID_MAC_PANID:
        memcpy((u8*)&rf_panID, pValue, 2);
        break;

    case RF_ID_MAC_FRAME_PENDING:
        rf_framePending = *pValue;
        break;

    case RF_ID_MAC_PAN_COORD:
        rf_isCoord = *pValue;
        break;
    }
}

_attribute_ram_code_ void rf_tx(u8* buf, u8 len)
{
	/* Fill the telink RF header */
	rf_setTrxState(RF_STATE_TX);

	rf_tx_buf[0] = len+1;
	rf_tx_buf[1] = 0;
	rf_tx_buf[2] = 0;
	rf_tx_buf[3] = 0;
	// RF length
	rf_tx_buf[4] = len+2;
	// Payload
	memcpy(rf_tx_buf+5, buf, len);

	//reg_rf_irq_status = (FLD_RF_IRQ_RX | FLD_RF_IRQ_TX);
	ZB_RADIO_TX_DONE_CLR;
	ZB_RADIO_RX_DONE_CLR;
	ZB_RADIO_TX_START(rf_tx_buf);//Manual mode
}


_CODE_MAC_ void rf_edDetect(void)
{
    s8 rssi;
    rssi = ZB_RADIO_RSSI_GET();

    //soft_rssi = rssi;//(rssi + soft_rssi)/2;
    sum_rssi += rssi;
    if(++cnt_rssi >= 0xfffffe){
    	sum_rssi = sum_rssi/cnt_rssi;
    	cnt_rssi = 1;
    }
}

/*********************************************************************
 * @fn      rf_startED
 *
 * @brief   Start ED detect
 *
 * @param   none
 *
 * @return  none
 */
 _CODE_MAC_ void rf_startED(void)
{
    soft_rssi = -110;
    sum_rssi = 0;
    cnt_rssi = 0;
    //ev_enable_poll(EV_POLL_ED_DETECT);
    rf_edDetect_ptr = rf_edDetect;
    /* Change to ED mode */
    rf_setTrxState(RF_STATE_ED);
}


/*********************************************************************
 * @fn      rf_stopED
 *
 * @brief   Stop Energy Detect
 *
 * @param   none
 *
 * @return  ED result
 */
 _CODE_MAC_ u8 rf_stopED(void){
    u8 ed;
    u32 temp;

    if(cnt_rssi == 0) cnt_rssi = 1;
    soft_rssi = sum_rssi/cnt_rssi;

    //ev_disable_poll(EV_POLL_ED_DETECT);
    rf_edDetect_ptr = NULL;
    /* Transfer the RSSI value to ED value */
    if (soft_rssi <= -106) {
        ed = 0;
    } else if (soft_rssi >= -6) {
        ed = 0xff;
    } else{
    	temp = (soft_rssi + 106) * 255;
        ed = temp/100;
    }
    return ed;
}


 /*********************************************************************
  * @fn      rf_getLqi
  *
  * @brief   Get calculated Link Quality value
  *
  * @param   none
  *
  * @return  lqi result
  */
 u8 rf_getLqi(u8 inRssi){
 	rf_rxGainMode_t mode = RF_GAIN_MODE_MANU_MAX;
 	return ZB_RADIO_RSSI_TO_LQI(mode, inRssi);
 }

_CODE_MAC_ u8 rf_performCCA(void)
{
   s8 rssi_peak = -110;
   s32 rssi_total = 0;

   return PHY_CCA_IDLE;

   for(int i = 0; i < 16; i++){
   	WaitUs(3);
   	rssi_total += ZB_RADIO_RSSI_GET();
   }
   rssi_peak = (rssi_total >> 4);

   if (rssi_peak > CCA_THRESHOLD) {
		return PHY_CCA_BUSY;
	} else {
		return PHY_CCA_IDLE;
	}
}


/*********************************************************************
 * @fn      rf_setRxBuf
 *
 * @brief   Set RX buffer to dma and enable DMA.
 *
 * @param   pBuf - the allocated rx buffer
 *
 * @return  none
 */
 _CODE_MAC_ void rf_setRxBuf(u8* pBuf)
{
    rf_rxBuf = pBuf + RF_RX_BUFFER_OFFSET;
#ifndef WIN32
    //rfhw_setRxBuf(rf_rxBuf);
    ZB_RADIO_RX_BUF_SET((u16)(u32)(rf_rxBuf));
#endif
}


 /*********************************************************************
  * @fn      rf_setTrxState
  *
  * @brief   Go to specified state and set related register.
  *
  * @param   state - Specified state
  *
  * @return  none
  */
  _CODE_MAC_ void rf_setTrxState(u8 state) {
	if (RF_STATE_RX == state || RF_STATE_ED == state) {
		ZB_RADIO_TRX_SWITCH(RF_MODE_RX,LOGICCHANNEL_TO_PHYSICAL(rf_getChannel()));
		rfMode = RF_STATE_RX;
	} else if (RF_STATE_TX == state) {
		ZB_RADIO_TRX_SWITCH(RF_MODE_TX,LOGICCHANNEL_TO_PHYSICAL(rf_getChannel()));
		rfMode = RF_STATE_TX;
	} else {
		/* Close RF */
		ZB_RADIO_TRX_SWITCH(RF_MODE_TX,LOGICCHANNEL_TO_PHYSICAL(rf_getChannel()));
		rfMode = RF_MODE_TX;
	}
//	if(rf_getChannel()!=0x14)
//		while(1);

 }

/*********************************************************************
* @fn      rf_RxGainSet
*
* @brief   Set gain mode.
*
* @param   mode - auto or maxium
*
* @return  none
*/
_CODE_MAC_ void rf_RxGainModeSet(u8 mode){
	if(mode == RF_GAIN_MODE_MANU_MAX){
		ZB_RADIO_MODE_MAX_GAIN();
	}else{
		ZB_RADIO_MODE_AUTO_GAIN();
	}
}


  /*********************************************************************
   * @fn      rf_setTxPower
   *
   * @brief   Set specified transmitted power.
   *
   * @param   txPower - Specified power
   *
   * @return  none
   */
_CODE_MAC_ void rf_setTxPower(u8 power){
	ZB_RADIO_TX_POWER_SET((RF_PowerTypeDef )power);
}

/*********************************************************************
 * @fn      rf_rx_irq_handler
 *
 * @brief   RX irq handler
 *
 * @param   none
 *
 * @return  none
 */
volatile u8 T_rf_rx_irq_handlerCnt[4] = {0};//debug
_attribute_ram_code_ __attribute__((optimize("-Os"))) void rf_rx_irq_handler(void)
{
    u8 *p = rf_rxBuf;
    int fNeedAck = 0;
    int fAck = 0;
    u8 fDrop = 0;
    u8 fcf1, fcf2;

    ZB_RADIO_RX_DISABLE;
	ZB_RADIO_RX_DONE_CLR;

    if ( (!ZB_RADIO_CRC_OK(p)) || (!ZB_RADIO_PACKET_LENGTH_OK(p)) || (rf_busyFlag&TX_BUSY) ) {
    	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
    	ZB_RADIO_RX_ENABLE;
        return;
    }
    T_rf_rx_irq_handlerCnt[0]++;


    /* Parse necessary field to be used later */
    u8 len = (u8)ZB_RADIO_ACTUAL_PAYLOAD_LEN(p);
    u8 *macPld = p + ZB_RADIO_RX_HDR_LEN;
    fcf1 = macPld[0];    // frame control byte 1
    fcf2 = macPld[1];    // frame control byte 2

//    u8 *pSrcAddr = zb_macDataFilter(macPld, len, &fDrop, &fAck);
    zb_macDataFilter(macPld, len, (u8 *)&fDrop, (u8 *)&fAck);

    u8 beaconFlt = 1;
#if RF4CE_TARGET
    if(mac_getState() == MAC_STATE_SCAN){
    	beaconFlt = 0;
    }
#endif
    if(beaconFlt){
    	if ((fcf1 & MAC_FCF_FRAME_TYPE) == MAC_FRAME_BEACON) {
    		fDrop = 1;
    	}
    }

	if (fDrop) {
		/* Drop the packet and recover the DMA */
		ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
		ZB_RADIO_RX_ENABLE;
		return;
	}

    /*---------------------------------------------------------
     *  Handle ACK Frame
     */
    if (fAck) {
        if (rf_ackCbFunc) {
            rf_ackCbFunc( fcf1&0x10, macPld[2]);
        }

        /* still use the rf_rxBuf to receive */
        ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
        ZB_RADIO_RX_ENABLE;
        return;
    }

	T_rf_rx_irq_handlerCnt[1]++;
    /*----------------------------------------------------------
     *  Send ACK
     */
    if (fcf1 & MAC_FCF_ACK_REQ_BIT) {
    	fNeedAck = 1;
    	T_rf_rx_irq_handlerCnt[2]++;
    	ZB_RADIO_TX_DONE_CLR;
    	ZB_RADIO_TRX_SWITCH(RF_STATE_TX, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel()));

        rf_ack_buf[7] = macPld[2];
        rf_ack_buf[5] |= ( rf_framePending<<4 );
        rf_isAck = 1;
    }

    /* Use the backup buffer to receive next packet */
    u8 *nextRxBuf = rf_getRxBuf();
    if(!nextRxBuf){
    	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
		ZB_RADIO_RX_ENABLE;
		return;
    }

   	rf_rxBuf = nextRxBuf;
    if(fNeedAck){
        ZB_RADIO_TX_START(rf_ack_buf);//Manual Mode
        rf_busyFlag |= (TX_ACKPACKET|TX_BUSY);
        while(!ZB_RADIO_TX_DONE);
    }
	ZB_RADIO_RX_BUF_CLEAR(rf_rxBuf);
	ZB_RADIO_RX_BUF_SET((u16)(u32)(rf_rxBuf));
	ZB_RADIO_RX_ENABLE;

    /*-------------------------------------------------------------------------------
     *  post the message to MAC layer
     */
	rx_buf_t* pRxEvt  = (rx_buf_t*)ev_buf_getTail(p, sizeof(rx_buf_t));
    // Save necessary informations
	pRxEvt->rxBuf     = macPld;  //p;
	pRxEvt->rssi      = ZB_RADION_PKT_RSSI_GET(p);
	pRxEvt->fPending  = rf_framePending;
	pRxEvt->timestamp = ZB_RADIO_TIMESTAMP_GET(p);
	pRxEvt->len = len;

    buf_message_post((u8 *)p,BUF_ITEM_STATE_PHY2MAC);
    T_rf_rx_irq_handlerCnt[3]++;
}



/*********************************************************************
 * @fn      rf_tx_irq_handler
 *
 * @brief   Tx Interrupt handler for RF module.
 *
 * @param   none
 *
 * @return  none
 */
_attribute_ram_code_ /*__attribute__((optimize("-Os")))*/ void rf_tx_irq_handler(void)
{
	ZB_RADIO_TX_DONE_CLR;

	/* Must change to RX mode first, otherwise the next ACK may miss */
	ZB_RADIO_TRX_SWITCH(RF_MODE_RX, LOGICCHANNEL_TO_PHYSICAL(rf_getChannel()));

	rf_busyFlag &= ~TX_BUSY;

	if (rf_isAck) {
		rf_busyFlag &= ~TX_ACKPACKET;
        rf_isAck = 0;
		rf_ack_buf[5] = 0x02;
	} else {
        /* Direct call the callback to cancel the ACK waiting timer
        more quick. */
        if (rf_txCbFunc) {
            rf_txCbFunc ((void*)RF_SUCC);
        }
	}
}
