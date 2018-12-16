
/*
 *  ����ͷ�ļ�
 */
  
#include "stm32f10x.h"

#include "voice.h"
#include "delay.h"
#include "timer2.h"
#include "timer3.h"
#include "timer4.h"

#include "stdarg.h"	 
#include "stdio.h"	 
#include "string.h"	 

#include "global.h"
 
#include "USARTx.H"
#include "debug_print.h"


#include "hc595.h"
#include "rfid.h"
#include "as606.h"
#include "fp_func.h"
#include "server.h"
#include "barrier.h"

#include "MyVirtualUart.h"

/*
 *  ȫ�ֱ���
 */
 

COMM_USART g_usart1;
COMM_USART g_usart2;
COMM_USART g_usart_spi;
COMM_USART g_usart3;
COMM_USART g_usartvirt;     //virtual usart, realize by timer && gpio

COMM_USART *g_pcom_fp        = &g_usart1;        // define USART1 used for finger print ;     default 57600
COMM_USART *g_pcom_ser       = &g_usart2;        // define USART2 used for server comm  ;    default 115200
//COMM_USART *g_pcom_ser       = &g_usart_spi;
COMM_USART *g_pcom_rfid      = &g_usart3;        // define USART2 used for rfid_label comm;   default 57600
COMM_USART *g_pcom_virt      = &g_usartvirt;
            
COMM_USART *g_p_usart_debug  = &g_usart3;        
COMM_USART *g_p_debug_recv  = &g_usart3;

//COMM_USART *g_p_usart_debug  = &g_usartvirt;
//COMM_USART *g_p_debug_recv  = &g_usartvirt;

//COMM_USART *g_p_usart_debug  = &g_usartvirt;
//COMM_USART *g_p_debug_recv  = &g_usart2;

GUN_CHECKING g_req_check_gun;
GUN_CHECKING g_real_check_gun;

BARRIER_CHECKING_FLOW g_barrier_check_flow;

BARRIER_CHECKING_FLOW *g_p_flow=&g_barrier_check_flow;

FP_SYNC g_fp_sync;

NETWORK_HANDSHAKE g_network_handshake;
u8 g_network_status=0;

u8   g_checksum_status=0;                   //checksum status, 0==checksum ok, others=checksum fail
u8   g_ack_status=0xff;                     //response for server's CMD, controller ACK executing result to server, 00---success.

//vu32 tim4_ticks=0;
vu16 tim4_ticks=0;
vu16 last_tim4_ticks=0;
vu16 tim4_pass=0;

u16          g_fp_frame_data_len=128;       //FP sensor default frame len=128 bytes

u8           g_fp_byte32[4][32]={0};        //read FP LIB list ,each bit stands for one id addr from 0 ~ 1023 ,4x256 areas
SearchResult g_fp_seach={0,0};              //FP press result,id,mathscore
SysPara      g_fp_syspara={0,0,0,0,0};      //FP system paras

u8   g_downStoreBufferID=1;                 //FP CharBuffer ID ,only use 1 and 2
u16  g_fp_id=0;

u8  g_fp_model_buf1[MODEL_LEN]={0};  //CharBuffer1 to mcu  
u8  g_fp_model_buf2[MODEL_LEN]={0};  //CharBuffer2 to mcu  

u8 g_mcu_model_buf1[MODEL_LEN]={0};  //mcu to CharBuffer1 
u8 g_mcu_model_buf2[MODEL_LEN]={0};  //mcu to CharBuffer2 


u16  g_debug_detail=0;                      //temp debug use��if==1��output the detail logs��normal is 0


