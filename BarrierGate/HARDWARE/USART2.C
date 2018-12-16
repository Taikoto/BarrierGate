/*******************************************************
 *  程序名称: USART2.c
 *  程序功能：串口2配置初始化
 *  程序作者：钟太松
 *  创建时间：2015-11-19
 *  修改时间：
 *  程序版本：V1.1
 ******************************************************/
 
/*
 *  硬件配置： TXD2 --- PA2 --- Pin16；   RXD2 --- PA3 --- Pin17
 */

/*
 *  包含头文件
 */


#include "USART1.H"
#include "USART2.H"

//u8 USART2_RX[USART2_RX_LEN] = {0};      // 串口接收缓存区 
volatile vu16 usart_rx2_num = 0;
volatile vu16 usart_rx2_use=0;
volatile u8 usart_rx2_full=0;   //0 or 1
volatile u8 usart_rx2_end=0;    /// timer ticks watch on rx , pass 20ms take as end transfer

volatile u8  rx2_begin_watching=0;
volatile vu16 last_usart_rx2_num=0;

//u8 usart2_rx_buf[USART2_RX_LEN];
vu16 usart2_recvd_len=0;

extern COMM_USART g_usart2; 

 /******************************************************
 * 函数名称：uart2_init
 * 函数功能：USART2 初始化函数
 * 入口参数：void
 * 出口参数：
*******************************************************/ 
/*
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
 	//g_usart2.send_over = 0;	 
	//g_usart2.receive_new = 0; 


		//USART_Cmd(USART2, ENABLE);                    //使能串口 

//}

*/

/******************************************************
 * 函数名称：USART2_transmit
 * 函数功能：USART2 发送一个字节
 * 入口参数：unsigned char byte
 * 出口参数：
*******************************************************/ 
void USART2_transmit(unsigned char byte)
{
	USART_SendData(USART2,byte);                                     // 发送数据
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		 // 等待UART1发送完毕		              				    
}


/******************************************************
 * 函数名称：USART2_transmitS
 * 函数功能：USART2 发送多个字节数组；dat：发送的字节数组名；num：发送的字节数
 * 入口参数：unsigned char *dat,unsigned char num
 * 出口参数：发送的数组   
*******************************************************/ 
void USART2_transmitS(unsigned char *dat,unsigned int num)
{
	unsigned int i;
	for(i=0;i<num;i++) 
	{
		USART2_transmit(dat[i]);
	}
}

/******************************************************
 * 函数名称：USART2_IRQHandler
 * 函数功能：串口2中断服务函数，接收电源信息
 * 入口参数：void
 * 出口参数：
*******************************************************/ 
void USART2_IRQHandler(void)         
{
//	   u8 i;
//	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) != RESET)
//  {
//	  USART_ReceiveData(USART2);
//  }

 	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)
	{ 
	  if(usart_rx2_full==0) //数据未满，可以接收
    {			
			//debug_print("aaaaaa",6);
				USART2_RX[usart_rx2_num++] = USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据

				if(usart_rx2_num >= USART2_RX_LEN) //到顶归零
				{
					//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);	 // 需要	
					usart_rx2_num = 0;		
					//USART2_transmitS(USART2_RX,USART2_RX_LEN);			  
				}
 		
				if(usart_rx2_num == usart_rx2_use)  //刚好满帧
				{			
					//接收数据溢出，停止继续接收//继续接收则丢掉一帧(USART2_RX_LEN)		, 取走后再开中断
					usart_rx2_full=1;
					//USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);				
				}
				else
					usart_rx2_full=0;

	 			//rx2_watching();
			
 	  }
		else  //数据已满，清掉中断数据寄存器
			USART_ReceiveData(USART2);		
	}
	
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		     //USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);	 // 需要	
       //debug_print("UART2 IRQ",9);	
	     //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	 // 需要		
}


/*********************************************END OF FILE**********************/
#if 0

111111111111111

int USART2_recv_buf(void)
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;

	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);	 //接收前关中断

	if (usart_rx2_full)  //处理满帧的情况
	{
		while(1)
		{	
		 usart2_rx_buf[recv_len++]=USART2_RX[usart_rx2_use++];
 		 
		 if(usart_rx2_use >= USART_RX_LEN)
		      usart_rx2_use=0;

	   if(((usart_rx2_use==usart_rx2_num))||recv_len==USART_RX_LEN)  //all data has been recved, recv to upper user end
		 {
			 //all data been received,reset num and use pointer to top
			 usart_rx2_full=0;
 			 break;
		 }
	  }
 	}
	else   //not full
	{
		while(1)  //一般情况
		{
			if(((usart_rx2_use==usart_rx2_num))||recv_len==USART_RX_LEN)  //all data has been recved, recv to upper user end
			{
				/*all data been received,reset num and use pointer to top*/
				break;
			}
		 
			usart2_rx_buf[recv_len++]=USART2_RX[usart_rx2_use++];
 		 
			if(usart_rx2_use >= USART_RX_LEN)
		      usart_rx2_use=0;
		}
    }
	
	usart2_recvd_len=recv_len;
	
  usart_rx2_num=0;   //IRQ BUF 归零
	usart_rx2_use=0;
  //usart_rx2_full=0;
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	 //接收后开中断

	return recv_len;
	
}

//extern void debug_print(u8* buf,unsigned int len);

int if_USART2_has_new_data(void)  //返回IRQ_BUF的数据长度
{
	
	int recv_len=0;
	int rx2_use=usart_rx2_use;
  //u8 debugchar[1025]={0};
	
	//debug_printf("usart_rx2_num=%d,usart_rx2_use=%d,usart_rx2_full=%d,\n\r",usart_rx2_num,usart_rx2_use,usart_rx2_full);
	
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);	 //计数前关中断
	
	if(usart_rx2_full)
	  recv_len=USART_RX_LEN;     //满帧
	else
	{
		/*
	  while(rx2_use!=usart_rx2_num)  //两指针相等时计数完毕
		{
		  if(rx2_use++ >= USART_RX_LEN)
		      rx2_use=0;			

			recv_len++;
		}
		*/
				recv_len = usart_rx2_num;

	  //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	 ////不满才开中断
  } 
	
	usart2_recvd_len = recv_len;
  
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	  
  
	return recv_len;
	
	
}

int USART2_clear(void)
{
	int recv_len=0;

	USART_ReceiveData(USART2);		
	
	if(usart_rx2_full==1)
	  recv_len=USART_RX_LEN;     
	else
	{	
 		recv_len = usart_rx2_num;
  } 
	usart2_recvd_len=recv_len;

  if(usart2_recvd_len>0 && usart_rx2_end==1 )  //有数据，已收完
	{	
		usart_rx2_num=0;   //IRQ BUF 归零
		usart_rx2_use=0;
		usart_rx2_full=0;
		usart_rx2_end=0;
		rx2_begin_watching=0;
		last_usart_rx2_num=0;
	}
	
	return recv_len;
}

#endif
