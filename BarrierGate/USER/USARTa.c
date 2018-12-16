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
 	    
	   //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//????USART1??GPIOA?��??
    USART_DeInit(USART1);  //��λ����1
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

	

    //Usart1 NVIC ���� 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 ////����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� 
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;  //��ռ���ȼ�2
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		   //�����ȼ�1
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	  NVIC_Init(&NVIC_InitStructure);	 //����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
  
   //USART ��ʼ������ 

	  USART_InitStructure.USART_BaudRate = bound; 
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	  USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	  USART_InitStructure.USART_Parity = USART_Parity_No; 
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ 

    USART_Init(USART1, &USART_InitStructure); //��ʼ������ 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //�����ж�  �����ڴ򿪴���֮ǰ
	
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� ,������� 
		
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
			//GPIO�˿�����
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //ʹ��USART2��GPIOAʱ��
		USART_DeInit(USART2);  //��λ����2
		//USART2_TX   PA.2
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		 
			//USART2_RX	  PA.3
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
		GPIO_Init(GPIOA, &GPIO_InitStructure);  
		
		 //Usart1 NVIC ����
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���0λ��ռ���ȼ���16λ��Ӧ���ȼ�
		
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
		 //USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

		USART_Init(USART2, &USART_InitStructure); //��ʼ������
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�

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


		USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}


void uart3_init(u32 bound)
{
    //GPIO�˿�����
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		 
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  //ʹ��USART2��GPIOAʱ��
		USART_DeInit(USART3);  //��λ����3
		//USART3_TX   PB.10
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB.10
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		 
			//USART3_RX	  PB.11
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
		GPIO_Init(GPIOB, &GPIO_InitStructure);  
		
		 //Usart1 NVIC ����
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�2
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		
		 //USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

		USART_Init(USART3, &USART_InitStructure); //��ʼ������
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�

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

		USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 

}

void USART_transmit(COMM_USART *p_usart,unsigned char byte)
{
	  USART_SendData(p_usart->usart,byte);                                    // �������� 
    while (USART_GetFlagStatus(p_usart->usart, USART_FLAG_TC) == RESET);		// �ȴ�UART1�������		   reset : 0 
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
	  if(p_usart->rx_state<2) //����δ�������Խ��� 
    {

			USART1_RX[p_usart->rx_num++] = USART_ReceiveData(p_usart->usart);  

			if(p_usart->rx_num >= p_usart->rx_MAXLEN) 
			{
		//     USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	  
				//usart_rx1_num = 0;
        p_usart->rx_state=2;   //���ս���	  
				//USART1_transmitS(USART1_RX,USART1_RX_LEN);			  
			} 
 			
		}
		else  //��������������ж����ݼĴ��� 
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
 	
  if(p_usart->rx_num>0 && p_usart->rx_state==2 )   //�����ݣ������� 
	{	
		p_usart->rx_num=0;   //IRQ BUF ���� 
 		p_usart->rx_last_num=0;
 		p_usart->rx_begin=0;
		p_usart->rx_use=0;   
 		p_usart->rx_state=0;
	}
	
 	debug_printf("in USART_clear(*p_usart) p_usart->rx_num=%d,p_usart->rx_state=%d=%d,\n\r",p_usart->rx_num,p_usart->rx_state);
	
	return recv_len;
}


//usart_rx1_num���������㣬usart_rx1_useҲ���������㣬��ʹ���߹���
int USART_recv_buf(COMM_USART *p_usart,u8 *p_rx_buf)
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;
  
 	//rx_use ���ϴν���λ�ÿ�ʼ
	
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
  
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	  //����ǰ���ж�
	
	recv_len = p_usart->rx_num;
		 	
 
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, ENABLE);	 
	
	return recv_len;
	
 	
}