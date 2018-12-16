//******************************
//ZACK Lynn --20170227
//******************************

#include "stm32f10x.h"

#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "stdlib.h"

#include "delay.h"
#include "timer4.h"
#include "global.h"

#include "USARTx.H"

#include "debug_print.h"
 
#include "hc595.h"
 
#include "as606.h"
#include "fp_func.h"

#include "comm_recv.h"
#include "server.h"
#include "barrier.h"

u16 ValidN;//ģ������Чģ�����


u8 FpSync_Init(FP_SYNC *pfp_sync)
{
	debug_printf("sizeof(FP_SYNC)=%d\r\n",sizeof(FP_SYNC));
  //debug_printf("sizeof(g_fp_model_buf1)=%d\r\n",sizeof(g_fp_model_buf1));
	//memset(pfp_sync,0,sizeof(FP_SYNC));
	
	pfp_sync->fp_id=1025;
	
	pfp_sync->downBufferID=g_downStoreBufferID;
	
	memset(g_fp_model_buf1,0,MODEL_LEN);
	memset(g_fp_model_buf2,0,MODEL_LEN);
	
	pfp_sync->p_model_buf1=g_fp_model_buf1;
	pfp_sync->p_model_buf2=g_fp_model_buf2;
	
	pfp_sync->checksum_result=0xff;
	
	pfp_sync->comm_server_result=0xff;
	
	pfp_sync->down_store_result=0xff;
	
	pfp_sync->ack_status=0xff;
	
	return 0;
}


//¼ָ��
u16 Add_FR(COMM_USART *pcomm,u16 id_addr)
{
	u8 i=0,processnum=0;
	u8 ensure=0xff, ensure1=0xff;
	u16 ID=0;
  
	ID=id_addr;
	
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
				//while(1)
				{					
					ensure=PS_GetImage(pcomm);   //begin in here!
					if(ensure==0x00) 
					{						   
				  		//debug_printf("\r\n case 0: GenChar(1)! \r\n");
 						  ensure=PS_GenChar(pcomm,CharBuffer1);//��������
							if(ensure==0x00)
							{
								i=0;
								debug_printf("\r\n case 0 : GenChar(1) success! press finger again!!!\r\n");
								processnum=1;//�����ڶ���
							}
							//delay_ms(3000);
						}
			  }
				//delay_ms(500);
				//delay_ms(10);
				break;
			
			case 1:
				i++;
				//�ٰ�һ����ָ
 				ensure=PS_GetImage(pcomm);
				if(ensure==0x00) 
				{
				  debug_printf("�ٰ�һ����ָ .... \r\n ");
					//debug_printf("\r\n case 1: GenChar(2)! \r\n");
 					ensure=PS_GenChar(pcomm,CharBuffer2);//��������
					if(ensure==0x00)
					{
						debug_printf("\r\n GenChar(2) success!\r\n");
						//ָ����ȷ
						i=0;
						processnum=2;//����������
					}
				}
				//delay_ms(500); 
				break;

			case 2:
				//�Ա�����ָ��
			  
			//debug_printf("\r\n case2: Match finger charter\r\n");
				ensure=PS_Match(pcomm);
				if(ensure==0x00) 
				{
					//����ָ����һ����
				  debug_printf("\r\n case2: Match oooooooooooooook!!!!\r\n");
					processnum=3;//�������Ĳ�
				}
				else 
				{
					//�Ա�ʧ�ܣ������°���ָ
					i=0;
				  debug_printf("\r\n case2: Match failed!!!,goto first step\r\n");
					processnum=0;//���ص�һ��
				}
				
				break;

			case 3: 
				//����һ��ָ��ģ�� 
  			//debug_printf("\r\n case3: RegModel\r\n"); 
				ensure=PS_RegModel(pcomm); 
				if(ensure==0x00) 
				{
  			   debug_printf("\r\n case3: RegModel seccess!\r\n"); 
					 //����ָ��ģ��ɹ�
					 processnum=4;//�������岽
				}
				else 
				{
			    debug_printf("\r\n case4: RegModel() failed!, goto case 0\r\n",ID); 
				  processnum=0;
				}
				
				break;
				
			case 4:	
				
			   //debug_printf("\r\n case4: StoreChar(ID=%d)!\r\n",ID); 
				ensure=PS_StoreChar(pcomm,CharBuffer2,ID);//����ģ��
				if(ensure==0x00) 
				{
					//���ָ�Ƴɹ�
			   debug_printf("\r\n case4: StoreChar(ID=%d) OK!\r\n",ID); 
			   debug_printf("\r\n case4: ���ָ�Ƴɹ�(ID=%d) OK!\r\n",ID); 
					
					ensure1=PS_ValidTempleteNum(pcomm,&ValidN);//����ָ�Ƹ���

			    //debug_printf("\r\n case4: ensure=0x%x,PS_ValidTempleteNum(ValidN=%d) \r\n",ensure,ValidN); 
					if(ensure1==0)
					{
			        debug_printf("\r\n case4: PS_ValidTempleteNum(ValidN=%d) ok!\r\n",ValidN);
					}
					else
						 debug_printf("\r\n case4: PS_ValidTempleteNum(ValidN=%d) failed!\r\n",ValidN); 
					
					return ensure;
					
				}else { 
			     debug_printf("\r\n case4: StoreChar(ID=%d) failed! goto case 0\r\n",ID); 					
				   processnum=0;
				} 
				break;				
			default:
				break;
		}
	
		if(i==20)//����5��û�а���ָ���˳�
		{			
	    //debug_printf("\r\n i==5, exit \r\n"); 
			break;			
	  }
	}
	return ensure;
}

