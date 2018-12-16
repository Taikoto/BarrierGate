/*******************************************************
 *  ��������: usart1.c
 *  �����ܣ�����1���ó�ʼ��
 *  �������ߣ���̫��
 *  ����ʱ�䣺2015-11-19
 *  �޸�ʱ�䣺
 *  ����汾��V1.1
 ******************************************************/
 
/*
 *  ����ͷ�ļ�
 */

#include "USART1.H"

extern COMM_USART g_usart1; 

//u8 USART1_RX[USART1_RX_LEN] = {0};   // ���ڽ��ջ�����
volatile vu16 usart_rx1_num = 0;       
volatile vu16 usart_rx1_use=0;         
//volatile u8 usart_rx1_full=0;        /// 0 or 1
volatile u8 usart_rx1_end=0;           /// timer ticks watch on rx , pass 20ms take as end transfer

volatile u8  rx1_begin_watching=0;
volatile vu16 last_usart_rx1_num=0;

//u8 usart1_rx_buf[USART1_RX_LEN];
vu16 usart1_recvd_len=0;

/*
 *  Ӳ�����ã� TXD1 --- PA9 --- Pin42��   RXD1 --- PA10 --- Pin43
 */

/******************************************************
 * �������ƣ�uart1_init
 * �������ܣ�USART1 ��ʼ������
 * ��ڲ�����void
 * ���ڲ�����
*******************************************************/ 

void uart1_init(u32 bound)
{
 	
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
    USART_DeInit(USART1);  //��λ����1
    //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

	

   //Usart1 NVIC ����
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	////����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;  //��ռ���ȼ�3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQͨ��ʹ��
	  NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	  USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	  USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //�����ж�  �����ڴ򿪴���֮ǰ
	
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

/******************************************************
 * �������ƣ�USART1_transmit
 * �������ܣ�USART1 ����һ���ֽ�
 * ��ڲ�����unsigned char byte
 * ���ڲ�����
*******************************************************/ 
void USART1_transmit(unsigned char byte)
{
	  USART_SendData(USART1,byte);                                     // ��������
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		 // �ȴ�UART1�������		   reset : 0           				    
} 

void USART_transmit(COMM_USART *p_usart,unsigned char byte)
{
	  USART_SendData(p_usart->usart,byte);                                     // ��������
    while (USART_GetFlagStatus(p_usart->usart, USART_FLAG_TC) == RESET);		 // �ȴ�UART1�������		   reset : 0           				    
} 

 
/******************************************************
 * �������ƣ�USART1_transmitS
 * �������ܣ�USART1 ���Ͷ���ֽ����飻dat�����͵��ֽ���������num�����͵��ֽ���
 * ��ڲ�����unsigned char *dat,unsigned char num
 * ���ڲ��������͵�����   
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
 * �������ƣ�USART1_IRQHandler
 * �������ܣ�����1�жϷ����������յ�Դ��Ϣ
 * ��ڲ�����void
 * ���ڲ�����
*******************************************************/ 


//���赽�������㣬����������֡ �� ����ֹͣ���գ���ʹ����������
//usart_rx1_num���ֵΪ USART_IT_RXNE�������ճ���ΪUSART_IT_RXNE, buf[0] to buf[USART_IT_RXNE-1],
//usart_rx1_numָ����һ���ܽ��յĵ�ַ�����usart_rx1_num�Ѿ�����USART_IT_RXNE, ��ʾ�����ٽ��ա�
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
	  if(usart_rx1_end<2) //����δ�������Խ���
    {

			USART1_RX[usart_rx1_num++] = USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������

			if(usart_rx1_num >= USART1_RX_LEN) 
			{
		//     USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 // ��Ҫ	
				//usart_rx1_num = 0;
        usart_rx1_end=2;   //���ս���	 
				//USART1_transmitS(USART1_RX,USART1_RX_LEN);			  
			} 
 			
		}
		else  //��������������ж����ݼĴ���
				USART_ReceiveData(USART1);		
	 }
		   //  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 // ��Ҫ	
       //debug_print("UART1 IRQ",9);	
	     //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	 // ��Ҫ	
}
*/

void UsartIRQ(COMM_USART *p_usart)
{ 	
  if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
	{ 
	  if(p_usart->rx_state<2) //����δ�������Խ���
    {

			USART1_RX[p_usart->rx_num++] = USART_ReceiveData(p_usart->usart);//(USART1->DR);	//��ȡ���յ�������

			if(p_usart->rx_num >= p_usart->rx_MAXLEN) 
			{
		//     USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 // ��Ҫ	
				//usart_rx1_num = 0;
        p_usart->rx_state=2;   //���ս���	 
				//USART1_transmitS(USART1_RX,USART1_RX_LEN);			  
			} 
 			
		}
		else  //��������������ж����ݼĴ���
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
 	
  if(usart_rx1_num>0 && usart_rx1_end==2 )  //�����ݣ�������
	{	
		usart_rx1_num=0;   //IRQ BUF ����
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
 	
  if(p_usart->rx_num>0 && p_usart->rx_state==2 )  //�����ݣ�������
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
int USART1_recv_buf(void)
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;
  
	//usart_rx1_use ���ϴν���λ�ÿ�ʼ
	
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

//usart_rx1_num���������㣬usart_rx1_useҲ���������㣬��ʹ���߹���
int USART_recv_buf(COMM_USART *p_usart,u8 *p_rx_buf)
{
	//return USART_recv_buf(usart2_rx_buf,USART2_RX,(int*)&usart_rx2_use,(int*)&usart_rx2_num);	
	int recv_len=0;
  
	//usart_rx1_use ���ϴν���λ�ÿ�ʼ
	
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
	
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	 //����ǰ���ж�
	
	recv_len = usart_rx1_num;
		 	
 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	 
	
	return recv_len;
	
 	
}

int if_USART_has_new_data(COMM_USART *p_usart)
{

	int recv_len=0;
  
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, DISABLE);	 //����ǰ���ж�
	
	recv_len = p_usart->rx_num;
		 	
 
	USART_ITConfig(p_usart->usart, USART_IT_RXNE, ENABLE);	 
	
	return recv_len;
	
 	
}

/*********************************************END OF FILE**********************/
