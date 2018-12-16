/*******************************************************
 *  程序名称: USART3.c
 *  程序功能：串口3配置初始化
 *  程序作者：钟太松
 *  创建时间：2015-11-19
 *  修改时间：
 *  程序版本：V1.1
 ******************************************************/
 
/*
 *  硬件配置： TXD3 --- PB10 --- Pin29；   RXD3 --- PB11 --- Pin30
 */

/*
 *  包含头文件
 */

 
 #include "USART1.H"
 #include "USART3.H"
 
 
//u8 USART3_RX[USART3_RX_LEN] = {0};      // 串口接收缓存区
volatile vu16 usart_rx3_num = 0;
volatile vu16 usart_rx3_use=0;
volatile u8 usart_rx3_full=0;   //0 or 1
volatile u8 usart_rx3_end=0;    /// timer ticks watch on rx , pass 20ms take as end transfer

volatile u8  rx3_begin_watching=0;
volatile vu16 last_usart_rx3_num=0;

//u8 usart3_rx_buf[USART3_RX_LEN];
vu16 usart3_recvd_len=0;

extern COMM_USART g_usart3; 

/******************************************************
 * 函数名称：uart3_init
 * 函数功能：USART3 初始化函数
 * 入口参数：void
 * 出口参数：
*******************************************************/ 
/*
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
 	//g_usart3.send_over = 0; 
	//g_usart3.receive_new = 0; 

		//USART_Cmd(USART3, ENABLE);                    //使能串口 

}
*/

/******************************************************
 * 函数名称：USART3_transmit
 * 函数功能：USART3 发送一个字节
 * 入口参数：unsigned char byte
 * 出口参数：
*******************************************************/ 
void USART3_transmit(unsigned char byte)
{
		USART_SendData(USART3,byte);                                     // 发送数据
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		 // 等待USART3发送完毕		              				    
}

/******************************************************
 * 函数名称：USART3_transmitS
 * 函数功能：USART3 发送多个字节数组；dat：发送的字节数组名；num：发送的字节数
 * 入口参数：unsigned char *dat,unsigned char num
 * 出口参数：发送的数组   
*******************************************************/ 
void USART3_transmitS(unsigned char *dat,unsigned int num)
{
		unsigned int i;
		for(i=0;i<num;i++) 
		{
			USART3_transmit(dat[i]);
		}
}


/******************************************************
 * 函数名称：USART3_IRQHandler
 * 函数功能：串口3中断服务函数，接收手柄数据
 * 入口参数：void
 * 出口参数：
*******************************************************/ 
void USART3_IRQHandler(void)         
{
//	   u8 i;
//	if(USART_GetFlagStatus(USART3,USART_FLAG_ORE) != RESET)
//  {
//	  USART_ReceiveData(USART3);
//  }
	
	
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{ 
	  if(usart_rx3_full==0) //数据未满，可以接收
    {			
 		
			USART3_RX[usart_rx3_num++] = USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据

			if(usart_rx3_num >= USART3_RX_LEN)
			{
				//USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	 // 需要	
				usart_rx3_num = 0;				
			
				//USART3_transmitS(USART3_RX,USART3_RX_LEN);     //debug
			
			}

 		
				if(usart_rx3_num == usart_rx3_use)  //刚好满帧
				{			
						//接收数据溢出， 关闭中断，停止接收，  继续接收则丢掉一帧(USART3_RX_LEN)			
						usart_rx3_full=1;
						//USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);				
				}
				else
					usart_rx3_full=~0;

 			//rx3_watching();
								
		}
		else  //数据已满，清除中断数据寄存器
				USART_ReceiveData(USART3);		
	}

 	
}

/*********************************************END OF FILE**********************/
 

//extern int USART_recv_buf(u8 *rx_buf,u8 *usart_buf,int *p_rx_use,int *p_rx_num);


int USART3_recv_buf(void)	
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;

	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	 //接收前关中断
	
	if (usart_rx3_full)  //处理满帧的情况
	{
		while(1)
		{	
		 usart3_rx_buf[recv_len++]=USART3_RX[usart_rx3_use++];
 		 
		 if(usart_rx3_use >= USART_RX_LEN)
		      usart_rx3_use=0;

	   if(((usart_rx3_use==usart_rx3_num))||recv_len==USART_RX_LEN)  //all data has been recved, recv to upper user end
		 {
			 //all data been received,reset num and use pointer to top
			 usart_rx3_full=0;
 			 break;
		 }
		 
	 }
		
	}	
  else
	{
			while(1) //不满帧的情况
			{
				if((usart_rx3_use==usart_rx3_num)||recv_len==USART_RX_LEN)  //all data has been recved, recv to upper user end
				{
					/*all data been received,reset num and use pointer to top*/
					break;
				}
		 
				usart3_rx_buf[recv_len++]=USART3_RX[usart_rx3_use++];
 		 
				if(usart_rx3_use >= USART_RX_LEN)
							usart_rx3_use=0;
			}
  } 
	
	usart3_recvd_len=recv_len;

  usart_rx3_num=0;   //IRQ BUF 指针归零
	usart_rx3_use=0;
	usart_rx3_full=0;
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	 //接收完成开中断
	
	return recv_len;
	
}


int if_USART3_has_new_data(void)  //返回IRQ_BUF的数据长度
{

	int recv_len=0;
	int rx3_use=usart_rx3_use;
  //u8 debugchar[1025]={0};
	
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	 //计数前关中断
	
	if(usart_rx3_full==1)
	  recv_len=USART_RX_LEN;     //满帧
	else
	{
		/*
	  while(rx3_use!=usart_rx3_num)  //两指针相等时计数完毕
		{
		  if(rx3_use++ >= USART_RX_LEN)
		      rx3_use=0;

			recv_len++;
		}
		*/
		recv_len = usart_rx3_num;
		
	  //USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	 ////??????
  } 
	
	usart3_recvd_len = recv_len;

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	  
  
	return recv_len;
	
	//if((usart_rx3_use!=usart_rx3_num)||(usart_rx3_full==1))  //两指针重合，一般情况是取完数据，特殊情况是刚好满帧
		//return 1;
  //else	
 		 //return 0;
 	
	//return ((usart_rx3_use!=usart_rx3_num)?1:0);
}
	
int USART3_clear(void)
{
	int recv_len=0;

	USART_ReceiveData(USART3);		
	
	if(usart_rx3_full==1)
	  recv_len=USART_RX_LEN;     
	else
	{	
 		recv_len = usart_rx3_num;
  } 
	usart3_recvd_len=recv_len;

 
  if(usart3_recvd_len>0 && usart_rx3_end==1 )  //有数据，已收完
	{	
		usart_rx3_num=0;   //IRQ BUF 归零
		usart_rx3_use=0;
		usart_rx3_full=0;
		usart_rx3_end=0;
		rx3_begin_watching=0;
		last_usart_rx3_num=0;
	}
	
	return recv_len;
}

#if 0

333333333333111111111111111

#endif
