
#pragma once

#include "../common/types.h"
#include "../config/user_config.h"
#include "../../platform/platform_includes.h"
#include "./timer_event.h"


#define MEASURE_TASK_TICKS (0)

typedef void (*ev_poll_callback_t)(void);
enum{EV_FIRED_EVENT_MAX_MASK = EV_FIRED_EVENT_MAX - 1};

//enum { EV_TIMER_SAFE_MARGIN_US = 4000000, EV_TIMER_SAFE_MARGIN = (EV_TIMER_SAFE_MARGIN_US * CLOCK_SYS_CLOCK_1US) };        // in us,


#if USE_NEW_EV_MAIN
void ev_main(u32 expired_ticks, u8 mode);
#else
void ev_main(void);
#endif

void ev_rf4ceStackEnable(u8 en);

