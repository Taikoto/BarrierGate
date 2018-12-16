
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdio.h>

#include "stm32f10x.h"

#include "stdarg.h"	 	 
#include "string.h"	   

//#include "as606.h"
//#include "USARTx.H"

#define MODEL_LEN             512           //FP charter/model data len =512
#define RFID_LABEL_LEN         12            //RFID LABEL LEN
#define MAX_C_GUN_NUM          10            //MAX_CHECKING_GUN_NUM

#define MAX_STORE_FP_NUM 1024        //FP sensor max store numbers

typedef struct
{
  USART_TypeDef *usart;	     /* USART1,USART2,USART3 */
 	uint8_t *tx_buf;			     /* send buf */
	uint8_t *rx_buf;			     /* receive buf */
	uint16_t rx_num;		       /* received num, bytes ,max=rx_maxlen, not loop*/
	uint16_t rx_last_num;		   /* last timer tick received num, use to watching receiving end */
	uint8_t  key_in;           /* receive: key input, enter key(0x0d) as end */
	uint16_t rx_begin;         /* passed head begin locate in rx_buf *///合格包头在rx_buf相对起始位置(前面可能有一串乱字符)，0 to rx_num
	
	uint16_t rx_use;		        /* read num, max rx_use=rx_num */
	uint16_t rx_state;		      /* 0--no any data,1--receiving, 2--receiv end ,wait for process*/
	uint16_t rx_MAXLEN;		      /* rx buf len = USART_RX_LEN */
 	void (*send_over)(void);   	/* after send, recall function , NULL */
	void (*receive_new)(void);	/* after send, recall function , NULL */
}COMM_USART;


typedef struct  
{
	u16 pageID;    // 
	u16 mathscore; //
}SearchResult;

typedef struct
{
	u16 PS_max;    //FP max store, 1000 or 300 
	u8  PS_level;  //securty level = 3
	u32 PS_addr;
	u8  PS_size;   //size ratio , 0=32,1=64,2=128,3=256
	u8  PS_N;      //baud ratio(1~12) 9600xPS_N, default:57600
}SysPara;


typedef struct
{
	u16 fp_id;
	
	u8  downBufferID;  //1 or 2, use FP module CharBuffer1 or CharBuffer2 ?
	
	u8  *p_model_buf1;  // point to g_fp_model_buf1[MODEL_LEN];
	
	u8  *p_model_buf2;  // point to g_fp_model_buf2[MODEL_LEN];
	
	u8  checksum_result;    //0 means ok, others fail
	
	u8  comm_server_result; //0 means ok, others fail
	
	u8  down_store_result;  //0 means ok, others fail
	
	u8  ack_status;         //0 means ok, others fail

}FP_SYNC;
//global_var  g_fp_sync;


typedef struct
{
	u8  seq;        //gun seq，
	//if come from sever, it is send seq
	//if barrier RFID read, it is check seq
  u8  match_seq;   //gun mach which seq?  >=100: means not match
	//check seq match which req seq?
  
	u8  mismatched;  //=0:match, others: not match
	
	u8  label[RFID_LABEL_LEN]; 
	
}GUN_LABEL; 


//global var: GUN_CHECKING g_req_check_gun
//            GUN_CHECKING g_real_check_gun
typedef struct
{
	u8 gun_num;          // total how many gun need check/checked?

  //gun_num=gun_check_match+gun_not_match
	
	u8 match_num;    //how many gun matching?  //0:match 0 gun, 1:match 1 gun, ...

	u8 mismatch_num;    //how many gun not matched?  //0:match 0 gun, 1:match 1 gun, ...
 	
	GUN_LABEL gun_label[MAX_C_GUN_NUM];  //gun seq and gun label read by RFID

}GUN_CHECKING;


//about server communication:
//usually I am receiving from Server, check_ServerCmd() state=2 ->RecvCmdFromServer(g_pcom_ser,500,&g_fp_id,g_mcu_model_buf1);
//server cmd byte[6]=01,byte[09]=08 means FP SYNC
//I ack byte[6]=07,byte[09]=00, means download ok
//-----------------------------------------------
//
//after FP check pass, I send request to server
//I send REQ to server(byte[6]=02,byte[9]=07 to server),
//I send byte[6]=02,byte[9]=07. means req open barrier. 
//server answer REQ other checking message, and response checking result
//server answer byte[6]=01,byte[9]=0x0A,0x09,0x0C
//	u8   answer_cmd;   //byte[9]=0x0A -- open directly
//	                   //byte[9]=0x09 -- forbidden
//	                   //byte[9]=0x0C -- need check gun
//store_id gun_num gun1_label gun2_label .... checksum



