
#ifndef __FP_FUNC_H
#define __FP_FUNC_H

#include <stdio.h>

#include "stm32f10x.h"
#include "global.h"
#include "as606.h"

//void Add_FR(void);	//Â¼Ö¸ÎÆ
//void Del_FR(void);	//É¾³ýÖ¸ÎÆ
//void press_FR(void);//Ë¢Ö¸ÎÆ

u8 check_press_FR(void);
u8 check_ServerCmd(void);

u8 FpSync_Init(FP_SYNC *pfp_sync);

u16 check_network_send(u16 loop_count);
u8 check_network_recv(void);

u16 Add_FR(COMM_USART *pcomm,u16 id_addr);
u8 press_FR(COMM_USART *pcomm,SearchResult *pseach);
u16 readFPFlash(COMM_USART *pcomm); //use global var g_fp_byte32[][];
u16 readSysPara(COMM_USART *pcomm,SysPara *p);

//u16 upChar_FR(FpModelSTRU *m1,FpModelSTRU *m2);
//u16 downChar_FR(FpModelSTRU *m1,FpModelSTRU *m2);

u16 upChar_FR(COMM_USART *pcomm,u8 *mbuf1,u8 *mbuf2);
u16 downChar_FR(COMM_USART *pcomm,u8 *mbuf1,u8 *mbuf2);
u16 upChar_FR_1(COMM_USART *pcomm,u8 *mbuf1,u8 BufferID);


//success , return MODEL_LEN(512)
//failed, return other value
u16 loadUpChar_FR(COMM_USART *pcomm,u16 addrid,u8 *mbuf1);

//success , return MODEL_LEN(512)
u16 downStore2Char_FR(COMM_USART *pcom,u8 *mbuf1,u8 *mbuf2,u16 addrid,u8 downBufferID);

//return len;
u16 downStoreChar_FR(COMM_USART *pcomm,u8 *mbuf1,u16 addrid,u8 downBufferID);

//return 0:means ok
u8 downStoreCharbuf(COMM_USART *pcomm, FP_SYNC *p_fp_down);

//MCU send
//up_len=MODEL_LEN  //512 
u16 UpChar_ToSer(COMM_USART *pcomm_server,u16 addrid,u8 *upchar_buf,u16 up_len);

//MCU receive
u16 RecvChar_FromSer(COMM_USART *pcomm_server,u16 *p_storeid,u8 *downchar_buf,u16 waittime);
u16 RecvChar2_FromSer(COMM_USART *pcomm_server,u16 *p_storeid,u8 *downchar_buf1,u8 *downchar_buf2,u16 waittime);


u16 delChar_FR(COMM_USART *pcomm,u16 del_page_id,u16 del_number);

u8 check_FP_Pass(SearchResult *pseach);

#endif
