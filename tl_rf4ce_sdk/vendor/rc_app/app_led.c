/********************************************************************************************************
 * @file    app_led.c
 *
 * @brief   This is the source file for app_led.c
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

#include "../../proj/tl_common.h"
#include "app_const.h"
#include "app_common.h"
#include "app_led.h"

#if   (RF4CE_CONTROLLER)
app_ledFlushInfo_t g_appLedAction;

static int led_BlinkTimerCB(void *arg)
{
    if ( rcu_appVars.blinkTimes == 0 ) {
        rcu_appVars.flags.bf.ledStatus = rcu_appVars.flags.bf.ledIdleState ^ 1;
    } else {
        rcu_appVars.flags.bf.ledStatus ^= 1;
    }

	if ( rcu_appVars.flags.bf.ledIndex == 0 ) {
    	gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledStatus);
	}
	else if ( rcu_appVars.flags.bf.ledIndex == 1 ) {
		gpio_write(APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledStatus);
	}
	else {
		gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledStatus);
		gpio_write(APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledStatus);
	}
	
	rcu_appVars.blinkTimes++;
    if ( rcu_appVars.ledStopCnt != 0 ) {
        if ( rcu_appVars.blinkTimes > rcu_appVars.ledStopCnt ) {
            rcu_appVars.flags.bf.ledStatus = 0;
			if ( rcu_appVars.flags.bf.ledIndex == 0 ) {
		    	gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledIdleState);
			}
			else if ( rcu_appVars.flags.bf.ledIndex == 1 ) {
				gpio_write(APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledIdleState);
			}
			else {
				gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledIdleState);
				gpio_write(APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledIdleState);
			}
            
			if ( rcu_appVars.flags.bf.ledIdleState == 0 ) {
				rcu_appVars.flags.bf.allowedDeep = 1;
			}
            rcu_appVars.ledBlinkTimer = NULL;
            return -1;
        }
    }

	if(g_appLedAction.modeNum){
		if(rcu_appVars.flags.bf.ledStatus){
			return g_appLedAction.onInterval;
		}else{
			u32 idx;
			idx = ((rcu_appVars.blinkTimes-1) >> 1 ) % g_appLedAction.modeNum;
			return g_appLedAction.offIntervalList[idx];
		}
	}else{
		u32 intv = (u32)arg;
		return intv;
	}

    return 0;
}

void led_startSet(u8 blinkTime, u32 blinkInterval, u8 ledIndex)
{
    if ( rcu_appVars.ledBlinkTimer ) {
        ev_unon_timer(&rcu_appVars.ledBlinkTimer);
    }
    led_Off(ledIndex);
	rcu_appVars.flags.bf.ledIndex = ledIndex;
	 rcu_appVars.blinkTimes = 0;
    if ( blinkTime == 0 ) {
        rcu_appVars.ledStopCnt = 0;
    } else  {
        rcu_appVars.ledStopCnt = blinkTime * 2 - 1;
    }

    g_appLedAction.modeNum = 0;
    rcu_appVars.flags.bf.ledStatus = 0;
    rcu_appVars.ledBlinkTimer = ev_on_timer(led_BlinkTimerCB, (void *)blinkInterval, 100);// blinkInterval);
}


void Led_startSpecial(u8 blinkTime, app_ledFlushInfo_t *ledAction, u8 ledIndex)
{
	led_startSet(blinkTime, ledAction->offIntervalList[0], ledIndex);
	memcpy(&g_appLedAction, ledAction, sizeof(app_ledFlushInfo_t));
}


void led_On(u8 ledIndex)
{
	if ( ledIndex == 0 ) {
    	gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_ON_LEVEL);
	}
	else if ( ledIndex == 1 ) {
		gpio_write(APP_RED_LED_GPIO, RED_LED_ON_LEVEL);
	}
	else {
		gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_ON_LEVEL);
		gpio_write(APP_RED_LED_GPIO, RED_LED_ON_LEVEL);
	}
    rcu_appVars.flags.bf.ledIdleState = 1;
}

void led_Off(u8 ledIndex)
{
    if ( ledIndex == 0 ) {
    	gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL);
	}
	else if ( ledIndex == 1 ) {
		gpio_write(APP_RED_LED_GPIO, RED_LED_OFF_LEVEL);
	}
	else {
		gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL);
		gpio_write(APP_RED_LED_GPIO, RED_LED_OFF_LEVEL);
	}
    rcu_appVars.flags.bf.ledIdleState = 0;
}

void led_stopAll(u8 ledIndex)
{

    if ( rcu_appVars.ledBlinkTimer ) {
        ev_unon_timer(&rcu_appVars.ledBlinkTimer);
    }
    rcu_appVars.flags.bf.ledStatus = 0;
	if ( ledIndex == 0 ) {
    	gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledStatus);
	}
	else if ( ledIndex == 1 ) {
		gpio_write(APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledStatus);
	}
	else {
		gpio_write(APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledStatus);
		gpio_write(APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^rcu_appVars.flags.bf.ledStatus);
	}
    rcu_appVars.ledStopCnt = 1;
    rcu_appVars.blinkTimes = 0;

}




#endif


