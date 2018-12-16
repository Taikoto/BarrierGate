#ifndef __AS606_H
#define __AS606_H
#include <stdio.h>

#include "stm32f10x.h"
#include "global.h"
#include "comm_recv.h"

#define CharBuffer1 0x01
#define CharBuffer2 0x02


//extern u32 AS606Addr;//模块地址
//extern u16 g_fp_frame_data_len;


u8 PS_GetImage(COMM_USART *pcomm); //录入图像 
 
u8 PS_GenChar(COMM_USART *pcomm,u8 BufferID);//生成特征 

u8 PS_Match(COMM_USART *pcomm);//精确比对两枚指纹特征 

u8 PS_Search(COMM_USART *pcomm,u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//搜索指纹 
 
u8 PS_RegModel(COMM_USART *pcomm);//合并特征（生成模板） 
 
u8 PS_StoreChar(COMM_USART *pcomm,u8 BufferID,u16 PageID);//储存模板 

u8 PS_DeletChar(COMM_USART *pcomm,u16 PageID,u16 N);//删除模板 

u8 PS_Empty(COMM_USART *pcomm);//清空指纹库 

u8 PS_WriteReg(COMM_USART *pcomm,u8 RegNum,u8 DATA);//写系统寄存器 
 
u8 PS_ReadSysPara(COMM_USART *pcomm,SysPara *p); //读系统基本参数 
 
u8 PS_SetAddr(COMM_USART *pcomm,u32 addr);  //设置模块地址 

u8 PS_WriteNotepad(COMM_USART *pcomm,u8 NotePageNum,u8 *content);//写记事本 

u8 PS_ReadNotepad(COMM_USART *pcomm,u8 NotePageNum,u8 *note);//读记事 

u8 PS_HighSpeedSearch(COMM_USART *pcomm,u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//高速搜索 
  
u8 PS_ValidTempleteNum(COMM_USART *pcomm,u16 *ValidN);//读有效模板个数 

u8 PS_HandShake(COMM_USART *pcomm,u32 *PS_Addr); //与AS606模块握手

const char *EnsureMessage(u8 ensure);//确认码错误信息解析

//
//ZACK Lynn ,from 20170222
//
u8 PS_LoadChar(COMM_USART *pcomm,u8 BufferID,u16 PageID);

u8 PS_ReadFPFlash(COMM_USART *pcomm,u8 PageNum,u8 *Byte32);

u16 PS_UpChar(COMM_USART *pcomm,u8 BufferID, u8 *upchar_buf); 

u16 PS_DownChar(COMM_USART *pcomm,u8 BufferID, u8 *downchar_buf, u16 down_len);

u16 PS_DownCharServer(COMM_USART *pcomm,u16 addrid, u8 *downchar_buf, u16 down_len);


u8 *JudgeStr_fromSer(COMM_USART *pcomm,u16 waittime,u8 *jstr);

u16 PS_RecvCharFromServer(COMM_USART *pcomm,u16 *p_storeid, u8 *upchar_buf,u16 waittime);


//

#endif

