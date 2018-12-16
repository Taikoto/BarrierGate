#include <string.h>
#include "delay.h" 	
#include "USARTx.H"
#include "debug_print.h"
#include "comm_recv.h"
#include "server.h"
#include "barrier.h"
#include "fp_func.h"
#include "check_gun.h"

u32 AS606_SerAddr = 0XFFFFFFFF; //默认

//static COMM_USART *p_fp_comm = &g_usart2;   // define USART1 used for finger print

//发送包头

static void SendHeadServer(COMM_USART *pcomm)
{
	//USART1_transmit(0xEF);
	//USART1_transmit(0x02);
 	USART_transmit(pcomm,0xEF);
	USART_transmit(pcomm,0x02);	
}

//发送地址
static void SendAS606_SerAddr(COMM_USART *pcomm)
{
	//USART1_transmit(AS606Addr>>24);
	//USART1_transmit(AS606Addr>>16);
	//USART1_transmit(AS606Addr>>8);
	//USART1_transmit(AS606Addr);
	USART_transmit(pcomm,AS606_SerAddr>>24);
	USART_transmit(pcomm,AS606_SerAddr>>16);
	USART_transmit(pcomm,AS606_SerAddr>>8);
	USART_transmit(pcomm,AS606_SerAddr);
	
}


//判断中断接收的数组有没有应答包
//waittime为等待中断接收数据的时间（单位1ms）
//返回值：数据包首地址

u8  AS60X_HEAD_SERVER[7]        ={0xef,0x02,0xff,0xff,0xff,0xff,0x0};      
u8  AS60X_ACK_CMD_SERVER[8]     ={0xef,0x02,0xff,0xff,0xff,0xff,0x07,0x0}; 
u8  AS60X_HEAD_SERVER_CMD[8]    ={0xef,0x02,0xff,0xff,0xff,0xff,0x01,0x00};
u8  AS60X_NETWORK_HANDSHAKE[13]  ={0xef,0x02,0xff,0xff,0xff,0xff,0x07,0x00,0x05,0x07,0x55,0xAA,0x0}; 
//ef02ffffffff0700050755AA00

void network_handshake_init(void)
{
	g_network_handshake.main_loop_count=0;
	g_network_handshake.wait_ms=0;
	g_network_handshake.max_wait_ms=MAX_NETWORK_WAIT_MS;
	g_network_handshake.wait_recv_pending=0;   //0:means no pending, 1:means pending receive
	g_network_handshake.network_status=0;      //0:means ok, 1:means break
	
}

//
//
//----------------------------------------------------------------------
//检测网络连接状态，
//控制器发送：
//byte[6]=03, byte[9]=07 ,byte[10]=0x55, byte[11]=0xAA;
//0 1  2 3 4 5     6     7 8     9     10 11       12 13
//EF02 FFFFFFFF   03    0005    07     55 AA     checksum
//------------------------------------------------------------
//服务器得到请求后回应同样的包
//
//0 1  2 3 4 5  6   7 8   9  10 11    12 13    
//EF02 FFFFFFFF 07  0005 07  55 AA    checksum 
//
//
//u16 network_handshake(COMM_USART *pcomm,u16 max_round_ms)
//return 0: means recv OK;
//return others: means no recv answer;
u16 recv_network_handshake(COMM_USART *pcomm,u16 *p_wait_ms)
{
	 
	u8  *data=0;
	u16 spend_ms=0;
	u8  ensure=0;
	u16 ack_testid=0x0;
	
	//Send_Req_To_Server(pcomm,0x07,testid);

  data=JudgeStr_fromSer_nowait(pcomm,AS60X_NETWORK_HANDSHAKE,&spend_ms);	//等待服务器回应, 
	
  //data=JudgeStr_fromSer_nowait(pcomm,AS60X_HEAD_SERVER,&spend_ms);	//等待服务器回应, 


	if(data) //得到回复 ,展开下面的流程
	{
		 ensure=data[9];		
		
		 if(ensure==0x07)
		 {
			 ack_testid=((data[10])<<8) + data[11];
			 
		   if((ack_testid)==0x55AA)
			 {	 
				  if(spend_ms<MAX_NETWORK_WAIT_MS)
					{
						debug_printf("newwork is in connected,spend_ms=%d\r\n",spend_ms);
						
						g_network_status=0;
						
						*p_wait_ms=spend_ms;
						
					  return 0;
					}					
			 }
		 }
	}
	
	no_recv:
	//未得到服务器回复，退出
	{
			spend_ms=g_network_handshake.wait_ms;
		
		  *p_wait_ms=spend_ms;
   }	

	return 1;
}

//byte[6]
// 0x01 : server command
// 0x03 : controler request to server
// 0x07 : answer
 
//byte[9]
//byte[9]=0x08 : byte[6]=0x01 , server cmd to sync FP charter
//byte[9]=0x0A : byte[6]=any  , open barrier directly
//byte[9]=0x0C : byte[6]=0x07 , need check gun
//byte[9]=0x09 : byte[6]=0x07 , forbidden

