/*******************************************************
 *  程序名称: USART2.h
 *  程序功能：串口2配置初始化头文件
 *  程序作者：钟太松
 *  创建时间：2015-11-19
 *  修改时间：
 *  程序版本：V1.0
 ******************************************************/
 
#ifndef __USART2__H_
#define	__USART2__H_

/*
 *  包含头文件
 */ 
#include "stm32f10x.h"


#define USART2_RX_LEN       USART_RX_LEN


extern u8 USART2_RX[];
extern volatile vu16 usart_rx2_num ;
extern volatile vu16 usart_rx2_use;
extern volatile u8 usart_rx2_full;   //0 or 1
extern volatile u8 usart_rx2_end;    /// timer ticks watch on rx , pass 20ms take as end transfer
extern volatile u8  rx2_begin_watching;
extern volatile vu16 last_usart_rx2_num;

extern u8 usart2_rx_buf[];
extern vu16 usart2_recvd_len;

 

/******************************************************
 * 函数名称：uart2_init
 * 函数功能：USART2 初始化函数
 * 入口参数：void

* 出口参数：
*******************************************************/ 
extern void uart2_init(u32 bound);

/******************************************************
 * 函数名称：USART2_transmit
 * 函数功能：USART2 发送一个字节
 * 入口参数：unsigned char byte
 * 出口参数：
*******************************************************/ 
extern void USART2_transmit(unsigned char byte);
/******************************************************
 * 函数名称：USART2_transmitS
 * 函数功能：USART2 发送多个字节数组；dat：发送的字节数组名；num：发送的字节数
 * 入口参数：unsigned char *dat,unsigned char num
 * 出口参数：发送的数组   
*******************************************************/ 
extern void USART2_transmitS(unsigned char *dat,unsigned int num);


//extern int USART_recv_buf(u8 *rx_buf,u8 *usart_buf,int *p_rx_use,int *p_rx_num);
extern int USART2_recv_buf(void);
extern int if_USART2_has_new_data(void);
extern int USART2_clear(void);

extern vu16	rx2_watching(void);

#endif
