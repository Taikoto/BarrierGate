

#ifndef __RFID__H__
#define __RFID__H__

/* 
 *  包含头文件
 */
#include "USARTx.H"
#include "sys.h"
#include "delay.h"
#include "string.h"


#define READ_LABEL_BASE_COUNT        20
#define READ_LABEL_BASE_INTERVAL_MS  300  //ms

extern u8 read_rfid_rxd_data[];   // RFID 数据接收缓存区

extern u8 read_rfid_flag;         // RFID 启动读取数据标志位
extern u8 read_rfid_data_flag;    // // RFID 读到一帧数据标志位
extern u8 read_rfid_repeat_flag;  // RFID 重复读标志位
extern u8 send_rfiddata_to_Pc_flag;  // RFID 数据发送给pc标志位

extern void rfid_init(void);

extern u8 Send_Cmd_To_rfid(COMM_USART *pcomm);

extern u8 Read_Data_From_rfid(COMM_USART *pcomm,u8 *label_buf); 

extern unsigned char CheckSum(unsigned char *ucBuff,unsigned char ucBuffLen);   // 校验和计算
  
extern u8 Rfid_Read_Label_Base_Proc(COMM_USART *pcomm_rfid,u8 *label_buf,u8 *p_waittime_left);

#endif