u8 check_FP_Pass(SearchResult *pseach)
{
	u8 ret=0;
					Flow_Init(g_p_flow);        //�����һ����

	        g_p_flow->fp_id=pseach->pageID;

	        //
					//g_p_flow->fp_id=g_fp_seach.pageID;
					//
					//g_p_flow->fp_id=26;  //for test //lynn_debug
	        //
					
					g_p_flow->fp_check_result=0;  //pass fp check
					
					debug_printf("search FP ok,g_p_flow->fp_id=%d,g_p_flow->fp_check_result=%d,call requestServer\r\n",g_p_flow->fp_id,g_p_flow->fp_check_result);
					
					ret=requestServer_byFP(g_p_flow->fp_id);  //���������ͨ��
	
					RX_Clear(g_pcom_ser);

	return ret;
}


//��һ��ָ��
//return 0: ����ָ�Ʋ��ҵ�ָ��
//return 1: ����ָ��δ�ҵ�
//return othters: û�˰�ָ��
u8 press_FR_working(COMM_USART *pcomm,SearchResult *pseach)
{
	//SearchResult seach;
	u8 ensure=0xff;
	 
	{
		ensure=PS_GetImage(pcomm);

		if(ensure==0x00)//��ȡͼ��ɹ�
		{
			
			ensure=PS_GenChar(pcomm,CharBuffer1);
			
			if(ensure==0x00) //���������ɹ�
			{
 				 
				//ensure=PS_Search(pcomm,CharBuffer1,0,1000,pseach);
				ensure=PS_HighSpeedSearch(pcomm,CharBuffer1,0,1000,pseach);		  
 
				if(ensure==0x00) //search find!!!!
				{
 				
					check_FP_Pass(pseach);		//�ҵ�ָ�� ��չ�����������			
 					 
				}
				else
				{						  					  
				   ensure=1; //��ͼ��δ�ҵ�ָ��
				}	
				
			}
			else
			{	
				debug_printf("after GetImage,not GetChar(),ensure=%d\r\n",ensure);
			  ensure=1;  //��ͼ��δ����������
			}	
		}
  }
	return ensure;	
}

static u8 repeat_times=0;


