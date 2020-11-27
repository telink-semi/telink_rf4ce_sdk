/**************************************************************************************************
  Filename:       ev_buffer.c
  Revised:        $Date: 2013-05-27 $

  Description:    Implementation of Telink EV Buffer Module
**************************************************************************************************/

#include "../tl_common.h"
#include "ev_buffer.h"
#include "../common/utility.h"

#ifdef WIN32
#include <malloc.h>
#endif

ev_bufItem_t ev_bufItem[BUFFER_NUM] = {{0}};

#if USE_NEW_MSG_MANAGER

u32 Msg_Ctrl_Matrix[Msg_Ctrl_Row_Num] = {0};

void ev_buf_reset(void){
    int i;
    u32 j = 1;
    for(i=0;i<Msg_Ctrl_Row_Num;i++){
        Msg_Ctrl_Matrix[i] = 0;
    }
    for(i=0;i<BUFFER_NUM;i++){
        ev_bufItem[i].magic = ~(j<<i);
    }
}

void ev_buf_init(void){
    ev_buf_reset();
}
//tianyy
//u16 maxsize = 0;
//end
u8* ev_buf_allocate(u16 size){
    u8 r;
    u32 temp;

    r = irq_disable();
    //tianyy
    //if(size>maxsize){
        //maxsize = size;
    //}
    //end
    temp = Msg_Ctrl_Matrix[BufItem_BitMap];
    temp ^= temp + 1;

    #if 1
    temp = (temp&0x55555555) + ((temp>>1)&0x55555555); 
    temp = (temp&0x33333333) + ((temp>>2)&0x33333333); 
    temp = (temp&0x0f0f0f0f) + ((temp>>4)&0x0f0f0f0f); 
    temp = (temp&0x00ff00ff) + ((temp>>8)&0x00ff00ff); 
    temp = (temp&0x0000ffff) + ((temp>>16)&0x0000ffff);
    #else
    temp = (temp&0x55) + ((temp>>1)&0x55); 
    temp = (temp&0x33) + ((temp>>2)&0x33); 
    temp = (temp&0x0f) + ((temp>>4)&0x0f); 
    #endif
    
    if((temp<1)||(temp>LengthOfArray(ev_bufItem))){
        irq_restore(r);
        return NULL;
    }else{
        temp--;
        Msg_Ctrl_Matrix[BufItem_BitMap] |= BIT(temp);
        irq_restore(r);
        return ev_bufItem[temp].data;
    }
}

buf_sts_t ev_buf_free(u8* pBuf)
{
    u8 r;
    ev_bufItem_t *pDelBuf;

    if (!pBuf) {
        return BUFFER_INVALID_PARAMETER;
    }

    r = irq_disable();

    pDelBuf = ev_buf_getHead(pBuf);
    //pDelBuf->state = BUF_ITEM_STATE_FREE;
    Msg_Ctrl_Matrix[BufItem_BitMap] &= pDelBuf->magic;
    irq_restore(r);
    return BUFFER_SUCC;
}

u8 buf_message_post(u8 *ptr,msg_ctrl_row_t state)
{
        u8 r;
		ev_bufItem_t *buffer_item_ptr;
        u32 i;

        r = irq_disable();
        i = (Msg_Ctrl_Matrix[Msg_Indicator_Vector]&INDICATOR_MASK(state))>>INDICATOR_OFFSET(state);
        
        if(i&8){
            irq_restore(r);
            return 1;
        }
        //Msg_Ctrl_Matrix[state] &= ~(0xf<<(i<<2));
        buffer_item_ptr = (ev_bufItem_t*)ev_buf_getHead(ptr);
        Msg_Ctrl_Matrix[state] |=(buffer_item_ptr - ev_bufItem)<<(i<<2);
        Msg_Ctrl_Matrix[Msg_Indicator_Vector] += 1<< INDICATOR_OFFSET(state);

        irq_restore(r);
        return 0; 
}

u8 *buf_message_poll(msg_ctrl_row_t state)
{
    u8 r;
    u32 i;
    
    r = irq_disable();
    if(Msg_Ctrl_Matrix[Msg_Indicator_Vector]&INDICATOR_MASK(state)){
        i = Msg_Ctrl_Matrix[state]&0xf;
        Msg_Ctrl_Matrix[state] >>= 4;
        Msg_Ctrl_Matrix[Msg_Indicator_Vector] -= 1<< INDICATOR_OFFSET(state);
        irq_restore(r);
        return ev_bufItem[i].data;
    }else{
        irq_restore(r);
        return NULL;
    }
}


