/********************************************************************************************************
 * @file    rf.h
 *
 * @brief   This is the header file for B92
 *
 * @author  Driver Group
 * @date    2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *
 *******************************************************************************************************/
#ifndef     RF_H
#define     RF_H

#include "lib/include/sys.h"
#include "gpio.h"

/**********************************************************************************************************************
 *                                         RF  global macro                                                           *
 *********************************************************************************************************************/
/**
 *  @brief This define serve to calculate the DMA length of packet.
 */
#define 	rf_tx_packet_dma_len(rf_data_len)			(((rf_data_len)+3)/4)|(((rf_data_len) % 4)<<22)

/***********************************************************FOR BLE******************************************************/
/**
 *  @brief Those setting of offset according to ble packet format, so this setting for ble only.
 */
#define 	RF_BLE_DMA_RFRX_LEN_HW_INFO					0
#define 	RF_BLE_DMA_RFRX_OFFSET_HEADER				4
#define 	RF_BLE_DMA_RFRX_OFFSET_RFLEN				5
#define 	RF_BLE_DMA_RFRX_OFFSET_DATA					6

/**
 *  @brief According to the packet format find the information of packet through offset.
 */
#define 	rf_ble_dma_rx_offset_crc24(p)				(p[RF_BLE_DMA_RFRX_OFFSET_RFLEN]+6)  //data len:3
#define 	rf_ble_dma_rx_offset_time_stamp(p)			(p[RF_BLE_DMA_RFRX_OFFSET_RFLEN]+9)  //data len:4
#define 	rf_ble_dma_rx_offset_freq_offset(p)			(p[RF_BLE_DMA_RFRX_OFFSET_RFLEN]+13) //data len:2
#define 	rf_ble_dma_rx_offset_rssi(p)				(p[RF_BLE_DMA_RFRX_OFFSET_RFLEN]+15) //data len:1, signed
#define		rf_ble_packet_length_ok(p)					( *((unsigned int*)p) == p[5]+13)    			//dma_len must 4 byte aligned
#define		rf_ble_packet_crc_ok(p)						((p[(p[5]+5 + 11)] & 0x01) == 0x0)

/**
 * @brief		This define for ble debug the effect of rx_dly.
 * 				when this function turn on the time of rx_dly will shorten 6.3us,
 */
#define 	RF_RX_SHORT_MODE_EN			1//In order to debug whether the problem is caused by rx_dly.

/******************************************************FOR TPLL************************************************************/

/**
 *  @brief Those setting of offset according to private tpll packet format, so this setting for ble only.
 */
#define 	RF_PRI_TPLL_DMA_RFRX_OFFSET_RFLEN				4

/**
 *  @brief According to the packet format find the information of packet through offset.
 */

#define 	rf_pri_tpll_dma_rx_offset_crc(p)					(p[RF_PRI_TPLL_DMA_RFRX_OFFSET_RFLEN]+5)  //data len:2
#define 	rf_pri_tpll_dma_rx_offset_time_stamp(p)			(p[RF_PRI_TPLL_DMA_RFRX_OFFSET_RFLEN]+7)  //data len:4
#define 	rf_pri_tpll_dma_rx_offset_freq_offset(p)			(p[RF_PRI_TPLL_DMA_RFRX_OFFSET_RFLEN]+11) //data len:2
#define 	rf_pri_tpll_dma_rx_offset_rssi(p)				(p[RF_PRI_TPLL_DMA_RFRX_OFFSET_RFLEN]+13) //data len:1, signed
#define     rf_pri_tpll_packet_crc_ok(p)            		((p[((p[4] & 0x3f) + 11+3)] & 0x01) == 0x00)


/******************************************************FOR ZIGBEE************************************************************/

/**
 *  @brief Those setting of offset according to zigbee packet format, so this setting for zigbee only.
 */

#define 	RF_ZIGBEE_DMA_RFRX_OFFSET_RFLEN				4

/**
 *  @brief According to the packet format find the information of packet through offset.
 */


#define 	rf_zigbee_dma_rx_offset_crc(p)					(p[RF_ZIGBEE_DMA_RFRX_OFFSET_RFLEN]+3)  //data len:2
#define 	rf_zigbee_dma_rx_offset_time_stamp(p)			(p[RF_ZIGBEE_DMA_RFRX_OFFSET_RFLEN]+5)  //data len:4
#define 	rf_zigbee_dma_rx_offset_freq_offset(p)			(p[RF_ZIGBEE_DMA_RFRX_OFFSET_RFLEN]+9) //data len:2
#define 	rf_zigbee_dma_rx_offset_rssi(p)				(p[RF_ZIGBEE_DMA_RFRX_OFFSET_RFLEN]+11) //data len:1, signed
#define     rf_zigbee_packet_crc_ok(p)       			((p[(p[4]+9+3)] & 0x51) == 0x0)
#define		rf_zigbee_get_payload_len(p)				(p[4])
#define		rf_zigbee_packet_length_ok(p)				(1)
/**
 *  @brief According to different packet format find the crc check digit.
 */
#define     rf_pri_sb_packet_crc_ok(p)              	((p[(reg_rf_sblen & 0x3f)+4+9] & 0x01) == 0x00)
#define     rf_hybee_packet_crc_ok(p)       			((p[(p[4]+9+3)] & 0x51) == 0x0)

#define     rf_ant_packet_crc_ok(p)              	((p[(reg_rf_sblen & 0x3f)+4+9] & 0x01) == 0x00)

/******************************************************FOR HADM************************************************************/

/**
 *  @brief This macro is used to enable interfaces related to HADM
 */
#define 	RF_HADM_EN				0

/**********************************************************************************************************************
 *                                       RF global data type                                                          *
 *********************************************************************************************************************/
#if RF_HADM_EN
/**
 * @brief	Select how you want to start IQ sampling.
 */
typedef enum
{
	RF_HADM_IQ_SAMPLE_SYNC_MODE,
	RF_HADM_IQ_SAMPLE_RXEN_MODE
}rf_hadm_iq_sample_mode_e;

/**
 * @brief	Select whether the antenna clock is normally open or turned on when the antenna is switched.
 */
typedef enum
{
	RF_HADM_ANT_CLK_ALWAYS_ON_MODE,
	RF_HADM_ANT_CLK_SWITCH_ON_MODE
}rf_hadm_ant_clk_mode_e;
#endif

/**
 * @brief	select baseband transmission unit
 */
typedef enum{
	RF_WORLD_WIDTH 	= 0,
	RF_DWORLD_WIDTH = 1,
	RF_QWORLD_WIDTH = 2,
}rf_trans_unit_e;

typedef enum{
	RF_NORMAL_LENGTH_PACKET 	= 0,
	RF_HYPER_LENGTH_PACKET		= 1,
	RF_ADV1_HYPER_LENGTH_PACKET = 2,
	RF_ADV2_HYPER_LENGTH_PACKET = 3
}rf_pkt_len_mode_e;

#if RF_HADM_EN
typedef enum{
	RF_RX_ACL_AOA_AOD_EN  = BIT(0),
	RF_RX_ADV_AOA_AOD_EN  = BIT(1),
	RF_TX_ACL_AOA_AOD_EN  = BIT(2),
	RF_TX_ADV_AOA_AOD_EN  = BIT(3),
	RF_AOA_AOD_OFF        = 0
}rf_aoa_aod_mode_e;

/*
 * @brief  Data length type of AOA/AOD sampling.
 * |                 |                         |
 * | :-------------- | :---------------------- |
 * |   	 <15:8>      |          <7:0>          |
 * |   IQ byte len   |   iq data bit num mode  |
 */
typedef enum{
	IQ_8_BIT_MODE   		= 0x0200,
	IQ_16_BIT_MODE  		= 0x0401,
	IQ_16_BIT_LOW_MODE		= 0x0402,
	IQ_16_BIT_HIGH_MODE		= 0x0403,
	IQ_20_BIT_MODE			= 0x0504
}rf_iq_data_mode_e;



/*
 * @brief  AOA/AOD sample interval time type enumeration.
 * @note   Attention:When the time is 0.25us, it cannot be used with the 20bit iq data type, which will cause the sampling data to overflow.
 *		   In normal mode, the sampling interval of AOA is 4us, and AOD will judge whether the sampling interval is 4us or 2us according to
 *		   CTE info.
 */
typedef enum{
	SAMPLE_NORMAL_INTERVAL   	= 0,//In this case sample interval of aoa is 4us, and aod will judge sample interval is 4us or 2us according to CTE info.
	SAMPLE_2US_INTERVAL   		= 3,
	SAMPLE_1US_INTERVAL  		= 4,
	SAMPLE_0P5US_INTERVAL 		= 5,
	SAMPLE_0P25US_INTERVAL 		= 6
}rf_aoa_aod_sample_interval_time_e;
#endif

/**
 *  @brief  set the modulation index.
 */
typedef enum {
	RF_MI_P0p00 = 0,          /**< MI = 0 */
	RF_MI_P0p076 = 76,          /**< MI = 0.076 */
	RF_MI_P0p32 = 320,		 	/**< MI = 0.32 */
	RF_MI_P0p50 = 500,		  	/**< MI = 0.5 */
	RF_MI_P0p60 = 600,		  	/**< MI = 0.6 */
	RF_MI_P0p70 = 700,		  	/**< MI = 0.7 */
	RF_MI_P0p80 = 800,		  	/**< MI = 0.8 */
	RF_MI_P0p90 = 900,		  	/**< MI = 0.9 */
	RF_MI_P1p20 = 1200,		/**< MI = 1.2 */
	RF_MI_P1p30 = 1300,		/**< MI = 1.3 */
	RF_MI_P1p40 = 1400,		/**< MI = 1.4 */
}rf_mi_value_e;


/**
 *  @brief  select status of rf.
 */
typedef enum {
    RF_MODE_TX = 0,		/**<  Tx mode */
    RF_MODE_RX = 1,		/**<  Rx mode */
    RF_MODE_AUTO=2,		/**<  Auto mode */
	RF_MODE_OFF =3		/**<  TXRX OFF mode */
} rf_status_e;

/**
 *  @brief  Rx fast settle time
 *  @note Some notice for timing sequence.
 *  1:The timing sequence can be configured once during initialization.
 *  2:The timing sequence of tx and rx can be configured separately.
 *  3:Call the enable function rf_tx_fast_settle_en or rf_rx_fast_settle_en when using the configured timing sequence.
 *    To close it, call rf_tx_fast_settle_dis or rf_rx_fast_settle_dis.
 *  4:According to the different parameters, a normal calibration should be done regularly, such as parameter notes.
 */
typedef enum{
	RX_SETTLE_TIME_45US		 = 0, /**<  disable rx_ldo_trim and rx_dcoc calibration,reduce 44.5us of rx settle time.
	                                    Receive for a period of time and then do a normal calibration.*/
	RX_SETTLE_TIME_80US		 = 1  /**<  disable rx_ldo_trim calibration,reduce 4.5us of rx settle time.
	                                    Do a normal calibration at the beginning.*/

}rf_rx_fast_settle_time_e;

/**
 *  @brief  Tx fast settle time
 */