//管理员直接开门(最高优先级开门), byte[9]=0x0A 
//0 1  2 3 4 5  6   7 8   9    10 11 12 13   14 15    
//EF02 FFFFFFFF 07  07    0A    openid*4     checksum 
//
//controller ack:
//0 1  2 3 4 5  6   7 8   9     10 11 12 13    14        15 16  
//EF02 FFFFFFFF 07  08    0A      openid*4    status*1   checksum
//
//---------------------------------------------------------------------------
//验指纹已过，请求服务器是否需要验枪，是否放行
//
//0 1  2 3 4 5     6     7 8     9     10 11       12 13
//EF02 FFFFFFFF   03    0005    09    id=0x0030    checksum
//------------------------------------
//[2] 服务器回复=命令 byte[6]=07  
//byte[9]=0x08 : byte[6]=0x01 , server cmd to sync FP charter
//byte[9]=0x0A : byte[6]=any  , open barrier directly
//byte[9]=0x0C : byte[6]=0x07 , need check gun
//byte[9]=0x09 : byte[6]=0x07 , forbidden
//
//--------------
//2.1) 指纹id不可以用, byte[9]=0x09 , status=0x02 , 表示该指纹id禁用  
//0 1  2 3 4 5  6    7 8     9    10 11       12               13  14 
//EF02 FFFFFFFF 07    06    09   fpid=0x0030  status=0x02     checksum
//
//procedure end , controller no need ACK
//
//----------------
//2.2) 直接开门, byte[9]=0x0E 
//0 1  2 3 4 5  6   7 8   9  10 11  12 13 14 15    16 17    
//EF02 FFFFFFFF 07  09   0E   fpid   openid*4     checksum 
//
//
//controller ack:
//0 1  2 3 4 5  6   7 8   9  10 11   12 13 14 15    16        17 18  
//EF02 FFFFFFFF 07  0A    0E  fpid    openid*4    status*1   checksum
//
//--------------
//2.3) 需验枪, byte[9]=0x0C    
//
//0 1  2 3 4 5  6    7 8            9    10 11     12       13~13+12-1=24     25~25+12-1=36  ...  13+gun_num*12  
//EF02 FFFFFFFF 07  len=6+num*12    0C   fpid*2   gun_num     gun1                gun2             checksum      
//
//controller ack:
//0 1  2 3 4 5  6   7 8   9    10 11      12        13 14    
//EF02 FFFFFFFF 07  06    0C   fpid*2   status*1   checksum  
//
//
//return : pflow->type
//
u8 requestServer_byFP(u16 fpid)
{
	u8  ret=0xff;
  u8  ensure=0xff;

	u16 temp=0x0;
	//u16 frame_checksum=0x0;

	u8  *data=0;
	u16 answer_id=0xffff;
	
	u8 frame_type=0x00;
	
	u8  cmd=0xff;
	
	u8 retry=0;
	
	u8 gun_num=0;
	
	//u8 open=BARRIER_CLOSE;
	
	u8 i=0,j=0;
	
  COMM_USART *pcomm=g_pcom_ser;    
	
  BARRIER_CHECKING_FLOW *pflow=g_p_flow;
	
	pflow->ack_status = 0xff;
	
 	//send request to server
	
  	RX_Clear(pcomm);  
 		
		Send_Req_To_Server(pcomm,0x09,fpid);

    while(retry++<3)
		{
				//wait for server answer
				delay_ms(50);	
		    
				g_debug_detail=0; 	
			  
				//data=JudgeStr(pcomm,500,AS60X_ACK_CMD_SERVER);	

			  if(g_network_status==NETWORK_OK)
				{
			     data=JudgeStr_fromSer(pcomm,400,AS60X_HEAD_SERVER);	//等待服务器回应, 等8 second, 50*20ms=1secs,60sec=1min, 400*20ms=8000ms=8sec
				}
				else
					data=0;
			  
				g_debug_detail=0; 
			 	
				if(data) //得到回复 ,展开下面的流程
				{
				   ensure=data[9];					
				}
				else  //未得到服务器回复，退出
				{
					ensure=0xff;
					debug_printf("JudgeStr_fromSer() return data=null!\r\n");					
          
					if(fpid<=5)  //网络不通，凭管理员指纹开门
					{
							pflow->type=FLOW_NETWORK_BREAK_OPEN;  //网络不通，管理员指纹，开门
							
						  pflow->cmd=0x0B;
						
						  pflow->fp_id=fpid;

						  pflow->fp_check_result=0;

						  pflow->open_id = 0xffffffff;

					    pflow->p_real_check_gun->gun_num=0;
					  					
							pflow->ack_status=Open_Barrier(pflow);
							
							// after break, barrier_status=BARRIER_OPEN, driver open relay				       
						  
						  ret=pflow->type;
						
						  //while(1)	
							{
                	RX_Clear(pcomm);  								
							    
								  Send_Req_To_Server(pcomm,pflow->cmd,fpid);
							  
								  delay_ms(100);
								
									Ack_To_Server(pcomm,pflow->cmd,pflow->fp_id,pflow->ack_status);
								
							}
					}
          goto end_exit;
				}	
	      
				frame_type=data[6];
				
				cmd=data[9];

			  pflow->cmd=cmd;
				
				if(cmd==0x0A) //最高优先级开门
				{
						   
 						  pflow->type=FLOW_SUPERVISIOR_OPEN;  //supervisior open
					     					 
						  pflow->fp_id=fpid;

							pflow->fp_check_result=0;

					    pflow->p_real_check_gun->gun_num=0;
					
						  pflow->open_id = ((data[10])<<24) + ((data[11])<<16) + ((data[12])<<8) + data[13] ;

						  pflow->fp_check_result=0;
						 							 
							pflow->ack_status=Open_Barrier(pflow);
						  
						  ret=pflow->type;
						  
						  Ack_To_Server_OpenId(g_pcom_ser,pflow->cmd,pflow->fp_id,pflow->open_id,pflow->ack_status);
					
					   	return ret;
				}
				
				answer_id = ((data[10])<<8) + data[11];
		
				if(answer_id==fpid) //answer_id == req_id
				{
					
					pflow->fp_id=fpid;         //指纹id
			    
					pflow->fp_check_result=0;  //指纹已通过
					
					switch(cmd)
					{
						case 0x09: //2.1) 指纹id不可以用, byte[9]=0x09 , status=0x02 , 表示该指纹id禁用
						  						   						
						  //cur_fpid = ((data[10])<<8)+data[11];
						
 						  pflow->fp_id_status=data[12];
						  
						  if(answer_id==pflow->fp_id)
							{
   						  pflow->fp_id_status=data[12];
								
								if(pflow->fp_id_status==0x02)
								{
									pflow->type=FLOW_FPID_FORBIDDEN;   //forbidden pass					
  						    
									pflow->ack_status=0xff; // 不需要ACK
		
									ret=pflow->type;
									
									VoicePrompt(CANNOT_PASS);
									
									debug_printf("CMD=0x%x, FPID FORBIDDEN! answer_id=%d\r\n",cmd,answer_id);
									
								}
						  }
							else
								debug_printf("err:answer_id=%d != pflow->fp_id=%d \r\n",answer_id,pflow->fp_id);
							
							break; 				
							
						case 0x0E:  //byte[9]=0x0E -- 直接放行，不验枪
	            						   
 						  pflow->type=FLOW_FP_PASS_OPEN;  //no need check gun
						
						  pflow->open_id = ((data[12])<<24) + ((data[13])<<16) + ((data[14])<<8) + data[15] ;

						  pflow->fp_check_result=0;
						
							pflow->ack_status=Open_Barrier(pflow);
							
							// after break, barrier_status=BARRIER_OPEN, driver open relay				       
						   
							debug_printf("CMD=0x%x, PASS OPEN! answer_id=%d\r\n",cmd,answer_id);
						  
						  Ack_To_Server_OpenId(g_pcom_ser,pflow->cmd,pflow->fp_id,pflow->open_id,pflow->ack_status);
						
							break;
						
 						case 0x0C:  //byte[9]=0x0C --需验枪 

//0 1  2 3 4 5  6    7 8            9    10 11     12       13~13+12-1=24     25~25+12-1=36  ...  13+gun_num*12  
//EF02 FFFFFFFF 07  len=6+num*12    0C   fpid*2   gun_num     gun1                gun2             checksum      

							pflow->type=FLOW_FP_PASS_NEED_CHECK_GUN;  //need check fp and check gun	
							 							
						  ret=FLOW_FP_PASS_NEED_CHECK_GUN;

							gun_num=data[12];

							debug_printf("CMD=0x%x, NEED CHECK GUN! answer_id=%d, gun_num=%d\r\n",cmd,answer_id,gun_num);

							if(gun_num>100)	//error	 
							{	
								ret=100;
								pflow->type=100;
                debug_printf(" gun_num>100,gun_num=%d, comm error! break\r\n",gun_num);
								
								break;
             //curr_checking.target_gun.gun_num=gun_num;  
							}
					
							pflow->p_req_check_gun->gun_num=gun_num;
				  
							for(i=0;i<gun_num;i++)
							{				
								pflow->p_req_check_gun->gun_label[i].seq=i;
						
								for(j=0;j<RFID_LABEL_LEN;j++)
								{	
									pflow->p_req_check_gun->gun_label[i].label[j]=data[13+j+i*RFID_LABEL_LEN]; 
								}
							}

							ret=Check_Gun(pflow->p_req_check_gun,pflow->p_real_check_gun);
					
							if(ret==0)
							{	
								pflow->gun_check_result=0;  //pass
								
								pflow->type=FLOW_FP_PASS_GUN_PASS;
								
							  pflow->ack_status=Open_Barrier(pflow);
								
							}	
							else
							{	

								pflow->type=FLOW_FP_CHECK_GUN_NOT_PASS;
								
								pflow->gun_check_result=ret; //not pass
					      pflow->ack_status=pflow->p_real_check_gun->gun_num; //
								
								if(pflow->p_real_check_gun->gun_num==0)
									pflow->ack_status=1;  //未读枪
								
								if(pflow->p_real_check_gun->gun_num>0)
									pflow->ack_status=2;  //已读枪，不匹配
								
							}	
						
						//0 1  2 3 4 5  6   7 8   9    10 11      12        13 14      
						//EF02 FFFFFFFF 07  06    0C   fpid*2   status*1   checksum    
							Ack_To_Server(g_pcom_ser,pflow->cmd,pflow->fp_id,pflow->ack_status);
							
							break;

						default: //byte[9]=others---其他：不放行
					
								pflow->type=0xff;
								pflow->ack_status=0xff;
						    VoicePrompt(CANNOT_PASS);
								break;  //for switch						
					}
					
					ret=pflow->type;
					
					break; //for while(retry++<3)
			}		
			// for if(answer_id==fpid) //answer_id == req_id	
			else
			{
				ret=0xff;
				pflow->type=100;          //收到的不是应答本fpid,再去JadgeStr()等待一次
				pflow->ack_status=0xff; 
			}
			
		}
		
		if(retry==3)
		{
				ret=0xff;
				pflow->type=0xff;          //收到的不是应答本fpid,无需理会, 没有继续发请求包？
				pflow->ack_status=0xff;   		 

    //u16	Ack_To_Server(COMM_USART *pcomm,u8 cmd,u16 addr_id,u8 status);			
  			//Ack_To_Server(g_pcom_ser,pflow->cmd,pflow->fp_id,pflow->ack_status);
		}
		else
		{
						//Open_Barrier(pflow);
			
						//Ack_Req_To_Server(pflow);			
		}
    		 		
end_exit:
		
		ret=pflow->type;
		
	return ret;
}


