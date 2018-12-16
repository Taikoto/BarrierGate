
#include "global.h"
#include "myvirtualuart.h"

u8 virtual_recv_len = 0;	//���ռ���
u8 USART_buf[30]={0};  //���ջ�����

u8 recvStat = COM_STOP_BIT;
u8 recvData = 0;


void IOConfig(void)
 {
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 	EXTI_InitTypeDef EXTI_InitStruct;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB,PC�˿�ʱ�� 
	 
	 //SoftWare Serial TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	    
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	 
  GPIO_Init(GPIOA, &GPIO_InitStructure);	  				
  GPIO_SetBits(GPIOA,GPIO_Pin_4); 						
	 
	 
	//SoftWare Serial RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);	 

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
	EXTI_InitStruct.EXTI_Line = EXTI_Line11;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling; //�½��ش����ж�
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_Init(&EXTI_InitStruct);


	NVIC_InitStructure.NVIC_IRQChannel= EXTI15_10_IRQn ; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  
	NVIC_Init(&NVIC_InitStructure);  
	
}
 
void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���			 
}

void Virtual_IO_TXD(u8 Data)
{
	IO_TXD(Data);
}

void Virtual_USART_Send(u8 *buf, u8 len)
{
	USART_Send(buf,len);	
}	


void virtual_uart_config(void)
{
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//???????????2:2??????,2??????	  
	 IOConfig();
   TIM2_Int_Init(107, 71);	 //1M????
}

void test_virt_progom(void)
{
	 //delay_init();
	
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//???????????2:2??????,2??????	  
	 IOConfig();
   TIM2_Int_Init(107, 71);	 //1M????
	
	 //uart3_init(9600);
	 
	 //USART3_transmit(0X5A);
	 //IO_TXD(0XBB);
	
  while(1)
	{
		//len=21;
		//if(len > 20)
		{
			//len = 0;
			//USART_Send(USART_buf,11);			
			//debug_printf("len=%d\r\n",len);
			debug_printf("%s",USART_buf);
			//p_usart_virt
			debug_printf("%s",g_pcom_virt->rx_buf);
		}
	}
	
}


#define VIRTUAL_USART_BUAD   BuadRate_57600

void IO_TXD(u8 Data)
{
	u8 i = 0;
	OI_TXD = 0;  
	 
	delay_us(VIRTUAL_USART_BUAD);
	for(i = 0; i < 8; i++)
	{
		if(Data&0x01)
			OI_TXD = 1;  
		else
			OI_TXD = 0; 	
		
		 
		delay_us(VIRTUAL_USART_BUAD);
		Data = Data>>1;
	}
	OI_TXD = 1;
	 
	delay_us(VIRTUAL_USART_BUAD);
}
	
void USART_Send(u8 *buf, u8 len)
{
	u8 t;
	for(t = 0; t < len; t++)
	{
		IO_TXD(buf[t]);
	}
}


void IO_COM_StringSend(unsigned char *str)   
{
  while(*str != 0)
	{
	  IO_TXD(*str);
		str++;
	}
}


void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line11) != RESET)
	{
		if(OI_RXD == 0) 
		{
			if(recvStat == COM_STOP_BIT)
			{
				recvStat = COM_START_BIT;
				TIM_Cmd(TIM2, ENABLE);
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}
	

extern void UsartVirtualIRQ_Recv(u8 recv_data);

void TIM2_IRQHandler(void)
{  
	if(TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);	
		 recvStat++;
		if(recvStat == COM_STOP_BIT)
		{
			TIM_Cmd(TIM2, DISABLE);
			USART_buf[virtual_recv_len++] = recvData;	
			
			//USART3_transmit(recvData);
			
			UsartVirtualIRQ_Recv(recvData);
		
			return;
		}
		if(OI_RXD)
		{
			recvData |= (1 << (recvStat - 1));
		}else{
			recvData &= ~(1 << (recvStat - 1));
		}	
  }		
}