typedef enum{
	TX_SETTLE_TIME_50US	 	= 0, /**<  disable tx_ldo_trim function and tx_hpmc,reduce 58us of tx settle time.
	                                   After frequency hopping, a normal calibration must be done.*/
	TX_SETTLE_TIME_104US    = 1  /**<  disable tx_ldo_trim function,reduce 4.5us of tx settle time.
	                                   Do a normal calibration at the beginning.*/

}rf_tx_fast_settle_time_e;

/**
 *  @brief  LDO trim calibration value
 */
typedef struct
{
	unsigned char LDO_CAL_TRIM;
	unsigned char LDO_RXTXHF_TRIM;
	unsigned char LDO_RXTXLF_TRIM;
	unsigned char LDO_PLL_TRIM;
	unsigned char LDO_VCO_TRIM;
}rf_ldo_trim_t;

/**
 *  @brief  DCOC calibration value
 */
typedef struct
{
	unsigned char DCOC_IDAC;
	unsigned char DCOC_QDAC;
	unsigned char DCOC_IADC_OFFSET;
	unsigned char DCOC_QADC_OFFSET;
}rf_dcoc_cal_t;

/**
 *  @brief  RCCAL calibration value
 */
typedef struct
{
	unsigned char RCCAL_CODE;
	unsigned char CBPF_CCODE_L;
	unsigned char CBPF_CCODE_H;
}rf_rccal_cal_t;

typedef struct
{
	unsigned short cal_tbl[40];
	rf_ldo_trim_t	ldo_trim;
	rf_dcoc_cal_t   dcoc_cal;
	rf_rccal_cal_t  rccal_cal;
}rf_fast_settle_t;

#if RF_HADM_EN
/**
 * @brief	Define function to set tx channel or rx channel.
 */
typedef enum
{
	TX_CHANNEL		= 0,
	RX_CHANNEL		= 1,
}rf_trx_chn_e;
#endif

/**
 *  @brief  Define power list of RF.
 *  @note	The energy value with the 1.8V keyword is the energy value when the GPIO output voltage is 1.8V
 *          (i.e. when the GPIO voltage in sys_init is configured to GPIO_VOLTAGE_1V8)
 *          Since the GPIO output voltage is set to 1.8V, the VDDO3 voltage supplying the RF is 1.8V, which will cause the energy to drop.
 *          Therefore, when the GPIO output voltage is set to 1.8V, the energy setting with the 1.8V keyword should be selected.
 */
typedef enum {
	 /*VBAT*/
	 RF_POWER_P9p90dBm  = 63, /**<  9.9 dbm */
	 RF_POWER_P9p61dBm  = 59, /**<  9.6 dbm */
	 RF_POWER_P9p15dBm  = 54, /**<  9.1 dbm */
	 RF_POWER_P8p82dBm  = 51, /**<  8.8 dbm */
	 RF_POWER_P8p25dBm  = 48, /**<  8.2 dbm */
	 RF_POWER_P7p72dBm  = 44, /**<  7.7 dbm */
	 RF_POWER_P7p00dBm  = 40, /**<  7.0 dbm */
	 RF_POWER_P6p32dBm  = 36, /**<  6.3 dbm */
	 RF_POWER_P5p21dBm  = 32, /**<  5.2 dbm */

	 /*VANT*/
	 RF_POWER_P4p61dBm  = BIT(7) | 63,   /**<   4.6 dbm */
	 RF_POWER_P4p02dBm  = BIT(7) | 55,   /**<   4.0 dbm */
	 RF_POWER_P3p51dBm  = BIT(7) | 49,   /**<   3.5 dbm */
	 RF_POWER_P3p00dBm  = BIT(7) | 45,   /**<   3.0 dbm */
	 RF_POWER_P2p51dBm  = BIT(7) | 41,   /**<   2.5 dbm */
	 RF_POWER_P2p01dBm  = BIT(7) | 38,   /**<   2.0 dbm */
	 RF_POWER_P1p62dBm  = BIT(7) | 35,   /**<   1.6 dbm */
	 RF_POWER_P1p03dBm  = BIT(7) | 33,   /**<   1.0 dbm */
	 RF_POWER_P0p31dBm  = BIT(7) | 29,   /**<   0.3 dbm */
	 RF_POWER_P0p01dBm  = BIT(7) | 28,   /**<   0.0 dbm */
	 RF_POWER_N0p43dBm  = BIT(7) | 26,   /**<  -0.4 dbm */
	 RF_POWER_N1p15dBm  = BIT(7) | 24,   /**<  -1.0 dbm */
	 RF_POWER_N1p52dBm  = BIT(7) | 23,   /**<  -1.5 dbm */
	 RF_POWER_N2p51dBm  = BIT(7) | 20,   /**<  -2.5 dbm */
	 RF_POWER_N3p95dBm  = BIT(7) | 17,   /**<  -3.9 dbm */
	 RF_POWER_N5p94dBm  = BIT(7) | 13,   /**<  -5.9 dbm */
	 RF_POWER_N9p03dBm  = BIT(7) | 9,    /**<  -9.0 dbm */
	 RF_POWER_N13p42dBm = BIT(7) | 5,    /**<  -13.4 dbm */
	 RF_POWER_N22p53dBm = BIT(7) | 2,    /**<  -22.5 dbm */

	 RF_POWER_N30dBm    = 0xff,          /**<  -30 dbm */
	 RF_POWER_N50dBm    = BIT(7) | 0,    /**<  -50 dbm */

	 /*VBAT*/
	 RF_1V8_POWER_P6p32dBm = 63, /**<  6.3 dbm */
	 RF_1V8_POWER_P5p81dBm = 56, /**<  5.8 dbm */
	 RF_1V8_POWER_P5p00dBm = 50, /**<  5.0 dbm */
	 RF_1V8_POWER_P4p50dBm = 45, /**<  4.5 dbm */

	 /*VANT*/
	 RF_1V8_POWER_P4p23dBm  = BIT(7) | 63,   /**<   4.2 dbm */
	 RF_1V8_POWER_P3p91dBm  = BIT(7) | 55,   /**<   3.9 dbm */
	 RF_1V8_POWER_P3p43dBm  = BIT(7) | 50,   /**<   3.4 dbm */
	 RF_1V8_POWER_P2p94dBm  = BIT(7) | 45,   /**<   2.9 dbm */
	 RF_1V8_POWER_P2p42dBm  = BIT(7) | 41,   /**<   2.4 dbm */
	 RF_1V8_POWER_P1p95dBm  = BIT(7) | 38,   /**<   1.9 dbm */
	 RF_1V8_POWER_P1p21dBm  = BIT(7) | 34,   /**<   1.2 dbm */
	 RF_1V8_POWER_P0p82dBm  = BIT(7) | 31,   /**<   0.8 dbm */
	 RF_1V8_POWER_P0p21dBm  = BIT(7) | 29,   /**<   0.0 dbm */
	 RF_1V8_POWER_N0p34dBm  = BIT(7) | 27,   /**<  -0.3 dbm */
	 RF_1V8_POWER_N1p33dBm  = BIT(7) | 23,   /**<  -1.3 dbm */
	 RF_1V8_POWER_N1p80dBm  = BIT(7) | 22,   /**<  -1.8 dbm */
	 RF_1V8_POWER_N2p53dBm  = BIT(7) | 20,   /**<  -2.5 dbm */
	 RF_1V8_POWER_N3p31dBm  = BIT(7) | 18,   /**<  -3.3 dbm */
	 RF_1V8_POWER_N5p32dBm  = BIT(7) | 14,   /**<  -5.3 dbm */
	 RF_1V8_POWER_N7p32dBm  = BIT(7) | 11,   /**<  -7.3 dbm */
	 RF_1V8_POWER_N9p81dBm  = BIT(7) | 9,     /**<  -9.8 dbm */
	 RF_1V8_POWER_N12p42dBm = BIT(7) | 6,    /**<  -12.4 dbm */
	 RF_1V8_POWER_N16p01dBm = BIT(7) | 4,    /**<  -16.0 dbm */
	 RF_1V8_POWER_N27p51dBm = BIT(7) | 1,    /**<  -27.5 dbm */


	 RF_1V8_POWER_N30p00dBm    = 0xff,          /**<  -30 dbm */
	 RF_1V8_POWER_N50p00dBm    = BIT(7) | 0,    /**<  -50 dbm */
} rf_power_level_e;

/**
 *  @brief  Define power index list of RF.
 *  @note	The energy value with the 1.8V keyword is the energy value when the GPIO output voltage is 1.8V
 *          (i.e. when the GPIO voltage in sys_init is configured to GPIO_VOLTAGE_1V8)
 *          Since the GPIO output voltage is set to 1.8V, the VDDO3 voltage supplying the RF is 1.8V, which will cause the energy to drop.
 *          Therefore, when the GPIO output voltage is set to 1.8V, the energy setting with the 1.8V keyword should be selected.
 */

