/********************************************************************************************************
 * @file    timer.h
 *
 * @brief   This is the header file for timer.h
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


/** @addtogroup  TELINK_COMMON_MODULE TELINK Common Module
 *  @{
 */

/** @addtogroup  HARDWARETIMER Hardware Timer (GPT)
 *  @{
 */
 
/** @defgroup HARDWARETIMER_TYPE Hardware Timer Constants
 *  @{
 */
 
/** @addtogroup hardware_timer_index Hardware Timer Index
 * @{
 */
#define TIMER_IDX_0             0                   //!< Timer0 used as system tick
#define TIMER_IDX_1             1                   //!< Timer1 used as software timer @ref SOFTTIMER
#define TIMER_IDX_2             2                   //!< Timer2 is for user use
#define TIMER_IDX_3             3                   //!< Timer3 is for system tick
#define TIMER_NUM               4                   //!< Total number of timer

/** @} end of group hardware_timer_index */

/** @addtogroup hardware_timer_mode Hardware Timer Mode
 * @{
 */
#define TIMER_MODE_SCLK         0                   //!< Timer running in the system clock mode, it will free run from 0 to 0xffffffff
#define TIMER_MODE_GPIO         1                   //!< Timer running in the system clock mode, it will free run from 0 to 0xffffffff
#define TIMER_MODE_WIDTH_GPI    2
#define TIMER_MODE_TICK         3
/** @} end of group hardware_timer_mode */


/** @addtogroup hardware_timer_state Hardware Timer State
 * @{
 */
#define TIMER_IDLE              0                   //!< Indicating the timer is not runing
#define TIMER_WOF               1                   //!< Waiting for overflow
#define TIMER_WTO               2                   //!< Waitting for time out
/** @} end of group hardware_timer_state */

 
 
/** @} end of group HARDWARETIMER_CONSTANTS */


/** @defgroup HARDWARETIMER_CONSTANTS Hardware Timer Types
 *  @{
 */


/**
 *  @brief  Definition for 64 bit timer unit
 */
typedef struct {
    u32 low;
    u32 high;
} ext_clk_t;

/**
 *  @brief  Definition for Timer callback function type
 */
typedef int (* timerCb_t)(void*);


typedef enum hw_timer_sts_e {
    // success = 0
	HW_TIMER_SUCC = 0,
    HW_TIMER_IS_RUNNING = 1,
} hw_timer_sts_t;

/** @} end of group HARDWARETIMER_TYPE */







/** @defgroup HARDWARETIMER_Functions Hardware Timer API
 *  @{
 */

 /**
  * @brief       Reset the specified hardware timer module
  *
  * @param[in]   tmrIdx - Index of timer @ref hardware_timer_index
  *
  * @return      None
  */
void hwTmr_reset(u8 tmrIdx);

 /**
  * @brief       Initialize the specified hardware timer
  *
  * @param[in]   tmrIdx - Index of timer @ref hardware_timer_index
  * @param[in]   mode   - Specify the timer running mode @ref hardware_timer_mode
  *
  * @return      None
  */
void hwTmr_init(u8 tmrIdx, u8 mode);

 /**
  * @brief       Set a new timer through relative time value
  *
  * @param[in]   tmrIdx   - Index of timer @ref hardware_timer_index
  * @param[in]   interval - The time interval from current time. The unit is microsecond
  * @param[in]   func     - Pointer to the callback function to be invoked when the timer expiry
  * @param[in]   arg      - Argument of callback function
  *
  * @return      Status
  */
hw_timer_sts_t hwTmr_set(u8 tmrIdx, u32 t_us, timerCb_t func, void* arg);

 /**
  * @brief       Set a new timer through absolute time value
  *
  * @param[in]   tmrIdx   - Index of timer @ref hardware_timer_index
  * @param[in]   absTimer - Absolute time value
  * @param[in]   func     - Pointer to the callback function to be invoked when the timer expiry
  * @param[in]   arg      - Argument of callback function
  *
  * @return      Status
  */
hw_timer_sts_t hwTmr_setAbs(u8 tmrIdx, ext_clk_t* absTimer, timerCb_t func, void* arg);

/**
  * @brief       Cancel an existed timer
  *
  * @param[in]   tmrIdx  - Index of timer @ref hardware_timer_index
  *
  * @return      None
  */
void hwTmr_cancel(u8 tmrIdx);

/**
  * @brief       Interrupt handler of Timer
  *
  * @param       None
  *
  * @return      None
  */
void timer_irq0_handler(void);
void timer_irq1_handler(void);
void timer_irq3_handler(void);
_attribute_ram_code_ void timer_irq_handler(u8 tmrIdx);

void hwTmr_setInterval(u8 tmrIdx, u32 interval);

void hwTmr_idle(u8 tmrIdx);

/**  @} end of group HARDWARETIMER_Functions */

/**  @} end of group HARDWARETIMER */

/**  @} end of group TELINK_COMMON_MODULE */


 
