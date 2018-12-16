
#include <string.h>
#include "global.h"
#include "delay.h" 	
#include "USARTx.H"
#include "debug_print.h"
#include "voice.h"
#include "barrier.h"
#include "check_gun.h"
#include "hc595.h"


#define BARRIER_CLOSE 0
#define BARRIER_OPEN  1

u8 Flow_Init(BARRIER_CHECKING_FLOW *p_flow)
{
	u8 ret=0;

	debug_printf("Flow_Init,p_flow=0x%x\r\n",p_flow);
	
	p_flow->type=FLOW_NO_IN_WORKING;     //forbidden pass
	
	p_flow->cmd=0x0;
	
	p_flow->fp_id=0xffff;

	p_flow->fp_id_status=0xff;

	p_flow->open_id=0xffffffff;
	
	p_flow->fp_check_result=0xff;  //not pass

	p_flow->ack_status=0xff;
	
 	
	//debug_printf("sizeof(GUN_CHECKING)=%d\r\n",sizeof(GUN_CHECKING));                    //153
  //debug_printf("sizeof(BARRIER_CHECKING_FLOW)=%d\r\n",sizeof(BARRIER_CHECKING_FLOW));  //28

	Gun_Init(&g_req_check_gun);
	
	Gun_Init(&g_real_check_gun);
	
	p_flow->p_req_check_gun = &g_req_check_gun;
	
  p_flow->p_real_check_gun = &g_real_check_gun;

	p_flow->p_req_check_gun->gun_num=0;
	
	p_flow->p_real_check_gun->gun_num=0;

	p_flow->gun_check_result=0xff;  //not pass
	
	p_flow->barrier_status=BARRIER_CLOSE;
	
	g_ack_status = 0xff;
	
	return ret;
}



 
//#define FLOW_NO_IN_WORKING              0   //not in working 
//#define FLOW_FPID_FORBIDDEN             1   //forbidden
//#define FLOW_NETWORK_BREAK_OPEN         2   //when power down / newwork break, supervisior open
//#define FLOW_SUPERVISIOR_OPEN           3   //supervisior open ( cmd=0A )
//#define FLOW_DIRECTLY_OPEN              4   //directly open
//#define FLOW_NO_NEED_CHECK_FP_OPEN      5   
//#define FLOW_FP_PASS_OPEN               6   //fp pass open
//#define FLOW_FP_PASS_NEED_CHECK_GUN     7   //need check gun
//#define FLOW_FP_PASS_GUN_PASS           8
//#define FLOW_PROTOCOL_ERROR             100


#define OPEN_BARRIER_DELAY_RATIO  2   //delay open barrier ratio, delay times(seconds = RATIO*gun_num)

#define OPEN_BARRIER_SECONDS      3   //open barrier last seconds, = SECONDS + gun_num


u8 Open_Barrier(BARRIER_CHECKING_FLOW *pflow)
{
	u8 ret=0xff;
	u8 open=0xff;
	u8 delay_secs=0;
	u8 last_secs=0;
	u8 gun_num=0;
	u8 i=0;
	
	 open=BARRIER_CLOSE;
	
	 pflow->barrier_status=BARRIER_CLOSE;
	 
		switch(pflow->type)
		{
			case FLOW_NO_IN_WORKING:
			case FLOW_FPID_FORBIDDEN:
			case FLOW_FP_PASS_NEED_CHECK_GUN:
			case FLOW_PROTOCOL_ERROR:
				
				   open=BARRIER_CLOSE;
		      ucBarrierGate_Control_flag = 0;

			break;				
			
      case FLOW_NETWORK_BREAK_OPEN:
			case FLOW_SUPERVISIOR_OPEN:
			case FLOW_DIRECTLY_OPEN:
			case FLOW_NO_NEED_CHECK_FP_OPEN:
		  case FLOW_FP_PASS_OPEN:
			case FLOW_FP_PASS_GUN_PASS:
				
				  open=BARRIER_OPEN;
		      ucBarrierGate_Control_flag = 1;
			
			    gun_num = pflow->p_real_check_gun->gun_num;			
			   break;			
			
			default:
				open=BARRIER_CLOSE;
			  break;
		}
	
	 
	 pflow->barrier_status=open;
 
 		
	 if(open==BARRIER_OPEN)
	 {
     gun_num = pflow->p_real_check_gun->gun_num;			
		 
   	 delay_secs=OPEN_BARRIER_DELAY_RATIO*gun_num;  //验了几把枪就延时几秒 

		 if(delay_secs>1)	 
			     delay_secs -= 1;  //减一秒
     
		 last_secs=OPEN_BARRIER_SECONDS+gun_num; 
		 
  	 debug_printf("barrier will OPENED!gun_num=%d,delay_secs=%d,last_secs=%d\r\n",gun_num,delay_secs,last_secs);
 		  		 
		 for(i=0;i<delay_secs;i++)		  //验完枪后稍延时再开门 
		    delay_ms(1000);  
		 
		 //voice 道闸已打开	    
		 VoicePrompt(COME_IN);

		 ucBarrierGate_Control_flag = 1;
 		 
		 HC595_IO_Out_Updata_Status(Led9_on);		 
		 
		 BarrierGate_Open();		 
		  		 
		 debug_printf("barrier OPENED!pls come in!\r\n");
		  
     for(i=0;i<last_secs;i++)		  
		   delay_ms(1000);              //开门持续秒数  
 
 
		 BarrierGate_Close();			
		 
		 HC595_IO_Out_Updata_Status(Led9_off);
		 
     open = BARRIER_CLOSE;   //开完后关闭 
	   pflow->barrier_status=open;
		 
 		 debug_printf("barrier CLOSED!\r\n");
		 
	 }
	 
	 pflow->barrier_status=open;
    
	 pflow->ack_status = 0x0;
	 
	 ret=pflow->ack_status;
	 
	return ret;
}