u8 fp_buf_test[512]={
0x03, 0x01, 0x51, 0x18, 0x7d, 0x00, 0xff, 0xfe, 0xff, 0xfe, 0xfd, 0xfe, 0xf0, 0x1e, 0xf0, 0x0e, 0xf0, 0x06, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0xc0, 0x02, 0xc0, 0x00,
0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x02, 0xe0, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x18, 0x9c, 0x96, 0x69, 0x9a, 0x63, 0x5e,
0x5a, 0x1e, 0x61, 0x1e, 0x14, 0xa9, 0xde, 0x7e, 0x50, 0x30, 0x27, 0x7e, 0x21, 0x3b, 0x44, 0xbe, 0x3f, 0x99, 0x48, 0xdf, 0x5e, 0x1a, 0xa0, 0xdf, 0x46, 0x1f, 0x1f, 0x5f, 0x66, 0x27, 0xa4, 0xdf,
0x54, 0xa9, 0x25, 0x3f, 0x29, 0xaa, 0x45, 0xdf, 0x61, 0x36, 0x28, 0x7f, 0x55, 0xbe, 0x6a, 0x7f, 0x45, 0xb4, 0x18, 0x3c, 0x53, 0xa4, 0xa2, 0x7d, 0x40, 0xa2, 0xdf, 0x3a, 0x6c, 0xba, 0x13, 0x3a,
0x6e, 0xbc, 0x11, 0xbb, 0x3c, 0xad, 0x00, 0xb3, 0x39, 0x27, 0xde, 0x16, 0x44, 0x30, 0x69, 0x37, 0x43, 0x2e, 0x99, 0x94, 0x4a, 0x27, 0x62, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0x01, 0x50, 0x21, 0x79, 0x00, 0xff, 0xfe, 0xff, 0xfe, 0xf8, 0x1e, 0xf0, 0x1e, 0xf0, 0x0e, 0xc0, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00,
0x80, 0x00, 0x80, 0x00, 0x80, 0x02, 0xc0, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x18, 0xdc, 0xfe, 0x69, 0x9a, 0x23, 0xfe,
0x11, 0x23, 0x87, 0x7e, 0x14, 0x29, 0x9e, 0xde, 0x3f, 0x99, 0x48, 0xdf, 0x5b, 0x9e, 0xe1, 0x3f, 0x46, 0x9f, 0x1f, 0x1f, 0x53, 0xa4, 0xa2, 0x7f, 0x66, 0x27, 0xe4, 0xbf, 0x39, 0xa9, 0x1d, 0x5f,
0x29, 0x2a, 0x05, 0xdf, 0x47, 0x33, 0x17, 0xbf, 0x21, 0x3a, 0x44, 0x7f, 0x55, 0x3e, 0x6a, 0x9f, 0x38, 0x0e, 0x05, 0xfc, 0x3e, 0x2d, 0x6c, 0xbd, 0x30, 0x0c, 0x06, 0x1a, 0x4e, 0x0c, 0xc6, 0x3a,
0x2e, 0x0f, 0xda, 0x92, 0x5c, 0x12, 0xc9, 0x92, 0x60, 0x19, 0xe0, 0x9a, 0x72, 0xa7, 0x8d, 0x92, 0x71, 0x29, 0xe4, 0x12, 0x4f, 0xb0, 0xa7, 0x7a, 0x55, 0x29, 0x25, 0x1b, 0x62, 0x95, 0x20, 0xb2,
0x5d, 0x95, 0xe0, 0x12, 0x6d, 0x3a, 0x0f, 0xb8, 0x42, 0xa3, 0x9f, 0x99, 0x65, 0xba, 0x92, 0xf9, 0x62, 0xb8, 0x28, 0x97, 0x65, 0xbf, 0x13, 0x57, 0x45, 0x2a, 0xda, 0x0f, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

u8 rfid_test_label_buf[RFID_LABEL_LEN]={0};

void initial_myself(void);
void initial_peripheral(void);
u16  test_progrom(void);

u8 Main_Working(void)
{
	u8 ret=0;
	u8 ret_fp=0xff;
	u8 ret_server=0xff;
	u8 ret_key=0x0;
 	u8 breakchar=0x0;
	
	while(1)
	{
		//
		//g_fp_seach.pageID=26;
		//ret_fp=check_FP_Pass(&g_fp_seach);  //lynn_debug
    //
		
		ret_fp=check_press_FR(); //formal
		
		//ret_server=check_ServerCmd();
		
		if(g_p_debug_recv->rx_num>0)   //press 'z' or 'Z' key ,goto test_progrom();
		{
			
			breakchar=g_p_debug_recv->rx_buf[g_p_debug_recv->rx_num-1];
			
			//debug_printf("g_p_usart_debug->rx_num=%d,breakchar=0x%x=%c\r\n",g_p_debug_recv->rx_num,breakchar,breakchar);
			
		  if(breakchar=='z'||breakchar=='Z')
			{
				RX_Clear(g_p_debug_recv);
			  test_progrom();
				RX_Clear(g_p_debug_recv);
			}
		}
    
				 
		 ret_server=check_ServerCmd();
		
  }
	
	return ret;
}


u16  test_progrom(void)
{ 
	u16 i=0,j=0;
	u16 ret=0xff;
	u16 ret1=0xff;
	u16 ret2=0xff;
	u16 len=0;//,len1=0,len2=0;
	u16 count=0;
	u16 del_id =0x0;
	u16 del_num=0x0;
	u16 load_id =0x0;
	u16 store_id =0x0;
	
	//u8 rfid_test_label_buf[RFID_LABEL_LEN]={0};
	
	u8 input_buf[32]={0};	 
	
	u8 ch_func=0xff;   //choice function, 1: Add_FR(inid); 2:press_FR(); 
	
  u16 inid=0;        //Add_FR store addr , 0x0 to 0xf   
	//���Գ���
  u8 waittime_left=0;
	u8 waittime_left2=0;
 	
	while(1)
 			{	
 
 				debug_print_nr();
				//debug_printf("g_p_debug_recv->rx_buf[0]=%c",g_p_debug_recv->rx_buf[0]);
				
				debug_printf("��Ԫ���ԣ���ѡ����: \r\n");				 
				debug_printf("1> ¼ָ�� \r\n");
				debug_printf("2> ��ָ֤�� \r\n");
				debug_printf("3> ��ȡָ�ƿ��б� \r\n");
				debug_printf("4> ��ȡϵͳ���� \r\n");
				debug_printf("5> ɾ��ָ��ģ��(512) \r\n");
				//debug_printf("5> �ϴ�ģ��(CharBuffer1,2 on FP to buf on MCU /512) \r\n");
				//debug_printf("6> ����ģ��(buf on MCU to CharBuffer1,2 on FP /512) \r\n");
				debug_printf("6> �ϴ�ָ��(from FP_lib id to MCU g_fp_model_buf1/512) \r\n");
				debug_printf("7> ����ָ��(from MCU g_fp_model_buf1 to FP_lib id /512) \r\n");	
 
				debug_printf("8> �ϴ�ָ�Ƶ�������(from FP_lib id to Server 512) \r\n");
				debug_printf("9> �ӷ���������ָ��(from Server to FP_lib id /512) \r\n");	
				debug_printf("a> �鿴ָ��ͷCharBuffer1,CharBuffer2 \r\n");	

				debug_printf("h> rfid����ǩ \r\n");	
				debug_printf("i> �������� \r\n");	
				debug_printf("j> ����բ \r\n");	
				debug_printf("k> flash��д \r\n");	
				//debug_printf("m> m \r\n");	
				//debug_printf("n> n \r\n");	
 				
				debug_print_nr();
				memset(input_buf,0,16);	
 				count=0;	
        g_ack_status=0;				
 				
				ch_func=debug_getch(input_buf);   
				RX_Clear(g_p_debug_recv);	 	

				//ch_func='h';
				//ch_func='2';
 				
        switch(ch_func)
					{
					  case 0x31: 		// '1'											 
							{									 
								count = 0;
 								debug_printf("¼ָ�� Add_FR>\r\n ");								
								debug_printf("������洢idλ>\r\n");
	 							memset(input_buf,0,16);	
								inid=(u16)debug_getid(input_buf);
								debug_printf("�洢idλ inid=0x%x=%03d \r\n",inid,inid);
								 
 								while(1)
								{	
  							  debug_printf("�뽫��ָ�ŵ�ָ������ .... \r\n ");
									ret1=Add_FR(g_pcom_fp,inid);
									if(ret1==0x0)
									{	
											debug_printf("���ָ�Ƴɹ�,�洢��ָ�ƿ��ַid=%03d\r\n ",inid);
											break;					
									}
									else
									{	
										count++;
									  if(count>5) {count =0 ;break;}
									}	
							  }
								//else
									//count++;
						  }
							break;
						case 0x32:	 //'2'
							count=0;
						 while(1)
						 {
							  debug_printf("��ָ֤�� press_FR :�뽫��ָ�ŵ�ָ������ .... \r\n ");
								ret2=press_FR(g_pcom_fp,&g_fp_seach);
								count++;
								if(ret2==0x0)
								{
									debug_printf("ok ��ָ֤�Ƴɹ�, λ��id=%03d,�÷�=%d\r\n ",g_fp_seach.pageID,g_fp_seach.mathscore);
									break;
								}
								else
								{
									debug_printf("������ָ֤�� ...��\r\n");
									if(count==7)
									{										
										count=0;
										debug_printf("δ�ҵ���Ӧָ�ƣ���\r\n ");
										break;
									}
								}
							}
							break;
						case 0x33:
							ret=readFPFlash(g_pcom_fp);  //use global var g_fp_byte32[][]
						  break;
						case 0x34:
							ret=readSysPara(g_pcom_fp,&g_fp_syspara);
						  break;
						case 0x35:   
								debug_printf("�����뽫ɾ���Ĵ洢idλ>\r\n");
	 							memset(input_buf,0,16);	 
								del_id=(u16)debug_getid(input_buf);
								debug_printf("��ɾ���Ĵ洢idλ del_id=0x%x=%03d \r\n",del_id,del_id);
						    debug_print_nr(); 
								debug_printf("�����뽫ɾ����ģ�����>\r\n");
	 							memset(input_buf,0,16);	
								del_num=(u16)debug_getid(input_buf);
								debug_printf("��ɾ����ָ��ģ����� del_num=0x%x=%03d \r\n",del_num,del_num);
						    debug_print_nr();
							  ret = delChar_FR(g_pcom_fp,del_id,del_num);
                						    
						    break;
						
 						case 0x36:  //load and up from FP lib addrid to mbuf1
							
								//success , return MODEL_LEN(512)
								//failed, return other value
								//u16 loadUpChar_FR(u16 addrid,u8 *mbuf1);
                 
							  debug_printf("�����뽫�ϴ���ָ�ƿ�idλ>\r\n");
	 							memset(input_buf,0,16);	 						    
								load_id=(u16)debug_getid(input_buf);
								debug_printf("���ϴ���ָ�ƿ�idλ load_id=0x%x=%03d \r\n",load_id,load_id);
						    debug_print_nr(); 
						
								memset(g_fp_model_buf1,0x0,MODEL_LEN);  //512

								len=loadUpChar_FR(g_pcom_fp,load_id,g_fp_model_buf1);
						
								if(len==MODEL_LEN)									
									debug_printf("�ϴ��ɹ�,ok, load_id=%d, len=%d\r\n",load_id,len);
								else
									debug_printf("�ϴ�ʧ��,failed!!!, load_id=%d, len=%d\r\n",load_id,len);
								
						   	debug_print_buf_multi(g_fp_model_buf1,len,32);  //ÿ��32�У��ܹ�len1��512��=16��
								debug_print_nr();
								
  							
						  break;						
 						case 0x37:   //dron and store from mbuf1 to FP lib addrid 
							
								//success , return MODEL_LEN(512)
								//failed, return other value
								//u16 downStoreChar_FR(u8 *mbuf1,u16 addrid)						

							  debug_printf("�����뽫���ص���ָ�ƿ�Ŀ��idλ>\r\n");
	 							memset(input_buf,0,16);	 
								store_id=(u16)debug_getid(input_buf);
								debug_printf("�����ص�ָ�ƿ�Ŀ��idλ load_id=0x%x=%03d \r\n",store_id,store_id);
						    debug_print_nr(); 
 
								len=downStoreChar_FR(g_pcom_fp,g_fp_model_buf1,store_id,1);

								if(len==MODEL_LEN)
									debug_printf("���سɹ�,ok, load_id=%d, len=%d\r\n",store_id,len);
								else
									debug_printf("����ʧ��,failed!!!, load_id=%d, len=%d\r\n",store_id,len);

						    debug_print_buf_multi(g_fp_model_buf1,len,32);  //ÿ��32�У��ܹ�len1��512��=16��

						  break;
 						case 0x38:   //�ϴ�ָ�Ƶ�������(from FP_lib id to Server 512)
 							
							  break;
								
						case 0x39:  //�ӷ���������ָ��(from Server to FP_lib id /512) 
 	           

								RecvCmdFromServer(g_pcom_ser,500);
								RX_Clear(g_pcom_ser);
						
								//u16 RecvCmdFromServer(COMM_USART *pcomm,u16 waittime)
						    break;
						case 'a':  //0x61 //upload CharBuffer1,CharBuffer2 on sensor to g_fp_model_buf1,g_fp_model_buf2 on MCU
						case 'A':  //0x41	
							 
						  ret = upChar_FR(g_pcom_fp,g_fp_model_buf1,g_fp_model_buf2);
						  
              debug_printf("g_fp_model_buf1[]\r\n");							
							debug_print_buf_multi(g_fp_model_buf1,MODEL_LEN,32);  //ÿ��32�У��ܹ�len1��512��=16��							
							debug_print_nr();
							debug_printf("g_fp_model_buf2[]\r\n");
							debug_print_buf_multi(g_fp_model_buf2,MODEL_LEN,32);  //ÿ��32�У��ܹ�len1��512��=16��
							debug_print_nr();
              break;						

            case 'h':	//0x68
            case 'H': //0x48
 						  waittime_left=0;
						
						  for(i=0;i<15;i++)
						  { 
						     //voice ����ǹ
								 VoicePrompt(CHECK_GUN);								
								
						     ret=Rfid_Read_Label_Base_Proc(g_pcom_rfid,rfid_test_label_buf,&waittime_left);
 						     
						  }
              break;		
						case 'i':  //0x69
						case 'I':  //0x49
								debug_printf("i> �������� \r\n");	

								for(i=COME_IN;i<=CANNOT_PASS;i++)
						    {
										VoicePrompt(i);	
						        delay_ms(1000);
									  delay_ms(1000);
                }
                 VoicePrompt(GUNPASS);
						
 						    break;
            case 'j':  //0x6A
						case 'J':	 //0x4A				
								  debug_printf("j> ����բ \r\n");	

									Flow_Init(g_p_flow);
									g_p_flow->type=FLOW_DIRECTLY_OPEN;
									g_p_flow->p_real_check_gun->gun_num=0;
									Open_Barrier(g_p_flow);
								
						    break;
						case 'k':  //0x6B
						case 'K':  //0x4B							
				//debug_printf("k> flash��д \r\n");	
						    break;
 						default:
							break;
           }
				
 				 count=0;
			 }	
			
				//1.ˢָ��
				//2.ɨ��RFID
				//3.ˢָ�ƽ����ɨ��RFID�������������
				//4.���շ�����Ӧ��,�����̵���������
				//5.���շ�����ͬ��ָ�ƿ�����				 
				//
				 			
			//while(1);
 return 0;
}

/******************************************************
 *  �������ƣ�main
 *  �������ܣ�������
 *  ��ڲ�����
 *  ���ڲ�����
 ******************************************************/

int main()
{
	u16 ret=0;
	static u16 i=0;
	//static unsigned char debugchar[1025];
	static u8 debugchar[256];
	//static int usart2_recvd_len=0;
 	
  initial_myself();   
	delay_ms(50);
	initial_peripheral();  
	delay_ms(50);	
  
	  BarrierGate_Close();
	
		last_tim4_ticks=tim4_ticks;		
		TIM4_Int_Init(99,7199);		//10ms Interrupt    
    TIM_Cmd(TIM4, ENABLE); 		 		

 		virtual_uart_config();
  
    network_handshake_init();

			USART_transmit(g_pcom_ser,0x55);
			USART_transmit(g_pcom_ser,0x55);
			USART_transmit(g_pcom_ser,0xaa);
			USART_transmit(g_pcom_ser,0xaa);

	  g_network_handshake.main_loop_count=10;
	  check_network_send(10);
		
		while(check_network_recv());
		
  	g_p_flow=&g_barrier_check_flow;  //
	
	while(1)
	{
//	  SendData(0x0a);
//	  SendData(0x00);
//	  SendData(0x01);
//	  SendData(0x0b);
//		
//		HC595SendData(Led6_on);
 		
		HC595_IO_Out();    

		//		HC595_IO_Out(Beep_on);
//		
//		HC595_IO_Out(Led6_on);
//		USART2_transmit(0x55);
//		USART2_transmit(0xaa);
		
		delay_ms(1000);
		
		HC595SendData(Led6_off);

		  //HC595_IO_Out(Led6_off);   

//		GPIO_SetBits(GPIOC,GPIO_Pin_15);
//		GPIO_SetBits(GPIOC,GPIO_Pin_14);
//		GPIO_SetBits(GPIOC,GPIO_Pin_13);

//    HC595_IO_Out(Beep_off);

		  delay_ms(100);

	    memset(debugchar,0,256);
			while(i<6) debugchar[i++]=0x0;
			debugchar[i-1]=0x0;

			USART_transmitS(&g_usart1,debugchar,16);
			delay_ms(100);

	 	  debug_print_nr();
			debug_print_nr();
 	    debug_printf("********************\r\n ");
	    debug_printf("BarrierGate init ok!\r\n ");
	    debug_printf("********************\r\n ");
	 		debug_print_nr();

  	  ret=readSysPara(g_pcom_fp,&g_fp_syspara);

 
      rfid_init();						
			

      //test_virt_progom();
			
      FpSync_Init(&g_fp_sync);
			
      Flow_Init(g_p_flow);
 			 			
			Main_Working();
			
      //test_progrom();
        
  			
				//1.ˢָ��
				//2.ɨ��RFID
				//3.ˢָ�ƽ����ɨ��RFID�������������
				//4.���շ�����Ӧ��,�����̵���������
				//5.���շ�����ͬ��ָ�ƿ�����				 
				//
			
	}
}


 /******************************************************
 * �������ƣ�initial_myself
 * �������ܣ��ڲ���ʼ������
 * ��ڲ�����void
 * ���ڲ�����
*******************************************************/  
void initial_myself(void)
{	
	
	delay_init();	    //��ʱ������ʼ��	

	uart1_init(57600);
	uart2_init(115200); 	     
	uart3_init(57600);
	
	uartvirt_init();
 	
	rfid_init();						

	 
  VOICE_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	TIM2_Int_Init(9,7199);   // 1ms ��ʱ�жϺ���
	
	
  HC595_Init_RelayPart();
	//BEEP_Init();
//	KEY_Init();

 
}
/************ ******************************************
 * �������ƣ�initial_peripheral
 * �������ܣ��ⲿ��ʼ������
 * ��ڲ�����void
 * ���ڲ�����
*******************************************************/ 
void initial_peripheral(void)
{
  SendVoiceData(0x0a);   // ��������Ϊ  96 ��ÿ�ο�����Ҫ����һ��
	SendVoiceData(0x06);
	SendVoiceData(0x00);
	SendVoiceData(0x0c);
}