u8 check_press_FR(void)
{ 
	u8 count=0;
	u8 ret2=0xff;
 	
			count=0;

	 	  while(1)
			{
				
				  ret2=press_FR_working(g_pcom_fp,&g_fp_seach);  //��һ��ָ��,���ƥ�䵽����չ�����������
 
  				count++;

 				  if(ret2==0x0)
					{
						//debug_printf("ok ��ָ֤�Ƴɹ�, λ��id=%03d,�÷�=%d\r\n ",g_fp_seach.pageID,g_fp_seach.mathscore);
						debug_printf("ok pressing FP success, id=%03d,score=%d\r\n",g_fp_seach.pageID,g_fp_seach.mathscore);
						break;
					}
					else
					{
						 
						//debug_printf("wait pressing FP...\r\n");
						//USART_transmitS(g_p_usart_debug,(u8*)"wait pres FP...\r\n ",18);
						
						if(count==5) //����count=5��ָ��,δ����
						{
							count=0;

							//debug_printf("not find FP\r\n ");
							if(ret2==1)   //���˰���ָ��
							{
							//voice ���ذ�ָ��
								USART_transmitS(g_p_usart_debug,(u8*)"pls repeat press!\r\n ",21);
								
								VoicePrompt(FINGERPRINT_AGAIN);
								
								if(repeat_times++>=3)
								{
									//voice ָ��δ�� ʶ��
									USART_transmitS(g_p_usart_debug,(u8*)"FP can not find!\r\n ",19);
									
									VoicePrompt(FP_FAI);
									
								  repeat_times=0;
									break;
								}
								else
								{
										//voice ���ذ�ָ��
										USART_transmitS(g_p_usart_debug,(u8*)"pls repeat press!\r\n ",21);
								
										VoicePrompt(FINGERPRINT_AGAIN);									
								}
							}
							else   //����״̬
							{
								USART_transmitS(g_p_usart_debug,(u8*)"wait pressing FP...\r\n ",22);
							}
							break;  //����count�Σ��˳�ˢָ��
						}
					}
			}
	
	return ret2;
}


u16 check_network_send(u16 loop_count)
{
	  u16 testid=0x55AA;	
    u16 network_wait_ms=0;

		//if(g_network_handshake.main_loop_count>=loop_count)
		{
			if(g_network_handshake.wait_recv_pending==0)  //begin checking
      {
 				 debug_printf("send 0x55AA to server\r\n");
					Send_Req_To_Server(g_pcom_ser,0x07,testid);

			    g_network_handshake.wait_recv_pending=1;
				
				  network_wait_ms=0;
			}
		}	
		//else
			//g_network_handshake.main_loop_count++;
		
  return 	network_wait_ms;
}

//return 0:mean check end

u8 check_network_recv(void)
{
    u16 network_wait_ms=0;

 	  u8 recv_result=1;   //0:means recv answer; 1:means no recv answer;
	  
	  //u16 testid=0x55AA;	
	
 			if(g_network_handshake.wait_recv_pending==1)  //in checking
			{		
				 
         recv_result = recv_network_handshake(g_pcom_ser,&network_wait_ms);
 				
				 if(recv_result==0) //checking OK; recv answer 0x55AA ok
				 {
					  
   				   debug_printf("NETWORK_OK=%d,network_wait_ms=%d ms\r\n",NETWORK_OK,network_wait_ms);
             g_network_status=NETWORK_OK;
						 network_handshake_init();
						 RX_Clear(g_pcom_ser);
					 return 0;
				 }
         else  //checking not end
				 {
					 
			      if(network_wait_ms>=MAX_NETWORK_WAIT_MS) //over time
			      {
						 
				      debug_printf("NETWORK_BREAK=%d,network break!! �����ж�\r\n",NETWORK_BREAK);
			        g_network_status=NETWORK_BREAK;
						  network_handshake_init();
						  RX_Clear(g_pcom_ser);
							return 0;
			      }
						else
						{
							;//continue checking
							 
						  g_network_handshake.wait_ms=network_wait_ms;
						}
					}	
			} 
 	
	return 	1;
}

