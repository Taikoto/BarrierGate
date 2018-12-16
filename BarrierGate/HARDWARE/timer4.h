/*******************************************************
 *  程序名称: timer4.h
 *  程序功能：定时器4初始化函数头文件
 *  程序作者：钟太松
 *  创建时间：2015-09-07
 *  修改时间：
 *  程序版本：V1.0
 ******************************************************/

#ifndef __TIMER4__H_
#define __TIMER4__H_

#include "stm32f10x.h"
#include "global.h"

extern void TIM4_Int_Init(unsigned int arr,unsigned int psc);


vu16	rx_watching(COMM_USART *p_usart) ;

u16 check_wait_pending(NETWORK_HANDSHAKE *p_handshake);

#endif

