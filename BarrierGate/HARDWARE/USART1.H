/*******************************************************
 *  ��������: USART1.H
 *  �����ܣ�����1���ó�ʼ��ͷ�ļ�
 *  �������ߣ���̫��
 *  ����ʱ�䣺2015-11-19
 *  �޸�ʱ�䣺
 *  ����汾��V1.1
 ******************************************************/
 
#ifndef  __USART1__H_
#define  __USART1__H_

/*
 *  ����ͷ�ļ�
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
	uint16_t rx_begin;       /* passed head begin locate in rx_buf *///�ϸ��ͷ��rx_buf�����ʼλ��(ǰ�������һ�����ַ�)��0 to rx_num
	
	uint16_t rx_use;		    /* read num, max rx_use=rx_num */
	uint16_t rx_state;		  /* 0--no any data,1--receiving, 2--receiv end ,wait for process*/
	uint16_t rx_MAXLEN;		  /* rx buf len = USART_RX_LEN */
 	void (*send_over)(void);   	/* after send, recall function , NULL */
	void (*receive_new)(void);	/* after send, recall function , NULL */
}COMM_USART;

extern COMM_USART g_usart1;

/******************************************************
 * �������ƣ�uart1_init
 * �������ܣ�USART1 ��ʼ������
 * ��ڲ�����void
 * ���ڲ�����
*******************************************************/ 
extern void uart1_init(u32 bound);

/******************************************************
 * �������ƣ�USART1_transmit
 * �������ܣ�USART1 ����һ���ֽ�
 * ��ڲ�����unsigned char byte
 * ���ڲ�����
*******************************************************/ 
extern void USART1_transmit(unsigned char byte);
extern void USART_transmit(COMM_USART *p_usart,unsigned char byte);
 
/******************************************************
 * �������ƣ�USART1_transmitS
 * �������ܣ�USART1 ���Ͷ���ֽ����飻dat�����͵��ֽ���������num�����͵��ֽ���
 * ��ڲ�����unsigned char *dat,unsigned char num
 * ���ڲ��������͵�����   
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