//------------------------------------
//barrier checking status
//global var : g_barrier_checking_follow

#define FLOW_NO_IN_WORKING              0   //not in working 
#define FLOW_FPID_FORBIDDEN             1   //forbidden
#define FLOW_NETWORK_BREAK_OPEN         2   //when power down / newwork break, supervisior open
#define FLOW_SUPERVISIOR_OPEN           3   //supervisior open ( cmd=0A )
#define FLOW_DIRECTLY_OPEN              4   //directly open
#define FLOW_NO_NEED_CHECK_FP_OPEN      5   
#define FLOW_FP_PASS_OPEN               6   //fp pass open
#define FLOW_FP_PASS_NEED_CHECK_GUN     7   //need check gun 
#define FLOW_FP_CHECK_GUN_NOT_PASS      8
#define FLOW_FP_PASS_GUN_PASS           9
#define FLOW_PROTOCOL_ERROR             100

typedef struct
{
	u8 type;   //
	//0: not in working
	//1: supervisior open(directly open)
  //2: no need check fp open(directly open)
	//3: need check fp open
	//4: need check fp and gun open
	//5: forbiden
	//>=100, comm protocol error;
	
	u8  cmd;  //come from server's cmd

	u16 fp_id;         //init:0xffff
  
	u8  fp_id_status;  //init:0xff
	
	u32 open_id;       //init:0xffffffff
	
	
	u8  ack_status;   //flow result , include midway/halfway error
		
	u8  fp_check_result;   //init:0xff//0: pass, others: not pass
	
	GUN_CHECKING *p_req_check_gun;     //point to g_req_check_gun[10];
	
	GUN_CHECKING *p_real_check_gun;    //point to g_real_check_gun[10];
	
	u8  gun_check_result;  //init:0xff//0: pass, others: not pass
	
	u8  barrier_status;    //0: close, 1:open ; init:0

}BARRIER_CHECKING_FLOW;


#define MAX_NETWORK_WAIT_MS 3000
#define NETWORK_OK     0
#define NETWORK_BREAK  1

typedef struct
{
	u16 main_loop_count;
	
	u16 wait_ms;
	
	u16 max_wait_ms;
	
	u8  wait_recv_pending;           //0: no pending receive; 1: pending receive,count wait_ms
	
  u8  network_status;              //0: network ok; 1: network break
	
}NETWORK_HANDSHAKE;
//----------------------------------------

extern COMM_USART g_usart1;
extern COMM_USART g_usart2;
extern COMM_USART g_usart3;

extern COMM_USART g_usartvirt;    //virtual usart, realize by timer && gpio

extern COMM_USART *g_pcom_fp ;  //= &g_usart1;   // define USART1 used for finger print
extern COMM_USART *g_pcom_ser ; //= &g_usart2;   // define USART2 used for server comm
extern COMM_USART *g_pcom_rfid; //=&g_usart3;

extern COMM_USART *g_pcom_virt;
extern COMM_USART *g_p_usart_debug ; // =&g_usartvirt;
extern COMM_USART *g_p_debug_recv;   // =&g_usart3;

extern GUN_CHECKING g_req_check_gun;
extern GUN_CHECKING g_real_check_gun;

extern BARRIER_CHECKING_FLOW g_barrier_check_flow;
extern BARRIER_CHECKING_FLOW *g_p_flow;  //=&g_barrier_check_flow;

extern FP_SYNC g_fp_sync;

extern NETWORK_HANDSHAKE g_network_handshake;
extern u8 g_network_status;

extern u16          g_fp_frame_data_len ; //=128;     

extern u8           g_fp_byte32[4][32] ;     //g_fp_byte32[4][32]={0};
extern SearchResult g_fp_seach; //={0,0};
extern SysPara      g_fp_syspara;    //={0,0,0,0,0};

extern u8 g_fp_model_buf1[] ;   //g_fp_model_buf1[MODEL_LEN]={0};  //CharBuffer1 to mcu  
extern u8 g_fp_model_buf2[] ;   //g_fp_model_buf2[MODEL_LEN]={0};  //CharBuffer2 to mcu  
 
extern u8 g_mcu_model_buf1[];   //g_mcu_model_buf1[MODEL_LEN]={0};  //mcu to CharBuffer1 
extern u8 g_mcu_model_buf2[];   //g_mcu_model_buf2[MODEL_LEN]={0};  //mcu to CharBuffer2 

extern u8 g_downStoreBufferID;  //=1

extern u8 g_ack_status;  //00--ok ,others --- failed

extern u16 g_debug_detail;   //0 -- normal; 1 -- print debug info

#endif
