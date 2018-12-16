#ifndef  __USARTx__H_
#define  __USARTx__H_

/*
 *  包含头文件
 */ 
#include "stm32f10x.h"
#include "global.h"

//#define USART1_RX_LEN       USART_RX_LEN
//#define USART2_RX_LEN       USART_RX_LEN
//#define USART3_RX_LEN       USART_RX_LEN

#define USART1_RX_LEN       1024    //to finger print
#define USART2_RX_LEN       1024    //ethernet to server
#define USART3_RX_LEN       512     //rfid

#define USARTVIRT_RX_LEN       64     //virtual usart receive

extern COMM_USART g_usart1;
extern COMM_USART g_usart2;
extern COMM_USART g_usart3;

void uart1_init(u32 bound);
void uart2_init(u32 bound);
void uart3_init(u32 bound);
void uartvirt_init(void);

extern void USART_transmit(COMM_USART *p_usart,unsigned char byte);
extern void USART_transmitS(COMM_USART *p_usart,unsigned char *dat,unsigned int num);

extern int RX_Clear(COMM_USART *p_usart);
extern vu16	rx_watching(COMM_USART *p_usart);

extern void debug_print_c(u8 ch);
extern void debug_print_nr(void);
extern void debug_printf(u8* fmt,...);
extern void debug_print(u8* buf,unsigned int len);
extern void debug_print_buf(u8* buf,unsigned int len);
extern u16 debug_input(u16 waittime,u8 *ibuf);
extern u8 input_function(u8 *in_buf);

extern void UsartVirtualIRQ_Recv(u8 recv_data);
extern void UsartVirtualIRQ(COMM_USART *p_usart_virt,u8 recv_data);

#endif 

