#include "../tl_common.h"
//#include "../common/utlist.h"
#include "ev.h"
#include "ev_buffer.h"

/*void process_Phy2Mac(void);
void process_CSMA2Mac(void);
void process_Mac2Nwk(void);
void process_Nwk2Profile(void);
void process_Profile2Nwk(void);
void process_Nwk2Mac(void);
void process_Mac2CSMA(void);*/

void process_Phy2Mac(u8 *pMsg);
void process_CSMA2Mac(u8 *pMsg);
void process_Mac2Nwk(u8 *pMsg);
void process_Nwk2Profile(u8 *pMsg);
void process_Profile2Nwk(u8 *pMsg);
void process_Nwk2Mac(u8 *pMsg);
void process_Mac2CSMA(u8 *pMsg);

ev_poll_callback_t sys_idle_handler_ptr = NULL,rf_edDetect_ptr = NULL;

#if (MODULE_USB_ENABLE)
ev_poll_callback_t usb_handle_irq_ptr = NULL;
#endif
static u8 ev_rf4ceStackRun = 1;

void ev_rf4ceStackEnable(u8 en){
	ev_rf4ceStackRun = en;
}

#if MEASURE_TASK_TICKS

u32 ev_process_timer_ticks = 0;
u32 process_Phy2Mac_ticks = 0;
u32 process_CSMA2Mac_ticks = 0;
u32 process_Mac2Nwk_ticks = 0;
u32 process_Nwk2Profile_ticks = 0;
u32 process_Profile2Nwk_ticks = 0;
u32 process_Nwk2Mac_ticks = 0;
u32 process_Mac2CSMA_ticks = 0;
int pair_flag = 0;

//#define U32_MAX (0xFFFFFFFF)
static u32 cal_cost_ticks(u32 old_ticks,u32 new_ticks){
    if(new_ticks>=old_ticks){
        return (new_ticks - old_ticks);
    }else{
        return (U32_MAX - old_ticks + new_ticks + 1);
    }
}


#endif



#if USE_NEW_EV_MAIN

#define PROCESS_TIMER_TICKS    (0x6400)
#define PHY2MAC_TICKS          (0x0c80)
#define CSMA2MAC_TICKS         (0x2580)
#define MAC2NWK_TICKS          (0x4b00)
#define NWK2PROFILE_TICKS      (0x2bc0)
#define PROFILE2NWK_TICKS      (0x30000)   //0xfa00)
#define NWK2MAC_TICKS          (0x20000)//(0x44c0)
#define MAC2CSMA_TICKS         (0x9000)

typedef struct _lc{
    u16 line;
}lc_t;

lc_t lc = {0};

#define EV_MAIN_SAFE_MARGIN_US (10000000)
#define EV_MAIN_SAFE_MARGIN_TICKS ((EV_MAIN_SAFE_MARGIN_US)*(CLOCK_SYS_CLOCK_1US))

#if 0
int run_next_task(u32 expired_ticks,u32 time_threshold){
    u32 now;
    u32 distance;
    now = clock_time();
    distance = (u32)(expired_ticks - now);
    return (distance > time_threshold)&&(distance < EV_MAIN_SAFE_MARGIN_TICKS);
}
#endif

#define CHECK_REMAINING_TICKS(expired_ticks,time_threshold, mode)   \
do{ \
	if(mode){ \
		u32 now = clock_time(); \
		u32 distance = (u32)(expired_ticks - now); \
		if(!((distance > time_threshold)&&(distance < EV_MAIN_SAFE_MARGIN_TICKS))) { return; } \
	}\
}while(0)


