/********************************************************************************************************
 * @file    timer_event.h
 *
 * @brief   This is the header file for timer_event.h
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

#pragma once

#include "../../proj/common/types.h"
//
#define MAC_TIMER_NUM (10)//please fix me
#define NWK_TIMER_NUM (10)
#define PROFILE_TIMER_NUM (10)
//
#define LengthOfArray(arr_name) (sizeof(arr_name)/sizeof(arr_name[0]))
#define TIMER_SAFE_MARGIN_US (5)//(10)//please fix me
#define TIMER_SAFE_MARGIN (TIMER_SAFE_MARGIN_US*CLOCK_SYS_CLOCK_1US)
#define __DEBUG_TIMER__ (0)

typedef int (*ev_timer_callback_t)(void *data);


/**
 *  @brief Definition for timer event
 */
typedef struct ev_time_event_t {
    ev_timer_callback_t     cb;            //!< Callback function when expire, this must be specified
    u32                     t;             //!< Used internal
    u32                     interval;      //!< Used internal
    void                    *data;         //!< Callback function arguments.
    u8                     valid;
    u8                     busy;
} ev_time_event_t;
#define USE_OLD_EV_UNON_TIMER   (0)
ev_time_event_t *ev_on_timer(ev_timer_callback_t cb,void *data, u32 t_us);

#if USE_OLD_EV_UNON_TIMER
void ev_unon_timer(ev_time_event_t **e);//ok
#else
ev_time_event_t *__ev_unon_timer__(ev_time_event_t *e);

#if __DEBUG_TIMER__
#define ev_unon_timer(x)    do{ WRITE_FUNC_CALL_POS(func_call_line,func_call_file,64); \
								u8 r = irq_disable();	\
                                *(x) = __ev_unon_timer__(*(x)); \
                                irq_restore(r);	\
                            }while(0)
                                
#else
#define ev_unon_timer(x)    do{ u8 r = irq_disable();	\
        						*(x) = __ev_unon_timer__(*(x)); \
        						irq_restore(r);	\
							}while(0)
#endif

#endif


void ev_process_timer();
int is_timer_expired(ev_time_event_t *e);//ok

int timer_event_idle(void);
