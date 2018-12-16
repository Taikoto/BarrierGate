/*******************************************************
 *  程序名称: usart1.c
 *  程序功能：串口1配置初始化
 *  程序作者：钟太松
 *  创建时间：2015-11-19
 *  修改时间：
 *  程序版本：V1.1
 ******************************************************/
 
/*
 *  包含头文件
 */

#include "USART1.H"

extern COMM_USART g_usart1; 

//u8 USART1_RX[USART1_RX_LEN] = {0};   // 串口接收缓存区
volatile vu16 usart_rx1_num = 0;       
volatile vu16 usart_rx1_use=0;         
//volatile u8 usart_rx1_full=0;        /// 0 or 1
volatile u8 usart_rx1_end=0;           /// timer ticks watch on rx , pass 20ms take as end transfer

volatile u8  rx1_begin_watching=0;
volatile vu16 last_usart_rx1_num=0;

//u8 usart1_rx_buf[USART1_RX_LEN];
vu16 usart1_recvd_len=0;

/*
 *  硬件配置： TXD1 --- PA9 --- Pin42；   RXD1 --- PA10 --- Pin43
 */

/******************************************************
 * 函数名称：uart1_init
 * 函数功能：USART1 初始化函数
 * 入口参数：void
 * 出口参数：
*******************************************************/ 