u8 check_ServerCmd(void)
{
	u8  ensure=0xff;
	
/*  //
	if(g_network_handshake.wait_recv_pending==1) //in checking network, not recv server's cmd	
  {
	  check_network_recv();   //in checking network
	}
	else
	{
*/	
		if(g_pcom_ser->rx_state==2)  //have receiving a frame
		{
 		
			RecvCmdFromServer(g_pcom_ser,20);
			RX_Clear(g_pcom_ser);
		}
		else
		{
			if(g_pcom_ser->rx_state==0)
			{
				if(g_network_handshake.main_loop_count>=50)
				{
				  check_network_send(50);				   
					 					
					while(check_network_recv());
					
				}
				else
				{
					g_network_handshake.main_loop_count++;
				}
				
				//check_network_recv();
				
				return ensure;  //not receive any
			}
		}
/*		
	}
*/
		
	return ensure;	
}


//ˢָ��
//u16 press_FR(void)
u8 press_FR(COMM_USART *pcomm,SearchResult *pseach)
{
	//SearchResult seach;
	u8 ensure=0xff;

	while(1)
	{
		ensure=PS_GetImage(pcomm);

		if(ensure==0x00)//��ȡͼ��ɹ�
		{	
			ensure=PS_GenChar(pcomm,CharBuffer1);
			if(ensure==0x00) //���������ɹ�
			{
 				 
				//ensure=PS_Search(pcomm,CharBuffer1,0,1000,pseach);
				
				ensure=PS_HighSpeedSearch(pcomm,CharBuffer1,0,1000,pseach);		  
				
        break;
			}
			else
				debug_printf("after GetImage,not GetChar(),ensure=%d\r\n",ensure);
		}
  }
	return ensure;	
}

 
extern u8 g_fp_byte32[4][32];

u16 readFPFlash(COMM_USART *pcomm)
//u16 readFPFlash(u8 pbyte32[][])
{
	u8 ensure1 = 0xff;
	u8 ensure[4]={0xff};
	u8 b32=0x0;
	u16 idaddr=0x0;
 	u16 i,j,k;
	u16 t=0;
	
	ValidN=0x00;
  
 	delay_ms(20);
	
	ensure1=PS_ValidTempleteNum(pcomm,&ValidN);//����ָ�Ƹ���  //����00=��ʾ��ȡ�ɹ�
	
  if(ensure1==0) 
	  debug_printf("ok ��ȡģ������ɹ���ensure1=%d,��Чģ�����=%d\r\n ",ensure1,ValidN);
	else 
		debug_printf("failed,��ȡģ�����ʧ��! ensure1=%d\r\n",ensure1); 
  
	debug_printf("��ȡָ�ƿ��б�\r\n");
	
	//u8 PS_ReadFPFlash(u8 PageNum,u8 *Byte32)
	for(i=0;i<3;i++)
	{
     ensure[i]=PS_ReadFPFlash(pcomm,i,g_fp_byte32[i]);
	   if(ensure[i]==0x0)
		 {
		    //debug_printf("ok,��ȡָ�ƿ��б��%dҳ �ɹ���\r\n",i);
		    for(j=0;j<32;j++)
			  {
		      b32=g_fp_byte32[i][j];
					if(b32>0)
					{	
						//debug_printf("i=%d,j=%d,0x%x ",i,j,b32);											 
					  for(k=0;k<8;k++)
					  {
					     if(b32&0x01) 
							 {	 
								 idaddr=k+j*8+i*256;
								 debug_printf("%03d ",idaddr);
							 }
							 b32>>=1;
						}
            debug_printf("	");					
            if(++t==16)						
						{	
							t=0;
						  debug_print_nr();
						}	
					}	
               							 
		      //for(k=0;k<8;k++)
					//{
		          //if(b32&0x01){
								//debug_printf("%d ",k+j*32+i*256); 
								//b32<<=1;
							//}
					//}		
				}			
			}
	}		 
  debug_print_nr();
 	
	if(ensure1==0)
	   return ValidN;
	else
		 return 0;
}


//typedef struct
//{
	//u16 PS_max;//ָ���������
	//u8  PS_level;//��ȫ�ȼ�
	//u32 PS_addr;
	//u8  PS_size;//ͨѶ���ݰ���С
	//u8  PS_N;//�����ʻ���N
