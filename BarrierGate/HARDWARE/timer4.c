
/*******************************************************
 *  程序名称: timer4.c
 *  程序功能：定时器4初始化函数
 *  程序作者：钟太松
 *  创建时间：2015-09-07
 *  修改时间：
 *  程序版本：V1.0
 ******************************************************/
#include "timer4.h"
//#include "USART1.H"
//#include "USART2.H"
//#include "USART3.H"
#include "USARTx.H"
#include "debug_print.h"
#include "global.h"

//////////////////////////////////////////////////////////////////////////////////   	 

//通用定时器4中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器4!
// 中断时间 Tout= ((arr+1)*(psc+1))/Tclk         Tclk : TIMx的输入时钟频率(MHZ),  Tout : TIMx的溢出时间 (uS)
void TIM4_Int_Init(unsigned int arr,unsigned int psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
 	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级3级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM4, DISABLE); 		 
}

//定时器4中断服务程序

extern COMM_USART g_usart1;
extern COMM_USART g_usart2;
extern COMM_USART g_usart3;

//vu16	rx1_watching(void);
vu16	rx_watching(COMM_USART *p_usart);

static uint16_t tim4_ticks_in=0;   //tim4 interrupt freq always is 1ms, can't change, why ?

void TIM4_IRQHandler(void)   //TIM4中断
{ 
  if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
  {
		tim4_ticks_in++;
		tim4_ticks_in=tim4_ticks_in%20;
	
		if(tim4_ticks_in==0)		
		   tim4_ticks++;           //20 times irq, tim4_ticks plus 1;      
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
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


vu16	rx_watching(COMM_USART *p_usart)  //间隔一个tick(20ms) 未收到数据表示收完
{	
	switch(p_usart->rx_state)
	{
		case 0:
	     if(p_usart->rx_num>0)		 	 //开始有数据
			 {
					p_usart->rx_last_num=p_usart->rx_num;   //last跟上来
					p_usart->rx_state=1;    //正在接收
			 }
			break;
		case 1:	
		  if(p_usart->rx_num!=p_usart->rx_last_num)  //本次数据量继续增长，last跟上来
			{	
				   p_usart->rx_last_num=p_usart->rx_num; //last跟上来				
           //继续接收
				   break;
			}	
			else
			{	
				if(p_usart->rx_num==p_usart->rx_last_num)  //本次数据量和上次一样， last无需跟上					
							p_usart->rx_state=2;   //接收结束	 
				  break;
			}
 			
		case 2:			
			if(p_usart->rx_num==0)    //缓存已被清空，重新归零
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
