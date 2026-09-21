/********************************************************************************************************
 * @file    task_queue.h
 *
 * @brief   This is the header file for task_queue
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
#ifndef ZB_TASK_QUEUE_H
#define ZB_TASK_QUEUE_H          



#define TL_TASKQ_USERUSE_SIZE    16

enum{
    ZB_RET_OK,            /*!< status: success */
    ZB_RET_OVERFLOW,    /*!< status: array or buffer overflow */
};

enum{
    TL_Q_EV_TASK = 0,
    TL_Q_TYPE_MAX
};

/**
   Callback function typedef.
   Callback is function planned to execute by another function.
   Note that callback must be declared as reentrant for dscc.

   @param param - callback parameter - usually, but not always, ref to packet buf

   @return none.
 */
typedef void (*tl_task_callback_t)(void *arg);


typedef struct tl_zb_task_s{
    tl_task_callback_t    tlCb;
                  void    *data;
}tl_zb_task_t;

typedef struct{
    tl_zb_task_t    evt[TL_TASKQ_USERUSE_SIZE];
              u8    wptr;
              u8    rptr;
}tl_taskq_user_t;



#define    TL_QUEUE_HAS_SPACE(wptr, rptr, size)        ((wptr - rptr) < (size))

/**
   Initialize scheduler subsystem.
 */
void task_sched_init(void);


void tl_taskProcedure(void);

/**
  * @brief       get the valid task from task quenue list
  *
  * @param[in]   idx - the index of the task list
  *
  * @return      the pointer to the task list
  */
tl_zb_task_t *tl_taskQPop(u8 idx, tl_zb_task_t *taskInfo);


/**
  * @brief       push task to task list
  *
  * @param[in]   idx - the index of the task list
  *
  * @param[in]   task - the task will be push to task list
  *
  * @return      the status
  */
u8 tl_taskQPush(u8 idx, tl_zb_task_t *task);




/**
  * @brief       push a task to task list
  *
  * @param[in]   func - the callback of the event
  *
  * @param[in]   arg - the parameter to the callback
  *
  * @return      the status
  */
u8 tl_taskPost(tl_task_callback_t func, void *arg);
#define    TL_SCHEDULE_TASK    tl_taskPost


u8 tl_isTaskDone(void);
u8 tl_userTaskQNum(void);



#endif /* ZB_TASK_QUEUE_H */