//}SysPara;

u16 readSysPara(COMM_USART *pcomm,SysPara *p)
{
	u8 ensure = 0xff;
 	
	//SysPara syspara;
 	
	//ensure=PS_ReadSysPara(&syspara);
	ensure=PS_ReadSysPara(pcomm,p);

	debug_print_nr();	
	
  if(ensure==0)  		
	  debug_printf("ok ��ȡϵͳ�����ɹ���ensure=%d\r\n ",ensure);
	else
		debug_printf("failed,��ȡϵͳ����ʧ��! ensure=%d\r\n",ensure);

	debug_printf("��ȡϵͳ������\r\n");
	debug_printf("ָ���������=%d , ",p->PS_max);
	debug_printf("��ȫ�ȼ�=%d , ",p->PS_level);
	debug_printf("��ַ=0x%x , ",p->PS_addr);
	debug_printf("���ݰ���С=");
	
	switch(p->PS_size)
	{
		case 0:
			g_fp_frame_data_len=32;
			debug_printf("32 , ");break;
		case 1:
			g_fp_frame_data_len=64;
			debug_printf("64 , ");break;
		case 2:
			g_fp_frame_data_len=128;
			debug_printf("128 , ");break;
		case 3:
			g_fp_frame_data_len=256;
			debug_printf("256 , ");break;
		default:
			debug_printf("������ %d , ",p->PS_size);
			break;
	}
	
	debug_printf("size����=%d , ",p->PS_size);
	debug_printf("������=%d , ",9600*(p->PS_N));
	
	debug_print_nr();	
 	
	if(ensure==0)
	   return ensure;
	else
		 return 0;
}

 
u16 upChar_FR(COMM_USART *pcomm,u8 *mbuf1,u8 *mbuf2)
{
	 u16 len1=0;
	 u16 len2=0;
	
	 //m1->id=1;
	 //m2->id=2;
	 
	 debug_printf("upchar CharBuffer1...\r\n"); 
	 memset(mbuf1,0x0,MODEL_LEN);  //512
	 len1=PS_UpChar(pcomm,1, mbuf1); 
	 //debug_printf("mbuf1\r\n");
  	//debug_print_buf_multi(mbuf1,len1,32);  //ÿ��32�У��ܹ�len1=��512��=16��
	
	 debug_printf("upchar CharBuffer2...\r\n");
	 memset(mbuf2,0x0,MODEL_LEN);
	 len2=PS_UpChar(pcomm,2, mbuf2);
	 //debug_printf("mbuf2\r\n"); 
	 //debug_print_buf_multi(mbuf2,len2,32);  
	
	 return len1+len2;
	 	 
}

u16 upChar_FR_1(COMM_USART *pcomm,u8 *mbuf1,u8 BufferID)
{
	 u16 len1=0;
	 //u16 len2=0;
	
	 //m1->id=1;
	 //m2->id=2;
	 
	 debug_printf("upChar_FR_1()/upchar CharBuffer1...\r\n"); 
	 memset(mbuf1,0x0,MODEL_LEN);  //512
	 len1=PS_UpChar(pcomm,BufferID, mbuf1); 
	 debug_printf("upChar_FR_1()/mbuf1\r\n");	
	 //debug_print_buf_multi(mbuf1,len1,32);  //ÿ��32�У��ܹ�len1=��512��=16��
	
 	
	 return len1;
	 	 
}

u16 downChar_FR(COMM_USART *pcomm,u8 *mbuf1,u8 *mbuf2)
{
	 u16 len1=0;
	 u16 len2=0;
	
	 //m1->id=1;
	 //m2->id=2;
	 
	 debug_printf("downchar to  CharBuffer1...\r\n");	  	  
	 //u16 PS_DownChar(u8 BufferID, u8 *downchar_buf, u16 down_len)
	 len1=PS_DownChar(pcomm,1, mbuf1,MODEL_LEN); 
	 debug_print_buf_multi(mbuf1,len1,32);  //ÿ��32�У��ܹ�len1��512��=16��
	
	 debug_printf("downchar to CharBuffer2...\r\n");	  
	 len2=PS_DownChar(pcomm,2, mbuf2,MODEL_LEN);
	 debug_print_buf_multi(mbuf2,len2,32);
	
	 return len1+len2;
}