typedef enum {

	 /*VBAT*/
	 RF_POWER_INDEX_P9p90dBm,    /**<  power index of 9.9 dbm */
	 RF_POWER_INDEX_P9p61dBm,    /**<  power index of 9.6 dbm */
	 RF_POWER_INDEX_P9p15dBm,    /**<  power index of 9.1 dbm */
	 RF_POWER_INDEX_P8p82dBm,    /**<  power index of 8.8 dbm */
	 RF_POWER_INDEX_P8p25dBm,    /**<  power index of 8.2 dbm */
	 RF_POWER_INDEX_P7p72dBm,    /**<  power index of 7.7 dbm */
	 RF_POWER_INDEX_P7p00dBm,    /**<  power index of 7.0 dbm */
	 RF_POWER_INDEX_P6p32dBm,    /**<  power index of 6.3 dbm */
	 RF_POWER_INDEX_P5p21dBm,    /**<  power index of 5.2 dbm */

	 /*VANT*/
	 RF_POWER_INDEX_P4p61dBm,    /**<  power index of 4.6 dbm */
	 RF_POWER_INDEX_P4p02dBm,    /**<  power index of 4.0 dbm */
	 RF_POWER_INDEX_P3p51dBm,    /**<  power index of 3.5 dbm */
	 RF_POWER_INDEX_P3p00dBm,    /**<  power index of 3.0 dbm */
	 RF_POWER_INDEX_P2p51dBm,    /**<  power index of 2.5 dbm */
	 RF_POWER_INDEX_P2p01dBm,    /**<  power index of 2.0 dbm */
	 RF_POWER_INDEX_P1p62dBm,    /**<  power index of 1.6 dbm */
	 RF_POWER_INDEX_P1p03dBm,    /**<  power index of 1.0 dbm */
	 RF_POWER_INDEX_P0p31dBm,    /**<  power index of 0.3 dbm */
	 RF_POWER_INDEX_P0p01dBm,    /**<  power index of 0.0 dbm */
	 RF_POWER_INDEX_N0p43dBm,    /**<  power index of -0.4 dbm */
	 RF_POWER_INDEX_N1p15dBm,    /**<  power index of -1.0 dbm */
	 RF_POWER_INDEX_N1p52dBm,    /**<  power index of -1.5 dbm */
	 RF_POWER_INDEX_N2p51dBm,    /**<  power index of -2.5 dbm */
	 RF_POWER_INDEX_N3p95dBm,    /**<  power index of -3.9 dbm */
	 RF_POWER_INDEX_N5p94dBm,    /**<  power index of -5.9 dbm */
	 RF_POWER_INDEX_N9p03dBm,    /**<  power index of -9.0 dbm */
	 RF_POWER_INDEX_N13p42dBm,   /**<  power index of -13.4 dbm */
	 RF_POWER_INDEX_N22p53dBm,   /**<  power index of -22.5 dbm */

	 RF_POWER_INDEX_N30dBm,      /**<  power index of -30 dbm */
	 RF_POWER_INDEX_N50dBm,      /**<  power index of -50 dbm */

	 /*VBAT*/
	 RF_1V8_POWER_INDEX_P6p32dBm,    /**<  power index of 6.3 dbm */
	 RF_1V8_POWER_INDEX_P5p81dBm,    /**<  power index of 5.8 dbm */
	 RF_1V8_POWER_INDEX_P5p00dBm,    /**<  power index of 5.0 dbm */
	 RF_1V8_POWER_INDEX_P4p50dBm,    /**<  power index of 4.5 dbm */

	 /*VANT*/
	 RF_1V8_POWER_INDEX_P4p23dBm,    /**<  power index of 4.2 dbm */
	 RF_1V8_POWER_INDEX_P3p91dBm,    /**<  power index of 3.9 dbm */
	 RF_1V8_POWER_INDEX_P3p43dBm,    /**<  power index of 3.4 dbm */
	 RF_1V8_POWER_INDEX_P2p94dBm,    /**<  power index of 2.9 dbm */
	 RF_1V8_POWER_INDEX_P2p42dBm,    /**<  power index of 2.4 dbm */
	 RF_1V8_POWER_INDEX_P1p95dBm,    /**<  power index of 1.9 dbm */
	 RF_1V8_POWER_INDEX_P1p21dBm,    /**<  power index of 1.2 dbm */
	 RF_1V8_POWER_INDEX_P0p82dBm,    /**<  power index of 0.8 dbm */
	 RF_1V8_POWER_INDEX_P0p21dBm,    /**<  power index of 0.0 dbm */
	 RF_1V8_POWER_INDEX_N0p34dBm,    /**<  power index of -0.3 dbm */
	 RF_1V8_POWER_INDEX_N1p33dBm,    /**<  power index of -1.3 dbm */
	 RF_1V8_POWER_INDEX_N1p80dBm,    /**<  power index of -1.8 dbm */
	 RF_1V8_POWER_INDEX_N2p53dBm,    /**<  power index of -2.5 dbm */
	 RF_1V8_POWER_INDEX_N3p31dBm,    /**<  power index of -3.3 dbm */
	 RF_1V8_POWER_INDEX_N5p32dBm,    /**<  power index of -5.3 dbm */
	 RF_1V8_POWER_INDEX_N7p32dBm,    /**<  power index of -7.3 dbm */
	 RF_1V8_POWER_INDEX_N9p81dBm,    /**<  power index of -9.8 dbm */
	 RF_1V8_POWER_INDEX_N12p42dBm,   /**<  power index of -12.4 dbm */
	 RF_1V8_POWER_INDEX_N16p01dBm,   /**<  power index of -16.0 dbm */
	 RF_1V8_POWER_INDEX_N27p51dBm,   /**<  power index of -27.5 dbm */


	 RF_1V8_POWER_INDEX_N30p00dBm,   /**<  power index of -30 dbm */
	 RF_1V8_POWER_INDEX_N50p00dBm,   /**<  power index of -50 dbm */


} rf_power_level_index_e;



/**
 *  @brief  Define RF mode.
 */
typedef enum {
	RF_MODE_BLE_2M 		   =    BIT(0),		/**< ble 2m mode */
	RF_MODE_BLE_1M 		   = 	BIT(1),		/**< ble 1M mode */
    RF_MODE_BLE_1M_NO_PN   =    BIT(2),		/**< ble 1M close pn mode */
	RF_MODE_ZIGBEE_250K    =    BIT(3),		/**< zigbee 250K mode */
    RF_MODE_LR_S2_500K     =    BIT(4),		/**< ble 500K mode */
    RF_MODE_LR_S8_125K     =    BIT(5),		/**< ble 125K mode */
    RF_MODE_PRIVATE_250K   =    BIT(6),		/**< private 250K mode */
    RF_MODE_PRIVATE_500K   =    BIT(7),		/**< private 500K mode */
    RF_MODE_PRIVATE_1M     =    BIT(8),		/**< private 1M mode */
    RF_MODE_PRIVATE_2M     =    BIT(9),		/**< private 2M mode */
    RF_MODE_ANT     	   =    BIT(10),	/**< ant mode */
    RF_MODE_BLE_2M_NO_PN   =    BIT(11),	/**< ble 2M close pn mode */
    RF_MODE_HYBEE_1M   	   =    BIT(12),	/**< hybee 1M mode */
    RF_MODE_HYBEE_2M   	   =    BIT(13),	/**< hybee 2M mode */
    RF_MODE_HYBEE_500K     =    BIT(14),	/**< hybee 500K mode */
} rf_mode_e;



/**
 *  @brief  Define RF channel.
 */
typedef enum {
	 RF_CHANNEL_0   =    BIT(0),	/**< RF channel 0 */
	 RF_CHANNEL_1   =    BIT(1),	/**< RF channel 1 */
	 RF_CHANNEL_2   =    BIT(2),	/**< RF channel 2 */
	 RF_CHANNEL_3   =    BIT(3),	/**< RF channel 3 */
	 RF_CHANNEL_4   =    BIT(4),	/**< RF channel 4 */
	 RF_CHANNEL_5   =    BIT(5),	/**< RF channel 5 */
	 RF_CHANNEL_NONE =   0x00,		/**< none RF channel*/
	 RF_CHANNEL_ALL =    0x0f,		/**< all RF channel */
} rf_channel_e;



/**********************************************************************************************************************
 *                                         RF global constants                                                        *
 *********************************************************************************************************************/
extern const rf_power_level_e rf_power_Level_list[60];


/**********************************************************************************************************************
 *                                         RF function declaration                                                    *
 *********************************************************************************************************************/
/**
 * @brief	    This function is used to enable the ldo rxtxlf bypass function, and the calibration value
 * 				written by the software will take effect after enabling.
 * @param[in]	none.
 * @return	 	none.
 */
static inline void rf_ldot_ldo_rxtxlf_bypass_en(void)
{
	write_reg8(0x17074e,0x45);//CBPF_TRIM_I && CBPF_TRIM_Q
	write_reg8(0x17074c,0x02);//LNA_ITRIM=0x01(default)(change to 0x02[TBD])
	write_reg8(0x1706e4,read_reg8(0x1706e4)|BIT(1));
}

/**
 * @brief	    This function is used to close the ldo rxtxlf bypass function, and the hardware will
 * 				automatically perform the calibration function after closing.
 * @param[in]	none.
 * @return	 	none.
 */
static inline void rf_ldot_ldo_rxtxlf_bypass_dis(void)
{
	write_reg8(0x17074e,0x40);//CBPF_TRIM_I && CBPF_TRIM_Q
	write_reg8(0x17074c,0x11);//LNA_ITRIM=0x01(default)(change to 0x02[TBD])
	write_reg8(0x1706e4,read_reg8(0x1706e4)&(~BIT(1)));
}

/**
 * @brief      This function serves to optimize RF performance
 * 			   This function must be called every time rx is turned on,
 * 			   and is called by an internal function.
 * 			   If there are other requirements that need to be called,
 * 			   turn off rx first, then call it again to make sure the Settings take effect
 * @param[in]  none
 * @return     none
 * @note	   1.Call this function after turning on rx 30us, and the calibration value set by the function
 * 			      will take effect after calling rf_ldot_ldo_rxtxlf_bypass_en;if automatic calibration is
 * 			      required, you can use rf_ldot_ldo_rxtxlf_bypass_dis to turn off the bypass function; how to
 * 			      use it can refer to bqb.c file or rf_emi_rx in emi.c
 *			   2. After using rf_ldot_ldo_rxtxlf_bypass_dis to turn off the bypass function and enter tx/rx
 *			      automatic calibration, to use this function again, you need to call the rf_set_rxpara function
 *			      again after entering rx 30us.
 *
 */

static inline void rf_set_rxpara(void)
{
	unsigned char reg_calibration=0;
	reg_calibration = ((read_reg8(0x1706ed)&0xf)<<2)|((read_reg8(0x1706ec)&0xc0)>>6);
	if(reg_calibration>10)	reg_calibration -= 10;
	write_reg8(0x1706e5,(read_reg8(0x1706e5)&0xc0)|reg_calibration);
}


/**
 * @brief   	This function serves to judge the statue of  RF receive.
 * @return  	-#0:idle
 * 				-#1:rx_busy
 */
static inline unsigned char rf_receiving_flag(void)
{
	//if the value of [2:0] of the reg_0x170040 isn't 0 , it means that the RF is in the receiving packet phase.(confirmed by junwen).
	return ((read_reg8(0x170040)&0x07) > 1);
}

/**
 * @brief	This function serve to judge whether it is in a certain state.
 * @param[in]	status	- Option of rf state machine status.
 * @return		-#0:Not in parameter setting state
 * 				-#1:In parameter setting state
 */
static inline unsigned short rf_get_state_machine_status(state_machine_status_e status)
{
	return	status == read_reg8(0x170224);
}
/**
 * @brief	  	This function serves to set the which irq enable.
 * @param[in]	mask 	- Options that need to be enabled.
 * @return	 	Yes: 1, NO: 0.
 */
static inline void rf_set_irq_mask(rf_irq_e mask)
{
	BM_SET(reg_rf_irq_mask,mask);
}


/**
 * @brief	  	This function serves to clear the TX/RX irq mask.
 * @param[in]   mask 	- RX/TX irq value.
 * @return	 	none.
 */
static inline void rf_clr_irq_mask(rf_irq_e mask)
{
	BM_CLR(reg_rf_irq_mask ,mask);
}


/**
 *	@brief	  	This function serves to judge whether it is in a certain state.
 *	@param[in]	mask 	- RX/TX irq status.
 *	@return	 	Yes: 1, NO: 0.
 */
static inline unsigned short rf_get_irq_status(rf_irq_e status)
{
	return ((unsigned short )BM_IS_SET(reg_rf_irq_status,status));
}


/**
 *@brief	This function serves to clear the Tx/Rx finish flag bit.
 *			After all packet data are sent, corresponding Tx finish flag bit
 *			will be set as 1.By reading this flag bit, it can check whether
 *			packet transmission is finished. After the check, it is needed to
 *			manually clear this flag bit so as to avoid misjudgment.
 *@return	none.
 */
static inline void rf_clr_irq_status(rf_irq_e status)
{
	 reg_rf_irq_status = status;
}


/**
 * @brief   	This function serves to settle adjust for RF Tx.This function for adjust the differ time
 * 				when rx_dly enable.
 * @param[in]   txstl_us   - adjust TX settle time.
 * @return  	none.
 */
static inline void 	rf_tx_settle_us(unsigned short txstl_us)
{
	REG_ADDR16(0x80170204) = txstl_us;
}


/**
 * @brief   	This function serves to set RF access code.
 * @param[in]   acc   - the value of access code.
 * @return  	none.
 */
