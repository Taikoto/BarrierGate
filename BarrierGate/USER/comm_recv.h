
#ifndef  __COMM_RECV__H_
#define  __COMM_RECV__H_

#include "stm32f10x.h"
#include "global.h"

void SendFlag(COMM_USART *pcomm,u8 flag);
void SendLength(COMM_USART *pcomm,int length);  //two bytes
void Sendcmd(COMM_USART *pcomm,u8 cmd);
void SendCheck(COMM_USART *pcomm,u16 check);    //two bytes
void SendData(COMM_USART *pcomm,u8 *data,u16 len);


u8 *JudgeStr(COMM_USART *pcomm,u16 waittime,u8 *jstr);
u8 *JudgeStr_fromSer(COMM_USART *pcomm,u16 waittime,u8 *jstr);
u8 *JudgeStr_UpChar(COMM_USART *pcomm,u16 waittime,u8 *jstr,u8 *buf_left);
u8 *JudgeStr_fromSer_nowait(COMM_USART *pcomm,u8 *jstr,u16 *p_spend_ms);

#endif

