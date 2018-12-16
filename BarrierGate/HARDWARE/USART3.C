/*******************************************************
 *  ��������: USART3.c
 *  �����ܣ�����3���ó�ʼ��
 *  �������ߣ���̫��
 *  ����ʱ�䣺2015-11-19
 *  �޸�ʱ�䣺
 *  ����汾��V1.1
 ******************************************************/
 
/*
 *  Ӳ�����ã� TXD3 --- PB10 --- Pin29��   RXD3 --- PB11 --- Pin30
 */

/*
 *  ����ͷ�ļ�
 */

 
 #include "USART1.H"
 #include "USART3.H"
 
 
//u8 USART3_RX[USART3_RX_LEN] = {0};      // ���ڽ��ջ�����
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
 * �������ƣ�uart3_init
 * �������ܣ�USART3 ��ʼ������
 * ��ڲ�����void
 * ���ڲ�����
*******************************************************/ 
/*
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
 	//g_usart3.send_over = 0; 
	//g_usart3.receive_new = 0; 

		//USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 

}
*/

/******************************************************
 * �������ƣ�USART3_transmit
 * �������ܣ�USART3 ����һ���ֽ�
 * ��ڲ�����unsigned char byte
 * ���ڲ�����
*******************************************************/ 
void USART3_transmit(unsigned char byte)
{
		USART_SendData(USART3,byte);                                     // ��������
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		 // �ȴ�USART3�������		              				    
}

/******************************************************
 * �������ƣ�USART3_transmitS
 * �������ܣ�USART3 ���Ͷ���ֽ����飻dat�����͵��ֽ���������num�����͵��ֽ���
 * ��ڲ�����unsigned char *dat,unsigned char num
 * ���ڲ��������͵�����   
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
 * �������ƣ�USART3_IRQHandler
 * �������ܣ�����3�жϷ������������ֱ�����
 * ��ڲ�����void
 * ���ڲ�����
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
	  if(usart_rx3_full==0) //����δ�������Խ���
    {			
 		
			USART3_RX[usart_rx3_num++] = USART_ReceiveData(USART3);//(USART3->DR);	//��ȡ���յ�������

			if(usart_rx3_num >= USART3_RX_LEN)
			{
				//USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	 // ��Ҫ	
				usart_rx3_num = 0;				
			
				//USART3_transmitS(USART3_RX,USART3_RX_LEN);     //debug
			
			}

 		
				if(usart_rx3_num == usart_rx3_use)  //�պ���֡
				{			
						//������������� �ر��жϣ�ֹͣ���գ�  ���������򶪵�һ֡(USART3_RX_LEN)			
						usart_rx3_full=1;
						//USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);				
				}
				else
					usart_rx3_full=~0;

 			//rx3_watching();
								
		}
		else  //��������������ж����ݼĴ���
				USART_ReceiveData(USART3);		
	}

 	
}

/*********************************************END OF FILE**********************/
 

//extern int USART_recv_buf(u8 *rx_buf,u8 *usart_buf,int *p_rx_use,int *p_rx_num);


int USART3_recv_buf(void)	
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;

	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	 //����ǰ���ж�
	
	if (usart_rx3_full)  //������֡�����
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
			while(1) //����֡�����
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

  usart_rx3_num=0;   //IRQ BUF ָ�����
	usart_rx3_use=0;
	usart_rx3_full=0;
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	 //������ɿ��ж�
	
	return recv_len;
	
}


int if_USART3_has_new_data(void)  //����IRQ_BUF�����ݳ���
{

	int recv_len=0;
	int rx3_use=usart_rx3_use;
  //u8 debugchar[1025]={0};
	
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	 //����ǰ���ж�
	
	if(usart_rx3_full==1)
	  recv_len=USART_RX_LEN;     //��֡
	else
	{
		/*
	  while(rx3_use!=usart_rx3_num)  //��ָ�����ʱ�������
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
	
	//if((usart_rx3_use!=usart_rx3_num)||(usart_rx3_full==1))  //��ָ���غϣ�һ�������ȡ�����ݣ���������Ǹպ���֡
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

 
  if(usart3_recvd_len>0 && usart_rx3_end==1 )  //�����ݣ�������
	{	
		usart_rx3_num=0;   //IRQ BUF ����
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