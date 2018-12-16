
/*************************************************
 *  �������ƣ�74hc595.c�ļ�
 *  �����ܣ�74hc595�������ü�Ӧ���ļ�
 *  �������ߣ���̫��
 *  ����ʱ�䣺2017-2-9
 *  �޸�ʱ�䣺
 *  ����汾��V0.1
 *************************************************/
 
/*
 *  ����ͷ�ļ�
 */
#include "hc595.h"

/*************************************************
 *  �������ƣ�74HC595_Init_RelayPart
 *  �������ܣ��̵�������74HC595��ʼ������
 *  ��ڲ�����void
 *  ���ڲ�����void
 ************************************************/
void HC595_Init_RelayPart(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				   //PC.13/PC.14 /PC.15 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 //�����趨������ʼ��PC.13/PC.14 /PC.15
  GPIO_SetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);						 //PC.13/PC.14 /PC.15 �����
}

/*************************************************
 *  �������ƣ�74HC595_Init_DigitalPart
 *  �������ܣ�74HC595��ʼ������
 *  ��ڲ�����void
 *  ���ڲ�����void
 ************************************************/
void HC595_Init_DigitalPart(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9;				   //PB.5/PB.8 /PB.9 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��PB.5/PB.8 /PB.9
  GPIO_SetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9);						 //PB.5/PB.8 /PB.9 �����
}

void NOP()
{
	unsigned char i;
  for(i=0; i<30; i++); 
}

/*************************************************
 *  �������ƣ�HC595SendData
 *  �������ܣ�HC595��SPI���߷�������
 *  ��ڲ�����unsigned char ucSendValue Ҫ���͵����� 
 *  ���ڲ�����void
 ************************************************/
void HC595SendData(unsigned char ucSendValue)
{
  unsigned char i;
	for(i = 0; i<8; i++)
	{
	  if((ucSendValue <<i)&0x80)
		{
		  GPIO_SetBits(GPIOC,MOSI_RelayPart);
		}
		else
		{
		  GPIO_ResetBits(GPIOC,MOSI_RelayPart);
		}
		GPIO_ResetBits(GPIOC,S_CLK_RelayPart);      // ����λ
		NOP();
		NOP();
		GPIO_SetBits(GPIOC,S_CLK_RelayPart);	
	}
	
	GPIO_ResetBits(GPIOC,R_CLK_RelayPart);         // �ٴ洢
	NOP();
	NOP();
	GPIO_SetBits(GPIOC,R_CLK_RelayPart);	
}

/*************************************************
 *  �������ƣ�HC595_IO_Out
 *  �������ܣ�HC595������������
 *  ��ڲ�����unsigned char ucHc595Status Ҫ���͵����� 
 *  ���ڲ�����void
 ************************************************/
void HC595_IO_Out(unsigned char ucHc595Status)
{
  static unsigned char ucTemp = 0;
	ucTemp = ucTemp | ucHc595Status;     // ��λ��
	
	HC595SendData(ucTemp);

}
	





