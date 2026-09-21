/********************************************************************************************************
 * @file    drv_hw.c
 *
 * @brief   This is the source file for drv_hw
 *
 * @author  Zigbee GROUP
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
#include "../../net/rf4ce/rf4ce_includes.h"
#include "./drv_hw.h"
/*
 * system clock configuration
 */
#if defined(MCU_CORE_826x)
    #if(CLOCK_SYS_CLOCK_HZ == 32000000)
        #define SYS_CLOCK_VALUE        SYS_CLK_32M_PLL
    #elif(CLOCK_SYS_CLOCK_HZ == 16000000)
        #define SYS_CLOCK_VALUE        SYS_CLK_16M_PLL
    #else
        #error please config system clock
    #endif
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    #if(CLOCK_SYS_CLOCK_HZ == 24000000)
        #define SYS_CLOCK_VALUE        SYS_CLK_24M_Crystal
    #elif(CLOCK_SYS_CLOCK_HZ == 16000000)
        #define SYS_CLOCK_VALUE        SYS_CLK_16M_Crystal
    #elif(CLOCK_SYS_CLOCK_HZ == 32000000)
        #define SYS_CLOCK_VALUE        SYS_CLK_32M_Crystal
    #elif(CLOCK_SYS_CLOCK_HZ == 48000000)
        #define SYS_CLOCK_VALUE        SYS_CLK_48M_Crystal
    #else
        #error please config system clock
    #endif
#elif defined(MCU_CORE_B92)
    #if(CLOCK_SYS_CLOCK_HZ == 32000000)
        #define CLOCK_INIT            CCLK_32M_HCLK_32M_PCLK_16M
    #else
        #error please config system clock
    #endif
#elif defined(MCU_CORE_TL321X)
    #if(CLOCK_SYS_CLOCK_HZ == 48000000)
        #define CLOCK_INIT            PLL_192M_CCLK_48M_HCLK_48M_PCLK_24M_MSPI_48M
    #elif(CLOCK_SYS_CLOCK_HZ == 32000000)
        #define CLOCK_INIT            PLL_192M_CCLK_32M_HCLK_16M_PCLK_16M_MSPI_48M
    #else
        #error please config system clock
    #endif
#endif

#define BATTERY_SAFETY_THRESHOLD    2200   //2.2v

//system ticks per US
u32 sysTimerPerUs;



static void randInit(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    random_generator_init();
#elif defined(MCU_CORE_B92) || defined(MCU_CORE_TL321X)
    trng_init();
#endif
}

/*********************************************************************
 * @fn      internalFlashSizeCheck
 *
 * @brief   This function is provided to get and update to the correct flash address
 *             where are stored the right MAC address and pre-configured parameters.
 *             NOTE: It should be called before ZB_RADIO_INIT().
 *
 * @param   None
 *
 * @return  None
 */
static void internalFlashSizeCheck(void){
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL321X)
    u32 mid = flash_read_mid();
    u8 *pMid = (u8 *)&mid;

    if ((pMid[2] < FLASH_SIZE_512K) || \
        ((g_u32MacFlashAddr == MAC_ADDR_1M_FLASH) && (pMid[2] < FLASH_SIZE_1M)) || \
        ((g_u32MacFlashAddr == MAC_ADDR_2M_FLASH) && (pMid[2] < FLASH_SIZE_2M)) || \
        ((g_u32MacFlashAddr == MAC_ADDR_4M_FLASH) && (pMid[2] < FLASH_SIZE_4M))) {
        /* Flash space not matched. */
        while(1);
    }

    switch (pMid[2]) {
    case FLASH_SIZE_1M:
        g_u32MacFlashAddr = MAC_ADDR_1M_FLASH;
        g_u32CfgFlashAddr = CFG_ADDR_1M_FLASH;
        break;
    case FLASH_SIZE_2M:
        g_u32MacFlashAddr = MAC_ADDR_2M_FLASH;
        g_u32CfgFlashAddr = CFG_ADDR_2M_FLASH;
        break;
    case FLASH_SIZE_4M:
        g_u32MacFlashAddr = MAC_ADDR_4M_FLASH;
        g_u32CfgFlashAddr = CFG_ADDR_4M_FLASH;
        break;
    default:
        break;
    }
#endif
}





static startup_state_e platform_wakeup_init(void)
{
    startup_state_e state = SYSTEM_POWER_ON;
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    extern void bss_section_clear(void);
    extern void data_section_load();
    bss_section_clear();
    data_section_load();
#endif

#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258)
    cpu_wakeup_init();
#elif defined(MCU_CORE_8278)
    cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);
#elif defined(MCU_CORE_B92)
    sys_init(LDO_1P4_LDO_2P0, VBAT_MAX_VALUE_GREATER_THAN_3V6, GPIO_VOLTAGE_3V3, INTERNAL_CAP_XTAL24M);
