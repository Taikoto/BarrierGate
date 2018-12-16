
#include <string.h>
#include "delay.h" 	
#include "USARTx.H"
#include "debug_print.h"
//#include "as606.h"
#include "global.h"


void SendFlag(COMM_USART *pcomm,u8 flag)
{
	//USART1_transmit(flag);
	USART_transmit(pcomm,flag);
}

void SendLength(COMM_USART *pcomm,int length)
{
	//USART1_transmit(length>>8);
	//USART1_transmit(length);
	USART_transmit(pcomm,length>>8);
	USART_transmit(pcomm,length);	
}

void Sendcmd(COMM_USART *pcomm,u8 cmd)
{
	//USART1_transmit(cmd);
	USART_transmit(pcomm,cmd);
}

void SendCheck(COMM_USART *pcomm,u16 check)
{
	//USART1_transmit(check>>8);
	//USART1_transmit(check);

	USART_transmit(pcomm,(check)>>8);
	USART_transmit(pcomm,check);
	
}

void SendData(COMM_USART *pcomm,u8 *data,u16 len)
{
 
  USART_transmitS(pcomm,data,len);
}	

//u8 *JudgeStr(u16 waittime,u8 *jstr)
//if searched *jstr, return the pointer of data  
//else return null pointer  
u8 *JudgeStr(COMM_USART *pcomm,u16 waittime,u8 *jstr)
{
	char *data=0;
	COMM_USART *p_usart = pcomm;
	
	uint8_t *rx_buf_judge_from=p_usart->rx_buf + p_usart->rx_use; 

	while(--waittime) //找waittime次 
	{
		delay_ms(10);
		//delay_ms(20);
		//debug_printf("p_usart->rx_state=%d\n\r",p_usart->rx_state);
	  
    if(g_debug_detail)
		{
			  debug_print_nr();
			  debug_printf("in JudgeStr()/p_usart->rx_buf=0%x,rx_buf_judge_from=0x%x,rx_state=0x%x\n\r",p_usart->rx_buf,rx_buf_judge_from,p_usart->rx_state);
        debug_print_buf(rx_buf_judge_from,16);
			  debug_print_nr();
		}
		
		
		if(p_usart->rx_state==2)   //接收到完整一帧数据
		{
			
 			//data=strstr((const char*)USART1_RX,(const char*)str);
 			data=strstr((const char*)rx_buf_judge_from,(const char*)jstr);   //找到包头，返回包头地址，找不到，返回0
			
			if(g_debug_detail)
			{	  
				debug_printf("in JudgeStr()/p_usart->rx_buf=0%x,rx_buf_judge_from=0x%x,rx_state=0x%x,rx_num=0x%x\n\r",p_usart->rx_buf,rx_buf_judge_from,p_usart->rx_state,p_usart->rx_num);				
				debug_print_buf(rx_buf_judge_from,16);
			}	
			
			if(data!=0)
			{				  			
 				 if(data[9]==0)
				 {
					 ;//debug_printf("answer ok!!\n\r"); 
				 }					 
			}
			
 			if((uint32_t)data >= (uint32_t)rx_buf_judge_from)  //如果搜到，rx_begin 为搜到的起始位置， data为起始位置的地址
			  p_usart->rx_begin=(uint32_t)data - (uint32_t)rx_buf_judge_from;
			else
				p_usart->rx_begin=0;  //搜不到，rx_begin 没意义

			p_usart->rx_use += p_usart->rx_begin;
			
 			
			if(data)
 				return (u8*)data;   //返回包头地址，			 				 
		}
	}
	
	return 0;
}


//u8 *JudgeStr(u16 waittime,u8 *jstr)
//if searched *jstr, return the pointer of data 
//else return null pointer 
//return data addr:means recv OK
//return 0:mean no recv
u8 *JudgeStr_fromSer_nowait(COMM_USART *pcomm,u8 *jstr,u16 *p_spend_ms)
{
	char *data=0;
	COMM_USART *p_usart = pcomm;
	
	uint8_t *rx_buf_judge_from=p_usart->rx_buf + p_usart->rx_use; 

	*p_spend_ms= g_network_handshake.wait_ms;	
 	
	if(g_network_handshake.wait_recv_pending)
	{
 		
		if(p_usart->rx_state==2)   //接收到完整一帧数据
		{
       debug_printf("JudgeStr_fromSer_nowait()/rx_state=2\r\n");
			
 			data = strstr((const char*)rx_buf_judge_from,(const char*)jstr);   //找到包头，返回包头地址，找不到，返回0
 
 			if((uint32_t)data >= (uint32_t)rx_buf_judge_from) 
			  p_usart->rx_begin=(uint32_t)data - (uint32_t)rx_buf_judge_from;
			else
				p_usart->rx_begin=0;  //搜不到，rx_begin 没意义

			p_usart->rx_use += p_usart->rx_begin;
			
      *p_spend_ms= g_network_handshake.wait_ms;
			
			if(data)  //得到回应
			{					 
 				return (u8*)data;   //收到该收的包，返回包头地址， 
			}
			else
				p_usart->rx_state==1; //继续接收
		}
		else
		{
			
			*p_spend_ms=g_network_handshake.wait_ms;
			
			return 0;
		}	
	}
 
	*p_spend_ms= g_network_handshake.wait_ms;	
  return 0;
	
}