static inline void rf_access_code_comm (unsigned int acc)
{
	reg_rf_access_code = acc;
	//The following two lines of code are for trigger access code in S2,S8 mode.It has no effect on other modes.
	reg_rf_modem_mode_cfg_rx1_0 &= ~FLD_RF_LR_TRIG_MODE;
	write_reg8(0x170425,read_reg8(0x170425)|0x01);
}


/**
 * @brief		this function is to enable/disable each access_code channel for
 *				RF Rx terminal.
 * @param[in]	pipe  	- Bit0~bit5 correspond to channel 0~5, respectively.
 *					  	- #0:Disable.
 *					  	- #1:Enable.
 *						  If "enable" is set as 0x3f (i.e. 00111111),
 *						  all access_code channels (0~5) are enabled.
 * @return	 	none
 */
static inline void rf_rx_acc_code_pipe_en(rf_channel_e pipe)
{
    write_reg8(0x17044d, (read_reg8(0x17044d)&0xc0) | pipe); //rx_access_code_chn_en
}


/**
 * @brief		this function is to select access_code channel for RF tx terminal.
 * @param[in]	pipe  	- Bit0~bit2 the value correspond to channel 0~5, respectively.
 *						  if value > 5 enable channel 5.And only 1 channel can be selected every time.
 *						- #0:Disable.
 *						- #1:Enable.
 *						  If "enable" is set as 0x7 (i.e. 0111),
 *						  the access_code channel (5) is enabled.
 * @return	 	none
 */
static inline void rf_tx_acc_code_pipe_en(rf_channel_e pipe)
{
    write_reg8(0x170215, (read_reg8(0x170215)&0xf8) | pipe); //Tx_Channel_man[2:0]
}


/**
 * @brief 	  This function serves to reset RF Tx/Rx mode.
 * @return 	  none.
 */
static inline void rf_set_tx_rx_off(void)
{
	write_reg8 (0x80170216, 0x29);
	write_reg8 (0x80170028, 0x80);	// rx disable
	write_reg8 (0x80170202, 0x45);	// reset tx/rx state machine
}


/**
 * @brief    This function serves to turn off RF auto mode.
 * @return   none.
 * @note	 Attention: When forcibly stopping the state machine through this interface, it must be ensured
 * 			 that rx is not in the process of receiving packets.Otherwise, an error may be caused.To determine
 * 			 whether the packet is being received, you can use the function rf_receiving_flag.
 */
static inline void rf_set_tx_rx_off_auto_mode(void)
{
	write_reg8 (0x80170200, 0x80);
}


/**
 * @brief    This function serves to set CRC advantage.
 * @return   none.
 */
static inline void rf_set_ble_crc_adv (void)
{
	write_reg32 (0x80170024, 0x555555);
}


/**
 * @brief  	  	This function serves to set CRC value for RF.
 * @param[in]  	crc  - CRC value.
 * @return 		none.
 */
static inline void rf_set_ble_crc_value (unsigned int crc)
{
	write_reg32 (0x80170024, crc);
}


/**
 * @brief  	   This function serves to set the max length of rx packet.Use byte_len to limit what DMA
 * 			   moves out will not exceed the buffer size we define.And old chip do this through dma size.
 * @param[in]  byte_len  - The longest of rx packet.
 * @return     none.
 */
static inline void rf_set_rx_maxlen(unsigned int byte_len)
{
	reg_rf_rxtmaxlen0 = byte_len&0xff;
	reg_rf_rxtmaxlen1 = (byte_len>>8)&0xff;
}


/**
 * @brief		This function serve to rx dma fifo size.
 * @param[in]	fifo_byte_size - The length of one dma fifo,the range is 1~0xffff(the corresponding number of fifo bytes is fifo_byte_size).
 * @return		none
 */
static inline void rf_set_rx_dma_fifo_size(unsigned short fifo_byte_size)
{
	reg_rf_bb_rx_size = fifo_byte_size&0xff;
	reg_rf_bb_rx_size_h = fifo_byte_size>>8;
}
/**
 * @brief		This function serve to set rx dma wptr.
 * @param[in]	fifo_num	-This parameter is used to set the mask value for the number of enabled FIFOs. The value of the mask must (0x00,0x01,0x03,0x07,0x0f,0x1f).
 * 						 	 The number of FIFOs enabled is the value of wptr_mask plus 1.(0x01,0x02,0x04,0x08,0x10,0x20)
 * @return 		none
 */
static inline void rf_set_rx_dma_fifo_num(unsigned char fifo_num)
{
	reg_rf_rx_wptr_mask = fifo_num;
}


/**
 * @brief	  	This function serves to DMA rxFIFO address
 *	            The function apply to the configuration of one rxFiFO when receiving packets,
 *	            In this case,the rxFiFo address can be changed every time a packet is received
 *	            Before setting, call the function "rf_set_rx_dma" to clear DMA fifo mask value(set 0)
 * @param[in]	rx_addr   - The address store receive packet.
 * @return	 	none
 * @note		rx_addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
static inline void rf_set_rx_buffer(unsigned char *rx_addr)
{
	rx_addr += 4;
	dma_set_dst_address(DMA1,(unsigned int)(rx_addr));
}
/**
 * @brief		This function serve to set the number of tx dma fifo.
 * @param[in]	fifo_dep - the number of dma fifo is 2 to the power of fifo_dep.
 * @return		none
 */
static inline void rf_set_tx_dma_fifo_num(unsigned char fifo_num)
{
	reg_rf_bb_tx_chn_dep = fifo_num;//tx_chn_dep = 2^2 =4 (have 4 fifo)
}

/**
 * @brief		This function serve to set the number of tx dma fifo.
 * @param[in]	fifo_byte_size - the size of each dma fifo.
 * @return		none
 */
static inline void rf_set_tx_dma_fifo_size(unsigned short fifo_byte_size)
{
	reg_rf_bb_tx_size	= fifo_byte_size&0xff;//tx_idx_addr = {tx_chn_adr*bb_tx_size,4'b0}// in this setting the max data in one dma buffer is 0x20<<4.And the The product of fifo_dep and bytesize cannot exceed 0xfff.
	reg_rf_bb_tx_size_h = fifo_byte_size>>8;
}
/**
 * @brief   This function serves to set RF tx settle time.
 * @tx_stl_us  tx settle time,the unit is us.The max value of this param is 0xfff;The default settling time value is 150us.
 * 			   The typical value is 113us (tx_settle time should not be less than this value).
 * @return  none.
 * @note	   Attention:It is not necessary to call this function to adjust the settling time in the normal sending state.
 */
static inline void rf_set_tx_settle_time(unsigned short tx_stl_us )
{
	tx_stl_us &= 0x0fff;
	write_reg16(0x170204, (read_reg16(0x170204)& 0xf000) |(tx_stl_us - 1));
}
/**
 * @brief   This function serves to set RF tx settle time and rx settle time.
 * @rx_stl_us  rx settle time,the unit is us.The max value of this param is 0xfff;The default settling time value is 150us.
 * 			   The typical value is 85us (rx_settle time should not be less than this value).
 * @return  none.
 * @note	   Attention:It is not necessary to call this function to adjust the settling time in the normal packet receiving state.
 */
static inline void rf_set_rx_settle_time( unsigned short rx_stl_us )
{
	 rx_stl_us &= 0x0fff;
	 write_reg16(0x17020c, (read_reg16(0x17020c)& 0xf000) |(rx_stl_us - 1));
}

/**
 * @brief	This function serve to get ptx wptr.
 * @param[in]	pipe_id	-	The number of tx fifo.0<= pipe_id <=5.
 * @return		The write pointer of the tx.
 */
static inline unsigned char rf_get_tx_wptr(unsigned char pipe_id)
{
	return reg_rf_dma_tx_wptr(pipe_id);
}

/**
 * @brief	This function serve to update the wptr of tx terminal.
 * @param[in]	pipe_id	-	The number of pipe which need to update wptr.
 * @param[in]	wptr	-	The pointer of write in tx terminal.
 * @return		none
 */
static inline void rf_set_tx_wptr(unsigned char pipe_id,unsigned char wptr)
{
	reg_rf_dma_tx_wptr(pipe_id) = wptr;
}


/**
 * @brief	This function serve to clear the writer pointer of tx terminal.
 * @param[in]	pipe_id	-	The number of tx DMA.0<= pipe_id <=5.
 * @return	none.
 */
static inline void rf_clr_tx_wptr(unsigned char pipe_id)
{
	reg_rf_dma_tx_wptr(pipe_id) = 0;
}

/**
 * @brief	This function serve to get ptx rptr.
 * @param[in]	pipe_id	-The number of tx pipe.0<= pipe_id <=5.
 * @return		The read pointer of the tx.
 */
static inline unsigned char rf_get_tx_rptr(unsigned char pipe_id)
{
	return reg_rf_dma_tx_rptr(pipe_id);
}


/**
 * @brief	This function serve to clear read pointer of tx terminal.
 * @param[in]	pipe_id	-	The number of tx DMA.0<= pipe_id <=5.
 * @return	none.
 */
static inline void rf_clr_tx_rptr(unsigned char pipe_id)
{
	reg_rf_dma_tx_rptr(pipe_id) = 0x80;
}

/**
 * @brief 	This function serve to get the pointer of read in rx terminal.
 * @return	wptr	-	The pointer of rx_rptr.
 */
static inline unsigned char rf_get_rx_rptr(void)
{
	return reg_rf_dma_rx_rptr;
}

/**
 * @brief	This function serve to clear read pointer of rx terminal.
 * @return	none.
 */
static inline void rf_clr_rx_rptr(void)
{
	write_reg8(0x1004f5, 0x80); //clear rptr
}


/**
 * @brief 	This function serve to get the pointer of write in rx terminal.
 * @return	wptr	-	The pointer of rx_wptr.
 */
static inline unsigned char rf_get_rx_wptr(void)
{
	return reg_rf_dma_rx_wptr;
}


/**
 * @brief	This function serve to get ptx initial pid value.
 * @return	The  value of ptx pid before update.
 */
static inline unsigned char rf_get_ptx_pid(void)
{
	return ((reg_rf_ll_ctrl_1 & 0xc0)>>6);
}


/**
 * @brief	This function serve to set the new ptx pid value.
 * @param[in]	pipe_id	-The number of pipe.0<= pipe_id <=5.
 * @return	none
 */
static inline void rf_set_ptx_pid(unsigned char pipe_pid)
{
	reg_rf_ll_ctrl_1 |= (pipe_pid << 6);
}


/**
 * @brief      This function serves to initiate information of RF.
 * @return	   none.
 */
void rf_mode_init(void);


/**
 * @brief     This function serves to set ble_1M  mode of RF.
 * @return	  none.
 */
void rf_set_ble_1M_mode(void);


/**
 * @brief     This function serves to  set ble_1M_NO_PN  mode of RF.
 * @return	  none.
 */
void rf_set_ble_1M_NO_PN_mode(void);


/**
 * @brief     This function serves to set ble_2M  mode of RF.
 * @return	  none.
 */
void rf_set_ble_2M_mode(void);


/**
 * @brief     This function serves to set ble_2M_NO_PN  mode of RF.
 * @return	  none.
 */
void rf_set_ble_2M_NO_PN_mode(void);


