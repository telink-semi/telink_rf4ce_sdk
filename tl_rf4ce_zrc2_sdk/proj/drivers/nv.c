/********************************************************************************************************
 * @file     nv.c
 *
 * @brief	 permanent information should stroed in the flash
 *
 * @author
 * @date     Oct. 8, 2016
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *           The information contained herein is confidential property of Telink
 *           Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *           of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *           Co., Ltd. and the licensee or the terms described here-in. This heading
 *           MUST NOT be removed from this file.
 *
 *           Licensees are granted free, non-transferable use of the information in this
 *           file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#include "../tl_common.h"
#include "drv_flash.h"
#include "nv.h"
#include "../os/ev_buffer.h"

#if (MODULE_FLASH_ENABLE)


#if (FLASH_PROTECT)
const u8 protect_flash_cmd = FLASH_PROTECT_CMD;
#endif


#define ENABLE_CHECK_SUM        1
typedef struct{
	u32 freq;
	u8 valid;
	u8 keycode;
	u16 cnt;
}nv_irCtxHdr_t;

typedef struct{
	nv_irCtxHdr_t hdr;
	u16 data[124];
}nv_irCtx_t _attribute_aligned_(4);

#define NV_IR_IDX_IDLE 			0xff
#define NV_IR_IDX_VALID 		0x5a
#define NV_IR_IDX_INVALID 		0x50
#define NV_IR_IDXBLK_INVALID 	0x00

#define NV_IR_ADDR_START		(384*1024)
#define NV_IR_CTX_BLOCK_NUM		2
#define NV_IR_CTX_BLOCK_SIZE	(32*1024)
#define NV_IR_CTX_UNIT_SIZE		(256)
#define NV_IR_CTX_NUM			(NV_IR_CTX_BLOCK_SIZE/NV_IR_CTX_UNIT_SIZE)

nv_sts_t nv_ir_write(u8 keycode,  u32 freq, u16 *buf, u16 len){
	nv_irCtx_t *pCtxBlk = (nv_irCtx_t *)NV_IR_ADDR_START;
	nv_irCtx_t *pCtx = pCtxBlk;
	u32 curBlk = 0;
	u32 i = 0;
	nv_irCtx_t *pCtxBlkTmp = pCtxBlk;

	for(i = 0; i < NV_IR_CTX_BLOCK_NUM; i++){
		if(pCtxBlkTmp->hdr.valid != NV_IR_IDX_IDLE){
			pCtx = pCtxBlkTmp;
			curBlk = i;
			break;
		}
		pCtxBlkTmp += NV_IR_CTX_NUM;
	}

	for(i = 0; i < NV_IR_CTX_NUM; i++){
		if(pCtx->hdr.valid == NV_IR_IDX_IDLE){
			break;
		}
		pCtx++;
	}

	u8 r = irq_disable();

#if (FLASH_PROTECT)
		if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
#endif

	if(i == NV_IR_CTX_NUM){
		/* copy valid data to new block; and then erase old block */
		u32 eraseAddr = (curBlk == 0) ? (NV_IR_ADDR_START + NV_IR_CTX_BLOCK_SIZE) : NV_IR_ADDR_START;
		/* erase new block if it not clean */
		for(u32 j = 0; j < NV_IR_CTX_BLOCK_SIZE/NV_SECTION_SIZE; j++){
			flash_erase_sector(eraseAddr + j * NV_SECTION_SIZE);
		}

		nv_irCtx_t *pNewCtx = (nv_irCtx_t *)eraseAddr;
		pCtx = pCtxBlk;
		u32 readAddr = (u32)pCtx;
		u32 validDataNum = 0;
		u8 *readBuf = ev_buf_allocate(LARGE_BUFFER);
		for(u32 j = 0; j < NV_IR_CTX_NUM; j++){
			readAddr = (u32)pCtx;
			if(pCtx->hdr.valid == NV_IR_IDX_VALID && pCtx->hdr.keycode != keycode ){
				for(u32 k = 0; k < 2; k++){
					flash_read(readAddr, 128, readBuf);
					flash_write(eraseAddr, 128, readBuf);
					eraseAddr += 128;
				}
				pNewCtx++;
				validDataNum++;
			}
			pCtx++;
		}
		ev_buf_free(readBuf);


		/* if have empty space, fill data, or return error */
		if(validDataNum >= NV_IR_CTX_NUM){
#if (FLASH_PROTECT)
			if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
				irq_restore(r);
				return NV_ENABLE_PROTECT_ERROR;
			}