//req send:  EF02 FFFFFFFF 03 0005 09 id*2 checksum
u8 Send_Req_To_Server(COMM_USART *pcomm,u8 cmd,u16 fpid)
{
	u16 temp=0;
	
 		SendHeadServer(pcomm);	 
		SendAS606_SerAddr(pcomm);
		
		SendFlag(pcomm,0x03);       //请求包标识
		SendLength(pcomm,0x05);     //cmd/req 类型(1) + fpid(2) + checksum(2)
		Sendcmd(pcomm,cmd);          //byte[9]=09 配合 byte[6]=03，表示请求包，请求类型为指纹id已验过，需要验证其他吗？
	
		//USART_transmit(pcomm,BufferID);
		USART_transmit(pcomm,((fpid)>>8));
		USART_transmit(pcomm,fpid);
	
		temp = 0x03+0x05+cmd + (((fpid)>>8)&0xff) + (fpid&0xff);
		
		SendCheck(pcomm,temp);
	
    return 0;
}

//------------------------------------
//3) 控制器执行完后应答执行情况
//0 1  2 3 4 5  6      7 8          9              10 11        12           13  14
//EF02 FFFFFFFF ACK  len=0005  cmd=OPEN/FORBIDDEN   id=0x0030  ack_status     checksum
//
//
//0 1  2 3 4 5  6    7 8       9             10 11        12           13           14         15        16          15+match_num   16+match_num~27+match_num    28+match_num ~ 39+match_num  ...  16+match_num+mismatch_num*12
//EF02 FFFFFFFF ACK  len=5+   cmd=NEED_CHECK_GUN         id=0x0030   ack_status   read_gun_num  match_num  matchseq1  matchseq2   mismatch_num       mismatch_gun1                     mismatch_gun2                   checksum
//
u8 Ack_Req_To_Server(BARRIER_CHECKING_FLOW *pflow)
{
	u8 ret=0xff;
  
	if(pflow->cmd!=0x16) //需验枪
	{
			//u16	Ack_To_Server(COMM_USART *pcomm,u8 cmd,u16 addr_id,u8 status)		
			Ack_To_Server(g_pcom_ser,pflow->cmd,pflow->fp_id,pflow->ack_status);
	}
	else
	{	
		 Ack_To_Server_Need_Gun(g_pcom_ser,pflow);
  }
	
	ret = 0x00;
	
	return ret;	
}	