#else


typedef struct _ev_pool_t{
	ev_bufItem_t *head;
	ev_bufItem_t *tail;
	u8 num;
}ev_pool_t;

ev_pool_t ev_pool;

void ev_pool_init(void){
	ev_pool.head = ev_pool.tail = NULL;
	ev_pool.num = 0;
}
/*********************************************************************
 * @fn      ev_buf_reset
 *
 * @brief   Reset the EV Buffer module
 *
 * @param   None
 *
 * @return  None
 */
void ev_buf_reset(void)
{
    int i;
    for(i=0;i<LengthOfArray(ev_bufItem);i++){
        #if __DEBUG_BUFFER__
        memset(ev_bufItem[i].file_name,0x00,LengthOfArray(ev_bufItem[i].file_name));
        ev_bufItem[i].line = 0;
        #endif
        ev_bufItem[i].next = NULL;//0xfefefefe;
        ev_bufItem[i].used = 0x0;
        memset(ev_bufItem[i].buf_item_state_tbl,0x00,LengthOfArray(ev_bufItem[i].buf_item_state_tbl));
    }

    ev_pool_init();
}


/*********************************************************************
 * @fn      ev_buf_init
 *
 * @brief   Initialize the EV Buffer module
 *
 * @param   None
 *
 * @return  None
 */
void ev_buf_init(void)
{
    ev_buf_reset();
}

#if __DEBUG_BUFFER__
static inline int getFileNamePos(u8 *str){
    int i;
    int pos = 0;
    if(str == NULL){
        return -1;
    }
    //../rf4ce/mac/mac_trx.c
    for(i=0;str[i];i++){
        if(str[i] == '/'){
            pos = i + 1;
        }
    }
    return pos;
}

#endif

/*********************************************************************
 * @fn      ev_buf_allocate
 *
 * @brief   Allocate an available buffer according to the requested size
 *          The allocated buffer will have only three kind of size, defined
 *          in @ref EV_BUFFER_CONSTANT
 *
 * @param   size - requested size
 *
 * @return  Pointer to an allocated buffer.
 *          NULL means the there is no available buffer.
 */
#if __DEBUG_BUFFER__
u8* __ev_buf_allocate__(u8 *filename,u16 line)
#else
u8* __ev_buf_allocate__()
#endif
{
    int i;
    u8 r;

    r = irq_disable();

    for(i=0;i<LengthOfArray(ev_bufItem);i++){
        if(ev_bufItem[i].buf_item_state_tbl[BUF_ITEM_STATE_BUSY] == 0){
            ev_bufItem[i].buf_item_state_tbl[BUF_ITEM_STATE_BUSY] = 1;
            ev_bufItem[i].used = 0xffff;
            #if __DEBUG_BUFFER__
            k = getFileNamePos(filename);
            if(k == -1){
                while(1){;}
            }
            
            for(j=0;(j<LengthOfArray(ev_bufItem[i].file_name))&&(filename[k]);j++,k++){
                ev_bufItem[i].file_name[j] = filename[k];
            }
            while(j<LengthOfArray(ev_bufItem[i].file_name)){
                ev_bufItem[i].file_name[j] = 0;
                j++;
            }
            ev_bufItem[i].line = line;
            #endif
            irq_restore(r);
            return ev_bufItem[i].data;
        }
    }
    irq_restore(r);

    return NULL;
}


/*********************************************************************
 * @fn      ev_buf_free
 *
 * @brief   Free the specified buffer
 *
 * @param   pBuf - the pointer to the specified buffer to free.
 *
 * @return  status
 */

#if __DEBUG_BUFFER__
u8 __filename__[64] = {0};
#endif