#endif
			irq_restore(r);
			return NV_NOT_ENOUGH_SAPCE;
		}else{
			nv_irCtxHdr_t hdr;
			hdr.freq = freq;
			hdr.valid = NV_IR_IDX_VALID;
			hdr.keycode = keycode;
			hdr.cnt = len;
			u32 addr = (u32)pNewCtx;
			flash_write(addr, sizeof(nv_irCtxHdr_t), (u8 *)&hdr);
			flash_write(addr + sizeof(nv_irCtxHdr_t), len*sizeof(u16), (u8 *)buf);
		}

		/* erase old block */
		u32 oldAddr = (u32)pCtxBlk;
		for(u32 j = 0; j < NV_IR_CTX_BLOCK_SIZE/NV_SECTION_SIZE; j++){
			flash_erase_sector(oldAddr + j * NV_SECTION_SIZE);
		}
	}else{
		nv_irCtxHdr_t hdr;
		hdr.freq = freq;
		hdr.valid = NV_IR_IDX_VALID;
		hdr.keycode = keycode;
		hdr.cnt = len;
		u32 addr = (u32)pCtx;

		/* invalid old data if the key code is same */
		nv_irCtx_t *pCtxCcancel = pCtxBlk;
		u8 valid  = NV_IR_IDX_INVALID;
		for(u32 j = 0; j < NV_IR_CTX_NUM; j++){
			if(pCtxCcancel->hdr.keycode == keycode){
				flash_write((u32)&((pCtxCcancel->hdr.valid)), 1, &valid);
			}
			pCtxCcancel++;
		}

		flash_write(addr, sizeof(nv_irCtxHdr_t), (u8 *)&hdr);
		flash_write(addr+sizeof(nv_irCtxHdr_t), len*sizeof(u16), (u8 *)buf);
	}

#if (FLASH_PROTECT)
		if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
#endif

	irq_restore(r);
	return NV_SUCC;
}

nv_sts_t nv_ir_read(u8 keyCode, u32 *freq, u16 *buf, u16 *len){
	nv_irCtx_t *pCtxBlk = (nv_irCtx_t *)NV_IR_ADDR_START;
	nv_irCtx_t *pCtx = pCtxBlk;

	for(u32 i = 0; i < NV_IR_CTX_BLOCK_NUM; i++){
		if(pCtxBlk->hdr.valid != NV_IR_IDX_IDLE){
			pCtx = pCtxBlk;
			break;
		}
		pCtxBlk += NV_IR_CTX_NUM;
	}

	for(u32 j = 0; j < NV_IR_CTX_NUM; j++){
		if(pCtx->hdr.keycode == keyCode && pCtx->hdr.valid == NV_IR_IDX_VALID){
			*freq = pCtx->hdr.freq;
			*len = pCtx->hdr.cnt;
			flash_read((u32)(pCtx->data), pCtx->hdr.cnt*sizeof(u16), (u8 *)buf);
			return NV_SUCC;
		}
		pCtx++;
	}
	return NV_ITEM_NOT_FOUND;
}

nv_sts_t nv_ir_reset(void){
	u8 r = irq_disable();

#if (FLASH_PROTECT)
	if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
		irq_restore(r);
		return NV_ENABLE_PROTECT_ERROR;
	}
#endif

	/* erase nv ir block */
	u32 eraseAddr = NV_IR_ADDR_START;
	for(u32 j = 0; j < 2*NV_IR_CTX_BLOCK_SIZE/NV_SECTION_SIZE; j++){
		flash_erase_sector(eraseAddr + j * NV_SECTION_SIZE);
	}