//ack check gun result to server
//0 1  2 3 4 5  6      7 8       9                        10 11        12           13           14         15        16             15+match_num   16+match_num~27+match_num    28+match_num ~ 39+match_num  ...  16+match_num + mismatch_num*12
//EF02 FFFFFFFF ACK   len=5+  cmd=NEED_CHECK_GUN         id=0x0030   ack_status   read_gun_num  match_num  matchseq1  matchseq2 ...  mismatch_num     mismatched_gun1               mismatched_gun2                   checksum
//
u8 Ack_To_Server_Need_Gun(COMM_USART *pcomm, BARRIER_CHECKING_FLOW *pflow)
{
	u8 ret=0xff;
	u16 checksum;
	u16 len=0;
	u8  read_gun_num=0;
	u8  match_num=0;
	u8  mismatch_num=0;
	u8  cmd=pflow->cmd;
	u8  status=pflow->ack_status;
	u16 addr_id=pflow->fp_id;
	
	u8  mismatched=0;
	u8  match_seq;
	u8  *p_label_mismatch_buf=0;
	 
	u8 i=0,j=0;
	
	read_gun_num=pflow->p_real_check_gun->gun_num;
	
	match_num=pflow->p_real_check_gun->match_num;
	
	mismatch_num=pflow->p_real_check_gun->mismatch_num;

	//len=cmd(1)+len(2)+ack_status(1)+read_gun_num(1)+match_num(1)+match_num*1+mismatch_num(1)+mismatch_num*12+checksum(2)
	len=1+2+1+1+1+match_num*1+ mismatch_num*RFID_LABEL_LEN+2;
	
	checksum =0x07 + (((len)>>8)&0xff) + ((len)&0xff) + cmd + (((addr_id)>>8)&0xff) + ((addr_id)&0xff) + status + read_gun_num + match_num;
  
	
	SendHeadServer(pcomm); //0：2
	SendAS606_SerAddr(pcomm);       //2：4   
	SendFlag(pcomm,0x07);  //6：1 应答包标识  
	SendLength(pcomm,len);              //7：2  
	Sendcmd(pcomm,cmd);                 //9：1             //send cmd=0x08 -- download FP data; 09 -- upload FP
 	SendLength(pcomm,addr_id);          //10：2
	Sendcmd(pcomm,status);              //12：1
  Sendcmd(pcomm,read_gun_num);        //13：1
	Sendcmd(pcomm,match_num);       //14：1
  
	for(i=0;i<MAX_C_GUN_NUM;i++)
	{
	  mismatched= pflow->p_real_check_gun->gun_label[i].mismatched;
   	if(mismatched==LABEL_MATCHED) //it's matched label
		{	
			match_seq=pflow->p_real_check_gun->gun_label[i].match_seq;
     	SendFlag(pcomm,match_seq);   //send one byte
			checksum += match_seq;
		}	
	}
	
  checksum += mismatch_num;

    //15 ~ 15+not_match_gun_num*12
	for(i=0;i<MAX_C_GUN_NUM;i++)
  {
	  mismatched= pflow->p_real_check_gun->gun_label[i].mismatched;
		
		if(mismatched==LABEL_MIS_MATCHED) //not_match==0 means match
		{
			p_label_mismatch_buf = pflow->p_real_check_gun->gun_label[i].label;
			
	    SendData(pcomm,p_label_mismatch_buf,RFID_LABEL_LEN);
			
			for(j=0;j<RFID_LABEL_LEN;j++)
			{
			  checksum += p_label_mismatch_buf[j];
			}
		}		
	}
	
	SendCheck(pcomm,checksum);
	ret = 0;
	
	return ret;
}