/**
 * @brief     This function serves to set ble_500K  mode of RF.
 * @return	  none.
 */
void rf_set_ble_500K_mode(void);


/**
 * @brief     This function serves to  set zigbee_125K  mode of RF.
 * @return	  none.
 */
void rf_set_ble_125K_mode(void);


/**
 * @brief     This function serves to set zigbee_250K  mode of RF.
 * @return	  none.
 */
void rf_set_zigbee_250K_mode(void);


/**
 * @brief     This function serves to set pri_250K  mode of RF.
 * @return	  none.
 */
void rf_set_pri_250K_mode(void);


/**
 * @brief     This function serves to  set pri_500K  mode of RF.
 * @return	  none.
 */
void rf_set_pri_500K_mode(void);


/**
 * @brief     This function serves to set pri_1M  mode of RF.
 * @return	  none.
 */
void rf_set_pri_1M_mode(void);


/**
 * @brief     This function serves to set pri_2M  mode of RF.
 * @return	  none.
 */
void rf_set_pri_2M_mode(void);


/**
 * @brief     This function serves to set hybee_500K  mode of RF.
 * @return	   none.
 */
void rf_set_hybee_500K_mode(void);


/**
 * @brief     This function serves to set hybee_2M  mode of RF.
 * @return	  none.
 */
void rf_set_hybee_2M_mode(void);


/**
 * @brief     This function serves to set hybee_1M  mode of RF.
 * @return	   none.
 */
void rf_set_hybee_1M_mode(void);


/**
 * @brief     This function serves to set ant  mode of RF.
 * @return	   none.
 */
void rf_set_ant_mode(void);

/**
 * @brief     This function serves to set RF tx DMA setting.
 * @param[in] none
 * @return	  none.
 */
void rf_set_tx_dma_config(void);
/**
 * @brief     This function serves to set RF tx DMA setting.
 * @param[in] fifo_depth 		- tx chn deep,fifo_depth range: 0~5,Number of fifo=2^fifo_depth.
 * @param[in] fifo_byte_size 	- The length of one dma fifo,the range is 1~0xffff(the corresponding number of fifo bytes is fifo_byte_size).
 * @return	  none.
 */
void rf_set_tx_dma(unsigned char fifo_depth,unsigned short fifo_byte_size);


/**
 * @brief      This function serves to rx dma setting.
 * @param[in]  buff - This parameter is the first address of the received data buffer, which must be 4 bytes aligned, otherwise the program will enter an exception.
 * @attention  The first four bytes in the buffer of the received data are the length of the received data.
 *             The actual buffer size that the user needs to set needs to be noted on two points:
 *			   -# you need to leave 4bytes of space for the length information.
 *			   -# dma is transmitted in accordance with 4bytes, so the length of the buffer needs to be a multiple of 4. Otherwise, there may be an out-of-bounds problem
 *			   For example, the actual received data length is 5bytes, the minimum value of the actual buffer size that the user needs to set is 12bytes, and the calculation of 12bytes is explained as follows::
 *			   4bytes (length information) + 5bytes (data) + 3bytes (the number of additional bytes to prevent out-of-bounds)
 * @param[in]  wptr_mask  	   - This parameter is used to set the mask value for the number of enabled FIFOs. The value of the mask must (0x00,0x01,0x03,0x07,0x0f,0x1f).
 * 								 The number of FIFOs enabled is the value of wptr_mask plus 1.(0x01,0x02,0x04,0x08,0x10,0x20)
 * @param[in]  fifo_byte_size  - The length of one dma fifo,the range is 1~0xffff(the corresponding number of fifo bytes is fifo_byte_size).
 * @return     none.
 */
void rf_set_rx_dma(unsigned char *buff,unsigned char wptr_mask,unsigned short fifo_byte_size);

/**
 * @brief		This function serve to rx dma config
 * @param[in]	none
 * @return		none
 */
void rf_set_rx_dma_config(void);

/**
 * @brief     This function serves to trigger srx on.
 * @param[in] tick  - Trigger rx receive packet after tick delay.
 * @return	  none.
 */
void rf_start_srx(unsigned int tick);


/**
 * @brief	  	This function serves to get rssi.
 * @return	 	rssi value.
 */
signed char rf_get_rssi(void);


/**
 * @brief	  	This function serves to set pin for RFFE of RF.
 * @param[in]   tx_pin   - select pin as rffe to send.
 * @param[in]   rx_pin   - select pin as rffe to receive.
 * @return	 	none.
 */
void rf_set_rffe_pin(gpio_func_pin_e tx_pin, gpio_func_pin_e rx_pin);



/**
 * @brief  	 	This function serves to set RF Tx mode.
 * @return  	none.
 */
void rf_set_txmode(void);


/**
 * @brief	  	This function serves to set RF Tx packet address to DMA src_addr.
 * @param[in]	addr   - The packet address which to send.
 * @return	 	none.
 * @note		addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
_attribute_ram_code_sec_ void rf_tx_pkt(void* addr);


/**
 * @brief	  	This function serves to judge RF Tx/Rx state.
 * @param[in]	rf_status   - Tx/Rx status.
 * @param[in]	rf_channel  - This param serve to set frequency channel(2400+rf_channel) .
 * @return	 	Whether the setting is successful(-1:failed;else success).
 */
int rf_set_trx_state(rf_status_e rf_status, signed char rf_channel);


/**
 * @brief   	This function serves to set rf channel for all mode.The actual channel set by this function is 2400+chn.
 * @param[in]   chn   - That you want to set the channel as 2400+chn.
 * @return  	none.
 */
_attribute_ram_code_sec_noinline_ void rf_set_chn(signed char chn);


/**
 * @brief   	This function serves to set pri sb mode enable.
 * @return  	none.
 */
void rf_private_sb_en(void);


/**
 * @brief   	This function serves to set pri sb mode payload length.
 * @param[in]   pay_len  - In private sb mode packet payload length.
 * @return  	none.
 */
void rf_set_private_sb_len(int pay_len);


/**
 * @brief   	This function serves to disable pn of ble mode.
 * @return  	none.
 */
void rf_pn_disable(void);


/**
 * @brief   	This function serves to get the right fifo packet.
 * @param[in]   fifo_num   - The number of fifo set in dma.
 * @param[in]   fifo_dep   - deepth of each fifo set in dma.
 * @param[in]   addr       - address of rx packet.
 * @return  	the next rx_packet address.
 * @note		addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
unsigned char* rf_get_rx_packet_addr(int fifo_num,int fifo_dep,void* addr);


/**
 * @brief   	This function serves to set RF power level.
 * @param[in]   level 	 - The power level to set.
 * @return 		none.
 */
void rf_set_power_level (rf_power_level_e level);


/**
 * @brief   	This function serves to set RF power through select the level index.
 * @param[in]   idx 	 - The index of power level which you want to set.
 * @return  	none.
 */
void rf_set_power_level_index(rf_power_level_index_e idx);


/**
 * @brief	  	This function serves to close internal cap.
 * @return	 	none.
 */
void rf_turn_off_internal_cap(void);


/**
 * @brief	  	This function serves to update the value of internal cap.
 * @param[in]  	value   - The value of internal cap which you want to set.
 * @return	 	none.
 */
void rf_update_internal_cap(unsigned char value);


/**
 * @brief	  	This function serves to get RF status.
 * @return	 	RF Rx/Tx status.
 */
rf_status_e rf_get_trx_state(void);


/**
 * @brief	This function serve to change the length of preamble.
 * @param[in]	len		-The value of preamble length.Set the register bit<0>~bit<4>.
 * @return		none
 */
void rf_set_preamble_len(unsigned char len);

/**
 * @brief	This function serve to set the private ack enable,mainly used in prx/ptx.
 * @param[in]	rf_mode		-	Must be one of the private mode.
 * @return		none
 */
void rf_set_pri_tx_ack_en(rf_mode_e rf_mode);


/**
 * @brief	This function serve to set the length of access code.
 * @param[in]	byte_len	-	The value of access code length.
 * @return		none
 */
void rf_set_access_code_len(unsigned char byte_len);

/**
 * @brief 	This function serve to set access code.This function will first get the length of access code from register 0x140805
 * 			and then set access code in addr.
 * @param[in]	pipe_id	-The number of pipe.0<= pipe_id <=5.
 * @param[in]	acc	-The value access code
 * @note		For compatibility with previous versions the access code should be bit transformed by bit_swap();
 */
void rf_set_pipe_access_code (unsigned int pipe_id, unsigned char *addr);

/**
 * @brief   This function serves to set RF rx timeout.
 * @param[in]	timeout_us	-	rx_timeout after timeout_us us,The maximum of this param is 0xfff.
 * @return  none.
 */
static inline void rf_set_rx_timeout(unsigned short timeout_us)
{
	reg_rf_rx_timeout = timeout_us - 1;
}

/**
 * @brief	This function serve to initial the ptx setting.
 * @return	none.
 */
void rf_ptx_config(void);

/**
 * @brief	This function serve to initial the prx setting.
 * @return	none.
 */
void rf_prx_config(void);

/**
 * @brief   This function serves to set RF ptx trigger.
 * @param[in]	addr	-	The address of tx_packet.
 * @param[in]	tick	-	Trigger ptx after (tick-current tick),If the difference is less than 0, trigger immediately.
 * @return  none.
 * @note		addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
void rf_start_ptx  (void* addr,  unsigned int tick);

/**
 * @brief   This function serves to set RF prx trigger.
 * @param[in]	tick	-	Trigger prx after (tick-current tick),If the difference is less than 0, trigger immediately.
 * @return  none.
 */
void rf_start_prx(unsigned int tick);


/**
 * @brief	This function to set retransmit and retransmit delay.
 * @param[in] 	retry_times	- Number of retransmit, 0: retransmit OFF
 * @param[in] 	retry_delay	- Retransmit delay time.
 * @return		none.
 */
void rf_set_ptx_retry(unsigned char retry_times, unsigned short retry_delay);


/**
 * @brief   This function serves to judge whether the FIFO is empty.
 * @param pipe_id specify the pipe.
 * @return TX FIFO empty bit.
 * 			-#0 TX FIFO NOT empty.
 * 			-#1 TX FIFO empty.
 */
unsigned char rf_is_rx_fifo_empty(unsigned char pipe_id);


/**
 * @brief     	This function serves to RF trigger stx
 * @param[in] 	addr  	- DMA tx buffer.
 * @param[in] 	tick  	- Send after tick delay.
 * @return	   	none.
 * @note		addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
_attribute_ram_code_sec_noinline_ void rf_start_stx(void* addr, unsigned int tick);


/**
 * @brief     	This function serves to RF trigger stx2rx
 * @param[in] 	addr  	- DMA tx buffer.
 * @param[in] 	tick  	- Send after tick delay.
 * @return	    none.
 * @note		addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
_attribute_ram_code_sec_noinline_ void rf_start_stx2rx  (void* addr, unsigned int tick);


/**
 * @brief   	This function serves to set RF baseband channel.This function is suitable for ble open PN mode.
 * @param[in]   chn_num  - Bluetooth channel set according to Bluetooth protocol standard.
 * @return  	none.
 */
_attribute_ram_code_sec_noinline_ void rf_set_ble_chn(signed char chn_num);


