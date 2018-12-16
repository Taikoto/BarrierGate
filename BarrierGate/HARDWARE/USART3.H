/*******************************************************
 *  程序名称: USART3.h
 *  程序功能：串口3配置初始化头文件
 *  程序作者：钟太松
 *  创建时间：2015-11-19
 *  修改时间：
 *  程序版本：V1.0
 ******************************************************/
 
#ifndef __USART3__H_
#define	__USART3__H_

/*
 *  包含头文件
 */ 
#include "stm32f10x.h"


//#define USART3_RX_LEN       USART_RX_LEN

extern u8 USART3_RX[];

extern volatile vu16 usart_rx3_num ;
extern volatile vu16 usart_rx3_use;
extern volatile u8 usart_rx3_full;   //0 or 1
extern volatile u8 usart_rx3_end;    /// timer ticks watch on rx , pass 20ms take as end transfer
extern volatile u8  rx3_begin_watching;
extern volatile vu16 last_usart_rx3_num;


extern u8 usart3_rx_buf[];
extern vu16 usart3_recvd_len;


/******************************************************
 * 函数名称：uart3_init
 * 函数功能：USART3 初始化函数
 * 入口参数：void

* 出口参数：
*******************************************************/ 
extern void uart3_init(u32 bound);

/******************************************************
 * 函数名称：USART3_transmit
 * 函数功能：USART3 发送一个字节
 * 入口参数：unsigned char byte
 * 出口参数：
*******************************************************/ 
extern void USART3_transmit(unsigned char byte);
/******************************************************
 * 函数名称：USART3_transmitS
 * 函数功能：USART3 发送多个字节数组；dat：发送的字节数组名；num：发送的字节数
 * 入口参数：unsigned char *dat,unsigned char num
 * 出口参数：发送的数组   
*******************************************************/ 
extern void USART3_transmitS(unsigned char *dat,unsigned int num);


extern int USART3_recv_buf(void);

extern int if_USART3_has_new_data(void);
extern int USART3_clear(void);
extern vu16	rx3_watching(void);


#endif
