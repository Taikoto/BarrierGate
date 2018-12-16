#include "USART.H"

u8 USART1_RX[USART1_RX_LEN+1] = {0};       //no loop buf, can receive USART1_RX_LEN bytes,last byte keep 0 
u8 usart1_rx_buf[USART1_RX_LEN+1] = {0};

u8 USART2_RX[USART2_RX_LEN] = {0};        
u8 usart2_rx_buf[USART2_RX_LEN+1] = {0};

u8 USART3_RX[USART3_RX_LEN] = {0};       
u8 usart3_rx_buf[USART3_RX_LEN+1] = {0};

extern COMM_USART g_usart1; 
extern COMM_USART g_usart2; 
extern COMM_USART g_usart3; 
 
void uart1_init(u32 bound)
{
 	    
	   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//????USART1??GPIOA?±??
    USART_DeInit(USART1);  //复位串口1
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

	

    //Usart1 NVIC 配置 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 ////设置NVIC中断分组2:2位抢占优先级，2位响应优先级 
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;  //抢占优先级2
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		   //子优先级1
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	  NVIC_Init(&NVIC_InitStructure);	 //根据指定的参数初始化VIC寄存器 
  
   //USART 初始化设置 

	  USART_InitStructure.USART_BaudRate = bound; 
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	  USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	  USART_InitStructure.USART_Parity = USART_Parity_No; 
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式 

    USART_Init(USART1, &USART_InitStructure); //初始化串口 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //开启中断  ，放在打开串口之前
	
    USART_Cmd(USART1, ENABLE);                    //使能串口 ,放在最后 
		
	g_usart1.usart = USART1;
	g_usart1.p_tx_buf = 0;
	g_usart1.p_rx_buf = USART1_RX;
	g_usart1.rx_MAXLEN = USART1_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0
	g_usart1.rx_state =  0;
	g_usart1.rx_num = 0;        //0 to rx_MAXLEN
	g_usart1.rx_last_num = 0;
	g_usart1.rx_begin = 0;
	g_usart1.rx_use = 0;
 	g_usart1.send_over = 0;	/*NULL */
	g_usart1.receive_new = 0; /*NULL */
		
}

void uart2_init(u32 bound)
{
			//GPIO端口设置
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //使能USART2，GPIOA时钟
		USART_DeInit(USART2);  //复位串口2
		//USART2_TX   PA.2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		 
			//USART2_RX	  PA.3
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(GPIOA, &GPIO_InitStructure);  
		
		 //Usart1 NVIC 配置
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组0位抢占优先级，16位响应优先级
		
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
		 //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

		USART_Init(USART2, &USART_InitStructure); //初始化串口
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断

	g_usart2.usart = USART2;
	g_usart2.p_tx_buf = 0;
	g_usart2.p_rx_buf = USART2_RX;
	g_usart2.rx_MAXLEN = USART2_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0
	g_usart2.rx_state =  0;
	g_usart2.rx_num = 0;        //0 to rx_MAXLEN
	g_usart2.rx_last_num = 0;
	g_usart2.rx_use = 0;
 	g_usart2.send_over = 0;	/*NULL */
	g_usart2.receive_new = 0; /*NULL */


		USART_Cmd(USART2, ENABLE);                    //使能串口 

}


void uart3_init(u32 bound)
{
    //GPIO端口设置
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  //使能USART2，GPIOA时钟
		USART_DeInit(USART3);  //复位串口3
		//USART3_TX   PB.10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		 
			//USART3_RX	  PB.11
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(GPIOB, &GPIO_InitStructure);  
		
		 //Usart1 NVIC 配置
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级2
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		
		 //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

		USART_Init(USART3, &USART_InitStructure); //初始化串口
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断

	g_usart3.usart = USART3;
	g_usart3.p_tx_buf = 0;
	g_usart3.p_rx_buf = USART3_RX;
	g_usart3.rx_MAXLEN = USART3_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0
	g_usart3.rx_state =  0;
	g_usart3.rx_num = 0;        //0 to rx_MAXLEN
	g_usart3.rx_last_num = 0;
	g_usart3.rx_use = 0;
 	g_usart3.send_over = 0;	/*NULL */
	g_usart3.receive_new = 0; /*NULL */

		USART_Cmd(USART3, ENABLE);                    //使能串口 

}

void USART_transmit(COMM_USART *p_usart,unsigned char byte)
{
	  USART_SendData(p_usart->usart,byte);                                    // 发送数据 
    while (USART_GetFlagStatus(p_usart->usart, USART_FLAG_TC) == RESET);		// 等待UART1发送完毕		   reset : 0 
}


void USART_transmitS(COMM_USART *p_usart,unsigned char *dat,unsigned int num)
{
	  unsigned int i;
	  for(i=0;i<num;i++) 
	  {
		  USART_transmit(p_usart,dat[i]);
	  }
}


void UsartIRQ(COMM_USART *p_usart)
{
  if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{ 
	  if(p_usart->rx_state<2) //数据未满，可以接收 
    {

			USART1_RX[p_usart->rx_num++] = USART_ReceiveData(p_usart->usart);  

			if(p_usart->rx_num >= p_usart->rx_MAXLEN) 
			{
		//     USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	  
				//usart_rx1_num = 0;
        p_usart->rx_state=2;   //接收结束	  
				//USART1_transmitS(USART1_RX,USART1_RX_LEN);			  
			} 
 			
		}
		else  //数据已满，清除中断数据寄存器 
				USART_ReceiveData(p_usart->usart);		
	 }

}

void USART1_IRQHandler(void)
{
	UsartIRQ(&g_usart1);
}


int USART_clear(COMM_USART *p_usart)
{
	int recv_len=0;
	
	USART_ReceiveData(p_usart->usart);		
	                                                                                                        
	debug_printf("in USART_clear(*p_usart) p_usart->rx_num=%d,p_usart->rx_state=%d \n\r",p_usart->rx_num,p_usart->rx_state);
	
 	recv_len = p_usart->rx_num;
 	
  if(p_usart->rx_num>0 && p_usart->rx_state==2 )   //有数据，已收完 
	{	
		p_usart->rx_num=0;   //IRQ BUF 归零 
 		p_usart->rx_last_num=0;
 		p_usart->rx_begin=0;
		p_usart->rx_use=0;   
 		p_usart->rx_state=0;
	}
	
 	debug_printf("in USART_clear(*p_usart) p_usart->rx_num=%d,p_usart->rx_state=%d=%d,\n\r",p_usart->rx_num,p_usart->rx_state);
	
	return recv_len;
}


//usart_rx1_num到顶不归零，usart_rx1_use也不到顶归零，由使用者归零
int USART_recv_buf(COMM_USART *p_usart,u8 *p_rx_buf)
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;
  
 	//rx_use 从上次接收位置开始
	
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	
	
 		while(1)
		{
 	   if(((p_usart->rx_use==p_usart->rx_num))||recv_len==p_usart->rx_MAXLEN)  //all data has been recved, recv to upper user end
		 {
			 /*all data been received,reset num and use pointer to top*/
 			 break;
		 }
		 
	   p_rx_buf[recv_len++]=p_usart->p_rx_buf[p_usart->rx_use++];
 		 
 		}
	 
 	USART_ITConfig(p_usart->usart, USART_IT_RXNE, ENABLE);	
	
	return recv_len;
	
}


int if_USART_has_new_data(COMM_USART *p_usart)
{

	int recv_len=0;
  
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	  //计数前关中断
	
	recv_len = p_usart->rx_num;
		 	
 
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, ENABLE);	 
	
	return recv_len;
	
 	
}