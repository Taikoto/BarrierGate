#ifndef __SERVER_H
#define __SERVER_H
#include <stdio.h>

#include "stm32f10x.h"
#include "global.h"
#include "comm_recv.h"


u8  requestServer_byFP(u16 fpid);

u8 RecvCmdFromServer(COMM_USART *pcomm,u16 waittime);

u8 Send_Req_To_Server(COMM_USART *pcomm,u8 cmd,u16 fpid);

u8 unpackFPCharbufFromServer(COMM_USART *pcomm,u8 *pframe,FP_SYNC *p_fp_sync);
u16 PS_RecvCharFromServer(COMM_USART *pcomm,u16 *p_storeid, u8 *upchar_buf,u16 waittime);

u8	Ack_To_Server(COMM_USART *pcomm,u8 cmd,u16 addr_id,u8 status);
u8	Ack_To_Server_OpenId(COMM_USART *pcomm,u8 cmd,u16 fp_id,u32 open_id,u8 status);


u8 Ack_Req_To_Server(BARRIER_CHECKING_FLOW *pflow);
u8 Ack_To_Server_Need_Gun(COMM_USART *pcomm, BARRIER_CHECKING_FLOW *pflow);

void network_handshake_init(void);
u16 recv_network_handshake(COMM_USART *pcomm,u16 *p_wait_ms);

#endif
