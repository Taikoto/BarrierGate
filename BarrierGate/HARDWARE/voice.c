
/*************************************************
 *  �������ƣ�voice.c
 *  �����ܣ���ʼ��IO�ڣ����ó�Ϊ����
 *  �������ߣ���̫��
 *  ����ʱ�䣺2017-1-20
 *  �޸�ʱ�䣺
 *  ����汾��V0.1
 *************************************************/

/*
 *  ����ͷ�ļ�
 */
#include "voice.h" 
 
/*************************************************
 *  �������ƣ�VOICE_Init
 *  �������ܣ�VOICE�ڳ�ʼ������
 *  ��ڲ�����void
 *  ���ڲ�����void
 ************************************************/
void VOICE_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				   //PA.12 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��PA.12
  GPIO_SetBits(GPIOA,GPIO_Pin_12);						 //PA.12 �����
	
	
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PA�˿�ʱ��
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				   //PA.5 �˿�����
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��PA.5
//  GPIO_SetBits(GPIOA,GPIO_Pin_5);						 //PA.5 �����
}

/******************************************************
 * �������ƣ�SendData
 * �������ܣ�MCU_voice���ݵ����
 * ��ڲ�����unsigned char ucaddr  ��ַ
 * ���ڲ�����void
 ******************************************************/ 
void SendVoiceData(unsigned char ucaddr)
{
  unsigned char i;
	
	VOICEout = 1;         // ��ʼ����
	delay_us(1200);
	VOICEout = 0;         // ��ʼ������
	delay_us(3000);    // �˴���ʱ����Ҫ����2ms���˲�����ʱΪ310ms
	
  for( i = 0; i <8; i++)     // 8λ����
	{
	  VOICEout = 1; 
		
		if(ucaddr & 0x01)    // 3:1 ��ʾ����λ1��ÿ��λ�����������ʾ
		{
		  delay_us(800);
			VOICEout = 0;
			delay_us(210);
		}
		else
		{
		  delay_us(210);
			VOICEout = 0;
			delay_us(800);
		}
		
		ucaddr >>= 1;
	}
	VOICEout = 1;
}

/******************************************************
 * �������ƣ�VoicePrompt
 * �������ܣ�������ʾ����
 * ��ڲ�����unsigned char ucvoices  ������ʾ����
 * ���ڲ�����void
 ******************************************************/ 
void VoicePrompt(unsigned char ucvoices)   // ������ʾ
{
	if(ucvoices<=0x09)     // ��ʾ��������10
	{
    SendVoiceData(0x0a);
	  SendVoiceData(0x00);
	  SendVoiceData(ucvoices);
	  SendVoiceData(0x0b);
	}
	else if((ucvoices>=0x10) && (ucvoices<0x19))   // ��ʾ��������10��������20
	{
	  SendVoiceData(0x0a);
	  SendVoiceData(0x01);
	  SendVoiceData(ucvoices - 0x10);
	  SendVoiceData(0x0b);
	}
	else if((ucvoices>=0x20) && (ucvoices<0x29))   // ��ʾ��������20��������30
	{
	  SendVoiceData(0x0a);
	  SendVoiceData(0x02);
	  SendVoiceData(ucvoices - 0x20);
	  SendVoiceData(0x0b);
	}
	
	else if((ucvoices>=0x30) && (ucvoices<0x39))   // ��ʾ��������30��������40
	{
	  SendVoiceData(0x0a);
	  SendVoiceData(0x03);
	  SendVoiceData(ucvoices - 0x30);
	  SendVoiceData(0x0b);
	}
}

