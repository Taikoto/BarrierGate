
 
/* 
 *  包含头文件
 */
#include <string.h>
#include "delay.h" 	
#include "global.h"
#include "USARTx.H"
#include "debug_print.h"
#include "comm_recv.h"
#include "rfid.h"

#define read_rfid_txd_data_size  sizeof(read_rfid_txd_data)	

//const u8 rfid_set_bound115200[6] = {0xa0,0x04,0xa9,0x00,0x04,0xaf};   // 设置波特率位 115200
const u8 rfid_set_bound57600[6]   = {0xa0,0x04,0xa9,0x00,0x03,0xb0};   // 设置波特率位 57600
const u8 rfid_set_bound9600[6]   = {0xa0,0x04,0xa9,0x00,0x00,0xb3};   // 设置波特率位 9600
const u8 rfid_bound_reply[6] = {0xe4,0x04,0xa9,0x00,0x00,0x6f};  // 波特率设置成功应答

//const u8 read_rfid_txd_data[15] = {0xa0,0x03,0x82,0x00,0xdb,0xa0,0x03,0x82,0x00,0xdb,0xa0,0x03,0x82,0x00,0xdb}; // 读取RFID标签指令
const u8 read_rfid_txd_data[10] = {0xa0,0x03,0x82,0x00,0xdb,0xa0,0x03,0x82,0x00,0xdb}; // 读取RFID标签指令
u8 READ_LABEL_SUCCESS[4] = {0xe0,0x10,0x82,0x00}; 
//u8  AS60X_HEAD_SERVER_CMD[8]    ={0xef,0x02,0xff,0xff,0xff,0xff,0x01,0x00}; 	

const u8 reset_rfid_cmd[10] = {0xa0,0x03,0x65,0x00,0xf8,0xa0,0x03,0x65,0x00,0xf8}; // 复位读卡器指令，用于定时模式时启动读卡器
const u8 stop_rfid_cmd[10] = {0xa0,0x03,0xa8,0x00,0xb5,0xa0,0x03,0xa8,0x00,0xb5};  // 停止读取标签指令，用于定时模式时停止读标签
u8 read_rfid_rxd_data[12] = {0};    // RFID 数据接收缓存区

u8 read_rfid_flag = 0;            // RFID 启动读取数据标志位
u8 read_rfid_data_flag = 0;       // RFID 读到一帧数据标志位
u8 read_rfid_repeat_flag = 0;     // RFID 重复读标志位
u8 send_rfiddata_to_Pc_flag = 0;  // RFID 数据发送给pc标志位

unsigned int uiRcMoveIndex = 0;


void rfid_init(void)
{
  //USART3_transmitS((u8*)rfid_set_bound9600,6);
	//USART3_transmitS((u8*)rfid_set_bound9600,6);    // 设置波特率位9600， 设备号0	
	
	USART_transmitS(g_pcom_rfid,(u8*)rfid_set_bound57600,6);    // 设置波特率位57600， 设备号0		
	USART_transmitS(g_pcom_rfid,(u8*)rfid_set_bound57600,6);    // 设置波特率位57600， 设备号0	
	
}

//void SendData(COMM_USART *pcomm,u8 *data,u16 len)
u8 Send_Cmd_To_rfid(COMM_USART *pcomm)
{
	u8 ret=0x0;	 
 
		USART_transmitS(pcomm,(u8*)read_rfid_txd_data,read_rfid_txd_data_size);
	
	return ret;
}

//return 0: read ok, output in label_buf; others : read  failed 
u8 Read_Data_From_rfid(COMM_USART *pcomm,u8 *label_buf)
{
	u8  ret=0xff;
	u8  *data;
  u8  frame_checksum=0;
	u8  temp_checksum=0;
	u8  frame_len=0;
 	u16 label_len=0;
	u16 i,j;

	COMM_USART *p_usart = pcomm;
	
	uint8_t *rx_buf_judge_from=p_usart->rx_buf + p_usart->rx_use; 
	
 	
	  data=strstr((const char*)rx_buf_judge_from,READ_LABEL_SUCCESS);  //找到包头，返回包头地址，找不到，返回0

  	//debug_printf("in Read_Data_From_rfid(),p_usart->rx_num=%d,data=0x%x\r\n",p_usart->rx_num,data);
    //debug_print_buf(rx_buf_judge_from,16);
	
	  ret = 0xff;	    		 
		//debug_print_conti(label_buf,RFID_LABEL_LEN);					 

	if(data) //receive answer ok
	{
     frame_len=data[1];	
	   frame_checksum=data[1+frame_len];

		 debug_printf("read label success!data=0x%x,frame_len=%d\r\n",data,frame_len);

		//unsigned char CheckSum(unsigned char *ucBuff,unsigned char ucBuffLen)   // 校验和计算
     //checksum_len= frame_len - 1 + 2;
		
		 temp_checksum=CheckSum(data,frame_len - 1+2);		 		  
		
		 if(frame_checksum==temp_checksum)
		 {
			    debug_printf("checksum ok=%d\r\n",frame_checksum);
			 
					for(j = 0; j< RFID_LABEL_LEN;j++)  //RFID_LABEL_LEN=12
					{
						label_buf[j] = data[5+j];
					}		
					label_len=12;		

          ret=0;					
		 }
		 else
		 { 
			  debug_printf("checksum error, frame_checksum=%d, temp_checksum=%d\r\n",frame_checksum,temp_checksum);			 
			  ret=1;
		 } 		  
		
		//debug_print_conti(label_buf,RFID_LABEL_LEN);					 		  
	}
  //else
     //debug_printf("rfid read data return error,\r\n");
 	
	return ret;
}

u8 Rfid_Read_Label_Base_Proc(COMM_USART *pcomm_rfid,u8 *label_buf,u8 *p_waittime_left)
{

	u8 ret=0xff;
	
  u8 waittime = READ_LABEL_BASE_COUNT+1;
	
	*p_waittime_left=waittime;
		
	while(--waittime)
	{
		
		RX_Clear(pcomm_rfid);
		memset(label_buf,0,RFID_LABEL_LEN);
		
		Send_Cmd_To_rfid(pcomm_rfid);
	  
		delay_ms(READ_LABEL_BASE_INTERVAL_MS);  //must>200ms
    
		ret=Read_Data_From_rfid(pcomm_rfid,label_buf);
		 
		
		if(ret==0)
		{
			debug_printf("label read OK!\r\n");
		  debug_print_conti(label_buf,RFID_LABEL_LEN);
       
			break;
		}
    
	}
	
	*p_waittime_left=waittime;
	
	return ret;
}


unsigned char CheckSum(unsigned char *ucBuff,unsigned char ucBuffLen)   // 校验和计算
{
  unsigned char i = 0;
	unsigned char ucSum = 0;
	for( i = 0; i< ucBuffLen; i++)
	{
	  ucSum = ucSum + ucBuff[i];
	}
	ucSum = (~ucSum)+1;
	
	return ucSum;
}