#if (FLASH_PROTECT)
	if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
		irq_restore(r);
		return NV_ENABLE_PROTECT_ERROR;
	}
#endif

	irq_restore(r);

	return NV_SUCC;
}

void ds_flashWrite(u32 startAddr, u16 len, u8 *buf)
{
	u8 r = irq_disable();
	u16 writeLen = (len > PAGE_AVALIABLE_SIZE(startAddr)) ? PAGE_AVALIABLE_SIZE(startAddr) : len;
	wd_clear();
	flash_write(startAddr, writeLen, buf);

    if ( len > PAGE_AVALIABLE_SIZE(startAddr) ) {
        u32 dstAddr = startAddr + writeLen;
        u16 dataOffset = writeLen;
        len -= writeLen;
        while ( len > 0 ) {
			writeLen = (len > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : len;
			flash_write(dstAddr, writeLen, buf + dataOffset);
            if ( len > FLASH_PAGE_SIZE ) {
                len -= FLASH_PAGE_SIZE;
                dataOffset += FLASH_PAGE_SIZE;
                dstAddr += FLASH_PAGE_SIZE;
            } else {
                len = 0;
            }
			wd_clear();
        }
    }
	irq_restore(r);
}

nv_sts_t  nv_init(u8 rst)
{
    if ( rst ) {
		u8 r = irq_disable();
        /* if reset is true, erase all flash for NV */
    	u8 i;
#if (FLASH_PROTECT)
		if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
#endif
	    for ( i=0; i<DS_MAX_MODULS; i++ ) {
#if(MODULE_WATCHDOG_ENABLE)
			wd_clear();
#endif
			flash_erase_sector(MOUDLES_START_ADDR(i));
#if(MODULE_WATCHDOG_ENABLE)
			wd_clear();
#endif
			flash_erase_sector(MOUDLES_SECOND_HALF_ADDR(i));
	    }
#if (FLASH_PROTECT)
		if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
#endif
		irq_restore(r);
		return NV_SUCC;
    }
	else {
		return NV_SUCC;
	}
}

u8 nv_calculateCheckSum(u8 *buf, u16 len)
{
    u8 checkSum = *buf;
    u16 i;
    for ( i=1; i<len; i++ ) {
        checkSum ^= buf[i];
    }
    return checkSum;
}

/* find the item address in nv */
nv_sts_t nv_findItem(u8 modules, u8 id, item_info_t *itemInfo, u8 verify)
{
    u8 j;
	u16 activeFalgs = 0;
	//u16 verifySize = 0;
    u8 buf[NV_HEADER_SIZE];
    nv_header_t *nvHdrPtr;
	u32 readAddr;
	u32 startAddr = MOUDLES_START_ADDR(modules);
	flash_read(startAddr, sizeof(u16), (u8*)&activeFalgs);
	/* read first two byte of every page and check the first two bytes */
	while( startAddr < MOUDLES_START_ADDR(modules + 1) && activeFalgs == 0 ) {
		startAddr += FLASH_PAGE_SIZE;
    	flash_read(startAddr, sizeof(u16), (u8*)&activeFalgs);
		wd_clear();
	}
	if ( startAddr == MOUDLES_START_ADDR(modules + 1) ) {
		return NV_ITEM_NOT_FOUND;
	}
	itemInfo->startAddr = startAddr;
	if ( activeFalgs != NV_ACTIVE_PAGE_FLAGS ) {
		itemInfo->hdrInfo.id = INVALID_NV_VALUE;
		itemInfo->usedSize = 0;
		itemInfo->nvOffset = NV_PAGE_HEADER_LEN;
		return NV_SUCC;
	}

	flash_read(startAddr + sizeof(u16), sizeof(u16), (u8*)&(itemInfo->usedSize));


    /* find the id page which contain the required id */
    for ( j=0; j<NV_HEADER_TABLE_SIZE; j++ ) {
		wd_clear();
		readAddr = startAddr + NV_PAGE_HEADER_LEN + NV_HEADER_SIZE * j;
		flash_read(readAddr, NV_HEADER_SIZE, buf);
        nvHdrPtr = (nv_header_t*)buf;
		itemInfo->nvOffset = NV_PAGE_HEADER_LEN + j * NV_HEADER_SIZE;
        /* if the id is found, return it */
        if ( nvHdrPtr->id == id ) {
            memcpy(&(itemInfo->hdrInfo), nvHdrPtr, sizeof(nv_header_t));
			return NV_SUCC;
        }
		else if ( nvHdrPtr->id == INVALID_NV_VALUE ) {
			itemInfo->hdrInfo.id = INVALID_NV_VALUE;
			return NV_SUCC;
        }
    }
    return NV_NOT_ENOUGH_SAPCE;
}

void  nv_xferData(u32 srcAddr, u32 dstAddr, u16 size)
{
    u32 srcA = srcAddr;
    u32 dstA = dstAddr;
    u16 readSize;

    u8 tmp[READ_BYTES_PER_TIME];
    u16 xferLen;
    u32 xferSrc;
    u32 xferDst;
    u8 xferSize;

    /* get the avaliable page size */
    readSize = PAGE_AVALIABLE_SIZE(srcAddr);
    readSize = (readSize > size) ? size : readSize;
	wd_clear();
    xferLen = readSize;
    xferSrc = srcAddr;
    xferDst = dstAddr;
    while ( xferLen > 0 ) {
        xferSize = (xferLen > READ_BYTES_PER_TIME) ? READ_BYTES_PER_TIME : xferLen;
        flash_read(xferSrc, xferSize, tmp);
        flash_write(xferDst, xferSize, tmp);
        xferSrc += xferSize;
        xferDst += xferSize;
        if ( xferLen > READ_BYTES_PER_TIME ) {
            xferLen -= READ_BYTES_PER_TIME;
        } else {
            xferLen = 0;
        }
		wd_clear();
    }

    if ( size > PAGE_AVALIABLE_SIZE(srcAddr) ) {
        size -= readSize;
        srcA += readSize;
        dstA += readSize;
        while ( size > 0 ) {
            readSize = (size > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : size;
            xferLen = readSize;
            xferSrc = srcA;
            xferDst = dstA;
            while ( xferLen > 0 ) {
                xferSize = (xferLen > READ_BYTES_PER_TIME) ? READ_BYTES_PER_TIME : xferLen;
                flash_read(xferSrc, xferSize, tmp);
				wd_clear();
				flash_write(xferDst, xferSize, tmp);
                xferSrc += xferSize;
                xferDst += xferSize;
                if ( xferLen > READ_BYTES_PER_TIME ) {
                    xferLen -= READ_BYTES_PER_TIME;
                } else {
                    xferLen = 0;
                }
            }
			wd_clear();
            srcA += readSize;
            dstA += readSize;
            if ( size > FLASH_PAGE_SIZE ) {
                size -= FLASH_PAGE_SIZE;
            } else {
                size = 0;
            }
        }
    }
}

nv_sts_t  nv_write(u8 modules, u8 id, u16 len, u8 *buf)
{
    item_info_t itemInfo;
	//u8 need2erase = FALSE;
	u32 nextPageAddr;
	u16 inactiveFlags = 0;
	u16 activeFlags = NV_ACTIVE_PAGE_FLAGS;
	u8 firstItem = 0;
	u8 flashUnportect = 0;
	u8 checksum = nv_calculateCheckSum(buf, len);

    if ( id == INVALID_NV_VALUE ) {
        return NV_INVALID_ID;
    }

    if ( modules > DS_MAX_MODULS ) {
        return NV_INVALID_MODULS;
    }

   	u8 r = irq_disable();

    if ( SUCCESS != nv_findItem(modules, id, &itemInfo, 1) ) {
		irq_restore(r);
        return NV_NOT_ENOUGH_SAPCE;
    }

    nv_header_t *nvHdr = &(itemInfo.hdrInfo);

	/* if the sector is empty and this the first item */
	if ( itemInfo.usedSize == 0 && itemInfo.startAddr == MOUDLES_START_ADDR(modules) ) {
		nextPageAddr = MOUDLES_START_ADDR(modules);
	}
	else {
		nextPageAddr = NV_NEXT_PAGE_ADDR(itemInfo.startAddr + itemInfo.usedSize);
		/* if the next start address is still in the same sector, check the available size */
		if ( nextPageAddr < MOUDLES_START_ADDR(modules+1) ) {
			/* if the id is already existed */
			if ( itemInfo.hdrInfo.id != INVALID_NV_VALUE && MOUDLES_AVALIABLE_SIZE(nextPageAddr) < itemInfo.usedSize ) {
				nextPageAddr = ( nextPageAddr > MOUDLES_SECOND_HALF_ADDR(modules) ) ? MOUDLES_START_ADDR(modules) : MOUDLES_SECOND_HALF_ADDR(modules);
			}
			else if ( (itemInfo.hdrInfo.id == INVALID_NV_VALUE) && MOUDLES_AVALIABLE_SIZE(nextPageAddr) < NV_ALIGN_LENTH(len + 1) ) {
				nextPageAddr = ( nextPageAddr > MOUDLES_SECOND_HALF_ADDR(modules) ) ? MOUDLES_START_ADDR(modules) : MOUDLES_SECOND_HALF_ADDR(modules);
			}
		}
		else {
			nextPageAddr = MOUDLES_START_ADDR(modules);
		}
	}

	/* if next address and current address is not in the same sector */
	if ( !NV_IN_SAME_SECTOR(itemInfo.startAddr, nextPageAddr) ) {
		/* erase the dest sector */
		flashUnportect = 1;
#if (FLASH_PROTECT)
		if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
#endif
		flash_erase_sector(nextPageAddr);
	}


    /* if the NV item is already in flash */
    if ( nvHdr->id == id ) {
        /* check the length */
        if ( nvHdr->len != NV_ALIGN_LENTH(len + 1) ) {
			if ( flashUnportect == 1 ) {
#if (FLASH_PROTECT)
				if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
					irq_restore(r);
					return NV_ENABLE_PROTECT_ERROR;
				}
#endif
			}
			irq_restore(r);
            return NV_ITEM_LEN_NOT_MATCH;
        }
#if (FLASH_PROTECT)
		if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
#endif
		wd_clear();

        /* write back the restored data before this item */
        nv_xferData(itemInfo.startAddr, nextPageAddr, itemInfo.hdrInfo.offset);

		wd_clear();

        /* if there are still some data after the item */
        if ( itemInfo.hdrInfo.offset + itemInfo.hdrInfo.len < itemInfo.usedSize ) {
            /* write bakc the restored data after this item */
            nv_xferData(itemInfo.startAddr + itemInfo.hdrInfo.offset + itemInfo.hdrInfo.len,
                        nextPageAddr + itemInfo.hdrInfo.offset + itemInfo.hdrInfo.len,
                        itemInfo.usedSize - itemInfo.hdrInfo.offset - itemInfo.hdrInfo.len);
        }


		wd_clear();
        /* update the NV item */
        ds_flashWrite(nextPageAddr + itemInfo.hdrInfo.offset, len, buf);

		/* mark the previous page is inactive */
        flash_write(itemInfo.startAddr, 2, (u8*)&inactiveFlags);

		wd_clear();
        /* write the fcs */
        ds_flashWrite(nextPageAddr + itemInfo.hdrInfo.offset + len, 1, &checksum);
#if (FLASH_PROTECT)
		if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
		else
#endif
		{
			u8 verifyFcs;
			if ( len < LARGE_BUFFER ) {
				u8 *tmpBuf = ev_buf_allocate(LARGE_BUFFER);
				if (tmpBuf == NULL ) {
					while(1);
				}
				/*
				foreach (i, len) {
					tmpBuf[i] = read_reg8(nextPageAddr + nvHdr->offset + i);
				}*/
				flash_read(nextPageAddr + nvHdr->offset, len, tmpBuf);
				flash_read(nextPageAddr + nvHdr->offset + len, 1, &verifyFcs);
				irq_restore(r);
				if ( verifyFcs == nv_calculateCheckSum(tmpBuf, len) ) {
					ev_buf_free(tmpBuf);
        			return NV_SUCC;
				}
				else {
					ev_buf_free(tmpBuf);
					return NV_CHECK_SUM_ERROR;
				}
			}
			else {
				irq_restore(r);
				return NV_SUCC;
			}
		}
    }
    /* it is a new NV item in flash */
    else {
        u16 usedSize;
        /* update the nv header info */
        nvHdr->id = id;
        nvHdr->len = NV_ALIGN_LENTH(len + 1);
        if ( itemInfo.usedSize == 0 ) {
            nvHdr->offset = PAGE_HEADER_SISE;
            usedSize = nvHdr->len + PAGE_HEADER_SISE;
            itemInfo.usedSize = PAGE_HEADER_SISE;
			firstItem = 1;
			nextPageAddr = itemInfo.startAddr;
        } else {
            nvHdr->offset = itemInfo.usedSize;
            usedSize = itemInfo.usedSize + nvHdr->len;
        }

#if (FLASH_PROTECT)
		if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
#endif
		if ( firstItem == 0 ) {
	        nv_xferData(itemInfo.startAddr + NV_PAGE_HEADER_LEN, nextPageAddr + NV_PAGE_HEADER_LEN, itemInfo.nvOffset);

			wd_clear();
		}

        /* save page header info */
		flash_write(nextPageAddr + sizeof(u16), sizeof(u16), (u8*)&usedSize);

		flash_write(nextPageAddr + itemInfo.nvOffset, sizeof(nv_header_t), (u8*)nvHdr);
		wd_clear();
		if ( firstItem == 0 ) {
        	nv_xferData(itemInfo.startAddr + itemInfo.nvOffset + sizeof(nv_header_t),
                    nextPageAddr + itemInfo.nvOffset + sizeof(nv_header_t),
                    itemInfo.usedSize - itemInfo.nvOffset - sizeof(nv_header_t));

			/* mark the previouse page is inactive */
        	flash_write(itemInfo.startAddr, 2, (u8*)&inactiveFlags);
		}

		flash_write(nextPageAddr, 2, (u8*)&activeFlags);

        /* save to Flash */
		ds_flashWrite(nextPageAddr + nvHdr->offset, len, buf);
		wd_clear();
        ds_flashWrite(nextPageAddr + nvHdr->offset + len, 1, &checksum);
        //flash_read(itemInfo.hdrAddr, sizeof(nv_header_t), (u8*)(&t_itemInfo.hdrInfo));


		/* protect the flash */
		wd_clear();
#if (FLASH_PROTECT)
		if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
			irq_restore(r);
			return NV_ENABLE_PROTECT_ERROR;
		}
		else
#endif
		{
			u8 verifyFcs;
			if ( len < LARGE_BUFFER ) {
				u8 *tmpBuf = ev_buf_allocate(LARGE_BUFFER);
				if (tmpBuf == NULL ) {
					while(1);
				}
				/*foreach (i, len) {
					tmpBuf[i] = read_reg8(nextPageAddr + nvHdr->offset + i);
				}*/
				flash_read(nextPageAddr + nvHdr->offset, len, tmpBuf);
				flash_read(nextPageAddr + nvHdr->offset + len, 1, &verifyFcs);
				irq_restore(r);
				if ( verifyFcs == nv_calculateCheckSum(tmpBuf, len) ) {
					ev_buf_free(tmpBuf);
					return NV_SUCC;
				}
				else {
					ev_buf_free(tmpBuf);
					return NV_CHECK_SUM_ERROR;
				}
			}
			else {
				irq_restore(r);
				return NV_SUCC;
			}
		}
    }
}

