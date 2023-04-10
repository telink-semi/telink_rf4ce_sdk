/********************************************************************************************************
 * @file    ev_buffer.h
 *
 * @brief   This is the header file for ev_buffer.h
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
  Filename:       ev_buffer.h
  Revised:        $Date: 2013-05-27 $

  Description:    Interface of Telink EV Buffer Module
**************************************************************************************************/

#pragma once

#include "../common/types.h"
#include "../common/compiler.h"

#define USE_NEW_MSG_MANAGER (0)


#define LengthOfArray(arr_name) (sizeof(arr_name)/sizeof(arr_name[0]))

enum {
    NWK_2_MAC_TYPE,
    PHY_2_MAC_TYPE,
};

#define BUFFER_NUM                       (14)
#define SMALL_BUFFER                     (24)
#define MIDDLE_BUFFER                    (48)
#define LARGE_BUFFER                     (168-6)
#define BUFFER_SIZE                      (168-6)

#define RF_RX_BUFFER_OFFSET					16

#if USE_NEW_MSG_MANAGER

#define MASK_PHY2MAC_INDICATOR          (0x0f)
#define MASK_CSMA2MAC_INDICATOR         (MASK_PHY2MAC_INDICATOR<<4)
#define MASK_MAC2NWK_INDICATOR          (MASK_CSMA2MAC_INDICATOR<<4)
#define MASK_NWK2PROFILE_INDICATOR      (MASK_MAC2NWK_INDICATOR<<4)
#define MASK_PROFILE2NWk_INDICATOR      (MASK_NWK2PROFILE_INDICATOR<<4)
#define MASK_NWK2MAC_INDICATOR          (MASK_PROFILE2NWk_INDICATOR<<4)
#define MASK_MAC2CSMA_INDICATOR         (MASK_NWK2MAC_INDICATOR<<4)

#define INDICATOR_OFFSET(state) ((state - BUF_ITEM_STATE_PHY2MAC)<<2)
#define INDICATOR_MASK(state) (0xf<<INDICATOR_OFFSET(state))
    
typedef enum{
    BufItem_BitMap = 0,
    Msg_Indicator_Vector,
    BUF_ITEM_STATE_PHY2MAC,
    BUF_ITEM_STATE_CSMA2MAC,
    BUF_ITEM_STATE_MAC2NWK,
    BUF_ITEM_STATE_NWK2PROFILE,
    BUF_ITEM_STATE_PROFILE2NWk,
    BUF_ITEM_STATE_NWK2MAC,
    BUF_ITEM_STATE_MAC2CSMA,
    Msg_Ctrl_Row_Num,
}msg_ctrl_row_t;

//#pragma pack(4)
typedef struct _ev_bufItem {
    struct _ev_bufItem *next;
    u16 reserved;
    u16 magic;
    u8 data[BUFFER_SIZE];
} ev_bufItem_t;
//#pragma pack()

typedef enum buf_sts_e {
    // SUCCESS always be ZERO
	BUFFER_SUCC,
    BUFFER_INVALID_PARAMETER = 1,  //!< Invalid parameter passed to the buffer API
    BUFFER_DUPLICATE_FREE          //!< The same buffer is freed more than once
} buf_sts_t;


#else

#define __DEBUG_BUFFER__    (0)
#define FILE_NAME_LENGTH    (6)

typedef enum buf_item_state_e{
    BUF_ITEM_STATE_BUSY = 0,
    BUF_ITEM_STATE_PHY2MAC,
    BUF_ITEM_STATE_CSMA2MAC,
    BUF_ITEM_STATE_MAC2NWK,
    BUF_ITEM_STATE_NWK2PROFILE,
    BUF_ITEM_STATE_PROFILE2NWk,
    BUF_ITEM_STATE_NWK2MAC,
    BUF_ITEM_STATE_MAC2CSMA,
    MAX_BUF_ITEM_STATE,
}buf_item_state_t;


typedef struct _ev_bufItem {
    struct _ev_bufItem *next;
    #if __DEBUG_BUFFER__
    u8 file_name[FILE_NAME_LENGTH];
    u16 line;
    #endif
    u8 buf_item_state_tbl[MAX_BUF_ITEM_STATE];
    u8 data[BUFFER_SIZE];
    u16 used;
} ev_bufItem_t;

typedef enum buf_sts_e {
    // SUCCESS always be ZERO
	BUFFER_SUCC,
    BUFFER_INVALID_PARAMETER = 1,  //!< Invalid parameter passed to the buffer API
    BUFFER_DUPLICATE_FREE          //!< The same buffer is freed more than once
} buf_sts_t;

#endif



#if USE_NEW_MSG_MANAGER

void ev_buf_reset(void);
void ev_buf_init(void);
u8* ev_buf_allocate(u16 size);
buf_sts_t ev_buf_free(u8* pBuf);
ev_bufItem_t* ev_buf_getHead(u8* pd);
u8* ev_buf_getTail(u8* pd, int offsetToTail);
u8 buf_message_post(u8 *ptr,msg_ctrl_row_t state);
u8 *buf_message_poll(msg_ctrl_row_t state);

#else

void ev_buf_reset(void);
void ev_buf_init(void);
u8 ev_isTaskDone(void);
#if __DEBUG_BUFFER__
u8* __ev_buf_allocate__(u8 *filename,u16 line);
buf_sts_t __ev_buf_free__(u8 *filename,u16 line,u8* pBuf);
#define ev_buf_allocate(size)   __ev_buf_allocate__(__FILE__,__LINE__)
#define ev_buf_free(pBuf)   ({ WRITE_FUNC_CALL_POS(func_call_line,func_call_file,64); \
                                buf_sts_t ret; \
                                ret = __ev_buf_free__(__FILE__,__LINE__,pBuf); \
                            })
#else
u8* __ev_buf_allocate__();
buf_sts_t __ev_buf_free__(u8* pBuf);
#define ev_buf_allocate(size)   __ev_buf_allocate__()
#define ev_buf_free(pBuf)       __ev_buf_free__(pBuf)
#endif
ev_bufItem_t* ev_buf_getHead(u8* pd);
u8* ev_buf_getTail(u8* pd, int offsetToTail);
u8 buf_message_post(u8 *ptr,buf_item_state_t state);
u8 *buf_message_poll(buf_item_state_t *state);
u16 ev_buf_getfreeSize(void);
#define		TL_RXBUF_TO_EVBUF(p)	(p - RF_RX_BUFFER_OFFSET)
#endif

