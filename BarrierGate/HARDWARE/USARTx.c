#include <string.h>
#include "USARTx.H"
#include "debug_print.h"
#include "global.h"
#include "MyVirtualUart.h"

static u8 USART1_RX[USART1_RX_LEN+1] = {0};       //no loop buf, can receive USART1_RX_LEN bytes,last byte keep 0 

static u8 USART2_RX[USART2_RX_LEN+1] = {0};        

static u8 USART3_RX[USART3_RX_LEN+1] = {0};       

static u8 USARTVIRT_RX[USARTVIRT_RX_LEN+1] = {0};       

static u8 USART1_TX[512+1] = {0};
static u8 USART2_TX[512+1] = {0};
static u8 USART3_TX[512+1] = {0};

static u8 USARTVIRT_TX[512+1] = {0};
 

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
	
    //USART_Cmd(USART1, ENABLE);                    //使能串口 ,放在最后 
		
	g_usart1.usart = USART1;
	g_usart1.tx_buf = USART1_TX;
	g_usart1.rx_buf = USART1_RX;
	//g_usart1.rx_MAXLEN = USART1_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0
	g_usart1.rx_MAXLEN = sizeof(USART1_RX);//USART1_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0
	g_usart1.rx_state =  0;
	g_usart1.rx_num = 0;        //0 to rx_MAXLEN
	g_usart1.rx_last_num = 0;
	g_usart1.rx_begin = 0;
	g_usart1.rx_use = 0;
 	g_usart1.send_over = 0;	/*NULL */
	g_usart1.receive_new = 0; /*NULL */
  g_usart1.key_in=0;
	
    USART_Cmd(USART1, ENABLE);                    //使能串口 ,放在最后 

   USART_ClearFlag(USART1, USART_FLAG_TC);     /* Transmission Complete flag */

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
	g_usart2.tx_buf = USART2_TX;
	g_usart2.rx_buf = USART2_RX;
	//g_usart2.rx_MAXLEN = USART2_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0 
 	g_usart2.rx_MAXLEN = sizeof(USART2_RX);//USART2_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0 
	g_usart2.rx_state =  0;
	g_usart2.rx_num = 0;        //0 to rx_MAXLEN
	g_usart2.rx_last_num = 0;
	g_usart2.rx_use = 0;
 	g_usart2.send_over = 0;	/*NULL */
	g_usart2.receive_new = 0; /*NULL */
  g_usart2.key_in=0;

		USART_Cmd(USART2, ENABLE);                    //使能串口 
 
   USART_ClearFlag(USART2, USART_FLAG_TC);     /* Transmission Complete flag */

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
	g_usart3.tx_buf = USART3_TX;
	g_usart3.rx_buf = USART3_RX;
	//g_usart3.rx_MAXLEN = USART3_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0
		
	g_usart3.rx_MAXLEN = sizeof(USART3_RX);//USART3_RX_LEN;  //1024 , can receive 1024 bytes, last byte keep 0
	g_usart3.rx_state =  0;
	g_usart3.rx_num = 0;        //0 to rx_MAXLEN
	g_usart3.rx_last_num = 0;
	g_usart3.rx_use = 0;
 	g_usart3.send_over = 0;	/*NULL */
	g_usart3.receive_new = 0; /*NULL */
  g_usart3.key_in=0;

	USART_Cmd(USART3, ENABLE);                    //使能串口 

 	USART_ClearFlag(USART3, USART_FLAG_TC);     /* Transmission Complete flag */

}

void uartvirt_init(void)
{	
	 //IOConfig();
   //TIM2_Int_Init(107, 71);	 //1M????
   
	 //IO_TXD(0XBB);

	 //g_usartvirt.usart = USART3;
	 g_usartvirt.usart = 0;
	 g_usartvirt.tx_buf = USARTVIRT_TX;
	 g_usartvirt.rx_buf = USARTVIRT_RX;
	 //g_usartvirt.rx_MAXLEN = USARTVIRT_RX_LEN;  //64 , only can receive 64 bytes, last byte keep 0
		
	 g_usartvirt.rx_MAXLEN = sizeof(USARTVIRT_RX);//USARTVIRT_RX_LEN;  //64 , only can receive 64 bytes, last byte keep 0
	 g_usartvirt.rx_state =  0;
	 g_usartvirt.rx_num = 0;        //0 to rx_MAXLEN
	 g_usartvirt.rx_last_num = 0;
	 g_usartvirt.rx_use = 0;
 	 g_usartvirt.send_over = 0;	/*NULL */
	 g_usartvirt.receive_new = 0; /*NULL */
	
	 g_usartvirt.key_in=1;
	
		 //IO_TXD(0XBB);
	
		 //IO_TXD(0XBB); 		 
		
}

