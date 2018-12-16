
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
#include "USART3.H"

/* 
 *  �궨��
 */
#define S_CLK_RelayPart    GPIO_Pin_15
#define R_CLK_RelayPart    GPIO_Pin_14
#define MOSI_RelayPart     GPIO_Pin_13

#define S_CLK_DigitalPart  GPIO_Pin_8
#define R_CLK_DigitalPart  GPIO_Pin_9
#define MOSI_DigitalPart   GPIO_Pin_5


extern unsigned char ucBarrierGate_Control_flag;
//#define RelayPart          GPIOC
//#define DigitalPart        GPIOB


//#define Beep_on         0x01         //  Q0
//#define Beep_off        0xfe

//#define Relay1_2_on     0xfd         //  Q1
//#define Relay1_2_off    0x02

//#define Relay3_4_on     0xfb         //  Q2
//#define Relay3_4_off    0x04

//#define Lock_on         0xf7         //  Q3
//#define Lock_off        0x08

//#define Led6_on         0xef         //  Q4
//#define Led6_off        0x10

//#define Led7_on         0xdf         //  Q5
//#define Led7_off        0x20         

//#define Led8_on         0xbf         //  Q6
//#define Led8_off        0x40   

//#define Led9_on         0x7f         //  Q7
//#define Led9_off        0x80

#define Beep_on         0x01         //  Q0
#define Beep_off        0xfe

#define Relay1_2_on     0xfc         //  Q1
#define Relay1_2_off    0xf1

//#define Relay1_2_on     0xf1         //  Q1
//#define Relay1_2_off    0xfc

#define Relay3_4_on     0xfa         //  Q2
#define Relay3_4_off    0xf2

//#define Lock_on         0xf6         //  Q3
//#define Lock_off        0xf3

#define W25X16_H        0xf6         //  Q3
#define W25X16_L        0xf3

#define Led6_on         0xee         //  Q4
//#define Led6_off        0xfe
#define Led6_off        0xf4

#define Led7_on         0xde         //  Q5
#define Led7_off        0xf5        

#define Led8_on         0xbe         //  Q6
#define Led8_off        0xf8   

#define Led9_on         0x7e         //  Q7
//#define Led9_off        0xfe
#define Led9_off        0xf7

#define All_off         0xf8
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
 *  �������ƣ�HC595_IO_Out_Updata_Status
 *  �������ܣ�HC595������������
 *  ��ڲ�����unsigned char ucStatus Ҫ���͵����� 
 *  ���ڲ�����void
 ************************************************/
extern void HC595_IO_Out(void );

extern void HC595_IO_Out_Updata_Status(unsigned char ucStatus);

extern void BarrierGate_Open(void);     //  ��բ����
	
extern void BarrierGate_Close(void);   // ��բ�ر�        ��բ��������ʱ5ms ����ִ�йر������ʱʱ�䲻�˳���10ms

//void NOP();

#endif