nv_sts_t  nv_read(u8 modules, u8 id, u16 len, u8 *buf)
{
    item_info_t itemInfo;
    u32 readAddr;
    nv_header_t *nvHdr = &(itemInfo.hdrInfo);

    /* id should be bigger than 0 */
    if ( id == INVALID_NV_VALUE ) {
        return NV_INVALID_ID;
    }

	u8 r = irq_disable();

	if ( SUCCESS != nv_findItem(modules, id, &itemInfo, 1) ) {
		irq_restore(r);
        return NV_ITEM_LEN_NOT_MATCH;
    }

	wd_clear();
    if ( nvHdr->id == INVALID_NV_VALUE ) {
		irq_restore(r);
        return NV_ITEM_NOT_FOUND;
    }

    /* read the nv item from flash */
    if ( NV_ALIGN_LENTH(len+1) == nvHdr->len ) {
        /* read from Flash */
        readAddr = itemInfo.startAddr + nvHdr->offset;
        /*foreach(i, len) {
            buf[i] = read_reg8(readAddr + i);
        }*/
        flash_read(readAddr, len, buf);
		wd_clear();
        u8 checksum ;//= read_reg8((itemInfo.startAddr + nvHdr->offset + len));
        flash_read(itemInfo.startAddr + nvHdr->offset + len, 1, &checksum);

        if ( checksum != nv_calculateCheckSum(buf, len) ) {
			irq_restore(r);
            return NV_CHECK_SUM_ERROR;
        }
		wd_clear();
		irq_restore(r);

        return NV_SUCC;
    }
	else {
        irq_restore(r);
		return NV_ITEM_LEN_NOT_MATCH;
    }
}