void USART_transmit(COMM_USART *p_usart,unsigned char byte)
{
	if(p_usart==g_pcom_virt)		 	 	 
	{	
		Virtual_IO_TXD(byte);
	}		
	else
	{
	  USART_SendData(p_usart->usart,byte);                                    // 发送数据 
    while (USART_GetFlagStatus(p_usart->usart, USART_FLAG_TC) == RESET);		// 等待UART1发送完毕		   reset : 0 
	}	
	
}


void USART_transmitS(COMM_USART *p_usart,unsigned char *dat,unsigned int num)
{
	  unsigned int i;
	
  	if(p_usart==g_pcom_virt)	   	   
		{
      //void Virtual_USART_Send(u8 *buf, u8 len)			
			Virtual_USART_Send(dat,num);
		}		
		else
		{
			for(i=0;i<num;i++) 
			{
				USART_transmit(p_usart,dat[i]);
			}
		}	
		
}


void UsartIRQ(COMM_USART *p_usart)
{
  u8	*USART_RX = p_usart->rx_buf;

  //USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	
	
  if(USART_GetITStatus(p_usart->usart,USART_IT_RXNE) != RESET)
	{ 
    
		USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	
		
	  if(p_usart->rx_state<2) //数据未满，可以接收 
    {
			  //p_usart->rx_buf[p_usart->rx_num++] = USART_ReceiveData(p_usart->usart);  
			   
        USART_RX[p_usart->rx_num++] = USART_ReceiveData(p_usart->usart);//(USART1->DR);	//读取接收到的数据
			
			if(p_usart->rx_num >= p_usart->rx_MAXLEN) 
			{
         p_usart->rx_state=2;   //接收结束	  				 
			} 
 			
		}
		else  //数据已满，清除中断数据寄存器 
				USART_ReceiveData(p_usart->usart);		
	 }

	 USART_ITConfig(p_usart->usart, USART_IT_RXNE, ENABLE);	

}

void USART1_IRQHandler(void)
{
	UsartIRQ(&g_usart1);
}

void USART2_IRQHandler(void)
{
	UsartIRQ(&g_usart2);
}

void USART3_IRQHandler(void)
{
	UsartIRQ(&g_usart3);
}

int RX_Clear(COMM_USART *p_usart)
{
	int recv_len=0;
	
	if(p_usart!=&g_usartvirt)		 
	{	
		while(USART_GetITStatus(p_usart->usart,USART_IT_RXNE) != RESET)		
				USART_ReceiveData(p_usart->usart);		

		USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	
  }
	
 	recv_len = p_usart->rx_num;
	
	//IRQ BUF 归零 
 	
  memset(p_usart->rx_buf,0x0,p_usart->rx_MAXLEN);   //1025,1025,513
	 	
 	{
		p_usart->rx_num=0;    
 		p_usart->rx_last_num=0;
		
		if(p_usart==&g_usartvirt)
			p_usart->key_in=1;
		else
		  p_usart->key_in=0;
		
 		p_usart->rx_begin=0;
		p_usart->rx_use=0;   
 		p_usart->rx_state=0;
	}
	
	if(p_usart!=&g_usartvirt)		 
			USART_ITConfig(p_usart->usart, USART_IT_RXNE, ENABLE);	
 	
	return recv_len;
}

 

//call by MyVirtualUart.c // Timer IRQ: TIM2_IRQHandler
void UsartVirtualIRQ_Recv(u8 recv_data)
{
	UsartVirtualIRQ(&g_usartvirt, recv_data);
}


void UsartVirtualIRQ(COMM_USART *p_usart_virt,u8 recv_data)
{
  u8	*USART_RX = p_usart_virt->rx_buf;

  //USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	
	
  //if(USART_GetITStatus(p_usart->usart,USART_IT_RXNE) != RESET)
	{ 
    
		//USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	

	  if(p_usart_virt->rx_state<2) //数据未满，可以接收 
    {
			  //p_usart->rx_buf[p_usart->rx_num++] = USART_ReceiveData(p_usart->usart);  

      USART_RX[p_usart_virt->rx_num++] =  recv_data;   //读取接收到的数据
			
			if(p_usart_virt->rx_num >= p_usart_virt->rx_MAXLEN) 
			{
         p_usart_virt->rx_state=2;   //接收结束	  				 
			}
 			
		}
		//else  //数据已满，清除中断数据寄存器 
				//USART_ReceiveData(p_usart->usart);		
	 }

	 //USART_ITConfig(p_usart->usart, USART_IT_RXNE, ENABLE);	

}