#elif defined(MCU_CORE_TL321X)
    sys_init(DCDC_1P25_LDO_1P8, VBAT_MAX_VALUE_GREATER_THAN_3V6, INTERNAL_CAP_XTAL24M);
#endif

#if defined(MCU_CORE_B92) || defined(MCU_CORE_TL321X)
    wd_32k_stop();
    wd_stop();

    pm_update_status_info(1);
#endif

    drv_calibration();

#if defined(MCU_CORE_826x) ||defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    state = (pm_get_mcu_status() == MCU_STATUS_DEEP_BACK) ? SYSTEM_DEEP_BACK : SYSTEM_POWER_ON;
#elif defined(MCU_CORE_B92)
    state = (g_pm_status_info.mcu_status == MCU_DEEP_BACK) ? SYSTEM_DEEP_BACK : SYSTEM_POWER_ON;
#elif defined(MCU_CORE_TL321X)
    state = (g_pm_status_info.mcu_status == MCU_STATUS_DEEP_BACK) ? SYSTEM_DEEP_BACK : SYSTEM_POWER_ON;
#endif

    return state;
}

/****************************************************************************************************
* @brief         platform initialization function
*
* @param[in]     none
*
* @return          1: startup with ram retention;
*                 0: no ram retention.
*/
startup_state_e drv_platform_init(void)
{
    startup_state_e state = platform_wakeup_init();

#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    clock_init(SYS_CLOCK_VALUE);
#elif defined(MCU_CORE_B92)
    CCLK_32M_HCLK_32M_PCLK_16M;
#elif defined(MCU_CORE_TL321X)
    CLOCK_INIT;
#endif

    /* Get system ticks per US, must be after the clock is initialized. */
#if defined(MCU_CORE_826x)
    sysTimerPerUs = tickPerUs;
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    sysTimerPerUs = sys_tick_per_us;
#elif defined(MCU_CORE_B92) || defined(MCU_CORE_TL321X)
    sysTimerPerUs = SYSTEM_TIMER_TICK_1US;
#endif

    gpio_init(TRUE);

#if UART_PRINTF_MODE
    DEBUG_TX_PIN_INIT();
#endif

    if(state == SYSTEM_POWER_ON){
        randInit();
#if defined(MCU_CORE_B92) || defined(MCU_CORE_TL321X)
        clock_cal_24m_rc();
        clock_32k_init(CLK_32K_RC);
        clock_cal_32k_rc();    //6.68ms
#elif defined(MCU_CORE_8278) || defined(MCU_CORE_8258)
        clock_32k_init(CLK_32K_RC);
        rc_32k_cal();
#endif
        internalFlashSizeCheck();
    }else{
    }

#if defined(MCU_CORE_8278) || defined(MCU_CORE_8258)
        pm_select_internal_32k_rc();
#endif


#if (MODULE_USB_ENABLE)
    usb_set_pin_en();
#if defined(MCU_CORE_TL321X)
    usb_edp_en();
#endif
#endif

#if defined(MCU_CORE_TL321X)
    /* Enable AES and ECC after clock_init */
    ske_dig_en();
    pke_dig_en();
#endif

    ZB_RADIO_INIT();

    ZB_TIMER_INIT();

    return state;
}



void drv_wd_setInterval(u32 ms)
{
    wd_set_interval_ms(ms);
}

void drv_wd_start(void)
{
    wd_start();
}

void drv_wd_clear(void)
{
    wd_clear();
}

u32 drv_u32Rand(void)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    return rand();
#elif defined(MCU_CORE_B92) || defined(MCU_CORE_TL321X)
    return trng_rand();
#endif
}

void drv_generateRandomData(u8 *pData, u16 len)
{
    u32 randNums = 0;
    /* if len is odd */
    for(u16 i = 0; i < len; i++){
        if((i & 3) == 0){
            randNums = drv_u32Rand();
        }

        pData[i] = randNums & 0xff;
        randNums >>= 8;
    }
}


startup_state_e drv_mcu_status(void)
{
    startup_state_e state = SYSTEM_POWER_ON;

#if defined(MCU_CORE_826x) ||defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    state = (pm_get_mcu_status() == MCU_STATUS_DEEP_BACK) ? SYSTEM_DEEP_BACK : SYSTEM_POWER_ON;
#elif defined(MCU_CORE_B92)
    state = (g_pm_status_info.mcu_status == MCU_DEEP_BACK) ? SYSTEM_DEEP_BACK : SYSTEM_POWER_ON;
#elif defined(MCU_CORE_TL321X)
    state = (g_pm_status_info.mcu_status == MCU_STATUS_DEEP_BACK) ? SYSTEM_DEEP_BACK : SYSTEM_POWER_ON;
#endif
    return state;
}