/**
 * @brief   	This function serves to set RF Rx manual on.
 * @return  	none.
 */
_attribute_ram_code_sec_noinline_ void rf_set_rxmode(void);


/**
 * @brief	  	This function serves to start Rx of auto mode. In this mode,
 *				RF module stays in Rx status until a packet is received or it fails to receive packet when timeout expires.
 *				Timeout duration is set by the parameter "tick".
 *				The address to store received data is set by the function "addr".
 * @param[in]	addr   - The address to store received data.
 * @param[in]	tick   - It indicates timeout duration in Rx status.Max value: 0xffffff (16777215)
 * @return	 	none
 * @note		addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
_attribute_ram_code_sec_noinline_ void rf_start_brx  (void* addr, unsigned int tick);


/**
 * @brief	  	This function serves to start tx of auto mode. In this mode,
 *				RF module stays in tx status until a packet is sent or it fails to sent packet when timeout expires.
 *				Timeout duration is set by the parameter "tick".
 *				The address to store send data is set by the function "addr".
 * @param[in]	addr   - The address to store send data.
 * @param[in]	tick   - It indicates timeout duration in Rx status.Max value: 0xffffff (16777215)
 * @return	 	none
 * @note		addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
_attribute_ram_code_sec_noinline_ void rf_start_btx (void* addr, unsigned int tick);


/**
 * @brief     	This function serves to RF trigger srx2rx.
 * @param[in] 	addr  - DMA tx buffer.
 * @param[in] 	tick  - Trigger rx receive packet after tick delay.
 * @return	    none.
 * @note		addr:must be aligned by word (4 bytes), otherwise the program will enter an exception.
 */
_attribute_ram_code_sec_noinline_ void rf_start_srx2tx  (void* addr, unsigned int tick);


/**
 * @brief	  	This function is used to set how many words as the transmission unit of baseband and dma.
 * 				You don't need to call this function for normal use. By default, the unit is 1 world!
 * 				After configuring the DMA, call this function to adjust the DMA rate.
 * @param[in]	rf_trans_unit_e size	- the unit of burst size .Identify how many bytes of data are
 * 										  handled by DMA each time
 * @return	 	none.
 */
void rf_set_baseband_trans_unit(rf_trans_unit_e size);

/**
 * @brief	  	This function is used to judge whether there is a CRC error in the received packet through hardware.
 * 				For the same packet, the value of this bit is consistent with the CRC flag bit in the packet.
 * @param[in]	none.
 * @return	 	none.
 */
unsigned char rf_get_crc_err(void);

/**
 * @brief	  	This function is mainly used to set the energy when sending a single carrier.
 * @param[in]	level		- The slice corresponding to the energy value.
 * @return	 	none.
 */
void rf_set_power_level_singletone(rf_power_level_e level);

/**
 * @brief	  	This function is used to  set the modulation index of the receiver.
 *              This function is common to all modes,the order of use requirement:configure mode first,
 *              then set the the modulation index,default is 0.5 in drive,both sides need to be consistent
 *              otherwise performance will suffer,if don't specifically request,don't need to call this function.
 * @param[in]	mi_value- the value of modulation_index*100.
 * @return	 	none.
 */
void rf_set_rx_modulation_index(rf_mi_value_e mi_value);

/**
 * @brief	  	This function is used to  set the modulation index of the sender.
 *              This function is common to all modes,the order of use requirement:configure mode first,
 *              then set the the modulation index,default is 0.5 in drive,both sides need to be consistent
 *              otherwise performance will suffer,if don't specifically request,don't need to call this function.
 * @param[in]	mi_value- the value of modulation_index*100.
 * @return	 	none.
 */
void rf_set_tx_modulation_index(rf_mi_value_e mi_value);

/**
 *	@brief	  	This function serve to adjust rx settle timing sequence.
 *	@param[in]	rx_settle_us  	After adjusting the timing sequence, the time required for rx to settle.
 *	@return	 	none
 *	@note		RX_SETTLE_TIME_45US - disable rx_ldo_trim and rx_dcoc calibration,reduce 44.5us of rx settle time.Receive for a period of time and then do a normal calibration.
 *				RX_SETTLE_TIME_80US - disable rx_ldo_trim calibration,reduce 4.5us of rx settle time. Do a normal calibration at the beginning.
*/
void rf_rx_fast_settle_init(rf_rx_fast_settle_time_e rx_settle_us);

/**
 *	@brief	  	This function serve to adjust tx settle timing sequence.
 *	@param[in]	tx_settle_us  	After adjusting the timing sequence, the time required for tx to settle.
 *	@return	 	none
 *	@note		TX_SETTLE_TIME_50US  - disable tx_ldo_trim function and tx_hpmc,reduce 58us of tx settle time.After frequency hopping, a normal calibration must be done.
 *	            TX_SETTLE_TIME_104US - disable tx_ldo_trim function,reduce 4.5us of tx settle time. Do a normal calibration at the beginning.
*/
void rf_tx_fast_settle_init(rf_tx_fast_settle_time_e tx_settle_us);

/**
 *	@brief	  	This function serve to enable the tx timing sequence adjusted.
 *	@param[in]	none
 *	@return	 	none
*/
void rf_tx_fast_settle_en(void);

/**
 *	@brief	  	This function serve to disable the tx timing sequence adjusted.
 *	@param[in]	none
 *	@return	 	none
*/
void rf_tx_fast_settle_dis(void);

/**
 *	@brief	  	This function serve to enable the rx timing sequence adjusted.
 *	@param[in]	none
 *	@return	 	none
*/
void rf_rx_fast_settle_en(void);

/**
 *	@brief	  	This function serve to disable the rx timing sequence adjusted.
 *	@param[in]	none
 *	@return	 	none
*/
void rf_rx_fast_settle_dis(void);

/**
 *  @brief		This function is mainly used to get LDO Calibration-related values.
 *	@param[in]	ldo_trim   - ldo trim calibration value address pointer
 *	@return	 	none
*/
void rf_get_ldo_trim_val(rf_ldo_trim_t *ldo_trim);

/**
 *  @brief		This function is mainly used to set LDO Calibration-related values.
 *	@param[in]  ldo_trim   - ldo trim Calibration-related values.
 *	@return	 	none
*/
void rf_set_ldo_trim_val(rf_ldo_trim_t ldo_trim);

/**
 *  @brief		This function is mainly used to get hpmc Calibration-related values.
 *	@param[in]	none
 *	@return	 	none
*/
_attribute_ram_code_sec_noinline_ unsigned short rf_get_hpmc_cal_val();

/**
 *  @brief		This function is mainly used to set hpmc Calibration-related values.
 *	@param[in]  value  - hpmc Calibration-related values.
 *	@return	 	none
*/
_attribute_ram_code_sec_noinline_ void rf_set_hpmc_cal_val(unsigned short value);

/**
 *  @brief		This function is mainly used to get LDO Calibration-related values.
 *	@param[in]	dcoc_cal   - dcoc calibration value address pointer
 *	@return	 	none
*/
void rf_get_dcoc_cal_val(rf_dcoc_cal_t *dcoc_cal);

/**
 *  @brief		This function is mainly used to set dcoc Calibration-related values.
 *	@param[in]  dcoc_cal    - dcoc Calibration-related values.
 *	@return	 	none
*/
void rf_set_dcoc_cal_val(rf_dcoc_cal_t dcoc_cal);


/**
 *  @brief		This function is mainly used to get rccal Calibration-related values.
 *	@param[in]	rccal_cal  - rccal calibration value address pointer
 *	@return	 	none
*/
void rf_get_rccal_cal_val(rf_rccal_cal_t *rccal_cal);

/**
 *  @brief		This function is mainly used to set rccal Calibration-related values.
 *	@param[in]	rccal_cal    - rccal Calibration-related values.
 *	@return	 	none
*/
void rf_set_rccal_cal_val(rf_rccal_cal_t rccal_cal);

/**
 * @brief		This function is mainly used for the disable hpmc trim function.
 * @return		none.
 */
void rf_dis_hpmc_trim(void);

/**
 * @brief		This function is mainly used for the disable ldo trim function.
 * @return		none.
 */
void rf_dis_ldo_trim(void);

/**
 * @brief		This function is mainly used for the disable dcoc trim function.
 * @return		none.
 */
void rf_dis_dcoc_trim(void);

/**
 * @brief		This function is mainly used for the disable rccal trim function.
 * @return		none.
 */
void rf_dis_rccal_trim(void);

/**
 * @brief		This function is mainly used for the disable fcal trim function.
 * @return		none.
 */
void rf_dis_fcal_trim(/*unsigned short fcal_dcap*/);

/****************************************************************************************************************************************
 *                                         RF User-defined package related functions                                  					*
 ****************************************************************************************************************************************/

/**
 * @brief	  	This function serves to enable user-defined packet sending function.
 *				In this mode, users can define the meaning of the contents of the package according to their own needs.
 * 				Only ble1M_NO_PN mode supports user-defined packet.
 * @param[in]	none.
 * @return	 	none.
 * @note		Attention:Only ble1M_NO_PN mode supports user-defined packet.
 */
static inline void rf_ble1m_no_pn_tx_customer_mode_en(void)
{
	BM_SET(reg_rf_tx_mode2,FLD_RF_R_CUSTOM_MADE);
}

/**
 * @brief	  	This function serves to disable user-defined packet sending function.
 * 				By default, the user-defined packet sending function is turned off, and normal protocol packets are sent;
 * 				the function rf_ble1M_no_pn_tx_customer_mode_en needs to be called when the user-defined packet sending
 * 				function is needed.
 * 				Only ble1M_NO_PN mode supports user-defined packet.
 * @param[in]	none.
 * @return	 	none.
 * @note		Attention:Only ble1M_NO_PN mode supports user-defined packet.
 */
static inline void rf_ble1m_no_pn_tx_customer_mode_dis(void)
{
	BM_CLR(reg_rf_tx_mode2,FLD_RF_R_CUSTOM_MADE);
}

/**
 * @brief	  	This function serves to disable user-defined packet sending function.
 * 				After enabling the hardware to calculate the CRC, the hardware will automatically
 * 				add the CRC information to the header information when sending and receiving packets.
 * @param[in]	none.
 * @return	 	none.
 * @note		Attention:The hardware CRC is enabled by default. If you need to turn off this function,
 * 				call the rf_tx_hw_crc_dis function.
 */
static inline void rf_tx_hw_crc_en(void)
{
	BM_SET(reg_rf_tx_mode1,FLD_RF_CRC_EN);
}

/**
 * @brief	  	This function is used to disable the hardware to calculate the value of crc.
 * 				After disabling the hardware to calculate the CRC, if you want to add the CRC
 * 				information to the package, you need to calculate it by the user software, and
 * 				then add it to the package information.
 * @param[in]	none.
 * @return	 	none.
 * @note		Attention:In this case, the receiver can only use software to check whether
 * 			    the CRC is wrong, and the hardware CRC flag is no longer accurate.
 */
static inline void rf_tx_hw_crc_dis(void)
{
	BM_CLR(reg_rf_tx_mode1,FLD_RF_CRC_EN);
}



/****************************************************************************************************************************************
 *                                         RF : functions used by hyperlength                               		  					*
 ****************************************************************************************************************************************/


