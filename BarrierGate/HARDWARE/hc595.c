
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


unsigned char ucTemp = 0;         

unsigned char ucHc595Out_update = 0;   //  ˢ�±�����ÿ�θ���LED��״̬��Ҫ����һ�Ρ�
unsigned char ucHc595Out_Temp = 0xfe;     //  ����ײ�74HC595���״̬���м����

unsigned char ucHc595Out_Q0 = 0;       //  ����8�������״̬��0 ������1������
unsigned char ucHc595Out_Q1 = 0; 
unsigned char ucHc595Out_Q2 = 0; 
unsigned char ucHc595Out_Q3 = 0; 
unsigned char ucHc595Out_Q4 = 0; 
unsigned char ucHc595Out_Q5 = 0; 
unsigned char ucHc595Out_Q6 = 0; 
unsigned char ucHc595Out_Q7 = 0; 


unsigned char ucBarrierGate_Control_flag = 0;
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
void HC595_IO_Out( void )
{
  if(ucHc595Out_update ==1)
	{
	  ucHc595Out_update = 0;
		
//		if(ucHc595Out_Q0 == 1)
//		{
//		  ucHc595Out_Temp = ucHc595Out_Temp | 0x01;
//		}
//		else
//		{
//		  ucHc595Out_Temp = ucHc595Out_Temp & 0xfe;
//		}
		
		if(ucHc595Out_Q1 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xfc;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x02;
		}
		
		if(ucHc595Out_Q2 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xfa;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x04;
		}
		
		if(ucHc595Out_Q3 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xf6;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x08;
		}
		
		if(ucHc595Out_Q4 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xee;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x10;
		}
		
		if(ucHc595Out_Q5 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xde;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x20;
		}
		
		if(ucHc595Out_Q6 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xbe;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x40;
		}
		
		if(ucHc595Out_Q7 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0x7e;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x80;
		}

		
		if(ucHc595Out_Q0 == 1)     // Ϊʲôһ��Ҫ�ŵ�����ѵ�ֻ����Ϊ ֻ�����Ǹߵ�ƽ������
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x01;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xfe;
		}
		
	}
	
	HC595SendData(ucHc595Out_Temp);
	
//  USART3_transmit(ucHc595Out_Temp);
}
	
void HC595_IO_Out_Updata_Status(unsigned char ucStatus)
{
  switch(ucStatus)
	{
	  case Beep_on:       ucHc595Out_Q0 = 0;  ucHc595Out_update = 1; break;
		case Beep_off:      ucHc595Out_Q0 = 1;  ucHc595Out_update = 1; break;
		
	  case Relay1_2_on:   ucHc595Out_Q1 = 0;  ucHc595Out_update = 1; break;
		case Relay1_2_off:  ucHc595Out_Q1 = 1;  ucHc595Out_update = 1; break;		
		
		case Relay3_4_on:   ucHc595Out_Q2 = 1;  ucHc595Out_update = 1; break;
		case Relay3_4_off:  ucHc595Out_Q2 = 0;  ucHc595Out_update = 1; break;	
		
//		case Lock_on:       ucHc595Out_Q3 = 1;  ucHc595Out_update = 1; break;    // �ⲿ������ӿ�
//		case Lock_off:      ucHc595Out_Q3 = 0;  ucHc595Out_update = 1; break;
		
		case W25X16_H:      ucHc595Out_Q3 = 0;  ucHc595Out_update = 1; break;        // �������ü��ɡ�
		case W25X16_L:      ucHc595Out_Q3 = 1;  ucHc595Out_update = 1; break;	 
    		
		case Led6_on:       ucHc595Out_Q4 = 1;  ucHc595Out_update = 1; break;
		case Led6_off:      ucHc595Out_Q4 = 0;  ucHc595Out_update = 1; break;	
		
		case Led7_on:       ucHc595Out_Q5 = 1;  ucHc595Out_update = 1; break;
  	case Led7_off:      ucHc595Out_Q5 = 0;  ucHc595Out_update = 1; break;	
		
		case Led8_on:       ucHc595Out_Q6 = 1;  ucHc595Out_update = 1; break;
		case Led8_off:      ucHc595Out_Q6 = 0;  ucHc595Out_update = 1; break;	
		
		case Led9_on:       ucHc595Out_Q7 = 1;  ucHc595Out_update = 1; break;
		case Led9_off:      ucHc595Out_Q7 = 0;  ucHc595Out_update = 1; break;		

		default: 
		{
//		  ucHc595Out_Q0 = 0;
//			ucHc595Out_Q1 = 0;
//			ucHc595Out_Q2 = 0;
//			ucHc595Out_Q3 = 0;
//			ucHc595Out_Q4 = 0;
//			ucHc595Out_Q5 = 0;
//			ucHc595Out_Q6 = 0;
//			ucHc595Out_Q7 = 0;
//			ucHc595Out_update = 1;
			break;
		}
	}
	
	HC595_IO_Out( );
}

void BarrierGate_Open(void)     //  ��բ����
{
  if(ucBarrierGate_Control_flag == 1)
	{
	  ucBarrierGate_Control_flag = 0;
		HC595_IO_Out_Updata_Status(Relay1_2_on);
		HC595_IO_Out_Updata_Status(Led6_on);	      // D6	��Ϊ�̵���ָʾ
	}

}

void BarrierGate_Close(void)   // ��բ�ر�        ��բ��������ʱ5ms ����ִ�йر������ʱʱ�䲻�˳���10ms
{
  if(ucBarrierGate_Control_flag != 1)
	{
	  HC595_IO_Out_Updata_Status(Relay1_2_off);
		HC595_IO_Out_Updata_Status(Led6_off);	
	}
}


