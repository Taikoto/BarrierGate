/*******************************************************
 *  程序名称: USART1.H
 *  程序功能：串口1配置初始化头文件
 *  程序作者：钟太松
 *  创建时间：2015-11-19
 *  修改时间：
 *  程序版本：V1.1
 ******************************************************/
 
#ifndef  __USART1__H_
#define  __USART1__H_

/*
 *  包含头文件
 */ 
#include "stm32f10x.h"

#define USART1_RX_LEN       USART_RX_LEN

extern u8 USART1_RX[];

extern volatile vu16 usart_rx1_num;
extern volatile vu16 usart_rx1_use;
//extern volatile u8 usart_rx1_full;   //0 or 1
extern volatile u8 usart_rx1_end;
extern volatile u8  rx1_begin_watching;
extern volatile vu16 last_usart_rx1_num;

extern u8 usart1_rx_buf[];
extern vu16 usart1_recvd_len;

typedef struct
{
  USART_TypeDef *usart;	  /* USART1,USART2,USART3 */		
 	uint8_t *tx_buf;			/* send buf */
	uint8_t *rx_buf;			/* receive buf */
	uint16_t rx_num;		    /* received num, bytes ,max=rx_maxlen, not loop*/
	uint16_t rx_last_num;		/* last timer tick received num, use to watching receiving end */
	uint16_t rx_begin;       /* passed head begin locate in rx_buf *///合格包头在rx_buf相对起始位置(前面可能有一串乱字符)，0 to rx_num
	
	uint16_t rx_use;		    /* read num, max rx_use=rx_num */
	uint16_t rx_state;		  /* 0--no any data,1--receiving, 2--receiv end ,wait for process*/
	uint16_t rx_MAXLEN;		  /* rx buf len = USART_RX_LEN */
 	void (*send_over)(void);   	/* after send, recall function , NULL */
	void (*receive_new)(void);	/* after send, recall function , NULL */
}COMM_USART;

extern COMM_USART g_usart1;

/******************************************************
 * 函数名称：uart1_init
 * 函数功能：USART1 初始化函数
 * 入口参数：void
 * 出口参数：
*******************************************************/ 
extern void uart1_init(u32 bound);

/******************************************************
 * 函数名称：USART1_transmit
 * 函数功能：USART1 发送一个字节
 * 入口参数：unsigned char byte
 * 出口参数：
*******************************************************/ 
extern void USART1_transmit(unsigned char byte);
extern void USART_transmit(COMM_USART *p_usart,unsigned char byte);
 
/******************************************************
 * 函数名称：USART1_transmitS
 * 函数功能：USART1 发送多个字节数组；dat：发送的字节数组名；num：发送的字节数
 * 入口参数：unsigned char *dat,unsigned char num
 * 出口参数：发送的数组   
*******************************************************/ 
extern void USART1_transmitS(unsigned char *dat,unsigned int num);
extern int if_USART1_has_new_data(void);
extern int USART1_recv_buf(void);
extern int USART1_clear(void);
extern vu16	rx1_watching(void);


extern void USART_transmitS(COMM_USART *p_usart,unsigned char *dat,unsigned int num);
extern int if_USART_has_new_data(COMM_USART *p_usart);
extern int USART_recv_buf(COMM_USART *p_usart,u8 *rx_buf);
extern int USART_clear(COMM_USART *p_usart);
extern vu16	rx_watching(COMM_USART *p_usart);

extern void debug_print_c(u8 ch);
extern void debug_print_nr(void);
extern void debug_printf(u8* fmt,...);
extern void debug_print(u8* buf,unsigned int len);

#endif 
