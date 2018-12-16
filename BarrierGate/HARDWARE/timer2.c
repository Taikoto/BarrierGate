/*******************************************************
 *  ��������: timer2.c
 *  �����ܣ���ʱ��2���ó�ʼ��
 *  �������ߣ���̫��
 *  ����ʱ�䣺2015-11-19
 *  �޸�ʱ�䣺
 *  ����汾��V1.1
 ******************************************************/
#include "timer2.h"
							  
//////////////////////////////////////////////////////////////////////////////////   	 
//ͨ�ö�ʱ��2�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��2!
// �ж�ʱ�� Tout= ((arr+1)*(psc+1))/Tclk         Tclk : TIMx������ʱ��Ƶ��(MHZ),  Tout : TIMx�����ʱ�� (uS)
// TIM2_Int_Init(9,7199);// 1ms
void TIM2_Int_Init_ori(unsigned int arr,unsigned  int psc)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
		
		//��ʱ��TIM2��ʼ��
		TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM2�ж�,��������ж�
		
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

		//�ж����ȼ�NVIC����
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�2��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

		TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx					 
}

// //��ʱ��2�жϷ������
// void TIM2_IRQHandler(void)   //TIM2�ж�
// {
// 	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
// 		{
// 		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
// //		LED1=!LED1;
// 		}
// }