void ev_main(u32 expired_ticks, u8 mode){
	u32 start_t = clock_time();
    switch(lc.line){
        case 0:
        while(1){
            lc.line = __LINE__; case __LINE__://note that they should always be in the same line.

                CHECK_REMAINING_TICKS(expired_ticks,PROCESS_TIMER_TICKS, mode);
                ev_process_timer();

            lc.line = __LINE__; case __LINE__:

                CHECK_REMAINING_TICKS(expired_ticks,USER_DEFINE_TICKS, mode);
                if(sys_idle_handler_ptr){
                    sys_idle_handler_ptr();
                }
                if(rf_edDetect_ptr){
                    rf_edDetect_ptr();
                }

                #if(MODULE_WATCHDOG_ENABLE)
	            wd_clear();
                #endif

            lc.line = __LINE__; case __LINE__:

                CHECK_REMAINING_TICKS(expired_ticks,PHY2MAC_TICKS, mode);
                process_Phy2Mac();

            lc.line = __LINE__; case __LINE__:

                CHECK_REMAINING_TICKS(expired_ticks,CSMA2MAC_TICKS, mode);
                process_CSMA2Mac();

            lc.line = __LINE__; case __LINE__:

                CHECK_REMAINING_TICKS(expired_ticks,MAC2NWK_TICKS, mode);
                process_Mac2Nwk();

            lc.line = __LINE__; case __LINE__:

                CHECK_REMAINING_TICKS(expired_ticks,NWK2PROFILE_TICKS, mode);
                process_Nwk2Profile();

            lc.line = __LINE__; case __LINE__:

                CHECK_REMAINING_TICKS(expired_ticks,PROFILE2NWK_TICKS, mode);
                process_Profile2Nwk();

            lc.line = __LINE__; case __LINE__:

                CHECK_REMAINING_TICKS(expired_ticks,NWK2MAC_TICKS, mode);
                process_Nwk2Mac();

            lc.line = __LINE__; case __LINE__:

                CHECK_REMAINING_TICKS(expired_ticks,MAC2CSMA_TICKS, mode);
                process_Mac2CSMA();

                if(mode == 0){
                	//return;
                	if(clock_time_exceed(start_t, 20000)){
                		return;
                	}
                }
        }
    }
}

#else
#if RF4CE_TEST_ENABLE
u8 dev2lua_start = 0;
u8 lua2dev_start = 0;
u8 nwk2profile_start = 1;
#endif


typedef  struct{
	u8 item;
	void (*itemCb)(u8 *pMsg);
}buf_item_t;

buf_item_t buf_item_cb[] = {
	{BUF_ITEM_STATE_PHY2MAC,		process_Phy2Mac},
	{BUF_ITEM_STATE_CSMA2MAC,		process_CSMA2Mac},
	{BUF_ITEM_STATE_MAC2NWK,		process_Mac2Nwk},
	{BUF_ITEM_STATE_NWK2PROFILE,	process_Nwk2Profile},
	{BUF_ITEM_STATE_PROFILE2NWk,	process_Profile2Nwk},
	{BUF_ITEM_STATE_NWK2MAC,		process_Nwk2Mac},
	{BUF_ITEM_STATE_MAC2CSMA,		process_Mac2CSMA},
};

u32 T_rf4ceTaskRun = 0;
void ev_main(void){

    #if RF4CE_TEST_ENABLE
    process_syn_buf();
    if(lua2dev_start){
        process_asyn_buf();
    }
    if(dev2lua_start){
        process_asyn_result_buf();
    }
    #endif

    #if MEASURE_TASK_TICKS
    u32 old_ticks = 0,new_ticks = 0,cost_ticks = 0;
    int measure_flag = 0;
    #endif

    #if MEASURE_TASK_TICKS
    if(pair_flag){
        old_ticks = clock_time();
        measure_flag = 1;
    }
    #endif

    ev_process_timer();

    #if MEASURE_TASK_TICKS
    if(measure_flag){
        new_ticks = clock_time();
        cost_ticks = cal_cost_ticks(old_ticks,new_ticks);
        if(cost_ticks>ev_process_timer_ticks){
            ev_process_timer_ticks = cost_ticks;
        }
    }
    #endif

    //ev_poll();
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
    //poll_rf4ce_msg();

	u8 *rf4ceTaskData = NULL;
	if(ev_rf4ceStackRun){
#if 1
		u8 rf4ceTask;
		//while(1)
		{
			rf4ceTaskData = buf_message_poll(&rf4ceTask);
			if(rf4ceTaskData == NULL){
				//break;
			}else{
				for(int i = 0; i < 7; i++){
					if(buf_item_cb[i].item == rf4ceTask){
						T_rf4ceTaskRun++;
						buf_item_cb[i].itemCb(rf4ceTaskData);
						break;
					}
				}
			}
		}
#else
		for(int i = 0; i < 7; i++){
			rf4ceTaskData = buf_message_poll(buf_item_cb[i].item);
			if(rf4ceTaskData){
				buf_item_cb[i].itemCb(rf4ceTaskData);
			}
		}

#endif
	}

	if(sys_idle_handler_ptr){
	   sys_idle_handler_ptr();
	}
	if(rf_edDetect_ptr){
	   rf_edDetect_ptr();
	}

	#if (MODULE_USB_ENABLE)
	if(usb_handle_irq_ptr){
	   usb_handle_irq_ptr();
	}
	#endif

}

#endif