//in: pcomm,waittime
//out p_gun_arr,p_storeid,fp_char_buf,p_checksum_result
//return 0: means ok 
u8 RecvCmdFromServer(COMM_USART *pcomm,u16 waittime)
{
	
	//u16 ret_id=1025; //return store_id; 1025 means error
	u8  ret_id=0xff;
  u8  ensure=0x00;
	u8  cmd=0x00;
	u8  *comm_data=0;
	u8  frame_type=0x0;

  BARRIER_CHECKING_FLOW *pflow=g_p_flow;
	 
 	
//1) 等待接收服务器的命令帧+数据帧 
//u8  AS60X_HEAD_SERVER_CMD[8]    ={0xef,0x02,0xff,0xff,0xff,0xff,0x01,0x00};  //接收来自服务器的命令帧	
//服务器应该发送命令帧+数据帧 
//0 1  2 3 4 5  6  7 8      9       10 11  12~523   524 525
//EF02 FFFFFFFF 01 0205     08  id=0x0030   data    checksum
//                5+512=517 
 
  debug_printf("waiting for server cmd ... \r\n");
  
	g_debug_detail=0;	
	
	//comm_data=JudgeStr_fromSer(pcomm,waittime,AS60X_HEAD_SERVER_CMD);
	comm_data=JudgeStr_fromSer(pcomm,waittime,AS60X_HEAD_SERVER);
	
	g_debug_detail=0;
  debug_printf("JudgeStr_fromSer() return,comm_data=0x%x,comm_data[9]=\r\n",comm_data,comm_data[9]);
	
 	if(comm_data)
	{
		 frame_type=comm_data[6];
		 ensure=comm_data[9];
	   cmd=comm_data[9];
		 debug_printf("received! comm_data=0x%x,cmd=0x%x \r\n",comm_data,cmd);
	}
	else
  {
		ensure=0xff; 
		debug_printf("nothing from server... or receivd data not correct! \r\n"); 
	 	// 
 	  goto recv_exit;		
  }
	g_debug_detail=0; 
	
	//ok , received from server
	//if(cmd!=0x08) //	
	//{
		//debug_printf("isn't cmd frame, cmd=0x%x \r\n", cmd);
 	  //goto recv_exit;
	//}


				if(cmd==0x0A) //最高优先级开门
				{
						  Flow_Init(pflow);
					
 						  pflow->type=FLOW_SUPERVISIOR_OPEN;  //supervisior open
					
					    pflow->cmd=cmd;
					
     					pflow->fp_id=0xffff;
					    
					    pflow->p_real_check_gun->gun_num=0;
					
						  pflow->open_id = ((comm_data[10])<<24) + ((comm_data[11])<<16) + ((comm_data[12])<<8) + comm_data[13] ;

						  pflow->fp_check_result=0;
						
					    pflow->ack_status=0xff;
					
							pflow->ack_status=Open_Barrier(pflow);
							
							// after break, barrier_status=BARRIER_OPEN, driver open relay				       
						  
						  ret_id=0;
					
					    debug_printf("cmd=0x0A,directly open barrier!,type=0x%x,fp_id=0x%x,open_id=0x%x,ack_status=0x%x\r\n",pflow->type,pflow->fp_id,pflow->open_id,pflow->ack_status);
					
						  Ack_To_Server_OpenId(g_pcom_ser,pflow->cmd,pflow->fp_id,pflow->open_id,pflow->ack_status);
					
					   	return ret_id;
				}
	
 	 if(frame_type==0x01) //command frame
	 {
			switch(cmd)
			{
					case 0x08: // 0x08: //yes,it is frame for download FPCharter data(512) to controller 
	 					
							FpSync_Init(&g_fp_sync);
		 
							ret_id=unpackFPCharbufFromServer(pcomm,comm_data,&g_fp_sync);

							if(ret_id==0) //next step
							{
								debug_printf("接收服务器数据成功,ok, store_id=%d, 继续下载到指纹库 ... \r\n",g_fp_sync.fp_id);
            
								g_fp_sync.downBufferID = g_downStoreBufferID;
						
								ret_id = downStoreCharbuf(g_pcom_fp, &g_fp_sync);
					  
								if(ret_id==0)		
								{	  
										debug_printf("downStoreCharbuf success, store_id=%d\r\n",g_fp_sync.fp_id);							
										g_fp_sync.down_store_result = 0x0;
										g_fp_sync.ack_status = 0x0;
								}	
								else
								{	
										if(g_fp_sync.down_store_result==0)
												g_fp_sync.down_store_result = 0xff;
							
										if(g_fp_sync.ack_status==0)
												g_fp_sync.ack_status = 0xff;
							
											debug_printf("downStoreCharbuf failed, store_id=%d,down_store_result=0x%x,ack_status=0x%x\r\n",g_fp_sync.fp_id,g_fp_sync.down_store_result,g_fp_sync.ack_status);	 							
								}
							}
							else
							{
									debug_printf("从服务器接收数据失败，failed!!!, store_id=%d,checksum_result=0x%x,comm_server_result=0x%x \r\n",g_fp_sync.fp_id,g_fp_sync.checksum_result,g_fp_sync.checksum_result); 
							}
					
							//g_fp_sync.ack_status=ret_id;
					
							Ack_To_Server(g_pcom_ser,cmd,g_fp_sync.fp_id,g_fp_sync.ack_status);
					
							break;
 				 
					default:
							break;
			}
   }
 	 
	recv_exit:
		
			return ret_id; //ret_len = store_id;
}


