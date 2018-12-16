#include <string.h>
#include "global.h"
#include "delay.h" 	
#include "USARTx.H"
#include "debug_print.h"
#include "rfid.h"
#include "comm_recv.h"
#include "voice.h"
#include "check_gun.h"

u8 Gun_Init(GUN_CHECKING *p_gun)
{
	u8 ret=0;
	u8 i=0;
	
	//debug_printf("sizeof(GUN_CHECKING)=%d\r\n",sizeof(GUN_CHECKING));  //153
	
	memset(p_gun,0,sizeof(GUN_CHECKING));
	
	for(i=0;i<MAX_C_GUN_NUM;i++)
	{
		  p_gun->gun_label[i].mismatched=0xff;      //0:matched,others:not matched
			p_gun->gun_label[i].match_seq=0xff;      //match_seq=100,means not matched
		  
	}
	
	return ret;
}

#define LABEL_NEW    0
#define LABEL_REPEAT 1

//repeat=If_Label_Repeat(GUN_CHECKING *p_has_read_gun,GUN_LABEL *p_label_buf);
//return LABEL_NEW //new label ; others LABEL_REPEAT //repeat,old_label
u8 If_Label_Repeat(GUN_CHECKING *p_had_read_gun,u8 *p_label_buf)
{
	u8 ret=LABEL_NEW;
	u8 cmp=0xff;
	u8 had_read_gun_num = p_had_read_gun->gun_num;
 
  u8 i=0;  
	
	for(i=0;i<had_read_gun_num;i++)
	{
		cmp=memcmp(p_had_read_gun->gun_label[i].label, p_label_buf, RFID_LABEL_LEN);
		
		if(cmp==0) //fix! means old label
		{	
			ret=LABEL_REPEAT; //repeat!
			break;
		}	
	}
	if(cmp!=0) //not fixed! new label
		ret=LABEL_NEW;
	
	return ret;
}


// label_seq = If_Label_Match_Seq(p_req_gun,label_buf);	//id>=100, not match
// output *match seq(<100)
// *match_seq >=100 , not match
// return 0: fixed!  ; others : not fixed!
u8 If_Label_Match_Seq(GUN_CHECKING *p_req_gun, u8 *p_label_buf,u8 *p_match_seq)
{
	u8 ret=0xff;
	u8 cmp=0xff;
	
	u8 gun_num=p_req_gun->gun_num;
	
	u8 i=0;
	
	ret=LABEL_MIS_MATCHED;
	
	*p_match_seq=100;
	
	for(i=0;i<gun_num;i++)
	{
	 		cmp=memcmp(p_req_gun->gun_label[i].label,p_label_buf,RFID_LABEL_LEN);
		
		  if(cmp==0) //fixed! Gun Match!
			{
				*p_match_seq=i;
				ret=LABEL_MATCHED;
			  break;
			}
	}
	
 return ret;
}

//return insert location: insert
u8 InsertBy_match_seq(GUN_CHECKING *p_read_gun,u8 valid_num,u8 cmp_match_seq)
{
	u8 insert=valid_num;
	char i=0;
	
	if(valid_num<1) 
		insert=0;
	else
	{
		 insert=valid_num;
		
		 for(i=valid_num-1;i>=0;i--)
		 {
			if(i>=valid_num) break;
			 
		  if( cmp_match_seq < p_read_gun->gun_label[i].match_seq )
		    insert=i; 
			 			  
		 }
		
		 		 
		if(insert<valid_num)
		{
		 for(i=valid_num;i>insert;i--)
			{			 
			  memcpy(&(p_read_gun->gun_label[i]),&(p_read_gun->gun_label[i-1]),sizeof(GUN_LABEL));
			}	
		}
	}
	
	return insert;
}

//in:  need_check
//out: check_result
//return 0: pass, others : not pass
//
//typedef struct
//{ 
//	u8  seq;
//  u8  match_seq;   
//  u8  mismatched;  //==0 means matched; >=100 means unmatched
//	u8  label[RFID_LABEL_LEN];
//}GUN_LABEL;

