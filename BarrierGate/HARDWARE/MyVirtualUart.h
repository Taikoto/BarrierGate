#ifndef __MYVIRTUALUART__H__
#define __MYVIRTUALUART__H__

#include "stm32f10x.h"
#include "delay.h"
#include "USART3.H"
#include "sys.h"


#define BuadRate_76800  12
#define BuadRate_57600  16      
#define BuadRate_38400  25
#define BuadRate_19200  52
// #define BuadRate_9600   104


#define BuadRate_9600	100

#define OI_TXD	PAout(4)
#define OI_RXD	PAin(11)

enum{
	COM_START_BIT,
	COM_D0_BIT,
	COM_D1_BIT,
	COM_D2_BIT,
	COM_D3_BIT,
	COM_D4_BIT,
	COM_D5_BIT,
	COM_D6_BIT,
	COM_D7_BIT,
	COM_STOP_BIT,
};


extern u8   virtual_recv_len ;	//接收计数
extern u8   USART_buf[30];  //接收缓冲区
extern void IOConfig(void);
extern void TIM2_Int_Init(u16 arr,u16 psc);
extern void IO_TXD(u8 Data);
extern void USART_Send(u8 *buf, u8 len);
extern void IO_COM_StringSend(unsigned char *str);  // 字符串

extern void Virtual_IO_TXD(u8 Data);
extern void Virtual_USART_Send(u8 *buf, u8 len);

void UsartVirtualIRQ(COMM_USART *p_usart_virt,u8 recv_data);

void virtual_uart_config(void);

#endif

