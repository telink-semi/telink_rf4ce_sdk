/********************************************************************************************************
 * @file    user_trace.h
 *
 * @brief   This is the header file for user_trace.h
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

#ifndef USER_TRACE_H_
#define USER_TRACE_H_



#define TR_T_Phy2Mac          1
#define TR_T_CSMA2Mac         2
#define TR_T_Mac2Nwk          3
#define TR_T_Nwk2Profile      4
#define TR_T_Profile2Nwk      5
#define TR_T_Nwk2Mac          6
#define TR_T_Mac2CSMA         7


#define TR_T_abuf_add           10
#define TR_T_abuf_dec           11
#define TR_T_abuf_usb           12
#define TR_T_abuf_zero          13
#define TR_T_abuf_overflow      14
#define TR_T_abuf_overflow_mic  15
#define TR_T_abuf_overflow_dec  16
#define TR_T_abuf_mic_dec       17




#define TR_24_abuf_mic_wptr          20
#define TR_24_abuf_dec_wptr          21
#define TR_24_abuf_dec_rptr          22
#define TR_24_abuf_reset             23
#define TR_24_abuf_rf_header         24

#define TR_T_main                    25
#define TR_T_audio_loop              26
#define TR_T_send_audio              27
#define TR_T_mac_ack                 28
#define TR_T_tx_cb                   29
#define TR_T_rx_cb                   30

#define TR_T_send_busy          31
#define TR_T_tx_irq             32
#define TR_T_rx_irq             33
#define TR_T_csma_delay         34










#endif
