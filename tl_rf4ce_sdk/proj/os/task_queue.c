/********************************************************************************************************
 * @file    task_queue.c
 *
 * @brief   This is the source file for task_queue
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

enum{
    BUF_TYPE_NORMAL
};

tl_taskq_user_t taskQ_user = {{{0}}};

_attribute_ram_code_ u8 tl_taskQPush(u8 idx, tl_zb_task_t *task){
    tl_zb_task_t *nTask = NULL;

    u32 r = irq_disable();

    if(!idx){
        if(TL_QUEUE_HAS_SPACE(taskQ_user.wptr, taskQ_user.rptr, TL_TASKQ_USERUSE_SIZE)){
            nTask = &taskQ_user.evt[taskQ_user.wptr%TL_TASKQ_USERUSE_SIZE];
            taskQ_user.wptr++;
        }
    }


    if(nTask){
        nTask->tlCb = task->tlCb;
        nTask->data = task->data;
    }else{
        irq_restore(r);
        return ZB_RET_OVERFLOW;
    }

    irq_restore(r);
    return ZB_RET_OK;
}


volatile u8 T_DBG_taskQPop_idx = 0;
volatile u32 T_DBG_taskQPop_cb = 0;
volatile u32 T_DBG_taskQPop_data = 0;
tl_zb_task_t *tl_taskQPop(u8 idx, tl_zb_task_t *taskInfo){
    tl_zb_task_t *nTask = NULL;
    //Reset task info
    taskInfo->data = 0;
    taskInfo->tlCb = 0;

    u32 r = irq_disable();

    if(!idx){
        if(taskQ_user.rptr != taskQ_user.wptr){
            nTask = &taskQ_user.evt[taskQ_user.rptr%TL_TASKQ_USERUSE_SIZE];
            taskQ_user.rptr++;
        }
    }


    if(nTask){
        taskInfo->data = nTask->data;
        taskInfo->tlCb = nTask->tlCb;
    }

    irq_restore(r);
    return nTask;
}



void task_sched_init(void){
    memset((u8 *)&taskQ_user, 0, sizeof(taskQ_user));
}

inline u8 tl_userTaskQNum(void){
    return (taskQ_user.wptr - taskQ_user.rptr);
}

void tl_taskProcedure(void){

    tl_zb_task_t zbNewTask;
    if(tl_taskQPop(TL_Q_EV_TASK, &zbNewTask)){
        zbNewTask.tlCb(zbNewTask.data);
    }
}

u8 tl_isTaskDone(void){
    if(taskQ_user.wptr != taskQ_user.rptr){
        return 0;
    }
    return 1;
}

u8 tl_taskPost(tl_task_callback_t func, void *arg){
    tl_zb_task_t taskEntry;
    taskEntry.tlCb = func;
    taskEntry.data = arg;

    u8 ret = tl_taskQPush(TL_Q_EV_TASK, &taskEntry);
    if(ret != ZB_RET_OK){
        /* TaskQ is full. */

    }

    return ret;
}