//typedef struct
//{
//	u8 gun_num;           //total how many guns
//	u8 match_num;         //how many guns matched 
//  u8 mismatch_num;      //how many guns not matched 
//
//	GUN_LABEL gun_label[10];
//
//}GUN_CHECKING;
//
//RFID read num        p_real_gun->gun_num
//matched num          p_real_gun->gun_check_match
//RFID read seq        p_real_gun->gun_label[i].seq;       
//match req seq        p_real_gun->gun_label[i].matchseq;  
//label                p_real_gun->gun_label[i].label[];   
//
u8 Check_Gun(GUN_CHECKING *p_req_gun,GUN_CHECKING *p_read_gun)
{
	u8 ret=0xff;
	u8 read_rfid_ret=0xff;
	
	u16 i=0,j=0;
  u8  reading_label_buf[RFID_LABEL_LEN]={0}; 
	u8  cur_match_seq=0;
  
	u8 need_match_num=p_req_gun->gun_num;  //总共需要匹配几把枪？
	
	u8 chance=0;         //<= MAX_C_GUN_NUM
	
	u8 retry_times=0;	   //<=3
	u8 repeat_times=0;   //<=3
	u8 valid_num=0;      //<=chance, total read num
	u8 match_num=0;      // target = need_match_num 
 	u8 mismatch_num=0;   //有几把不匹配
	
	u8 matched=0;
	
	u8 repeat=0;
	
	u8 ins=0;
  	 
 	u8 waittime_left=0;
	
 	 //debug_printf("sizeof(*p_req_gun)=%d\r\n",sizeof(*p_req_gun));
	 debug_printf("Check_Gun()!\r\n");
 	 debug_printf("need_gun_num=%d\r\n",need_match_num);
	 for(i=0;i<need_match_num;i++)
	 {
	    debug_print_conti(p_req_gun->gun_label[i].label, RFID_LABEL_LEN);
			debug_print_nr();
	 }
	 
   Gun_Init(p_read_gun);
	 
 	 ret=0xff;
   waittime_left=0;

	 for(chance=0;i<MAX_C_GUN_NUM;chance++) //验10次，每次6s
	 {
		 
			   if(match_num==0)
			   {
   		     //voice 请验枪
					 VoicePrompt(CHECK_GUN);
			   }
         else
				 {	 
           //voice 请验下一把枪				
					 VoicePrompt(CHECK_NEXT_GUN);
				   delay_ms(600);
				 }

 			  RX_Clear(g_pcom_rfid);

		    memset(reading_label_buf,0,sizeof(reading_label_buf));

			  read_rfid_ret = 0xff;
 			   				 
		    read_rfid_ret = Rfid_Read_Label_Base_Proc(g_pcom_rfid,reading_label_buf,&waittime_left);

	  	  if(read_rfid_ret==0) //rfid read ok, read in label_buf
		    {
					 retry_times=0;

				   repeat=If_Label_Repeat(p_read_gun,reading_label_buf);
					//return 0 :mean new label

				   if(repeat==LABEL_NEW) //new label ,valid_num++
				   {
						  repeat_times=0;

 						  p_read_gun->gun_num++;

			        matched = If_Label_Match_Seq(p_req_gun,reading_label_buf,&cur_match_seq);	//match_seq>=100, not match
						  //return 0 :means matched						 
 						  //debug_printf("valid_num=%d,cur_match_seq=%d\r\n",valid_num,cur_match_seq);
						 
							if( matched==LABEL_MATCHED ) // fixed! it match
							{
								if(valid_num>0)
								{
 						      //debug_printf("before ins , valid_num=%d,cur_match_seq=%d\r\n",valid_num,cur_match_seq);
								  ins=InsertBy_match_seq(p_read_gun,valid_num,cur_match_seq);
 								}
								
						    p_read_gun->gun_label[ins].seq=valid_num;

								p_read_gun->gun_label[ins].match_seq = cur_match_seq;  //init = 100

								p_read_gun->gun_label[ins].mismatched = LABEL_MATCHED;  //mismatch==0 means matched
								
						    memcpy(p_read_gun->gun_label[ins].label,reading_label_buf,RFID_LABEL_LEN);
                
								p_read_gun->match_num++;
                
								match_num++;
															 									 								
								if(match_num>=need_match_num) //匹配后，如果还有更多的枪就不能读入
								{
								 	    ret=0;
                      break;
								}
							}
							else  //not match
							{
								//voice 枪支与人员信息不匹配，重验枪

						    p_read_gun->gun_label[valid_num].seq=valid_num;

								p_read_gun->gun_label[valid_num].match_seq=100+mismatch_num;

								p_read_gun->gun_label[valid_num].mismatched=100;
 
						    memcpy(p_read_gun->gun_label[valid_num].label,reading_label_buf,RFID_LABEL_LEN);
								
								p_read_gun->mismatch_num++;
								
								mismatch_num++;
							}

							valid_num++; 
					}
					//if(repeat==0) //new label
					else //repeat
					{								 
						   //repeat 
						  if(++repeat_times>=4) 
							{
								ret=2;            //ret=2,重复读4次以上退出
								debug_printf("repeat_times=%d,break;\r\n",repeat_times);
 								break;
							}
							//else
							  //repeat_times++;							
					}
					
				}
				else  //RFID not read label, no need check? retry 3 times
				{
						  if(++retry_times>=3) //连续超过3次读不到标签(每次6s)，认为无标签可读，退出
							{
								ret=1;   //ret=1,超时退出
								debug_printf("retry_times=%d,break;\r\n",retry_times);								 
								break;
							}
							//else
							  //retry_times++;	
				}
	 }

	 if(ret!=0)
	 {
		 VoicePrompt(GUN_FAIL);
	   debug_printf("VoicePrompt(GUN_FAIL);\r\n");
	 }	 
	 else
	 {	 
		 VoicePrompt(GUNPASS);
	   debug_printf("VoicePrompt(GUNPASS);\r\n");
	 }	 

	debug_printf("after Check_Gun() \r\n");
	debug_printf("/////////p_req_gun=>"); 
  print_gun_checking(p_req_gun);
	debug_print_nr();
	debug_printf("/////////p_read_gun=>");
	print_gun_checking(p_read_gun);

	return ret;
}