/**
 * @brief		This function is used to set tx  package of ordinary length format or a certain hyper length format package.
 *				After configuring rf initialization, call this function to set the packet length type, the default is the
 *				normal length.If it is powered on to send a normal-length packet, this function does not need to be called.
 * @param[in]	mode - Packet length mode.
 * @return		none.
 */
static inline void rf_set_tx_pkt_len_mode(rf_pkt_len_mode_e mode)
{
	reg_rf_tx_hlen_mode = mode;
}

/**
 * @brief		This function is used to set rx  package of ordinary length format or a certain hyper length format package.
 * 				After configuring the RF initialization, call this function to configure the length mode of the receiving
 * 				packet before entering the packet receiving mode. The default is the normal length. If it is powered on to
 * 				receive a normal length packet, this function does not need to be called.
 * @param[in]	mode - Packet length mode.
 * @return		none.
 */
static inline void rf_set_rx_pkt_len_mode(rf_pkt_len_mode_e mode)
{
	reg_rf_burst_size = ((reg_rf_burst_size & (~FLD_RF_RX_HLEN_MODE))|(mode << 2));
}


/**
 * @brief		This function is mainly used to enable the sending and receiving of BIS/CIS format packets.After initializing the RF,
 * 				you can choose whether you need to call this function according to whether you need to send or receive a BIS/CIS format
 * 				packet. Power-on initialization RF defaults to receive and send packets in common format.
 * @param[in]	none.
 * @return		none.
 * @note		Attention:These two package formats do not support AOA and AOD functions temporarily.
 */
static inline void rf_set_bis_cis_en(void)
{
	BM_SET(reg_rf_rxtmaxlen1,FLD_RF_RX_ISO_PDU);
}

/**
 * @brief		This function disables the new BIS and CIS packet formats in core5.2.
 * 				After enabling the BIS/CIS function, if you want to resume sending packets in the normal format, you need to call this
 * 				function to turn off the BIS/CIS function.
 * @param[in]	none.
 * @return		none.
 */
static inline void rf_set_bis_cis_dis(void)
{
	BM_CLR(reg_rf_rxtmaxlen1,FLD_RF_RX_ISO_PDU);
}

#if RF_HADM_EN
/****************************************************************************************************************************************
 *                                         RF : AOA/AOD related functions                          		 			  					*
 ****************************************************************************************************************************************/

/**
 * @brief		This function enables the sending and receiving functions of AOA/AOD in ordinary format packets or ADV format packets.
 * 				After configuring the RF function, if you want to send or receive a packet with AOA/AOD information, you can call this
 * 				function to make the chip enter the corresponding mode to receive or send the packet. The default state is a normal
 * 				package without AOA/AOD information.
 * @param[in]	mode - AOA/AOD broadcast package or normal package trx mode.
 * @return		none.
 */
static inline void rf_aoa_aod_set_trx_mode(rf_aoa_aod_mode_e mode)
{
	reg_rf_rxsupp = ((reg_rf_rxsupp & 0xf0) | mode);
}

/**
 * @brief		This function is used to calibrate AOA, AOD sampling frequency offset.This function is mainly used to set the position
 * 				of iq data sampling point in sampleslot to optimize the sampling data result. By default, sampling is performed at the
 * 				middle position of iqsampleslot, and the sampling point is 0.125us ahead of time for each decrease of 1 code.
 * 				Each additional code will move the sampling point back by 0.125us
 * @param[in]	samp_locate:Compare the parameter with the default value, reduce one code to advance 0.125us, increase or decrease 1 to move
 * 							back 0.125us.
 * @return		none.
 */
static inline void rf_aoa_aod_sample_point_adjust(unsigned char samp_locate)
{
	reg_rf_samp_offset = samp_locate;
}


/**
 * @brief		This function is used to set the position of the first antenna switch after the reference.The default is in the middle of the
 * 				first switch_slot; and the switch point is 0.125us ahead of time for each decrease of 1 code.
 * 				Each additional code will move the switch point back by 0.125us
 * @param[in]	swt_offset : Compare the parameter with the default value, reduce 1 to advance 0.125us, increase or decrease 1 to move
 * 							back 0.125us.
 * @return		none.
 */
void rf_aoa_rx_ant_switch_point_adjust(unsigned short swt_offset);


/**
 * @brief		This function is used to set the position of the first antenna switch after the AOD transmitter reference.The default is in the middle of the
 * 				first switch_slot; and the switch point is 0.125us ahead of time for each decrease of 1 code. Each additional code will move
 * 				the switch point back by 0.125us
 * @param[in]	swt_offset : Compare the parameter with the default value, reduce 1 to advance 0.125us, increase or decrease 1 to move
 * 							back 0.125us.
 * @return		none.
 */
void rf_aod_tx_ant_switch_point_adjust(unsigned short swt_offset);


/**
 * @brief		This function is mainly used to set the IQ data sample interval time. In normal mode, the sampling interval of AOA is 4us, and AOD will judge whether
 * 				the sampling interval is 4us or 2us according to CTE info.The 4us/2us sampling interval corresponds to the 2us/1us slot mode stipulated in the protocol.
 * 				Since the current hardware only supports the antenna switching interval of 4us/2us, setting the sampling interval to 1us or less will cause multiple
 * 				sampling at the interval of one antenna switching. Therefore, the sampling data needs to be processed by the upper layer according to the needs, and
 * 				currently it is mostly used Used in the debug process.
 * 				After configuring RF, you can call this function to configure slot time.
 * @param[in]	time_us	- AOA or AOD slot time mode.
 * @return		none.
 * @note	    Attention:(1)When the time is 0.25us, it cannot be used with the 20bit iq data type, which will cause the sampling data to overflow.
 * 						  (2)Since only the antenna switching interval of 4us/2us is supported, the sampling interval of 1us and shorter time intervals
 * 						      will be sampled multiple times in one antenna switching interval. Suggestions can be used according to specific needs.
 */
void rf_aoa_aod_sample_interval_time(rf_aoa_aod_sample_interval_time_e time_us);


/**
 * @brief		This function is mainly used to set the type of AOA/AODiq data. The default data type is 8bit. This configuration can be done before starting to receive
 * 				the package.
 * @param[in]	mode	- The length of each I or Q data.
 * @return		none.
 */
void rf_aoa_aod_iq_data_mode(rf_iq_data_mode_e mode);


/****************************************************************************************************************************************
 *                                         RF : HADM related functions                          		 			  					*
 ****************************************************************************************************************************************/

/**
 * @brief		This function is mainly used to initialize some parameter settings of the HADM IQ sample.
 * @param[in]	samp_num	- Number of groups to sample IQ data.
 * @param[in]	interval	- The interval time between each IQ sampling is (interval + 1)*0.125us.
 * @param[in]	start_point	- Set the starting point of the sample.If it is rx_en mode, sampling starts
 * 							  at 0.25us+start_point*0.125us after settle. If it is in sync mode, sampling
 * 							  starts at (start_point + 1) * 0.125us after sync.
 * @param[in]	suppmode    - The length of each I or Q data.
 * @param[in]	sample_mode - IQ sampling starts after syncing packets or after the rx_en is pulled up.
 * @return		none.
 */
void rf_hadm_iq_sample_init(unsigned short samp_num,unsigned char interval,unsigned char start_point,rf_iq_data_mode_e suppmode,rf_hadm_iq_sample_mode_e sample_mode);


/**
 * @brief		This function is mainly used to set the sample interval.
 * @param[in]	ant_interval- Set the interval for IQ sample, (interval + 1)*0.125us.
 * @return		none.
 * @note 		The max sample rate is 4Mhz.
 */
void rf_hadm_sample_interval_time(unsigned char interval);

/**
 * @brief		This function is mainly used to initialize the parameters related to HADM antennas.
 * @param[in]	clk_mode	- Set whether the antenna-related clock is always on or only when switching antennas.
 * @param[in]	ant_interval- Set the interval for antenna switching, (interval + 1)*0.125us.
 * @param[in]	ant_rxoffset- Adjust the switching start point of the rx-side antenna,(ant_rxoffset + 1)*0.125us.
 * @param[in]	ant_txoffset- Adjust the switching start point of the tx-side antenna,(ant_rxoffset + 1)*0.125us.
 * @return		none.
 */
void rf_hadm_ant_init(rf_hadm_ant_clk_mode_e clk_mode,unsigned char ant_interval,unsigned char ant_rxoffset,unsigned char ant_txoffset);

/**
 * @brief		This function is mainly used to set the antenna switching interval.
 * @param[in]	ant_interval- Set the interval for antenna switching, (interval + 1)*0.125us.
 * @return		none.
 */
void rf_set_hadm_ant_interval(unsigned char ant_interval);

/**
 * @brief		This function is mainly used to set the starting position of the antenna switching at the rx-side.
 * @param[in]	ant_rxoffset- Adjust the switching start point of the rx-side antenna,(ant_rxoffset + 1)*0.125us.
 * @return		none.
 */
void rf_set_hadm_rx_ant_offset(unsigned char ant_rxoffset);

/**
 * @brief		This function is mainly used to set the starting position of the antenna switching at the tx-side.
 * @param[in]	ant_txoffset- Adjust the switching start point of the rx-side antenna,(ant_txoffset + 1)*0.125us.
 * @return		none.
 */
void rf_set_hadm_tx_ant_offset(unsigned char ant_txoffset);

/**
 * @brief		This function is mainly used to set the clock working mode of the antenna.
 * @para[in]	clk_mode	- Open all the time or only when switching antennas.
 * @return		none.
 */
void rf_hadm_ant_clk_mode(rf_hadm_ant_clk_mode_e clk_mode);

/**
 * @brief		This function is mainly used to set the way IQ sampling starts.
 * @para[in]	sample_mode	- IQ sampling starts after syncing packets or after the rx_en is pulled up.
 * @return		none.
 */
void rf_hadm_iq_sample_mode(rf_hadm_iq_sample_mode_e sample_mode);

/**
 * @brief		This function is mainly used to set the starting position of IQ sampling.
 * @para[in]	start_point  - Set the starting point of the sample.If it is rx_en mode, sampling starts
 * 							  at 0.25us+start_point*0.125us after settle. If it is in sync mode, sampling
 * 							  starts at (start_point + 1) * 0.125us after sync.
 * @return		none.
 */
void rf_hadm_iq_start_point(unsigned char pos);

/**
 * @brief		This function is mainly used to set the number of IQ samples in groups.
 * @para[in]	samp_num    - Number of groups to sample IQ data.
 * @return		none.
 */
void rf_hadm_iq_sample_number(unsigned short samp_num);

/**
 * @brief		Mainly used to set thresholds when sync data packets.
 * @para[in]	thres_value  - The value of thresholds.
 * @return		none.
 */
void rf_set_ble_sync_threshold(unsigned char thres_value);

/**
 * @brief		This function is mainly used to enable the IQ sampling function.
 * @return		none.
 */
void rf_iq_sample_enable(void);

/**
 * @brief		This function is mainly used to disable the IQ sampling function.
 * @return		none.
 */
void rf_iq_sample_disable(void);

/**
 * @brief		This function is mainly used to obtain the synchronisation flag bit from the packet, which is
 * 				used to identify whether the packet is data received after passing synchronisation.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the Sync flag information in the packet.
 */