//success , return MODEL_LEN(512)
//failed, return other value
u16 loadUpChar_FR(COMM_USART *pcomm,u16 addrid,u8 *mbuf1)
{
	 u16 len1=0;
	 u8 ensure=0xff;
	
	 //m1->id=1;
	 //m2->id=2;
	
	if(addrid>1000) addrid = 1000;
   //PS_LoadChar(u8 BufferID,u16 PageID)
	
	 debug_printf("Load Char from FP id=%d to CharBuffer1...\r\n",addrid); 
	
 	 ensure=PS_LoadChar(pcomm,1,addrid);
	
	 if(ensure==0)
	 {	 
		 debug_printf("Load Char ok! \r\n");
		 
		 debug_printf("Up Char from CharBuffer1 to mcu buf mbuf1=0x%x ...\r\n",mbuf1); 
		 //memset(mbuf1,0x0,MODEL_LEN);  //512
	   len1=PS_UpChar(pcomm,1, mbuf1); 
		 
	   if(len1==MODEL_LEN)
		   debug_printf("Up Char ok! uplen=%d\r\n",len1);
	   else
	     debug_printf("Up Char failed! uplen=%d\r\n",len1);

	   //debug_print_buf_multi(mbuf1,len1,32);  //ÿ��32�У��ܹ�len1��512��=16��
	 }
	 else
		 debug_printf("Load Char failed! ,ensure=0x%x\r\n",ensure);
	  	
	 return len1;
	
}

//MCU send data to server
u16 UpChar_ToSer(COMM_USART *pcomm_server,u16 addrid,u8 *upchar_buf,u16 up_len)
{
	u16 len2=0;
		//len2=PS_DownChar(g_pcom_ser,0, g_fp_model_buf1, MODEL_LEN);
	len2=PS_DownCharServer(pcomm_server,addrid,upchar_buf, up_len);  //up_len=MODEL_LEN
	
	return len2;
}

//upchar_buf maxlen=512 //MODEL_LEN
//return received len
//MCU receive forom server
u16 RecvChar_FromSer(COMM_USART *pcomm_server,u16 *p_storeid,u8 *downchar_buf,u16 waittime)
{
	u16 len2=0;
 
				    //USART_transmit(pcomm_server,0x55);
				    //USART_transmit(pcomm_server,0x55);
						//USART_transmit(pcomm_server,0xaa);
						//USART_transmit(pcomm_server,0xaa);
	
	 len2=PS_RecvCharFromServer(pcomm_server,p_storeid,downchar_buf,waittime);

	return len2;
}	

//upchar_buf maxlen=512*2 //MODEL_LEN
//return received len
//MCU receive forom server
u16 RecvChar2_FromSer(COMM_USART *pcomm_server,u16 *p_storeid,u8 *downchar_buf1,u8 *downchar_buf2,u16 waittime)
{
	u16 len1=0,len2=0;
  //u8 bufchar1_and2[MODEL_LEN*2]={0};
 
					  USART_transmit(pcomm_server,0x55);
					  USART_transmit(pcomm_server,0x55);
						USART_transmit(pcomm_server,0xaa);
						USART_transmit(pcomm_server,0xaa);

  	len1=PS_RecvCharFromServer(pcomm_server,p_storeid,downchar_buf1,waittime);
  	len2=PS_RecvCharFromServer(pcomm_server,p_storeid,downchar_buf2,waittime);
    debug_printf("after call PS_RecvCharFromServer(downchar_buf1),len1=%d\r\n",len1);
    debug_printf("after call PS_RecvCharFromServer(downchar_buf2),len2=%d\r\n",len2);
	  //memcpy(downchar_buf1,bufchar1_and2,MODEL_LEN);
	  //memcpy(downchar_buf2,&bufchar1_and2[MODEL_LEN],MODEL_LEN);
	
	return len1+len2;
}	

