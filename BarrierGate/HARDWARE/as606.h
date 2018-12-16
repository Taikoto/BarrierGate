#ifndef __AS606_H
#define __AS606_H
#include <stdio.h>

#include "stm32f10x.h"
#include "global.h"
#include "comm_recv.h"

#define CharBuffer1 0x01
#define CharBuffer2 0x02


//extern u32 AS606Addr;//ģ���ַ
//extern u16 g_fp_frame_data_len;


u8 PS_GetImage(COMM_USART *pcomm); //¼��ͼ�� 
 
u8 PS_GenChar(COMM_USART *pcomm,u8 BufferID);//�������� 

u8 PS_Match(COMM_USART *pcomm);//��ȷ�ȶ���öָ������ 

u8 PS_Search(COMM_USART *pcomm,u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//����ָ�� 
 
u8 PS_RegModel(COMM_USART *pcomm);//�ϲ�����������ģ�壩 
 
u8 PS_StoreChar(COMM_USART *pcomm,u8 BufferID,u16 PageID);//����ģ�� 

u8 PS_DeletChar(COMM_USART *pcomm,u16 PageID,u16 N);//ɾ��ģ�� 

u8 PS_Empty(COMM_USART *pcomm);//���ָ�ƿ� 

u8 PS_WriteReg(COMM_USART *pcomm,u8 RegNum,u8 DATA);//дϵͳ�Ĵ��� 
 
u8 PS_ReadSysPara(COMM_USART *pcomm,SysPara *p); //��ϵͳ�������� 
 
u8 PS_SetAddr(COMM_USART *pcomm,u32 addr);  //����ģ���ַ 

u8 PS_WriteNotepad(COMM_USART *pcomm,u8 NotePageNum,u8 *content);//д���±� 

u8 PS_ReadNotepad(COMM_USART *pcomm,u8 NotePageNum,u8 *note);//������ 

u8 PS_HighSpeedSearch(COMM_USART *pcomm,u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//�������� 
  
u8 PS_ValidTempleteNum(COMM_USART *pcomm,u16 *ValidN);//����Чģ����� 

u8 PS_HandShake(COMM_USART *pcomm,u32 *PS_Addr); //��AS606ģ������

const char *EnsureMessage(u8 ensure);//ȷ���������Ϣ����

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

