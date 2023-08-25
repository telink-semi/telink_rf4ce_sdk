/********************************************************************************************************
 * @file	zb_task_queue.c
 *
 * @brief	This is the source file for zb_task_queue
 *
 * @author	Zigbee Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/
#include "../tl_common.h"

enum{
	BUF_TYPE_NORMAL
};

tl_taskq_user_t	taskQ_user = {{0},0,0};


static u8 buf_type_get(void *arg){
	u8 ret = BUF_TYPE_NORMAL;

	return ret;
}

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






