#include "../tl_common.h"
#include "./timer_event.h"

#define EV_TIMER_SAFE_MARGIN_US  4000000

//u32 EV_TIMER_SAFE_MARGIN = (EV_TIMER_SAFE_MARGIN_US * CLOCK_SYS_CLOCK_1US)

ev_time_event_t timer_list[10] = {0};
int timer_current_pos = 0;
static int inline ev_is_timer_expired(ev_time_event_t *e, u32 now){
    return ((u32)(now - e->t) < (EV_TIMER_SAFE_MARGIN_US * CLOCK_SYS_CLOCK_1US));
}


void ev_start_timer(ev_time_event_t * e){
   
    u8 r = irq_disable();

    u32 now = clock_time();

    u32 t = now + e->interval;    // becare of overflow

    e->t = t;
    e->valid = 1;
   
    irq_restore(r);
}

void ev_cancel_timer(ev_time_event_t * e){
	if ( e == NULL ) {
		while(1);
	}
	u8 r = irq_disable();
    e->valid = 0;
    e->busy = 0;
	irq_restore(r);
}

ev_time_event_t *ev_on_timer(ev_timer_callback_t cb, void *data, u32 t_us){
    int i;
    ev_time_event_t *e = NULL;
	if ( cb == NULL ) {
		while(1);
	}
    
    for(i=0;i<LengthOfArray(timer_list);i++){
        if(timer_list[timer_current_pos].busy == 0){
            timer_list[timer_current_pos].busy = 1;
            e = timer_list + timer_current_pos;
            timer_current_pos = (timer_current_pos + 1)%LengthOfArray(timer_list);
            break;
        }else{
            timer_current_pos = (timer_current_pos + 1)%LengthOfArray(timer_list);
        }
    }
    if ( e == NULL ) {
        while(1);
    }
    e->interval = t_us * CLOCK_SYS_CLOCK_1US;
    e->cb = cb;
    e->data = data;
    ev_start_timer(e);
    return e;
}

#if USE_OLD_EV_UNON_TIMER
void ev_unon_timer(ev_time_event_t ** e){
	if ( e == NULL || *e == NULL ) {
		return;
	}
	ev_cancel_timer(*e);
    *e = NULL;
}
#else

ev_time_event_t *__ev_unon_timer__(ev_time_event_t *e){
	if ( e == NULL) {
		return NULL;
	}
    //CHECK_ARRAY_ELEM_POINTER(timer_list,e,step,p_value);
	//ev_cancel_timer(e);
	e->valid = 0;
    e->busy = 0;
    return NULL;
}
#endif

/* Process time events */
void ev_process_timer(){

    u32 now = clock_time();
    ev_time_event_t *te;
    for(te = timer_list;te < timer_list + LengthOfArray(timer_list);te++){
        if((!is_timer_expired(te))&&ev_is_timer_expired(te, now)){
            int t;
			if ( (u32)(te->cb) < 0x100 || (u32)(te->cb) > 0x20000 ) {
				while(1);
			}
            t = te->cb(te->data);
            if(t < 0){
                ev_cancel_timer(te);        // delete timer
            }else if(0 == t){
                te->t = now + te->interval;    // becare of overflow
            }else{
                te->interval = t * CLOCK_SYS_CLOCK_1US;
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


int timer_event_idle(void){
	ev_time_event_t *te;
	for(te = timer_list;te < timer_list + LengthOfArray(timer_list);te++){
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
