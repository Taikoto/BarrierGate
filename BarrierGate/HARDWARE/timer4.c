
/*******************************************************
 *  ��������: timer4.c
 *  �����ܣ���ʱ��4��ʼ������
 *  �������ߣ���̫��
 *  ����ʱ�䣺2015-09-07
 *  �޸�ʱ�䣺
 *  ����汾��V1.0
 ******************************************************/
#include "timer4.h"
//#include "USART1.H"
//#include "USART2.H"
//#include "USART3.H"
#include "USARTx.H"
#include "debug_print.h"
#include "global.h"

//////////////////////////////////////////////////////////////////////////////////   	 

//ͨ�ö�ʱ��4�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��4!
// �ж�ʱ�� Tout= ((arr+1)*(psc+1))/Tclk         Tclk : TIMx������ʱ��Ƶ��(MHZ),  Tout : TIMx�����ʱ�� (uS)
void TIM4_Int_Init(unsigned int arr,unsigned int psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
 	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM4, DISABLE); 		 
}

//��ʱ��4�жϷ������

extern COMM_USART g_usart1;
extern COMM_USART g_usart2;
extern COMM_USART g_usart3;

//vu16	rx1_watching(void);
vu16	rx_watching(COMM_USART *p_usart);

static uint16_t tim4_ticks_in=0;   //tim4 interrupt freq always is 1ms, can't change, why ?

void TIM4_IRQHandler(void)   //TIM4�ж�
{ 
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
  {
		tim4_ticks_in++;
		tim4_ticks_in=tim4_ticks_in%20;
	
		if(tim4_ticks_in==0)		
		   tim4_ticks++;           //20 times irq, tim4_ticks plus 1;      
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
	}
	
	//keep watch on usart_rx1 received end over time
	if(tim4_ticks_in==0)
	{	
		 if(g_usart1.key_in==0)
				rx_watching(&g_usart1);
		 if(g_usart2.key_in==0)
				rx_watching(&g_usart2);
		 if(g_usart3.key_in==0)
   	    rx_watching(&g_usart3);
		 //if(g_usartvirt.key_in==0)
   	    //rx_watching(&g_usartvirt);
		  check_wait_pending(&g_network_handshake);
 	}	
 	
 	//rx2_watching();
	//rx3_watching();
		
}


vu16	rx_watching(COMM_USART *p_usart)  //���һ��tick(20ms) δ�յ����ݱ�ʾ����
{	
	switch(p_usart->rx_state)
	{
		case 0:
	     if(p_usart->rx_num>0)		 	 //��ʼ������
			 {
					p_usart->rx_last_num=p_usart->rx_num;   //last������
					p_usart->rx_state=1;    //���ڽ���
			 }
			break;
		case 1:	
		  if(p_usart->rx_num!=p_usart->rx_last_num)  //��������������������last������
			{	
				   p_usart->rx_last_num=p_usart->rx_num; //last������				
           //��������
				   break;
			}	
			else
			{	
				if(p_usart->rx_num==p_usart->rx_last_num)  //�������������ϴ�һ���� last�������					
							p_usart->rx_state=2;   //���ս���	 
				  break;
			}
 			
		case 2:			
			if(p_usart->rx_num==0)    //�����ѱ���գ����¹���
			{	
				p_usart->rx_last_num=0;
			  p_usart->rx_state=0;
			}	
			break;
	}
 
	
	return p_usart->rx_state;
}

u16 check_wait_pending(NETWORK_HANDSHAKE *p_handshake)
{
	
	 if(p_handshake->wait_recv_pending==1)		
		 p_handshake->wait_ms += 20;
	 else
	   p_handshake->wait_ms = 0;
	 
	 return p_handshake->wait_ms;
}
