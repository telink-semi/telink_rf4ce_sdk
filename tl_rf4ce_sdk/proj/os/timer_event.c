/********************************************************************************************************
 * @file    timer_event.c
 *
 * @brief   This is the source file for timer_event.c
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

#include "../tl_common.h"
#include "./timer_event.h"

#define EV_TIMER_SAFE_MARGIN_US  4000000

//u32 EV_TIMER_SAFE_MARGIN = (EV_TIMER_SAFE_MARGIN_US * CLOCK_SYS_CLOCK_1US)

ev_time_event_t evTimer_list[12] = {{0}};
ev_time_event_t stackTimer_list[10] = {{0}};
int evtimer_current_pos = 0,stacktimer_current_pos = 0;
static int inline ev_is_timer_expired(ev_time_event_t *e, u32 now){
    return ((u32)(now - e->t) < (EV_TIMER_SAFE_MARGIN_US * S_TIMER_CLOCK_1US));
}


static void start_timer(ev_time_event_t * e){
   
    u8 r = irq_disable();

    u32 now = clock_time();

    u32 t = now + e->interval;    // becare of overflow

    e->t = t;
    e->valid = 1;
   
    irq_restore(r);
}

static void cancel_timer(ev_time_event_t * e){
	if ( e == NULL ) {
		while(1);
	}
	u8 r = irq_disable();
    e->valid = 2;
    e->busy = 2;
	irq_restore(r);
}

static ev_time_event_t *on_timer(u8 idx,ev_timer_callback_t cb, void *data, u32 t_us){
    int i;
    ev_time_event_t *e = NULL;
    ev_time_event_t *time_list = NULL;
    u32 timer_current_pos=0,timer_len=0;
    u32 *pos=NULL;
	if ( cb == NULL ) {
		while(1);
	}
	if(idx==TL_EV_TASK)
	{
		time_list = evTimer_list;
		timer_current_pos = evtimer_current_pos;
		pos = &evtimer_current_pos;
		timer_len = LengthOfArray(evTimer_list);
	}
	else if(idx==TL_STACK_TASK)
	{
		time_list = stackTimer_list;
		timer_current_pos = stacktimer_current_pos;
		pos = &stacktimer_current_pos;
		timer_len = LengthOfArray(stackTimer_list);
	}
	else
	{
		return NULL;
	}



    for(i=0;i<timer_len;i++){
        if(time_list[timer_current_pos].busy == 0){
        	time_list[timer_current_pos].busy = 1;
            e = time_list + timer_current_pos;
            timer_current_pos = (timer_current_pos + 1)%timer_len;
            break;
        }else{
        	timer_current_pos = (timer_current_pos + 1)%timer_len;
        }
    }
    if ( e == NULL ) {
        while(1);
    }
    *pos = timer_current_pos;
    e->interval = t_us * S_TIMER_CLOCK_1US;
    e->cb = cb;
    e->data = data;
    start_timer(e);
    return e;
}

ev_time_event_t *ev_on_timer(ev_timer_callback_t cb, void *data, u32 t_us)
{
	return on_timer(TL_EV_TASK ,cb, data, t_us);
}

ev_time_event_t *stk_on_timer(ev_timer_callback_t cb, void *data, u32 t_us)
{
	return on_timer(TL_STACK_TASK ,cb, data, t_us);
}

#if USE_OLD_EV_UNON_TIMER
void ev_unon_timer(ev_time_event_t ** e){
	if ( e == NULL || *e == NULL ) {
		return;
	}
	cancel_timer(*e);
    *e = NULL;
}
#else



ev_time_event_t *__ev_unon_timer__(ev_time_event_t *e){
	if ( e == NULL) {
		return NULL;
	}

    ev_time_event_t *te=NULL;

    for(te = evTimer_list;te < evTimer_list + LengthOfArray(evTimer_list);te++)
    {
    		if(te==e)
    			break;
    }

	if(te!=e)
	{
		while(1);
		return NULL;
	}

    //CHECK_ARRAY_ELEM_POINTER(timer_list,e,step,p_value);
	//ev_cancel_timer(e);
	e->valid = 2;
    e->busy = 2;

    return NULL;
}



ev_time_event_t *__stk_unon_timer__(ev_time_event_t *e){
	if ( e == NULL) {
		return NULL;
	}

    ev_time_event_t *te=NULL;

    for(te = stackTimer_list;te < stackTimer_list + LengthOfArray(stackTimer_list);te++)
    {
    		if(te==e)
    			break;
    }

	if(te!=e)
	{
		while(1);//non-stack event list timer
	}

    //CHECK_ARRAY_ELEM_POINTER(timer_list,e,step,p_value);
	//ev_cancel_timer(e);
	e->valid = 2;
    e->busy = 2;

    return NULL;
}
#endif

/* Process time events */
void ev_process_timer(){

    u32 now = clock_time();
    ev_time_event_t *te;

    for(te = evTimer_list;te < evTimer_list + LengthOfArray(evTimer_list);te++)
    {
    	if(te->valid ==2)
    	{
			te->busy = 0;
			te->valid = 0;
    	}
    }

    for(te = stackTimer_list;te < stackTimer_list + LengthOfArray(stackTimer_list);te++)
    {
    	if(te->valid ==2)
    	{
			te->busy = 0;
			te->valid = 0;
    	}
    }


    for(te = evTimer_list;te < evTimer_list + LengthOfArray(evTimer_list);te++){
        if((!is_timer_expired(te))&&ev_is_timer_expired(te, now)){
            int t;
//			if ( (u32)(te->cb) < 0x100 || (u32)(te->cb) > 0x20000 ) {
//				while(1);
//			}
            t = te->cb(te->data);
            if(t < 0){
            	cancel_timer(te);        // delete timer
            }else if(0 == t){
                te->t = now + te->interval;    // becare of overflow
            }else{
                te->interval = t * S_TIMER_CLOCK_1US;
                te->t = now + te->interval;    // becare of overflow
            }
            
        }
    }

    for(te = stackTimer_list;te < stackTimer_list + LengthOfArray(stackTimer_list);te++){
        if((!is_timer_expired(te))&&ev_is_timer_expired(te, now)){
            int t;
//			if ( (u32)(te->cb) < 0x100 || (u32)(te->cb) > 0x20000 ) {
//				while(1);
//			}
            t = te->cb(te->data);
            if(t < 0){
            	cancel_timer(te);        // delete timer
            }else if(0 == t){
                te->t = now + te->interval;    // becare of overflow
            }else{
                te->interval = t * S_TIMER_CLOCK_1US;
                te->t = now + te->interval;    // becare of overflow
            }

        }
    }


}


