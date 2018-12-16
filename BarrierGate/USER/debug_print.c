
#include "stm32f10x.h"

#include "voice.h"
#include "delay.h"
#include "timer2.h"
#include "timer3.h"
#include "timer4.h"
#include "USARTx.H"
#include "debug_print.h"

//#include "USART1.H"
//#include "USART2.H"
//#include "USART3.H"

#include "hc595.h"
#include "as606.h"

#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 

#include "stdlib.h"
#include "global.h"


void debug_print_c(u8 ch)
{	
   USART_transmit(g_p_usart_debug,ch);
}

void debug_print_nr(void)
{	
	u8 nrchar[3]={0};
	
	//sprintf((char*)nrchar,"\n\r",2);
	nrchar[0]='\r';
	nrchar[1]='\n';
	nrchar[2]=0;
  		 
  USART_transmitS(g_p_usart_debug,nrchar,2);
}	
 
//len<=16
//print buf one line
void debug_print_buf(u8* buf,unsigned int len) 
{
	   u8 dbuf[16]={0};  
		 u16 i=0,ilen=0;
		 
	   for(i=0;i<len;i++) 
		 {
			 memset((char*)dbuf,0,16);
       sprintf((char*)dbuf,"0x%02x ",buf[i]);
       ilen=strlen((const char*)dbuf);		 
	     USART_transmitS(g_p_usart_debug,dbuf,ilen);
		 }	 
	          		     
	    debug_print_nr();
}

//output data for copy, print buf one line
void debug_print_conti(u8* buf,unsigned int len) 	
{
	   u8 dbuf[16]={0};  
		 u16 i=0,ilen=0;
		 
		 debug_print_nr();
		 
	   for(i=0;i<len;i++) 
		 {
			 memset((char*)dbuf,0,16);
       sprintf((char*)dbuf,"%02x",buf[i]);
       ilen=strlen((const char*)dbuf); 
	     USART_transmitS(g_p_usart_debug,dbuf,ilen);
		 }	 
	          		     
	    debug_print_nr();
}

//print buf multi line
//return line num;
u16 debug_print_buf_multi(u8* buf,u16 totlen,u16 column) 
{
	   u8 dbuf[16]={0};
		 u16 i=0,j=0,icolumn=0,current_column=0;
		 
		 u16 paragraphs = totlen/column;
		 u16 lastcolumn = totlen%column;

		 if(lastcolumn>0)     //有余数，多加一行
			 paragraphs++;
		 else 
		 {	 
			 if(lastcolumn==0) //没有余数，最后一行等于一般行
				 lastcolumn=column;
		 } 
		 
		 for(j=0;j<paragraphs;j++)
		 {
				if(j==(paragraphs-1)) //last line
					current_column=lastcolumn;
				else
					current_column=column;
			
				for(i=0;i<current_column;i++)
				{
						memset((char*)dbuf,0,16);
						sprintf((char*)dbuf,"0x%02x ",buf[i+j*column]);
						icolumn=strlen((const char*)dbuf); //ilen is each char print len = 2/3/4
						USART_transmitS(g_p_usart_debug,dbuf,icolumn);
				}
				debug_print_nr();
	   }
		 return paragraphs;
}

//u8 debug_printf_buf[1025]={0};
u8 debug_printf_buf[256]={0};

void debug_printf(u8* fmt,...)  
{ 
	u16 len; 
	va_list ap; 
	va_start(ap,fmt);
  
	vsprintf((char *)debug_printf_buf,(char*)fmt,ap);
	va_end(ap);
  
	len=strlen((const char*)debug_printf_buf);	 
	
	USART_transmitS(g_p_usart_debug,(u8*)debug_printf_buf,len);
	//debug_print_nr();	
	
}



 
u16 debug_input_ch(u16 waittime,u8 *ibuf)
{
	u16 len=0,i=0;
	u8 *data=0;
	COMM_USART *p_usart = g_p_debug_recv;
	
	uint8_t *rx_buf=p_usart->rx_buf + p_usart->rx_use; 
  
	while(--waittime) //找waittime次 
	{
		delay_ms(10);
		//debug_printf("p_usart->rx_state=%d\n\r",p_usart->rx_state);
	   
 		
		if(p_usart->rx_state==2)   //接收到完整一帧数据
		{
			data=rx_buf;	
			len=p_usart->rx_num;

			if(g_debug_detail)
			{
			  debug_print_nr();
			  debug_printf("in ()/p_usart->rx_buf=0%x,data=0x%x,rx_state=0x%x,len=0x%x\n\r",p_usart->rx_buf,data,p_usart->rx_state,len);
        debug_print_buf(rx_buf,16);
			  debug_print_nr();
			}
			
			for(i=0;i<len;i++)
			   ibuf[i]=data[i];
			        			 
 			if(len>0) 
				return len;
		}
	}
 	
	return 0;
}


