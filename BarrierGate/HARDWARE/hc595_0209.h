
/*************************************************
 *  �������ƣ�74hc595.hͷ�ļ�
 *  �����ܣ�74hc595�������ü�Ӧ���ļ�
 *  �������ߣ���̫��
 *  ����ʱ�䣺2017-2-9
 *  �޸�ʱ�䣺
 *  ����汾��V0.1
 *************************************************/

#ifndef __HC595__H__
#define __HC595__H__

/* 
 *  ����ͷ�ļ�
 */
#include "stm32f10x.h"

/* 
 *  �궨��
 */
#define S_CLK_RelayPart    GPIO_Pin_15
#define R_CLK_RelayPart    GPIO_Pin_14
#define MOSI_RelayPart     GPIO_Pin_13

#define S_CLK_DigitalPart  GPIO_Pin_8
#define R_CLK_DigitalPart  GPIO_Pin_9
#define MOSI_DigitalPart   GPIO_Pin_5

//#define RelayPart          GPIOC
//#define DigitalPart        GPIOB

#define Beep_on         0x01         //  Q0
#define Beep_off        0xfe

#define Relay1_2_on     0xfd         //  Q1
#define Relay1_2_off    0x02

#define Relay3_4_on     0xfb         //  Q2
#define Relay3_4_off    0x04

#define Lock_on         0xf7         //  Q3
#define Lock_off        0x08

#define Led6_on         0xef         //  Q4
#define Led6_off        0x10

#define Led7_on         0xdf         //  Q5
#define Led7_off        0x20         

#define Led8_on         0xbf         //  Q6
#define Led8_off        0x40   

#define Led9_on         0x7f         //  Q7
#define Led9_off        0x80

/*************************************************
 *  �������ƣ�74HC595_Init_RelayPart
 *  �������ܣ��̵�������74HC595��ʼ������
 *  ��ڲ�����void
 *  ���ڲ�����void
 ************************************************/
extern void HC595_Init_RelayPart(void);

/*************************************************
 *  �������ƣ�74HC595_Init_DigitalPart
 *  �������ܣ�����ܲ���74HC595��ʼ������
 *  ��ڲ�����void
 *  ���ڲ�����void
 ************************************************/
extern void HC595_Init_DigitalPart(void);

/*************************************************
 *  �������ƣ�HC595SendData
 *  �������ܣ�HC595��SPI���߷�������
 *  ��ڲ�����unsigned char ucSendValue Ҫ���͵����� 
 *  ���ڲ�����void
 ************************************************/
extern void HC595SendData(unsigned char ucSendValue);

/*************************************************
 *  �������ƣ�HC595_IO_Out
 *  �������ܣ�HC595������������
 *  ��ڲ�����unsigned char ucHc595Status Ҫ���͵����� 
 *  ���ڲ�����void
 ************************************************/
extern void HC595_IO_Out(unsigned char ucHc595Status);

//void NOP();

#endif
