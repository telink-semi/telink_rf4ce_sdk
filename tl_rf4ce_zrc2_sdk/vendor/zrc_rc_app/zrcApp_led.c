/********************************************************************************************************
 * @file     zrcApp_led.c
 *
 * @brief    LED blinking for RCU
 *
 * @author
 * @date     July. 2, 2019
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#include "../../proj/tl_common.h"
#include "zrc_const.h"
#include "zrcApp.h"

#include "zrcApp_led.h"
#if __PROJECT_ZRC_2_RC__

app_ledFlushInfo_t g_appLedAction;

int zrc_ledBlinkTimer(void *arg) 
{
    if ( zrc_appVars.blinkTimes == 0 ) {
        zrc_appVars.flags.bf.ledStatus = zrc_appVars.flags.bf.ledIdleState ^ 1;
    } else {
        zrc_appVars.flags.bf.ledStatus ^= 1;
    }

	if ( zrc_appVars.flags.bf.ledIndex == 0 ) {
    	gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledStatus);
	}
	else if ( zrc_appVars.flags.bf.ledIndex == 1 ) {
		gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledStatus);
	}
	else {
		gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledStatus);
		gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledStatus);
	}
	
	zrc_appVars.blinkTimes++;
    if ( zrc_appVars.ledStopCnt != 0 ) {
        if ( zrc_appVars.blinkTimes > zrc_appVars.ledStopCnt ) {
            zrc_appVars.flags.bf.ledStatus = 0;   
			if ( zrc_appVars.flags.bf.ledIndex == 0 ) {
		    	gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledIdleState);
			}
			else if ( zrc_appVars.flags.bf.ledIndex == 1 ) {
				gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledIdleState);
			}
			else {
				gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledIdleState);
				gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledIdleState);
			}
            
			if ( zrc_appVars.flags.bf.ledIdleState == 0 ) {
				zrc_appVars.flags.bf.allowedDeep = 1;
			}
            zrc_appVars.ledBlinkTimer = NULL;
            return -1;
        }
    }

	if(g_appLedAction.modeNum){
		if(zrc_appVars.flags.bf.ledStatus){
			return g_appLedAction.onInterval;
		}else{
			u32 idx;
			idx = ((zrc_appVars.blinkTimes-1) >> 1 ) % g_appLedAction.modeNum;
			return g_appLedAction.offIntervalList[idx];
		}
	}else{
		u32 intv = (u32)arg;
		return intv;
	}

    return 0;
}

void zrc_startLed(u8 blinkTime, u32 blinkInterval, u8 ledIndex)
{
    if ( zrc_appVars.ledBlinkTimer ) {
        ev_unon_timer(&zrc_appVars.ledBlinkTimer);
    }
    zrc_ledOff(ledIndex);
	zrc_appVars.flags.bf.ledIndex = ledIndex;
	 zrc_appVars.blinkTimes = 0;
    if ( blinkTime == 0 ) {
        zrc_appVars.ledStopCnt = 0;
    } else  {
        zrc_appVars.ledStopCnt = blinkTime * 2 - 1;
    }

    g_appLedAction.modeNum = 0;
    zrc_appVars.flags.bf.ledStatus = 0;
    zrc_appVars.ledBlinkTimer = ev_on_timer(zrc_ledBlinkTimer, (void *)blinkInterval, 100);// blinkInterval);
}


void zrc_startLedSpecial(u8 blinkTime, app_ledFlushInfo_t *ledAction, u8 ledIndex){
	zrc_startLed(blinkTime, ledAction->offIntervalList[0], ledIndex);
	memcpy(&g_appLedAction, ledAction, sizeof(app_ledFlushInfo_t));
}


void zrc_ledOn(u8 ledIndex)
{
	if ( ledIndex == 0 ) {
    	gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_ON_LEVEL);
	}
	else if ( ledIndex == 1 ) {
		gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_ON_LEVEL);
	}
	else {
		gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_ON_LEVEL);
		gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_ON_LEVEL);
	}
    zrc_appVars.flags.bf.ledIdleState = 1;
}

void zrc_ledOff(u8 ledIndex)
{
    if ( ledIndex == 0 ) {
    	gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL);
	}
	else if ( ledIndex == 1 ) {
		gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_OFF_LEVEL);
	}
	else {
		gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL);
		gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_OFF_LEVEL);
	}
    zrc_appVars.flags.bf.ledIdleState = 0;
}

void zrc_stopLed(u8 ledIndex)
{

    if ( zrc_appVars.ledBlinkTimer ) {
        ev_unon_timer(&zrc_appVars.ledBlinkTimer);
    }
    zrc_appVars.flags.bf.ledStatus = 0;    
	if ( ledIndex == 0 ) {
    	gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledStatus);
	}
	else if ( ledIndex == 1 ) {
		gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledStatus);
	}
	else {
		gpio_write(ZRC_APP_GREEN_LED_GPIO, GREEN_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledStatus);
		gpio_write(ZRC_APP_RED_LED_GPIO, RED_LED_OFF_LEVEL^zrc_appVars.flags.bf.ledStatus);
	}
    zrc_appVars.ledStopCnt = 1;
    zrc_appVars.blinkTimes = 0;

}

#endif