void uart1_init(u32 bound)
{
 	
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
    USART_DeInit(USART1);  //复位串口1
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

	

   //Usart1 NVIC 配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	////设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;  //抢占优先级3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ通道使能
	  NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	  USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	  USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //开启中断  ，放在打开串口之前
	
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

/******************************************************
 * 函数名称：USART1_transmit
 * 函数功能：USART1 发送一个字节
 * 入口参数：unsigned char byte
 * 出口参数：
*******************************************************/ 
void USART1_transmit(unsigned char byte)
{
	  USART_SendData(USART1,byte);                                     // 发送数据
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		 // 等待UART1发送完毕		   reset : 0           				    
} 

void USART_transmit(COMM_USART *p_usart,unsigned char byte)
{
	  USART_SendData(p_usart->usart,byte);                                     // 发送数据
    while (USART_GetFlagStatus(p_usart->usart, USART_FLAG_TC) == RESET);		 // 等待UART1发送完毕		   reset : 0           				    
} 

 
/******************************************************
 * 函数名称：USART1_transmitS
 * 函数功能：USART1 发送多个字节数组；dat：发送的字节数组名；num：发送的字节数
 * 入口参数：unsigned char *dat,unsigned char num
 * 出口参数：发送的数组   
*******************************************************/ 
void USART1_transmitS(unsigned char *dat,unsigned int num)
{
	  unsigned int i;
	  for(i=0;i<num;i++) 
	  {
		  USART1_transmit(dat[i]);
	  }
}

void USART_transmitS(COMM_USART *p_usart,unsigned char *dat,unsigned int num)
{
	  unsigned int i;
	  for(i=0;i<num;i++) 
	  {
		  USART_transmit(p_usart,dat[i]);
	  }
}



/******************************************************
 * 函数名称：USART1_IRQHandler
 * 函数功能：串口1中断服务函数，接收电源信息
 * 入口参数：void
 * 出口参数：
*******************************************************/ 


//假设到顶不归零，假设永不满帧 ， 到顶停止接收，由使用者来清零
//usart_rx1_num最大值为 USART_IT_RXNE，最大接收长度为USART_IT_RXNE, buf[0] to buf[USART_IT_RXNE-1],
//usart_rx1_num指向下一个能接收的地址，如果usart_rx1_num已经等于USART_IT_RXNE, 表示不能再接收。
/*
void USART1_IRQHandler(void)         
//void UartIRQ(COMM_USART *p_usart)
{
//	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) != RESET)
//	{
//	  USART_ReceiveData(USART1);
//	}

 	
  if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{ 
	  if(usart_rx1_end<2) //数据未满，可以接收
    {

			USART1_RX[usart_rx1_num++] = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据

			if(usart_rx1_num >= USART1_RX_LEN) 
			{
		//     USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 // 需要	
				//usart_rx1_num = 0;
        usart_rx1_end=2;   //接收结束	 
				//USART1_transmitS(USART1_RX,USART1_RX_LEN);			  
			} 
 			
		}
		else  //数据已满，清除中断数据寄存器
				USART_ReceiveData(USART1);		
	 }
		   //  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 // 需要	
       //debug_print("UART1 IRQ",9);	
	     //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	 // 需要	
}
*/

void UsartIRQ(COMM_USART *p_usart)
{ 	
  if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{ 
	  if(p_usart->rx_state<2) //数据未满，可以接收
    {

			USART1_RX[p_usart->rx_num++] = USART_ReceiveData(p_usart->usart);//(USART1->DR);	//读取接收到的数据

			if(p_usart->rx_num >= p_usart->rx_MAXLEN) 
			{
		//     USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 // 需要	
				//usart_rx1_num = 0;
        p_usart->rx_state=2;   //接收结束	 
				//USART1_transmitS(USART1_RX,USART1_RX_LEN);			  
			} 
 			
		}
		else  //数据已满，清除中断数据寄存器
				USART_ReceiveData(p_usart->usart);		
	 }
		   //  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	  
       //debug_print("UART1 IRQ",9);	
	     //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	  
}

void USART1_IRQHandler(void)
{
	UsartIRQ(&g_usart1);
}


int USART1_clear(void)
{
	int recv_len=0;
	
	USART_ReceiveData(USART1);		
	
	debug_printf("in USART1_clear() usart_rx1_num=%d,usart_rx1_end=%d,usart1_recvd_len=%d,\r\n",usart_rx1_num,usart_rx1_end,usart1_recvd_len);
	
 	recv_len = usart_rx1_num;
 	
  if(usart_rx1_num>0 && usart_rx1_end==2 )  //有数据，已收完
	{	
		usart_rx1_num=0;   //IRQ BUF 归零
 		last_usart_rx1_num=0;
		usart_rx1_use=0;   
 		usart_rx1_end=0;    
	}
	
 	debug_printf("in USART1_clear() usart_rx1_num=%d,usart_rx1_end=%d,usart1_recvd_len=%d,\r\n",usart_rx1_num,usart_rx1_end,usart1_recvd_len);
	
	return recv_len;
}

int USART_clear(COMM_USART *p_usart)
{
	int recv_len=0;
	
	USART_ReceiveData(p_usart->usart);		
	                                                                                                        
	debug_printf("in USART_clear(*p_usart) p_usart->rx_num=%d,p_usart->rx_state=%d \n\r",p_usart->rx_num,p_usart->rx_state);
	
 	recv_len = usart_rx1_num;
 	
  if(p_usart->rx_num>0 && p_usart->rx_state==2 )  //有数据，已收完
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
int USART1_recv_buf(void)
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;
  
	//usart_rx1_use 从上次接收位置开始
	
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	
	
 		while(1) 
		{
 	   if(((usart_rx1_use==usart_rx1_num))||recv_len==USART_RX_LEN)  //all data has been recved, recv to upper user end
		 {
			 /*all data been received,reset num and use pointer to top*/
 			 break;
		 }
		 
	   usart1_rx_buf[recv_len++]=USART1_RX[usart_rx1_use++];
 		 
 		}
	 
 	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	
	
	return recv_len;
	
}

//usart_rx1_num到顶不归零，usart_rx1_use也不到顶归零，由使用者归零
int USART_recv_buf(COMM_USART *p_usart,u8 *p_rx_buf)
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;
  
	//usart_rx1_use 从上次接收位置开始
	
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

//extern void debug_print(u8* buf,unsigned int len);

int if_USART1_has_new_data(void)
{

	int recv_len=0;
	int rx1_use=usart_rx1_use;
//  u8 debugchar[1025]={0};
	
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 //计数前关中断
	
	recv_len = usart_rx1_num;
		 	
 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	 
	
	return recv_len;
	
 	
}

int if_USART_has_new_data(COMM_USART *p_usart)
{

	int recv_len=0;
  
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	 //计数前关中断
	
	recv_len = p_usart->rx_num;
		 	
 
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, ENABLE);	 
	
	return recv_len;
	
 	
}

/*********************************************END OF FILE**********************/

