/********************************************************************************************************
 * @file    drv_hw.h
 *
 * @brief   This is the header file for drv_hw.h
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
#pragma once

#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
//    #define SYSTEM_RESET()            mcu_reset()
#elif defined(MCU_CORE_B92) || defined(MCU_CORE_TL321X)
    #define SYSTEM_RESET()     sys_reboot()
    #define rand()             drv_u32Rand()
    #define irq_enable()       core_interrupt_enable()
    #define irq_disable()      core_interrupt_disable()
    #define irq_restore(en)    core_restore_interrupt(en)

    #define reg_mac_channel    0x36
    #define reg_nwk_seq_no     0x37
#endif

typedef enum{
    SYSTEM_POWER_ON,
    SYSTEM_DEEP_BACK,
}startup_state_e;

extern u32 sysTimerPerUs;

startup_state_e drv_platform_init(void);



void drv_wd_setInterval(u32 ms);
void drv_wd_start(void);
void drv_wd_clear(void);

u32 drv_u32Rand(void);
void drv_generateRandomData(u8 *pData, u16 len);

void voltage_detect(void);
startup_state_e drv_mcu_status(void);

