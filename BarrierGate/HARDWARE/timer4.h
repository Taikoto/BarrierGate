/*******************************************************
 *  ��������: timer4.h
 *  �����ܣ���ʱ��4��ʼ������ͷ�ļ�
 *  �������ߣ���̫��
 *  ����ʱ�䣺2015-09-07
 *  �޸�ʱ�䣺
 *  ����汾��V1.0
 ******************************************************/

#ifndef __TIMER4__H_
#define __TIMER4__H_

#include "stm32f10x.h"
#include "global.h"

extern void TIM4_Int_Init(unsigned int arr,unsigned int psc);


vu16	rx_watching(COMM_USART *p_usart) ;

u16 check_wait_pending(NETWORK_HANDSHAKE *p_handshake);

#endif