nv_sts_t  nv_resetModule(u8 modules)
{
#if (FLASH_PROTECT)
	if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
		return NV_ENABLE_PROTECT_ERROR;
	}
#endif
	flash_erase_sector(MOUDLES_START_ADDR(modules));
	wd_clear();
	flash_erase_sector(MOUDLES_SECOND_HALF_ADDR(modules));

#if (FLASH_PROTECT)
	if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
		return NV_ENABLE_PROTECT_ERROR;
	}
#endif
    return NV_SUCC;
}


nv_sts_t nv_userSaveToFlash(u8 id, u16 len, u8 *buf)
{
    return nv_write(DS_USER_MODULS, id, len, buf);
}

nv_sts_t nv_userLoadFromFlash(u8 id, u16 len, u8 *buf)
{
    return nv_read(DS_USER_MODULS, id, len, buf);
}


nv_sts_t nv_userReset(void)
{
    return nv_resetModule(DS_USER_MODULS);
}

nv_sts_t nv_nwkFrameCountSaveToFlash(u32 frameCount){
	nv_sts_t ret = NV_CHECK_SUM_ERROR;

	u32 startAddr = MOUDLES_START_ADDR(DS_NWK_FRAMECOUNT_MODULE);
	u32 module_startAddr = startAddr;
	u32 wAddr = module_startAddr + FLASH_4K_PAGE_NUM*FLASH_PAGE_SIZE;

	u32 nvBuf[NV_SUBPAGE_LENGHT/4];
	for(s32 i = 0; i < FLASH_4K_32B_NUM; i++){
		flash_read(startAddr, NV_SUBPAGE_LENGHT, (u8 *)nvBuf);
		if(nvBuf[NV_SUBPAGE_LENGHT/4-1] == 0xffffffff){
			for(s32 j = 0; j < NV_SUBPAGE_LENGHT/4; j++){
				if(nvBuf[j] == 0xffffffff){
					wAddr = startAddr + j * 4;
					break;
				}
			}
		}
		if(wAddr != module_startAddr + FLASH_4K_PAGE_NUM*FLASH_PAGE_SIZE){
			break;
		}
		startAddr += NV_SUBPAGE_LENGHT;
		//nv_buf += (FLASH_PAGE_SIZE/4);
	}

#if 0
	u32 *nv_buf = (u32 *)startAddr;
	for(s32 i = 0; i < FLASH_4K_PAGE_NUM; i++){
		//flash_read(startAddr, 256, (u8 *)nv_buf);
		if(nv_buf[FLASH_PAGE_SIZE/4-1] == 0xffffffff){
			for(s32 j = 0; j < FLASH_PAGE_SIZE/4; j++){
				if(nv_buf[j] == 0xffffffff){
					wAddr = startAddr + j * 4;
					break;
				}
			}
		}
		if(wAddr != module_startAddr + FLASH_4K_PAGE_NUM*FLASH_PAGE_SIZE){
			break;
		}
		startAddr += FLASH_PAGE_SIZE;
		nv_buf += (FLASH_PAGE_SIZE/4);
	}
#endif
#if (FLASH_PROTECT)
	if ( FLASH_PROTECT_NONE != flash_write_status(FLASH_PROTECT_NONE) ) {
		return NV_ENABLE_PROTECT_ERROR;
	}
#endif

	u32 wCurAddr = wAddr;
	for(s32 i = 0; i < 3; i++){
		if(wAddr == (module_startAddr + FLASH_4K_PAGE_NUM*FLASH_PAGE_SIZE)){
			flash_erase_sector(module_startAddr);
			wd_clear();
			wCurAddr = module_startAddr;
		}
		flash_write(wCurAddr, 4, (u8 *)&frameCount);

		u32 valueCheck = 0;
		flash_read(wCurAddr, 4, (u8 *)&valueCheck);
		if(valueCheck == frameCount){
			ret = NV_SUCC;
			break;
		}
	}
#if (FLASH_PROTECT)
	if ( protect_flash_cmd != flash_write_status(protect_flash_cmd) ) {
		return NV_ENABLE_PROTECT_ERROR;
	}
#endif

	return ret;
}