//success , return 0
//failed, return other value
u16 downStoreChar_FR(COMM_USART *pcomm,u8 *mbuf1,u16 addrid,u8 downBufferID)
{
	 u16 len1=0;
	 u8 ensure=0xff;
	 u8 ret_del=0xff;
 	 u16 ret=0xff; 
	
	 debug_printf("DownChar from mcu buf=0x%x to  CharBuffer1...\r\n",mbuf1);
	 //u16 PS_DownChar(u8 BufferID, u8 *downchar_buf, u16 down_len)
	
	 //memset(mbuf1,0x43,MODEL_LEN);
	
 	 len1=PS_DownChar(pcomm,downBufferID, mbuf1, MODEL_LEN); 
	 //debug_print_buf_multi(mbuf1,len1,32);   //ÿ��32�У��ܹ�len1��512��=16��

	 debug_printf("2.after transfer to CharBuffer1, PS_DownChar(), len1=%d \r\n",len1);
	
	  //return len1;
	 ret=len1;
	
	 if(len1==MODEL_LEN)
	 {
		  debug_printf("DownChar ok,total down len=%d\r\n",len1);
			
		  if(addrid>1000) addrid=1000;
      
			//				  ret = delChar_FR(pcom_fp,del_id,del_num);
		 
			ret_del = delChar_FR(pcomm,addrid,1);
		 
		  if(ret_del==0x00)
			{
			 
				debug_printf("id=%d has beed deleted!\r\n",addrid);	 
				
				debug_printf("Store from CharBuffer1 to FP id=%d ...\r\n",addrid);	 
		 
				ensure=PS_StoreChar(pcomm,downBufferID,addrid);
	   
				debug_printf("3.Store from CharBuffer1 to FP, PS_StoreChar(), ensure=%d \r\n",ensure);

				if(ensure==0x00)
				{	
						debug_printf("Store to FP id=%d ok!\r\n",addrid);
				    ret=0;
				}	
				else
					debug_printf("Store to FP id=%d failed! ensure=0x%x\r\n",addrid,ensure);		
			}
			
	 }
	 else
		 debug_printf("DownChar failed,total down len=%d\r\n",len1);
	 	  	  
	 //debug_print_buf_multi(charbuf2,len2,32);
	
	 return len1;	  
}


u16 downStore2Char_FR(COMM_USART *pcom,u8 *mbuf1,u8 *mbuf2,u16 addrid,u8 downBufferID)
{
	 u16 len1=0,len2=0;
	 u8 ensure=0xff;
	 u8 downBufferID2=0x0;
	
   debug_printf("2.transfer mbuf1 to CharBuffer1, mbuf2 to CharBuffer2\r\n");
 	 
	 //debug_printf("DownChar from mcu buf1=0x%x to  CharBuffer1...\r\n",mbuf1);
 	 len1=PS_DownChar(pcom,downBufferID, mbuf1, MODEL_LEN); 
	 //debug_print_buf_multi(mbuf1,len1,32);   //ÿ��32�У��ܹ�len1��512��=16��

	 debug_printf("DownChar from mcu buf2=0x%x to  CharBuffer2...\r\n",mbuf2);
	 if(downBufferID==1) 
		  downBufferID2=2;
	 else downBufferID2=1;
	
 	 len2=PS_DownChar(pcom,downBufferID2, mbuf1, MODEL_LEN); 

 
	  //debug_printf("2.transfer to CharBuffer1, PS_DownChar(), len1=%d \r\n",len1);
	
	  //return len1;
	  
	 if((len1+len2)==2*MODEL_LEN)
	 {	 
		  debug_printf("DownChar ok,total down len=len1+len2=%d\r\n",len1+len2);

 			
		  if(addrid>1000) addrid=1000;
	  	
		 
      ensure=PS_RegModel(pcom);

			debug_printf("3.call RegModel generate model in CharBuffer1 and CharBuffer2, ensure=%d \r\n",ensure);

			//debug_printf("Store from CharBuffer1 to FP id=%d ...\r\n",addrid);	 
			ensure=PS_StoreChar(pcom,downBufferID,addrid);
	   
 		  debug_printf("4.Store from CharBuffer1 to FP, PS_StoreChar(), ensure=%d \r\n",ensure);

		 if(ensure==0x00)
				debug_printf("Store to FP id=%d ok!\r\n",addrid);
			else
				debug_printf("Store to FP id=%d failed! ensure=0x%x\r\n",addrid,ensure);		 
	 }
	 else
		 debug_printf("DownChar failed,total down len=len1+len2=%d\r\n",len1+len2);
	 	  	  	
	 return (len1+len2);
}