unsigned char rf_hadm_sync_flag(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len);

/**
 * @brief		This function is mainly used to obtain the packet quality indicator from the packet, which is
 * 				used to identify whether the packet is data received after passing synchronisation.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the packet quality information in the packet.
 */
unsigned char rf_hadm_get_packet_quality_indicator(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len);

/**
 * @brief		This function is mainly used to get the timestamp information from the packet that is
 * 				synchronised to the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the Sync timestamp information in the packet.
 */
unsigned int rf_hadm_get_pkt_rx_sync_timestamp(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len);

/**
 * @brief		This function is mainly used to obtain the timestamp information of the tx_pos from the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the timestamp information in the packet.
 */
unsigned int rf_hadm_get_pkt_tx_pos_timestamp(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len);

/**
 * @brief		This function is mainly used to obtain the timestamp information of the tx_neg from the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the timestamp information in the packet.
 */
unsigned int rf_hadm_get_pkt_tx_neg_timestamp(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len);

/**
 * @brief		This function is mainly used to obtain the timestamp information of the iq_start from the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the timestamp information in the packet.
 */
unsigned int rf_hadm_get_pkt_iq_start_timestamp(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len);

/**
 * @brief		This function is mainly used to obtain the rssi information from the packet.
 * @param[in]	p			- The packet address.
 * @param[in]	sample_num	- The number of sample points that the packet contains.
 * @param[in]	data_len	- The data length of the sample point in the packet.
 * @return		Returns the rssi information in the packet.
 */
signed char rf_hadm_get_pkt_rssi_value(unsigned char *p,unsigned short sample_num,rf_iq_data_mode_e data_len);

/**
 * @brief   	This function serves to set RF's channel.The step of this function is in KHz.
 *				The frequency set by this function is (chn+2400) MHz+chn_k KHz.
 * @param[in]   chn_m - RF channel. The unit of this parameter is MHz, and its set frequency
 * 					 	point is (2400+chn)MHz.
 * @param[in]   chn_k - The unit of this parameter is KHz, which means to shift chn_k KHz to
 * 						the right on the basis of chn.Its value ranges from 0 to 999.
 * @param[in]	trx_mode - Defines the frequency point setting of tx mode or rx mode.
 * @return  	none.
 */
void rf_set_channel_k_step(signed char chn_m,unsigned int chn_k,rf_trx_chn_e trx_mode);//general

/**
 * @brief		This function is mainly used for frezee agc.
 * @return		none.
 * @note		It should be noted that this function should be called after receiving the package.
 */
void rf_agc_disable(void);

/**
 * @brief		This function is mainly used to set the sequence related to Fast Settle in HADM.
 * @return		none.
 */
void rf_fast_settle_sequence_set(void);

/**
 * @brief		This function is mainly used to set the adc_pup to auto mode.
 * @return		none.
 */
static inline void rf_adc_pup_auto(void)
{
	write_reg8(0x17078c,read_reg8(0x17078c)&0x7f);
}

/**
 * @brief		This function is mainly used to set the tx_pa to auto mode.
 * @return		none.
 */
static inline void rf_tx_pa_pup_auto(void)
{
	write_reg8(0x170778,read_reg8(0x170778)&(~BIT(5)));
}

/**
 * @brief		This function is mainly used to get the gain lat value.
 * @return		Returns the value of gain lat.
 * @note		Call this function after agc disable.
 */
static inline unsigned char rf_get_gain_lat_value(void)
{
	return ((reg_rf_max_match1>>4)&0x07);
}

/**
 * @brief		This function is mainly used to get the cal_trim value of LDO.
 * @return		Returns the cal_trim value of the LDO.
 * @note		Call this function after packets are sent and received.
 */
static inline unsigned char rf_get_ldo_cal_trim_value(void)
{
	return read_reg8(0x1706ea);
}

/**
 * @brief		This function is mainly used to get the rxtxhf_trim value of LDO.
 * @return		Returns the rxtxhf_trim value of the LDO.
 * @note		Call this function after packets are sent and received.
 */
static inline unsigned char rf_get_ldo_rxtxhf_trim_value(void)
{
	return (read_reg8(0x1706ec)&0x3f);
}

/**
 * @brief		This function is mainly used to get the rxtxlf_trim value of LDO.
 * @return		Returns the rxtxlf_trim value of the LDO.
 * @note		Call this function after packets are sent and received.
 */
static inline unsigned char rf_get_ldo_rxtxlf_trim_value(void)
{
	return (((read_reg8(0x1706ed) & 0x0f) << 2) + ((read_reg8(0x1706ec) & 0xc0) >> 6));
}

/**
 * @brief		This function is mainly used to get the pll_trim value of LDO.
 * @return		Returns the pll_trim value of the LDO.
 * @note		Call this function after packets are sent and received.
 */
static inline unsigned char rf_get_ldo_pll_trim_value(void)
{
	return (read_reg8(0x1706ee) & 0x3f);
}

/**
 * @brief		This function is mainly used to get the vco_trim value of LDO.
 * @return		Returns the vco_trim value of the LDO.
 * @note		Call this function after packets are sent and received.
 */
static inline unsigned char rf_get_ldo_vco_trim_value(void)
{
	return (((read_reg8(0x1706ef) & 0x0f) << 2) + ((read_reg8(0x1706ee) & 0xc0) >> 6));
}

/**
 * @brief		This function is mainly used to get the fcal_dcap value.
 * @return		Returns the fcal_dcap value.
 * @note		Call this function after the packet has been sent.
 */
static inline unsigned short rf_get_fcal_dcap_value(void)
{
	write_reg8(0x1706c3,(read_reg8(0x1706c3)&0xc3)|(0x0b<<2));
	return (read_reg16(0x1706c0));
}

/**
 * @brief		This function is mainly used to get the hpmc_gain value.
 * @return		Returns the hpmc_gain value.
 * @note		Call this function after the packet has been sent.
 */
static inline unsigned short rf_get_hpmc_gain_value(void)
{
	return (read_reg16(0x1706fe));
}

/**
 * @brief		This function is mainly used to get the rccal_code value.
 * @return		Returns the rccal_code value.
 * @note		Call this function after the packet has been received.
 */
static inline unsigned short rf_get_rccal_code_value(void)
{
	return (read_reg16(0x1706ca));
}

/**
 * @brief		This function is mainly used to get the idac_code value.
 * @return		Returns the idac_code value.
 * @note		Call this function after the packet has been received.
 */
static inline unsigned char rf_get_dcoc_idac_code_value(void)
{
	return (read_reg8(0x1706d8));
}

/**
 * @brief		This function is mainly used to get the qdac_code value.
 * @return		Returns the qdac_code value.
 * @note		Call this function after the packet has been received.
 */
static inline unsigned char rf_get_dcoc_qdac_code_value(void)
{
	return (read_reg8(0x1706da));
}

/**
 * @brief		This function is mainly used to get the dcoc_offset_code value.
 * @return		Returns the dcoc_offset_code value.
 * @note		Call this function after the packet has been received.
 */
static inline unsigned short rf_get_dcoc_offset_code_value(void)
{
	return (read_reg16(0x1706dc));
}

/**
 * @brief		This function is mainly used to enable LNA.
 * @return		none.
 */
static inline void rf_lna_pup(void)
{
	write_reg8(0x17077a,read_reg8(0x17077a)|BIT(0));
	write_reg8(0x170778,read_reg8(0x170778)|BIT(0));
}

/**
 * @brief		This function is mainly used to disable LNA.
 * @return		none.
 */
static inline void rf_lna_pup_off(void)
{
	write_reg8(0x17077a,read_reg8(0x17077a)&(~BIT(0)));
	write_reg8(0x170778,read_reg8(0x170778)|BIT(0));
}

/**
 * @brief		This function is mainly used to set the value of the dac_pup.
 * @param[in]	value	- The value of dac_pup.
 * @return		none.
 */
void rf_seq_dac_pup_ow(unsigned char value);

/**
 * @brief		This function is mainly used to set the value of the pa_pup.
 * @param[in]	value	- The value of pa_pup.
 * @return		none.
 */
void rf_seq_tx_pa_pup_ow(unsigned char value);

/**
 * @brief		This function is mainly used to open the PA module.
 * @param[in]	pwr		- The slice value of power.
 * @return		none.
 */
void rf_pa_pwr_on(unsigned char pwr);

/**
 * @brief		This function is mainly used to close the PA module.
 * @return		none.
 */
void rf_pa_pwr_off(void);

/**
 * @brief		This function is mainly used to enable the HADM extension function of the hd_info.
 * @return		none.
 */
static inline void rf_hadm_hd_info_enable()
{
	reg_rf_mode_ctrl0 |= FLD_RF_INFO_EXTENSION;
}

/**
 * @brief		This function is mainly used to disable the HADM extension function of the hd_info.
 * @return		none.
 */
static inline void rf_hadm_hd_info_disable()
{
	reg_rf_mode_ctrl0 &= (~FLD_RF_INFO_EXTENSION);
}

/**
 * @brief		This function is mainly used to get the timestamp information in the process of sending 
 * 				and receiving packets; in the packet receiving stage, this register stores the sync moment 
 * 				timestamp, and this information remains unchanged until the next sending and receiving packets. 
 * 				In the send packet stage, the register stores the timestamp value of the tx_on moment, which 
 * 				remains unchanged until the next send/receive packet..
 * @return		TX:timestamp value of the tx_on moment.
 * 				RX:timestamp value of the sync moment.
 */
static inline unsigned int rf_get_hadm_timestamp()
{
	return reg_rf_timestamp;
}

/**
 * @brief		This function is mainly used to return the timestamp of the start point of tx_en through a register.
 * @return		The timestamp of the start point of tx.
 */
static inline unsigned int rf_get_hadm_tx_pos_timestamp()
{
	return reg_rf_tr_turnaround_pos_time;
}

/**
 * @brief		This function is mainly used to return the timestamp of the end point of tx_en through a register.
 * @return		The timestamp of the end point of tx.
 */
static inline unsigned int rf_get_hadm_tx_neg_timestamp()
{
	return reg_rf_tr_turnaround_neg_time;
}

/**
 * @brief		This function is mainly used to return the timestamp of the IQ sampling start point through the register.
 * @return		The timestamp of IQ sampling.
 */
static inline unsigned int rf_get_hadm_iq_start_timestamp()
{
	return reg_rf_iqstart_tstamp;
}

/**
 * @brief		This function is mainly used to wait for the state machine to return to the idle state.
 * @return		none.
 */
static inline void rf_wait_ll_sm_idle()
{
	while(reg_rf_ll_2d_sclk != FLD_RF_STATE_MACHINE_IDLE);
}

/**
 * @brief		This function is mainly used to set the preparation and enable of manual fcal.
 * @return		none.
 */
void rf_manual_fcal_setup(void);

/**
 * @brief		This function is mainly used to set the relevant value after manual fcal.
 * @return		none.
 * @note		The function needs to be called after the rf_manual_fcal_setup call 22us.
 */
void rf_manual_fcal_done(void);


/**
 * @brief		This function is mainly used for agc auto run.
 * @return		none.
 * @note		It needs to be called before sending and receiving packets after the tone interaction is complete.
 */
void rf_agc_enable(void);
#endif
#endif