int is_timer_expired(ev_time_event_t *e)
{
    if ( e == NULL ) {
        return TRUE;
    }
    if(e->valid == 1){
        return FALSE;
    }else{
        return TRUE;
    }
}


int ev_timer_event_idle(void){
	ev_time_event_t *te;
	for(te = evTimer_list;te < evTimer_list + LengthOfArray(evTimer_list);te++){
		if(te->busy){
			return 0;
		}
	}
	return 1;
}

int stack_timer_event_idle(void){
	ev_time_event_t *te;
	for(te = stackTimer_list;te < stackTimer_list + LengthOfArray(stackTimer_list);te++){
		if(te->busy){
			return 0;
		}
	}
	return 1;
}


int timer_event_idle(void){
	ev_time_event_t *te;

	for(te = evTimer_list;te < evTimer_list + LengthOfArray(evTimer_list);te++){
		if(te->busy){
			return 0;
		}
	}

	for(te = stackTimer_list;te < stackTimer_list + LengthOfArray(stackTimer_list);te++){
		if(te->busy){
			return 0;
		}
	}

	return 1;
}



#if 0
void process_mac_timer(u32 now){

    process_timer(mac_timer_list,MAC_TIMER_NUM,now);
}
void process_nwk_timer(u32 now){

    process_timer(nwk_timer_list,NWK_TIMER_NUM,now);
}
void process_profile_timer(u32 now){

    process_timer(profile_timer_list,PROFILE_TIMER_NUM,now);
}
#endif