//cmd = 0x08; //
//p_checksum_status; 0 --ok, 1--checksum failed
//in: pcomm,pframe
//out: p_fp_sync->fp_id, p_fp_sync->p_model_buf1, p_fp_sync->checksum_result, p_fp_sync->comm_server_result ; // p_storeid,fp_char_buf,p_checksum_status
//return :0 means ok, others: fail
u8 unpackFPCharbufFromServer(COMM_USART *pcomm,u8 *pframe,FP_SYNC *p_fp_sync) //u16 *p_storeid,u8 *fp_char_buf,u8 *p_checksum_status)
{
  u8  ret=0xff;    //return :0 means ok, others: fail

 	u16  temp=0;
	u16  frame_len=0;
	u16  up_len=0;	
 	u16  data_begin=0;
	u16  i=0;
 
	//u16 checksum_from=6;  //checksum 除掉包头6个字节
	//u16 checksum_len=517+1;   //[6]+[7]+[8]+frame_len-2 = frame_len+1; frame_len=cmd(1)+store_id(2)+512+checksum(2)=5+512=517
	u8  frame_checksum1=0;
	u8  frame_checksum2=0;
	u8  checksum_failed=0;
	u16 frame_checksum=0;

	u16  store_id=0;
	u8  *fp_char_buf=p_fp_sync->p_model_buf1;
	
 	 //if(cmd==0x08) //yes,it is frame for download FPCharter data(512) to MCU
	 {
     store_id = (pframe[10]<<8) + pframe[11];

		 if(store_id>1000) store_id = 1000;
		 
		 //*p_storeid = store_id;
		              
		 temp = pframe[6]+ pframe[7] + pframe[8] + pframe[9] +pframe[10]+pframe[11];
		                                                         //store_id     
 
		 frame_len = (pframe[7]<<8) + pframe[8]; 

		 debug_printf("cmd frame, store id =%d,frame_len=%d\r\n",store_id,frame_len);
		 
		 if(frame_len>(MODEL_LEN*2+5))  //receiving frame len error, exit
		 {
			  debug_printf("frame len shouled less %d, receiving frame len error, frame_len=%d ,exit\r\n",MODEL_LEN*2+5,frame_len);

			  //ret=MAX_STORE_FP_NUM+1;
			 ret=0xff;

			 p_fp_sync->checksum_result = 0xff;
			 p_fp_sync->comm_server_result = 0xff;
			 
 			 goto recv_exit;
		 }									
     
		 up_len=0;
		 
		 data_begin=12;
		 
		 for(i=0;i<frame_len-5;i++)
			{
			   fp_char_buf[up_len++] = pframe[data_begin+i];	 
			   temp += pframe[data_begin+i];
			}
 			
      frame_checksum1 = pframe[data_begin+i];
      frame_checksum2 = pframe[data_begin+i+1];
			
			frame_checksum =  (frame_checksum1<<8) + frame_checksum2; //checksum come from dframe, 最后两个字节为帧带过来的checksum
      
			debug_printf("pcomm->rx_num=%d,pcomm->rx_MAXLEN=%d,data_begin+i=%d\n",pcomm->rx_num,pcomm->rx_MAXLEN,data_begin+i);
			
			debug_printf("frame_len=%d,temp=0x%x,frame_checksum=0x%x,frame_checksum1=0x%x,frame_checksum2=0x%x,up_len=%d\r\n",frame_len,temp,frame_checksum,frame_checksum1,frame_checksum2,up_len);

 			//RX_Clear();
//2) 控制端应回应ACK帧 
//EF02 FFFFFFFF 07 0003 00 000A
          			//成功为00,失败0x7f
			
			if(temp == frame_checksum) 
  		{		
				//debug_printf("frame up data checksum ok!,temp=0x%x\r\n",temp);
				checksum_failed=0;
				debug_printf("frame up data checksum OK! , return up_len =%d\r\n",up_len);
 
        //ret=store_id;
				ret=0;				
			  p_fp_sync->comm_server_result = 0;
				
			}	
			else
			{	
					checksum_failed=1;
					debug_printf("frame up data checksum failed!, return 0\r\n");

          //ret=store_id;
				  ret=1;
				  p_fp_sync->comm_server_result = 0xff;
			}
		}
	 
		//*p_checksum_status = checksum_failed;

		p_fp_sync->fp_id = store_id;
		p_fp_sync->checksum_result = checksum_failed;
	
	recv_exit:
	 
		ret = p_fp_sync->comm_server_result;
		
	 return ret;
		
	}