//ɾ��ָ��
u16 delChar_FR(COMM_USART *pcomm,u16 del_page_id,u16 del_number)
{
	u16 ret=0xff;
	  ret = PS_DeletChar(pcomm,del_page_id,del_number);
	
	if(ret==0)
	   debug_printf("ɾ��ָ�Ƴɹ�,λ��id=%d,����=%d\r\n",	del_page_id, del_number);
	else
		debug_printf("ɾ��ָ��ʧ��,λ��id=%d,����=%d\r\n",	del_page_id, del_number);
	
	return ret;
}

/*
return 0: means ok
*/
u8 downStoreCharbuf(COMM_USART *pcomm, FP_SYNC *p_fp_down)
{
	  u8 ret=0xff;
	
		u16 len2=0;
	
	  u8 *p_buf1=p_fp_down->p_model_buf1;
	
	  u8 *p_buf2=p_fp_down->p_model_buf2;
	
    //u16 store_id=p_fp_down->fp_id;  
	
		//len2=downStoreChar_FR(g_pcom_fp,g_mcu_model_buf1,store_id,g_downStoreBufferID);
		len2=downStoreChar_FR(pcomm,p_fp_down->p_model_buf1,p_fp_down->fp_id,p_fp_down->downBufferID);

		if(len2==MODEL_LEN)									
		{	
							//debug_printf("���ص�ָ��ͷ�ɹ�,ok, store_id=%d, len2=%d\r\n",store_id,len2);
						  //debug_print_buf_multi(g_mcu_model_buf1,MODEL_LEN,32);  //ÿ��32�У��ܹ�len1��512��=16��			

				  //upChar_FR_1(g_pcom_fp,g_fp_model_buf1,g_downStoreBufferID);
				  upChar_FR_1(pcomm,p_fp_down->p_model_buf2,p_fp_down->downBufferID);
			    
			    p_buf1=p_fp_down->p_model_buf1;
			    p_buf1=p_fp_down->p_model_buf2;
			
			    //if(memcmp(&g_fp_model_buf1[8],&g_mcu_model_buf1[8],128)!=0)
			    if(memcmp(&p_buf1[8],&p_buf2[8],128)!=0)
					{	
							//g_ack_status=3;
 						  p_fp_down->down_store_result=3; //down_store failed
						  p_fp_down->ack_status=3;
						
						  debug_printf("memcmp()!=0 downChar failed!!!, store_id=%d, len2=%d\r\n",p_fp_down->fp_id,len2);
						
           		//delChar_FR(g_pcom_fp,store_id,1);
						  delChar_FR(pcomm, p_fp_down->fp_id, 1);
						
					}
					else
					{

							p_fp_down->down_store_result=0; //down_store failed
							p_fp_down->ack_status=0;
						
							debug_printf("memcmp()=ok,���ص�ָ���ǳɹ�,ok, store_id=%d, len2=%d\r\n",p_fp_down->fp_id,len2);
						
					}	 
 
		}	
		else
		{
	 			  p_fp_down->down_store_result=4; //down_store failed
				  p_fp_down->ack_status=4;
			
					debug_printf("���ص�ָ����ʧ��,failed!!!, store_id=%d, len2=%d\r\n",p_fp_down->fp_id,len2);
       		//delChar_FR(g_pcom_fp,store_id,1);
			
			    delChar_FR(pcomm, p_fp_down->fp_id,1);
			  		
		}

		ret = p_fp_down->ack_status;
		
		return ret;
}