//u8 *JudgeStr(u16 waittime,u8 *jstr)
//if searched *jstr, return the pointer of data 
//else return null pointer 
u8 *JudgeStr_fromSer(COMM_USART *pcomm,u16 waittime,u8 *jstr)
{
	char *data=0;
	COMM_USART *p_usart = pcomm;
	
	uint8_t *rx_buf_judge_from=p_usart->rx_buf + p_usart->rx_use; 
	
	while(--waittime) //找waittime次 
	{
		//delay_ms(10);
		delay_ms(20);		 
	   
    if(g_debug_detail)
		{
			  debug_print_nr();
			  debug_printf("in JudgeStr()/p_usart->rx_buf=0%x,rx_buf_judge_from=0x%x,rx_state=0x%x\n\r",p_usart->rx_buf,rx_buf_judge_from,p_usart->rx_state);
        debug_print_buf(rx_buf_judge_from,16);
			  debug_print_nr();
		}	
				
		if(p_usart->rx_state==2)   //接收到完整一帧数据
		{
			
 			//data=strstr((const char*)USART1_RX,(const char*)str);
 			data=strstr((const char*)rx_buf_judge_from,(const char*)jstr);   //找到包头，返回包头地址，找不到，返回0
			
			if(g_debug_detail)
			{	  
				debug_printf("in JudgeStr()/data=0x%x,p_usart->rx_buf=0%x,rx_buf_judge_from=0x%x,rx_state=0x%x,rx_num=0x%x\n\r",data,p_usart->rx_buf,rx_buf_judge_from,p_usart->rx_state,p_usart->rx_num);				
				debug_print_buf(rx_buf_judge_from,16);
			}	
			
			if(data!=0) //得到回应
			{				  			
 				 if(data[9]==0)
				 {
					 ;//debug_printf("fromSer answer ok!!\n\r"); 
				 }					 
			}
 						
 			if((uint32_t)data >= (uint32_t)rx_buf_judge_from)  //如果搜到，rx_begin 为搜到的起始位置， data为起始位置的地址
			  p_usart->rx_begin=(uint32_t)data - (uint32_t)rx_buf_judge_from;
			else
				p_usart->rx_begin=0;  //搜不到，rx_begin 没意义

			p_usart->rx_use += p_usart->rx_begin;
			
 			
			if(data)
 				return (u8*)data;   //收到该收的包，返回包头地址， 
			else
				p_usart->rx_state==1; //继续接收
		}
	}
	
 	return 0;
}

//dframe=JudgeStr_UpChar(50,AS60X_HEAD,rx_buf_left);

//u8 *JudgeStr_UpChar(u16 waittime,u8 *jstr,u8 *buf_left)
u8 *JudgeStr_UpChar(COMM_USART *pcomm,u16 waittime,u8 *jstr,u8 *buf_left)
{
	char *data=0;
	COMM_USART *p_usart = pcomm;
	
 	//一个buf可以 一次性接收多个包, 比如上传特征，应答帧后面直接跟多个数据包 
	
	//uint8_t *rx_buf_judge_from=p_usart->rx_buf + p_usart->rx_use; 
	uint8_t *rx_buf_judge_from=buf_left; 
  
	
	while(--waittime) //找waittime次 
	{
		delay_ms(20);
		//debug_printf("p_usart->rx_state=%d\n\r",p_usart->rx_state);
	   
    if(g_debug_detail)
		{	  
			  debug_print_nr();
			  debug_printf("in JudgeStr_UpChar()/p_usart->rx_buf=0%x,rx_buf_judge_from=0x%x,rx_state=0x%x\n\r",p_usart->rx_buf,rx_buf_judge_from,p_usart->rx_state);
        debug_print_buf(rx_buf_judge_from,16);
			  debug_print_nr();
		}	
		
		
		if(p_usart->rx_state==2)   //接收到完整一帧数据
		{
			
 			//data=strstr((const char*)USART1_RX,(const char*)str);
 			data=strstr((const char*)rx_buf_judge_from,(const char*)jstr);   //找到包头，返回包头地址，找不到，返回0
 
			if(data!=0) //fix ok!
			{
				
 			}
			
 			if((uint32_t)data >= (uint32_t)rx_buf_judge_from)  //如果搜到，rx_begin 为搜到的起始位置， data为起始位置的地址
			  p_usart->rx_begin=(uint32_t)data - (uint32_t)rx_buf_judge_from;
			else
				p_usart->rx_begin=0;  //搜不到，rx_begin 没意义

			p_usart->rx_use += p_usart->rx_begin;
			
			//debug_printf("JudgeStr()/data=0x%x,*data=0x%x,data[9]=%d,rx_begin=%d,rx_use=%d, USART_RX= ",data,*data,data[9],p_usart->rx_begin,p_usart->rx_use);
			 			
			if(data)
 				return (u8*)data;   //返回包头地址，
		}
	}
 	return 0;
}