//
//ZACK Lynn ,20170303
//if checksum ok , return up_len
//if checksum failed , return 0
//MCU receive data from server , MODEL_LEN=512 ;
//return received data len. return 512=ok
//storeid also received from server
//服务器将storeid和指纹数据一次性传给MCU ,只有一帧，
u16 PS_RecvCharFromServer(COMM_USART *pcomm,u16 *p_storeid, u8 *upchar_buf,u16 waittime)
{
	
	u16 temp=0;
	//u16 temp_total=0;
	//u16 ack_temp=0x0;
	
	u16 frame_checksum=0;
	u8  checksum_failed=0;
  u8  ensure;
	u8  cmd=0x00;
	u8  *data;
	//u8  *dframe=0;
	 
	u16 data_begin=0;
	u16 i=0;
	u16 up_len=0;
	u16 frame_len=0;
	u16 store_id=0;
  u16 ret_len=0;
	
	//u16 checksum_from=6;  //checksum 除掉包头6个字节
	//u16 checksum_len=517+1;   //[6]+[7]+[8]+frame_len-2 = frame_len+1; frame_len=cmd(1)+store_id(2)+512+checksum(2)=5+512=517
	u8 frame_checksum1=0;
	u8 frame_checksum2=0;
	

//1) 等待接收服务器的命令帧+数据帧
//u8  AS60X_HEAD_SERVER_CMD[8]    ={0xef,0x02,0xff,0xff,0xff,0xff,0x01,0x00};  //接收来自服务器的命令帧	
//服务器应该发送命令帧+数据帧
//0 1  2 3 4 5  6  7 8      9       10 11  12~523   524 525
//EF02 FFFFFFFF 01 0205     08  id=0x0030   data     checksum
//                5+512=517 

	RX_Clear(pcomm);

  debug_printf("waiting for server cmd ... \r\n");
  
	g_debug_detail=0;	
	data=JudgeStr_fromSer(pcomm,waittime,AS60X_HEAD_SERVER_CMD);
	
	//data=JudgeStr_fromSer(pcomm,waittime,AS60X_HEAD_SERVER);	 
	g_debug_detail=0;
  debug_printf("JudgeStr_fromSer() return,data=0x%x,data[9]=\r\n",data,data[9]);
	
 	if(data)
	{
		 ensure=data[9];
	   cmd=data[9];
		 debug_printf("received! data=0x%x,cmd=0x%x \r\n",data,cmd);
	}	
	else		
  {
		ensure=0xff;	   
		debug_printf("nothing from server... or receivd data not correct! \r\n");
	 	//
 	  goto recv_exit;		
  } 
	g_debug_detail=0;
	
	//ok , received from server
	if(cmd!=0x08) //	
	{
		debug_printf("isn't cmd frame, cmd=0x%x \r\n", cmd);
 	  goto recv_exit;
	}
 		
	 if(cmd==0x08) //yes,it is cmd frame
	 {
     store_id = (data[10]<<8) + data[11];

		 if(store_id>1000) store_id = 1000;
		 
		 *p_storeid = store_id;
		              
		 temp = data[6]+ data[7] + data[8] + data[9] +data[10]+data[11];
		         //0x01  //0x02    //0x05    //0x08    //store_id     
 
		 frame_len = (data[7]<<8) + data[8]; 

		 debug_printf("cmd frame, store id =%d,frame_len=%d\r\n",*p_storeid,frame_len);
		 
		 if(frame_len>(MODEL_LEN*2+5))  //receiving frame len error, exit
		 {
			  debug_printf("frame len shouled less %d, receiving frame len error, frame_len=%d ,exit\r\n",MODEL_LEN+2,frame_len);

			  ret_len=0x0;

 			 goto recv_exit;
		 }									
 
		 up_len=0;
		 
		 data_begin=12;
		 
		 for(i=0;i<frame_len-5;i++)
			{
			   upchar_buf[up_len++] = data[data_begin+i];	 
			   temp += data[data_begin+i];
			}
 			
      frame_checksum1 = data[data_begin+i];
      frame_checksum2 = data[data_begin+i+1];
			
			frame_checksum =  (frame_checksum1<<8) + frame_checksum2; //checksum come from dframe, 最后两个字节为帧带过来的checksum
      
			debug_printf("pcomm->rx_num=%d,pcomm->rx_MAXLEN=%d,data_begin+i=%d\n",pcomm->rx_num,pcomm->rx_MAXLEN,data_begin+i);
			
			debug_printf("frame_len=%d,temp=0x%x,frame_checksum=0x%x,frame_checksum1=0x%x,frame_checksum2=0x%x,up_len=%d\r\n",frame_len,temp,frame_checksum,frame_checksum1,frame_checksum2,up_len);

 			//RX_Clear();
//2) 控制端应回应ACK帧 
//EF02 FFFFFFFF 07 0003 00 000A
          			//成功为00,失败0x7f
			
			if(temp == frame_checksum) 
  		{		
				//debug_printf("frame up data checksum ok!,temp=0x%x\r\n",temp);
				checksum_failed=0;
				debug_printf("frame up data checksum OK! , return up_len =%d\r\n",up_len);

		 	  //SendHeadServer(pcomm);
			  //SendAddr(pcomm);
			  //SendFlag(pcomm,0x07);//命令包标识
			  //SendLength(pcomm,0x03);
			  //Sendcmd(pcomm,0x00);   //send ACK = 0x00
			  //ack_temp = 0x07+0x03+0x00;
			  //SendCheck(pcomm,temp);

        ret_len=up_len;
				 
			}	
			else
			{	
					checksum_failed=1;
					debug_printf("frame up data checksum failed!, return 0\r\n");

		 	  //SendHeadServer(pcomm);
			  //SendAddr(pcomm);
			  //SendFlag(pcomm,0x07);//命令包标识
			  //SendLength(pcomm,0x03);
			  //Sendcmd(pcomm,0x7f);   //send ACK = 0x7f
			  //ack_temp = 0x07+0x03+0x7f;
			  //SendCheck(pcomm,temp);
			 
        ret_len=up_len;
			}
		}
	 
	recv_exit:
		
			return ret_len; 
}