u16 debug_input_id(u16 waittime,u8 *ibuf)
{
	u16 len=0,i=0;
	u8 *data=0;
	u16 getnum=0x0;
	u16 rx_num=0,rx_last_num=0;
	
  u8 print_buf[32]={0};
	
	COMM_USART *p_usart = g_p_debug_recv;
	
	uint8_t *rx_buf=p_usart->rx_buf + p_usart->rx_use; 
	
	data=rx_buf;	
	
  g_p_debug_recv->key_in=1; 
	
	p_usart->rx_num=0;
	p_usart->rx_last_num=0;
	
	while(--waittime) //找waittime次 
	{
		delay_ms(10);
    
		rx_num=p_usart->rx_num;
		rx_last_num=p_usart->rx_last_num;
		
 		len=rx_num;
		
    memset(print_buf,0,32);
		
		if(rx_num<32)
 	     memcpy(print_buf,&data[rx_last_num],rx_num-rx_last_num);
		else
			 memcpy(print_buf,data,31);
		
		if(data[len-1]==0x0d)    //按下回车键 
		{
			if(g_debug_detail)
			{
			  debug_print_nr();
			  debug_printf("in ()/p_usart->rx_buf=0%x,data=0x%x,rx_state=0x%x,len=0x%x\n\r",p_usart->rx_buf,data,p_usart->rx_state,len);
        debug_print_buf(data,16);
			  debug_print_nr();
			
			  data[len-1]=0x0;       //remove 'enter' char, remain pure digit     
 			
			}
			  for(i=0;i<len;i++)
			    ibuf[i]=data[i];
			
        //debug_print_buf(ibuf,16);
			 		
			  getnum=atoi((char*)ibuf);
			 
				//if(getnum>1000) getnum=1000;
			
			  debug_printf(" %d ",getnum);		
	      			
 			//if(n>0) 
			{
				g_p_debug_recv->key_in=0;
				return getnum;
			}	
		}
		else  //正在输入数字，还没按回车
		{
			if(p_usart->rx_num >0)
			{
				if(p_usart->rx_num >=31)  //输入太多字符
					{	
						getnum = 0;
						debug_printf("输入太多字符，返回 %d !",getnum);		
						return 0;
					}	
				else
					if(p_usart->rx_num != p_usart->rx_last_num)				
					{	
		       //debug_printf("%s, last_num=%d, p_usart->rx_num=%d",print_buf,last_num,p_usart->rx_num);				
					 debug_printf("%s",print_buf);				
					 p_usart->rx_last_num=p_usart->rx_num;   //last跟上来					    
					}
			}	
		}	
	}
 	p_usart->key_in=0; 	
	
	return 0;
}


//最多计时一分钟，否则溢出两次以上
vu16 tim_pass(vu16 cur_ticks,vu16 last_ticks)
{
	int pass=0;

	pass=cur_ticks-last_ticks;
				
  if(pass<0) 
		pass+=0x10000;
	  
  return pass	;
}

//u8 input_function(u8 *in_buf)
u8 debug_getch(u8 *in_buf)
{  
 u8 chc=0xff;
 u16 input_len=0;

				//RX_Clear(g_p_debug_recv);	 	
				input_len=debug_input_ch(1000,in_buf);
				//debug_printf("indata[0]=%c\r\n",indata[0]);
        //debug_print_buf(input_buf,2);
				chc=in_buf[0];
	
	return chc;
}

u16 debug_getid(u8 *in_buf)
{
 u16 input_len=0x0;
 u16 id=0x0;	
	       
	      g_debug_detail=0;				
	 
     	  RX_Clear(g_p_debug_recv);
  
 				id=debug_input_id(1000,in_buf);
 				//debug_printf("in_buf[0]=0x%x,in_buf[1]=0x%x,in_buf[2]=0x%x,in_buf[3]=0xx%x,in_buf[4]=0x%x,id=%d\r\n",in_buf[0],in_buf[1],in_buf[2],in_buf[3],in_buf[4],id);
        //debug_print_buf(input_buf,2);
				//chc=in_buf[0];
				g_debug_detail=0;
	      g_p_debug_recv->key_in=0; 	
	       
        if(id>1000) id=1000;
	
	      debug_printf("id=0x%x=%d\r\n",id,id);
	
	return id;
}


u8 print_gun_checking(GUN_CHECKING *pgun)
{
	u8 i=0; 
	
	debug_printf("gun_num=%d,match_num=%d,mismatch_num=%d\r\n",pgun->gun_num,pgun->match_num,pgun->mismatch_num);
	
	for(i=0;i<pgun->gun_num;i++)
	{
		debug_printf("gun_label[%d].seq=%d,gun_label[%d].match_seq=%d,gun_label[%d].mismatched=%d\r\n",i,pgun->gun_label[i].seq,i,pgun->gun_label[i].match_seq,i,pgun->gun_label[i].mismatched);
		debug_print_conti(pgun->gun_label[i].label, RFID_LABEL_LEN);
		debug_print_nr();		 
	}
	debug_print_nr();
	
}