#if __DEBUG_BUFFER__
buf_sts_t __ev_buf_free__(u8 *filename,u16 line,u8* pBuf)
#else
buf_sts_t __ev_buf_free__(u8* pBuf)
#endif
{
    u8 r;
    ev_bufItem_t *pDelBuf;

    
    if (!pBuf) {
        return BUFFER_INVALID_PARAMETER;
        //while(1){;}
    }

    r = irq_disable();

    pDelBuf = ev_buf_getHead(pBuf);
    //CHECK_ARRAY_ELEM_POINTER(ev_bufItem,pDelBuf,step,p_value);
    pDelBuf->buf_item_state_tbl[BUF_ITEM_STATE_BUSY] = 0;
    pDelBuf->used = 0;
    #if __DEBUG_BUFFER__
    k = getFileNamePos(filename);
    if(k == -1){
        int index;
        for(index =0;(filename[index])&&(index<LengthOfArray(__filename__));index++){
            __filename__[index] = filename[index];
        }
        while(1){;}
    }
    for(j=0;(j<LengthOfArray(pDelBuf->file_name))&&(filename[k]);j++,k++){
                pDelBuf->file_name[j] = filename[k];
            }
            while(j<LengthOfArray(pDelBuf->file_name)){
                pDelBuf->file_name[j] = 0;
                j++;
            }
            pDelBuf->line = line;
    #endif
    irq_restore(r);
    return BUFFER_SUCC;
}

u8 ev_isTaskDone(void){
    return (ev_pool.num > 0 ? 0 : 1);
#if 0
    for(state = BUF_ITEM_STATE_PHY2MAC;state<MAX_BUF_ITEM_STATE;state++){
        for(i=0;i<LengthOfArray(ev_bufItem);i++){
            if(ev_bufItem[i].buf_item_state_tbl[state] == 1){
                return 0;
            }
        }
    }
    return 1;
#endif
}
u8 buf_message_post(u8 *ptr,buf_item_state_t state)
{
	u8 r;
	ev_bufItem_t *buffer_item_ptr;

	r = irq_disable();

	buffer_item_ptr = (ev_bufItem_t*)ev_buf_getHead(ptr);
	buffer_item_ptr->buf_item_state_tbl[state] = 1;
	buffer_item_ptr->next = NULL;

	if(ev_pool.head == NULL){
		ev_pool.head = ev_pool.tail = buffer_item_ptr;
	}else{
		ev_pool.tail->next = buffer_item_ptr;
		ev_pool.tail = ev_pool.tail->next;
	}
	ev_pool.num++;

	irq_restore(r);
	return 0;
}

u8 *buf_message_poll(buf_item_state_t *state)
{
	int i;
    u8 r;
    u8 *pBuf = NULL;

    r = irq_disable();

    *state = 0xff;
    if(ev_pool.head){
    	for(i = BUF_ITEM_STATE_PHY2MAC; i <= BUF_ITEM_STATE_MAC2CSMA; i++){
    		if(ev_pool.head->buf_item_state_tbl[i] == 1){
				 ev_pool.head->buf_item_state_tbl[i] = 0;
				 *state = i;
				 pBuf = ev_pool.head->data;

				 ev_bufItem_t *p = ev_pool.head;
				 ev_pool.head = ev_pool.head->next;
				 p->next = NULL;
				 ev_pool.num--;
				 break;
			 }
    	}
    }
    irq_restore(r);
    return pBuf;
}



#endif



/*********************************************************************
 * @fn      ev_buf_getHead
 *
 * @brief   Get the header pointer of a buffer item
 *
 * @param   pd - the pointer of a data, which is previously allocated
 *
 * @return  Pointer of bufferItem
 */
ev_bufItem_t* ev_buf_getHead(u8* pd)
{
    return (ev_bufItem_t*)(pd - OFFSETOF(ev_bufItem_t, data));
}

/*********************************************************************
 * @fn      ev_buf_getTail
 *
 * @brief   Get the pointer from a EV BUFFER tail.
 *
 * @param   pd - the pointer of a data, which is previously allocated
 * @param   offsetToTail - The offset to Tail
 *
 * @return  Pointer of the specified memory
 */
u8* ev_buf_getTail(u8* pd, int offsetToTail)
{
	return (u8*)(pd - RF_RX_BUFFER_OFFSET + BUFFER_SIZE - offsetToTail);
}




_attribute_ram_code_ u16 ev_buf_getfreeSize(void)
{
	u16 size = 0;

    for(u16 i=0;i<LengthOfArray(ev_bufItem);i++){
        if(ev_bufItem[i].buf_item_state_tbl[BUF_ITEM_STATE_BUSY] == 0){
        	size++;
        }
    }
	return size;
}