//
//ZACK Lynn ,20170224
//down charter from my MCU to module's BufferID=1 or 2; downchar buf len ==512;
//return total downchar len
//发送到服务器 , 数据只发一帧，   0xef02+0xffffffff + 08(last data package) + 2bytes len(514=0x202) + 512bytes data + 2bytes checksum
//addrid 放在命令帧中
//MCU send
u16 PS_DownCharServer(COMM_USART *pcomm,u16 addrid, u8 *downchar_buf, u16 down_len)
{
	u16 temp=0x0;
	u16 temp_total=0x0;
  u8  ensure;
	 
	u8  *data;
  u8  *p_data_head=0; //分次发送数据包的包头
	
 	//u16  FRAME_DATA_LEN= g_fp_frame_data_len;	//128
	
	u16  pack_len=0;    //分次包的长度，pure data len
	u16  pack_len_plus_checksum = 0;  //data len plus checksum , =pack_len + 2; 
  u16  buf_use=0;     //total count dwonload datas;
	 
  u8   final_pack_flag=0x02; //0x02--后续还有数据包，0x08--最后一个数据包
	   
	u16  i=0,count=0;
	u16  left=down_len;
 	
	RX_Clear(pcomm);  
	
 		SendHeadServer(pcomm);		    
	 
		SendAS606_SerAddr(pcomm);
		SendFlag(pcomm,0x01);//命令包标识
		SendLength(pcomm,0x04);
		Sendcmd(pcomm,0x09);     //命令码09为主控端发送，发送可以是下载给指纹头，也可以是上传给服务器
	
		//USART_transmit(pcomm,BufferID);
		USART_transmit(pcomm,addrid>>8);
		USART_transmit(pcomm,addrid);
	
		temp = 0x01+0x04+0x09+addrid;
		SendCheck(pcomm,temp);

		 //ok
		 		  
			g_debug_detail=0;
   		
			data=JudgeStr(pcomm,100,AS60X_ACK_CMD_SERVER);			 
			
			//debug_printf("data=0x%x,pcomm->rx_buf=0x%x\r\n",data,pcomm->rx_buf);
			//debug_printf("BufferID=%d,buf_use=%d,temp_total=0x%x\r\n",BufferID,buf_use,temp_total);
			g_debug_detail=0;
			
 		
	
	if(data)
		ensure=data[9];
	else
		ensure=0xff;

	  pcomm->rx_use += 12;  // 应答包长为12，下次如果再读取接收缓冲buf，从rx_buf[] + rx_use 开始

	  delay_ms(50);	
	   
		if(ensure == 0x00)   //can receive data package
		{
			left=down_len;
			
			data=downchar_buf;
			
			buf_use=0;
			
			final_pack_flag=0x08;

    	temp=0;
			temp_total=0;
			
			count=0;
			
			do{
				  temp = 0;				
				  //if(left>g_fp_frame_data_len) 
				   if(left>MODEL_LEN) 
           { 
						 final_pack_flag=0x02;
						 pack_len=MODEL_LEN ;  
						 left-=MODEL_LEN;
 					 }
				  else
				   {
						final_pack_flag=0x08;
					  pack_len=left ;                
						left=0;
 				   }	
           
					 pack_len_plus_checksum = pack_len + 2;
					 
			     temp += final_pack_flag + pack_len_plus_checksum;
					 
				   //temp += final_pack_flag + (pack_len>>8)&0xff + pack_len&0xff;
					 //debug_printf("temp=0x%x,final_pack_flag=0x%x,pack_len=0x%x\r\n",temp,final_pack_flag,pack_len);
					 
					 p_data_head = &data[buf_use];   // == downchar_buf + buf_use
 					 
	 				 
				   for(i=0;i<pack_len;i++)
           { 
				       temp += data[buf_use];       //校验和，所有字节相加
						   buf_use++;
					 } 
					 
					 RX_Clear(pcomm);  
					 
 					 SendHeadServer(pcomm);
						
					 SendAS606_SerAddr(pcomm);
           SendFlag(pcomm,final_pack_flag); //数据包标识		  
	  			 SendLength(pcomm,pack_len);
 			     SendData(pcomm,p_data_head,pack_len);
					 SendCheck(pcomm,temp);
 					 
					 count++;

					 //debug_printf("downchar_buf=0x%x, p_data_head=0x%x, downchar_buf+buf_use=0x%x ,pack_len=%d,left=%d,buf_use=%d,temp=0x%x,count=%d\r\n", downchar_buf, p_data_head, downchar_buf+buf_use, pack_len,left,buf_use,temp,count);

					 //while(1);
					 temp_total += temp; 
 					 //debug_printf("downchar_buf=0x%x, p_data_head=0x%x, pack_len=%d,left=%d,buf_use=%d,temp=0x%x,temp_total=0x%x,count=%d\r\n", downchar_buf, p_data_head, pack_len,left,buf_use,temp,temp_total,count);

			   }while(left>0);						     	  	       			
		 
	 		 } 					 
			 
			 debug_printf("addrid=%d,buf_use=%d,temp_total=0x%x\r\n",addrid,buf_use,temp_total);
			 
			return buf_use;	  //返回总共download的数据
}


//
//ZACK Lynn 20170304
//only ack status to server, no need others 
//u16	Ack_To_Server(08,store_id,ack_status);
//0 1 2 3 4 5  6     7 8      9     10  11        12              13 14
//EF02FFFFFFFF 07    len     cmd    fp_id       ack_status        checksum
//
u8	Ack_To_Server(COMM_USART *pcomm,u8 cmd,u16 fp_id,u8 status)
{
	u16 len=0;
	u16 checksum=0;
	
	len = 6; //cmd(1)+fp_id(2)+status(1)+checksum(2)
	
	checksum = 0x07 + (((len)>>8)&0xff) + ((len)&0xff) + cmd + (((fp_id)>>8)&0xff) + ((fp_id)&0xff) + status;

	SendHeadServer(pcomm);          //0：2
	SendAS606_SerAddr(pcomm);       //2：4   
	SendFlag(pcomm,0x07);           //6：1 应答包标识  
	SendLength(pcomm,len);          //7：2  
	Sendcmd(pcomm,cmd);             //9：1     //send cmd=0x08 -- download FP data; 
 	SendLength(pcomm,fp_id);        //10：2
	Sendcmd(pcomm,status);          //12：1
	SendCheck(pcomm,checksum);      //13：2
  
  return 0;
}

//controller ack:
//0 1  2 3 4 5  6   7 8   9  10 11   12 13 14 15    16        17 18  
//EF02 FFFFFFFF 07  08    0E  fpid    openid*4    status*1   checksum
u8	Ack_To_Server_OpenId(COMM_USART *pcomm,u8 cmd,u16 fp_id,u32 open_id,u8 status)
{
	u16 len=0;
	u16 checksum=0;
	u8 open_id_buf[4]={0};
	u8 i=0;

	len = 8; //cmd(1)+open_id(4)+status(1)+checksum(2)
	
	open_id_buf[0] = (open_id>>24)&0xff;
	open_id_buf[1] = (open_id>>16)&0xff;
	open_id_buf[2] = (open_id>>8)&0xff;
	open_id_buf[3] = (open_id)&0xff;
	
	if(fp_id!=0xffff)
	   checksum = 0x07 + (((len)>>8)&0xff) + ((len)&0xff) + cmd + (((fp_id)>>8)&0xff) + ((fp_id)&0xff) + status;
  else	
		 checksum = 0x07 + (((len)>>8)&0xff) + ((len)&0xff) + cmd + status;
	 	
	//(((open_id)>>8)&0xff) + ((open_id)&0xff) + status;

	SendHeadServer(pcomm);          //0：2
	SendAS606_SerAddr(pcomm);       //2：4   
	SendFlag(pcomm,0x07);           //6：1 应答包标识  
	SendLength(pcomm,len);          //7：2  
	Sendcmd(pcomm,cmd);             //9：1     //send cmd=0x0A -- open barrier directly
	
	if(fp_id!=0xffff)
 	   SendLength(pcomm,fp_id);        //10:2
	
	//12:4
  for(i=0;i<4;i++)
	{
	  SendFlag(pcomm,open_id_buf[i]);
		checksum += open_id_buf[i];
	}
	
	Sendcmd(pcomm,status);          //16：1
	
	SendCheck(pcomm,checksum);      //17：2
  
  return 0;
}