nv_sts_t nv_nwkFrameCountFromFlash(u32 *frameCount)
{
	u32 startAddr = MOUDLES_START_ADDR(DS_NWK_FRAMECOUNT_MODULE);

	u32 addr = startAddr + (FLASH_4K_32B_NUM - 1) * NV_SUBPAGE_LENGHT;
	u32 nvBuf[NV_SUBPAGE_LENGHT/4];
	for(s32 i = 0; i < FLASH_4K_32B_NUM; i++){
		flash_read(addr, NV_SUBPAGE_LENGHT, (u8 *)nvBuf);
		if(nvBuf[0] != 0xffffffff){
			for(s32 j = NV_SUBPAGE_LENGHT/4 - 1; j >= 0; j--){
				if(nvBuf[j] != 0xffffffff){
					*frameCount = nvBuf[j];
					return NV_SUCC;
				}
			}
		}
		addr -= NV_SUBPAGE_LENGHT;
	}

#if 0
	u32 *pAddr = (u32 *)(startAddr + (FLASH_4K_PAGE_NUM - 1) * FLASH_PAGE_SIZE);
	for(s32 i = 0; i < FLASH_4K_PAGE_NUM; i++){
		if(pAddr[0] != 0xffffffff){
			for(s32 j = FLASH_PAGE_SIZE/4 - 1; j >= 0; j--){
				if(pAddr[j] != 0xffffffff){
					*frameCount = pAddr[j];
					return NV_SUCC;
				}
			}
		}
		pAddr -= (FLASH_PAGE_SIZE/4);
	}
#endif
	return NV_ITEM_NOT_FOUND;
}

#else
nv_sts_t nv_write(u8 modules, u8 id, u16 len, u8 *buf){return NV_NO_MEDIA;}
nv_sts_t nv_init(u8 rst){return NV_NO_MEDIA;}
nv_sts_t nv_read(u8 modules, u8 id, u16 len, u8 *buf){return NV_NO_MEDIA;}
nv_sts_t nv_resetModule(u8 modules){return NV_NO_MEDIA;}

nv_sts_t nv_userSaveToFlash(u8 id, u16 len, u8 *buf){return NV_NO_MEDIA;}
nv_sts_t nv_userLoadFromFlash(u8 id, u16 len, u8 *buf){return NV_NO_MEDIA;}
nv_sts_t nv_userReset(void){return NV_NO_MEDIA;}
void ds_flashWrite(u32 startAddr, u16 len, u8 *buf){return NV_NO_MEDIA;}

#endif
