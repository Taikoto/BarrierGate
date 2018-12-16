

#include <string.h>
#include "delay.h" 	
#include "global.h"
#include "USARTx.H"
#include "debug_print.h"
#include "comm_recv.h"
#include "as606.h"

u32 AS606Addr = 0XFFFFFFFF; //默认


//发送包头
/*
static void SendHead(void)
{
	//USART1_transmit(0xEF);
	//USART1_transmit(0x01);
	USART_transmit(p_fp_comm,0xEF);
	USART_transmit(p_fp_comm,0x01);	
}
*/

static void SendAS606Head(COMM_USART *pcomm)
{
	//USART1_transmit(0xEF);
	//USART1_transmit(0x01);
	USART_transmit(pcomm,0xEF);
	USART_transmit(pcomm,0x01);	
}


//发送地址
/*
static void SendAddr(void)
{
	//USART1_transmit(AS606Addr>>24);
	//USART1_transmit(AS606Addr>>16);
	//USART1_transmit(AS606Addr>>8);
	//USART1_transmit(AS606Addr);
	USART_transmit(p_fp_comm,AS606Addr>>24);
	USART_transmit(p_fp_comm,AS606Addr>>16);
	USART_transmit(p_fp_comm,AS606Addr>>8);
	USART_transmit(p_fp_comm,AS606Addr);	
}
*/
static void SendAS606Addr(COMM_USART *pcomm)
{
	//USART1_transmit(AS606Addr>>24);
	//USART1_transmit(AS606Addr>>16);
	//USART1_transmit(AS606Addr>>8);
	//USART1_transmit(AS606Addr);
	USART_transmit(pcomm,AS606Addr>>24);
	USART_transmit(pcomm,AS606Addr>>16);
	USART_transmit(pcomm,AS606Addr>>8);
	USART_transmit(pcomm,AS606Addr);	
}


//发送包标识,
/*
static void SendFlag(u8 flag)
{
	//USART1_transmit(flag);
	USART_transmit(p_fp_comm,flag);
}
*/
/*
static void SendFlag(COMM_USART *pcomm,u8 flag)
{
	//USART1_transmit(flag);
	USART_transmit(pcomm,flag);
}
*/
//发送包长度
/*
static void SendLength(int length)
{
	//USART1_transmit(length>>8);
	//USART1_transmit(length);

	USART_transmit(p_fp_comm,length>>8);
	USART_transmit(p_fp_comm,length);
	
}
*/
/*
static void SendLength(COMM_USART *pcomm,int length)
{
	//USART1_transmit(length>>8);
	//USART1_transmit(length);
	USART_transmit(pcomm,length>>8);
	USART_transmit(pcomm,length);	
}
*/
//发送指令码
/*
static void Sendcmd(u8 cmd)
{
	//USART1_transmit(cmd);
	USART_transmit(p_fp_comm,cmd);
}
*/
/*
static void Sendcmd(COMM_USART *pcomm,u8 cmd)
{
	//USART1_transmit(cmd);
	USART_transmit(pcomm,cmd);
}
*/
//发送校验和
/*
static void SendCheck(u16 check)
{
	//USART1_transmit(check>>8);
	//USART1_transmit(check);

	USART_transmit(p_fp_comm,check>>8);
	USART_transmit(p_fp_comm,check);
	
}
*/
/*
static void SendCheck(COMM_USART *pcomm,u16 check)
{
	//USART1_transmit(check>>8);
	//USART1_transmit(check);

	USART_transmit(pcomm,(check)>>8);
	USART_transmit(pcomm,check);
	
}
*/
//发送数据
/*
static void SendData(u8 *data,u16 len)
{
 
  USART_transmitS(p_fp_comm,data,len);	
}	
*/
/*
static void SendData(COMM_USART *pcomm,u8 *data,u16 len)
{
 
  USART_transmitS(pcomm,data,len);
}	
*/
//判断中断接收的数组有没有应答包
//waittime为等待中断接收数据的时间（单位1ms）
//返回值：数据包首地址

u8  AS60X_TEST[7]     ={0x30,0x31,0x32,0x33,0x34,0x35,0x0};
u8  AS60X_HEAD[7]     ={0xef,0x01,0xff,0xff,0xff,0xff,0x0};
u8  AS60X_ACK[8]      ={0xef,0x01,0xff,0xff,0xff,0xff,0x07,0x0};
u8  AS60X_DATA[8]     ={0xef,0x01,0xff,0xff,0xff,0xff,0x02,0x0};   //数据包，不是最后一个
u8  AS60X_DATA_LAST[8]={0xef,0x01,0xff,0xff,0xff,0xff,0x08,0x0};   //数据包，最后一个


//static u8 *JudgeStr(u16 waittime)
//static u8 *JudgeStr_ori_debug(u16 waittime,u8 *jstr)
//static u8 *JudgeStr_ori(u16 waittime,u8 *jstr)
static u8 *JudgeStr_ori(COMM_USART *pcomm,u16 waittime,u8 *jstr)
{
	char *data;
	//COMM_USART *p_usart = p_fp_comm;
	COMM_USART *p_usart = pcomm;
	
	uint16_t i;
	
 	u8 str[8];
	u8 str_test[8]={0};
	str[0]=0xef;str[1]=0x01;str[2]=AS606Addr>>24;
	str[3]=AS606Addr>>16;str[4]=AS606Addr>>8;
	str[5]=AS606Addr;str[6]=0x07;str[7]='\0';
	str_test[0]=0x30; 	str_test[1]=0x31; 	str_test[2]=0x32; 	str_test[3]=0x33; 	str_test[4]=0x34;	str_test[5]=0x35; 	str_test[6]=0x36; 	str_test[7]='\0'; 
	
	//USART1_RX_STA=0;
	//debug_printf("JudgeStr()/before while()/p_usart->rx_num=%d,p_usart->rx_state=%d\r\n",p_usart->rx_num,p_usart->rx_state);
	debug_printf("JudgeStr()/before while()/jstr=");
			for(i=0;i<7;i++)
			   debug_printf("0x%x ",jstr[i]);

	debug_print_nr();

	debug_printf("JudgeStr()/before while()/str=");
			for(i=0;i<7;i++)
			   debug_printf("0x%x ",str[i]);

	debug_print_nr();
	 
	debug_printf("JudgeStr()/before while()/str_test=");
			for(i=0;i<7;i++)
			   debug_printf("0x%x ",str_test[i]);

	debug_print_nr();	 
	
	while(--waittime) //找waittime次
 	//while(1)
	{
		delay_ms(20);
		//if(USART1_RX_STA&0X8000)  //接收到一次数据
 		//debug_printf("JudgeStr()/while(1)/usart_rx1_num=%d,usart_rx1_end=%d\r\n",usart_rx1_num,usart_rx1_end);
		
			//debug_printf("JudgeStr()/p_usart->rx_num=%d,p_usart>rx_state=%d\n\r",p_usart->rx_num,p_usart->rx_state);
		
		if(p_usart->rx_state==2)   //接收到完整一帧数据
		{
			//USART1_RX_STA=0;
 
			debug_print_nr();

			//debug_printf("JudgeStr()/p_usart->rx_num=%d,p_usart>rx_state=%d\n\r",p_usart->rx_num,p_usart->rx_state);

			//data=strstr((const char*)USART1_RX,(const char*)jstr);
			data=strstr((const char*)p_usart->rx_buf,(const char*)str_test);   //找到包头，返回包头地址，找不到，返回0
      
			if((uint32_t)data > (uint32_t)p_usart->rx_buf)  //如果搜到，rx_begin 为搜到的首地址
			  p_usart->rx_begin=(uint32_t)data - (uint32_t)p_usart->rx_buf;
			else
				p_usart->rx_begin=0;  //搜不到，rx_begin没意义
			
			p_usart->rx_use += p_usart->rx_begin;
			
			debug_printf("JudgeStr()/data=0x%x,*data=0x%x,rx_begin=%d,USART_RX= ",data,*data,p_usart->rx_begin);
			
			for(i=0;i<7;i++)
			   debug_printf("%c",p_usart->rx_buf[p_usart->rx_begin+i]);
			
			debug_print_nr();
			debug_print_nr();
			
			if(data)
 				return (u8*)data;   //返回包头地址，
		}
	}
 	return 0;
}

//wait waittime times, search jstr in rx_buf
//u8 *JudgeStr_0225(u16 waittime,u8 *jstr)
u8 *JudgeStr_0225(COMM_USART *pcomm,u16 waittime,u8 *jstr)
{
	char *data;
	//COMM_USART *p_usart = p_fp_comm;
	COMM_USART *p_usart = pcomm;
	
	uint8_t *rx_buf_judge_from=p_usart->rx_buf + p_usart->rx_use;
	
	uint16_t i;

	debug_printf("JudgeStr()/before while()/jstr=");
			for(i=0;i<7;i++)
			   debug_printf("0x%x ",jstr[i]);

	
	while(--waittime) //找waittime次 
	{
		delay_ms(20);

		debug_printf("p_usart->rx_state=%d\n\r",p_usart->rx_state);
 		
		if(p_usart->rx_state==2)   //接收到完整一帧数据
		{
			
			debug_print_nr();
 
			//data=strstr((const char*)USART1_RX,(const char*)str);
 			data=strstr((const char*)rx_buf_judge_from,(const char*)jstr);   //找到包头，返回包头地址，找不到，返回0
 
			if(data!=0)
			{	
				 //debug_printf("received from finger answer!\n\r");
				 debug_printf("received from finger answer!\n\r");
				 if(data[9]==0)
					 debug_printf("answer ok!!\n\r");
			}	
			
 			if((uint32_t)data >= (uint32_t)rx_buf_judge_from)  //如果搜到，rx_begin 为搜到的起始位置， data为起始位置的地址
			  p_usart->rx_begin=(uint32_t)data - (uint32_t)rx_buf_judge_from;
			else
				p_usart->rx_begin=0;  //搜不到，rx_begin 没意义

			p_usart->rx_use += p_usart->rx_begin;
			
			debug_printf("JudgeStr()/data=0x%x,*data=0x%x,data[9]=%d,rx_begin=%d,rx_use=%d, USART_RX= ",data,*data,data[9],p_usart->rx_begin,p_usart->rx_use);
			
			for(i=0;i<7;i++)
			   debug_printf("0x%x",rx_buf_judge_from[p_usart->rx_begin+i]);
			
			debug_print_nr();
			debug_print_nr();
			
			if(data)
 				return (u8*)data;   //返回包头地址，
		}
	}
 	return 0;
}

 
//录入图像 PS_GetImage
//功能:探测手指，探测到后录入指纹图像存于ImageBuffer。 
//模块返回确认字

//u8 PS_GetImage(void)
u8 PS_GetImage(COMM_USART *pcomm)
{
  u16 temp;
  u8  ensure;
	u8  *data;
	
	//while(1)
	{
    if(pcomm->rx_num!=0)
	    RX_Clear(pcomm); //先清理接收缓冲区，准备接收应答数据

	  /*发命令*/        //session state = 1  ,发命令
	 
	  SendAS606Head(pcomm);
	  SendAS606Addr(pcomm);
	  SendFlag(pcomm,0x01);    //命令包标识
	  SendLength(pcomm,0x03);
	  Sendcmd(pcomm,0x01);
    temp =  0x01+0x03+0x01;   //包标识+包长度+包内容
	  SendCheck(pcomm,temp);
		
	  //debug_printf("in PS_GetImage()/after sendCheck(),wait JudgeStr(50)\n\r");

    /*等应答，收数据*/ //session state = 2 ,等接收
	
 	  data=JudgeStr(pcomm,200,AS60X_ACK);   //去等待接收数据包完成 ，并看包头是否匹配，循环检测20次，每次10ms，最多等待500ms 	 		
	  
		//GetImage()指令发出后，
		//如果无图像，大约100ms返回，状态字节data[9]!=0,
		//如果有图像，大约300ms~500ms返回，状态字节data[9]=0，展开下面的流程
		
		
		
	  /*等完 ，处理应答结果， 已经等了500ms*/  //session state = 3，处理接收结果
	
	  if(data)   //已等到且有应答
	  {
		  ensure=data[9];		//应答结果, 00--录入成功；01--包有错；02--传感器上无手指； 03--录入不成功  
		                  //data为应答包首地址，注意，首地址不一定为包头
	  }
	  else      //未等到，或者有数据但不是应答包   
		  ensure=0xff;	
	
	  pcomm->rx_use += 12;  // 应答包长为12，下次如果再读取接收rx_buf，从 rx_buf 内部的 rx_use 相对位置开始
  
   }
	
	return ensure;     //回复应答结果
	
}

//生成特征 PS_GenChar
//功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2			 
//参数:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//模块返回确认字
//u8 PS_GenChar(u8 BufferID)
u8 PS_GenChar(COMM_USART *pcomm,u8 BufferID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
 
  if(pcomm->rx_num!=0)
   	RX_Clear(pcomm); //先清理接收缓冲区，准备好接收应答数据

	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x04);
	Sendcmd(pcomm,0x02);
	//USART1_transmit(BufferID);
	USART_transmit(pcomm,BufferID);
	temp = 0x01+0x04+0x02+BufferID;
	SendCheck(pcomm,temp);
	
	//debug_printf("PS_GenChar() before goto JudgeStr()\n");
	
	data=JudgeStr(pcomm,200,AS60X_ACK);
	
	if(data)
		ensure=data[9];
	else
		ensure=0xff;

	//debug_printf("PS_GenChar()/after  JudgeStr()/ensure=0x%x\n",ensure);
	g_debug_detail=0;
	
	pcomm->rx_use +=  12;   
  
	return ensure;
}
//精确比对两枚指纹特征 PS_Match
//功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件 
//模块返回确认字
//u8 PS_Match(void)
u8 PS_Match(COMM_USART *pcomm)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm);  

	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x03);
	Sendcmd(pcomm,0x03);
	temp = 0x01+0x03+0x03;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,500,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;

	pcomm->rx_use += 12;   
	
	return ensure;
}
//搜索指纹 PS_Search
//功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码。			
//参数:  BufferID @ref CharBuffer1	CharBuffer2
//return 0--ok = 搜索到, others--fail
//u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
u8 PS_Search(COMM_USART *pcomm,u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;

  if(pcomm->rx_num!=0)
   	RX_Clear(pcomm);  
	
	p->pageID=0;
	p->mathscore=0;
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x08);
	Sendcmd(pcomm,0x04);
	
	//USART1_transmit(BufferID);
	//USART1_transmit(StartPage>>8);
	//USART1_transmit(StartPage);
	//USART1_transmit(PageNum>>8);
	//USART1_transmit(PageNum);

	USART_transmit(pcomm,BufferID);
	USART_transmit(pcomm,StartPage>>8);
	USART_transmit(pcomm,StartPage);
	USART_transmit(pcomm,PageNum>>8);
	USART_transmit(pcomm,PageNum);
	
	temp = 0x01+0x08+0x04+BufferID
	+(StartPage>>8)+(u8)StartPage
	+(PageNum>>8)+(u8)PageNum;
	SendCheck(pcomm,temp);
	
	data=JudgeStr(pcomm,500,AS60X_ACK);
	
	if(data)
	{
		ensure = data[9];
		p->pageID   =(data[10]<<8)+data[11];
		p->mathscore=(data[12]<<8)+data[13];
	}
	else
		ensure = 0xff;

	pcomm->rx_use +=  12;   
	
	return ensure;	
}
//合并特征（生成模板）PS_RegModel
//功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2	
//说明:  模块返回确认字
//u8 PS_RegModel(void)
u8 PS_RegModel(COMM_USART *pcomm)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	
	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x03);
	Sendcmd(pcomm,0x05);
	temp = 0x01+0x03+0x05;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;

	pcomm->rx_use += 12;   
	
	return ensure;		
}

//储存模板 PS_StoreChar
//功能:将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置。			
//参数:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID（指纹库位置号）
//说明:  返回0 表示成功
//u8 PS_StoreChar(u8 BufferID,u16 PageID)
u8 PS_StoreChar(COMM_USART *pcomm,u8 BufferID,u16 PageID)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x06);
	Sendcmd(pcomm,0x06);
	 
	USART_transmit(pcomm,BufferID);
	USART_transmit(pcomm,PageID>>8);
	USART_transmit(pcomm,PageID);
	
	temp = 0x01+0x06+0x06+BufferID
	+(PageID>>8)+(u8)PageID;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;

	pcomm->rx_use += 12;   
	
	return ensure;	
}

//
//ZACK Lynn ,20170224
//load from flash , PageID to BufferID; BufferID=1 or 2; PageID=0 to 999
//
//u8 PS_LoadChar(u8 BufferID,u16 PageID)
u8 PS_LoadChar(COMM_USART *pcomm,u8 BufferID,u16 PageID)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x06);
	Sendcmd(pcomm,0x07);
	 
	USART_transmit(pcomm,BufferID);
	USART_transmit(pcomm,PageID>>8);
	USART_transmit(pcomm,PageID);
	
	temp = 0x01+0x06+0x07+BufferID
	+(PageID>>8)+(u8)PageID;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;

	pcomm->rx_use += 12;   
	
	return ensure;	
}

//
//ZACK Lynn ,20170224
//load from module's BufferID to my MCU upchar_buf,BufferID=1 or 2; upchar_buf len == 512;
//if checksum ok , return up_len
//if checksum failed , return 0
//u16 PS_UpChar(u8 BufferID, u8 *upchar_buf)
//MCU receive
u16 PS_UpChar(COMM_USART *pcomm,u8 BufferID, u8 *upchar_buf)
{
	
	u16 temp=0;
	u16 temp_total=0;
	
	u16 frame_checksum=0;
	u16 temp_puredata=0;

	u8  checksum_failed=0;
  u8  ensure;
	u8  *data;
	u8  *dframe=0;
	u8  *rx_buf_left=pcomm->rx_buf;
	u8  *rx_buf_left_by_use=pcomm->rx_buf;
 	
	u16 i=0;
	u16 last_data_package=0;
	u16 up_len=0;
	u16 frame_len=0;
 
	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x04);
	Sendcmd(pcomm,0x08);
	
	USART_transmit(pcomm,BufferID);
	//USART_transmit(p_fp_comm,PageID>>8);
	//USART_transmit(p_fp_comm,PageID);
	
	temp = 0x01+0x04+0x08+BufferID;
 	SendCheck(pcomm,temp);
	
	data=JudgeStr(pcomm,100,AS60X_ACK);
	
	if(data)
		ensure=data[9];
	else
		ensure=0xff;

	  pcomm->rx_use += 12;  // 应答包长为12，下次如果再读取接收缓冲buf，从rx_buf[] + rx_use 开始
    rx_buf_left +=12;
	  //rx_buf_left_by_use = pcomm->rx_buf + pcomm->rx_use;
	
	  delay_ms(50);	
	 	
		if(ensure == 0x00)   //next step ,continue receive data package			
		{
				temp=0;   //local checksum, one time each frame
				temp_total=0;  //all loop temp ++ ,== 512 bytes ++
			  temp_puredata = 0;  //checksum not include package type , include data and length
			
			do{
				   g_debug_detail=0;
				
   				 dframe=JudgeStr_UpChar(pcomm,100,AS60X_HEAD,rx_buf_left);
				   g_debug_detail=0;
				
 				   temp = 0; //local checksum, one time each frame
				   //dframe=JudgeStr_UpChar(100,AS60X_DATA);
			     if(dframe)  //fit frame head
				   {
 					    if(dframe[6]==0x02||dframe[6]==0x08)  //it is data frame
					    {
 
 						    frame_len = (dframe[7]<<8) + dframe[8]; 
								temp += dframe[6] + dframe[7] + dframe[8];
								
								//debug_printf("temp=0x%x,dframe[6]=0x%x,dframe[7]=0x%x,dframe[8]=0x%x,frame_len=0x%x=%d\r\n",temp,dframe[6],dframe[7],dframe[8],frame_len,frame_len);
								
						    //debug_printf("p_fp_comm->rx_buf=0x%x,rx_buf_left=0x%x,dframe[6]=0x%x , frame_len=%d, up_len=%d \r\n",p_fp_comm->rx_buf,rx_buf_left, dframe[6],frame_len,up_len);

						    if(dframe[6]==0x08)
								{	
							     last_data_package=1;								   																	
								   //debug_printf("last data frame,frame_len=%d ",frame_len);
								}	
								else
								{	
							     last_data_package=0;								   																	
								   //debug_printf("data frame,frame_len=%d  ",frame_len);
								}	

 								 
						    for(i=0;i<frame_len-2;i++)
								{
						        upchar_buf[up_len++] = dframe[9+i];	 
								    temp += dframe[9+i];
									  temp_puredata += dframe[9+i];
							  }  
								
								frame_checksum = ((dframe[9+frame_len-2]) << 8) + dframe[9+frame_len-1];
								
								pcomm->rx_use += 9 + frame_len;         //数据包长度为 9 + frame_len；下次再读取接收缓冲区，从 rx_use 开始
								
								rx_buf_left = dframe + 9 + frame_len;
								rx_buf_left_by_use = pcomm->rx_buf + pcomm->rx_use;
								
								//debug_printf("p_fp_comm->rx_buf=0x%x,dframe=0x%x,rx_buf_left=0x%x,rx_buf_left_by_use=0x%x\r\n",p_fp_comm->rx_buf,dframe,rx_buf_left,rx_buf_left_by_use);
								//left buf ok: rx_buf_left=rx_buf_left_by_use;
								//
								
								//debug_printf("frame_len=%d,temp=0x%x,frame_checksum=0x%x,dframe[9+frame_len-2]=0x%x,dframe[9+frame_len-1]=0x%x\r\n",frame_len,temp,frame_checksum,dframe[9+frame_len-2],dframe[9+frame_len-1]);
								//checksum ok: temp == frame_checksum
								if(temp == frame_checksum) 
								{		
									//debug_printf("frame up data checksum ok!,temp=0x%x\r\n",temp);
									checksum_failed=0;
								} 
								else
								{	
									checksum_failed++;
									debug_printf("frame up data checksum failed!\r\n");
									debug_printf("frame_len=%d,temp=0x%x,frame_checksum=0x%x,dframe[9+frame_len-2]=0x%x,dframe[9+frame_len-1]=0x%x\r\n",frame_len,temp,frame_checksum,dframe[9+frame_len-2],dframe[9+frame_len-1]);
								}	
								
 					    }
					    else  // isn't data frame
						    break;

							temp_total += temp;
							//temp_puredata += (temp - dframe[6] - dframe[7] -dframe[8]);							 	
							
			    }
				  else   //no fit frame head
					   break;

 			 
			}while(!last_data_package);
			
		 }
     		  
	   if(checksum_failed)
		 { 
			 debug_printf("UpChar end, checksum_failed=%d, original up_len=%d \r\n",checksum_failed,up_len);
			 up_len=0;
		   debug_printf("UpChar end, checksum_failed=%d, up_len changes to =%d \r\n",checksum_failed,up_len);
		   debug_printf("tot up_len=%d,pcomm->rx_use=%d,frame_checksum=0x%x, temp_total=0x%x,temp_puredata=0x%x\r\n",up_len,pcomm->rx_use,frame_checksum, temp_total,temp_puredata);
		 }
		 else
		   debug_printf("UpChar end, tot up_len=%d,pcomm->rx_use=%d, checksum ok,temp_total=0x%x,temp_puredata=0x%x\r\n",up_len,pcomm->rx_use,temp_total,temp_puredata);
			 
		 
	if(last_data_package==1) //接收完整		
		if(checksum_failed)
			return 0;
		else
	    return up_len;	
  else
			return 0;	  //接收不完整，返回0
}




//
//ZACK Lynn ,20170224
//down charter from my MCU to module's BufferID=1 or 2; downchar buf len ==512;
//return total downchar len
//BufferID ==1 or ==2,下载到指纹头的CharBuffer1 or CharBuffer2;
//BufferID==0,上传到服务器; 
//MCU send
u16 PS_DownChar(COMM_USART *pcomm,u8 BufferID, u8 *downchar_buf, u16 down_len)
{
	u16 temp_check=0x0;
	u16 temp_check_data=0x0;
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
	
  	SendAS606Head(pcomm);	   
		//SendHead(g_pcom_ser);	   
	 
  	SendAS606Addr(pcomm);
		//SendAddr(g_pcom_ser);
		SendFlag(pcomm,0x01);//命令包标识
		//SendFlag(g_pcom_ser,0x01);//命令包标识
		SendLength(pcomm,0x04);
		//SendLength(g_pcom_ser,0x04);
		Sendcmd(pcomm,0x09);     //命令码09为主控端发送，发送可以是下载给指纹头，也可以是上传给服务器
	  //Sendcmd(g_pcom_ser,0x09);
		USART_transmit(pcomm,BufferID);
	  //USART_transmit(g_pcom_ser,BufferID);
  	
		//temp = 0x01+0x04+0x09+BufferID;
		temp_check = 0x01+0x04+0x09+BufferID;
		SendCheck(pcomm,temp_check);
		//SendCheck(g_pcom_ser,temp_check);

		//ok
		 
		
 	 data=JudgeStr(pcomm,100,AS60X_ACK);
	 		
	
	if(data)
		ensure=data[9];
	else
		ensure=0xff;

	  pcomm->rx_use += 12;  // 应答包长为12，下次如果再读取接收缓冲buf，从rx_buf[] + rx_use 开始

 	   
		if(ensure == 0x00)   //can receive data package
		{
			left=down_len;
			
			data=downchar_buf;
			
			buf_use=0;
			
			final_pack_flag=0x02;

    	temp_check_data=0;
			temp_total=0;
			
			count=0;
		  delay_ms(10);
 
			do{
				  temp_check_data = 0;				
				  if(left>g_fp_frame_data_len) 
           { 
						 final_pack_flag=0x02;
						 pack_len=g_fp_frame_data_len;
						 left-=g_fp_frame_data_len;
 					 }
				  else
				   {
						final_pack_flag=0x08;
					  pack_len=left;
						left=0;
 				   }	
            
					 pack_len_plus_checksum = pack_len + 2;
					 
			     //temp += final_pack_flag + pack_len_plus_checksum;
			     temp_check_data += final_pack_flag + ((pack_len_plus_checksum>>8)&0xff) + (pack_len_plus_checksum&0xff) ;
					 
 					 
					 p_data_head = &data[buf_use];   // == downchar_buf + buf_use
 					 
 	 				 
				   for(i=0;i<pack_len;i++)
           { 
				       temp_check_data += data[buf_use];       //校验和，所有字节相加
						   buf_use++;
					 } 
					 					 
					 //RX_Clear(pcomm);  
 					 
 			 	   SendAS606Head(pcomm);
				   //SendHead(g_pcom_ser);
		 			 SendAS606Addr(pcomm);
					 //SendAddr(g_pcom_ser);
           SendFlag(pcomm,final_pack_flag); //数据包标识		
           //SendFlag(g_pcom_ser,final_pack_flag); //数据包标识							
	  			 //SendLength(pcomm,pack_len);
	  			 SendLength(pcomm,pack_len_plus_checksum);
	  			 //SendLength(g_pcom_ser,pack_len_plus_checksum);
					
 			     SendData(pcomm,p_data_head,pack_len);
 			     //SendData(g_pcom_ser,p_data_head,pack_len);

					 SendCheck(pcomm,temp_check_data);
					 //SendCheck(g_pcom_ser,temp_check_data);
 
					 count++;

					 //debug_printf("downchar_buf=0x%x, p_data_head=0x%x, downchar_buf+buf_use=0x%x ,pack_len=%d,left=%d,buf_use=%d,temp=0x%x,count=%d\r\n", downchar_buf, p_data_head, downchar_buf+buf_use, pack_len,left,buf_use,temp,count);

					 //while(1);
					 //delay_ms(10);
					 temp_total += temp_check_data; 
 					 //debug_printf("downchar_buf=0x%x, p_data_head=0x%x, pack_len=%d,left=%d,buf_use=%d,temp_check_data=0x%x,temp_total=0x%x,count=%d\r\n", downchar_buf, p_data_head, pack_len,left,buf_use,temp_check_data,temp_total,count);

			   }while(left>0);	 

	 		 } 					 
			 
			 debug_printf("BufferID=%d,buf_use=%d,temp_total=0x%x\r\n",BufferID,buf_use,temp_total);
			 
			return buf_use;	  //返回总共download的数据
}


 
//删除模板 PS_DeletChar
//功能:  删除flash数据库中指定ID号开始的N个指纹模板
//参数:  PageID(指纹库模板号)，N删除的模板个数。
//说明:  返回 00 --表示删除成功。
//u8 PS_DeletChar(u16 PageID,u16 N)
u8 PS_DeletChar(COMM_USART *pcomm,u16 PageID,u16 N)
{
	u16 temp=0;
 	
  u8  ensure;
	u8  *data;
  
	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x07);
	Sendcmd(pcomm,0x0C);
	
	//USART1_transmit(PageID>>8);
	//USART1_transmit(PageID);
	//USART1_transmit(N>>8);
	//USART1_transmit(N);

	USART_transmit(pcomm,PageID>>8);
	USART_transmit(pcomm,PageID);
	USART_transmit(pcomm,N>>8);
	USART_transmit(pcomm,N);
	
	temp = 0x01+0x07+0x0C+(PageID>>8)+(u8)PageID+(N>>8)+(u8)N;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//清空指纹库 PS_Empty
//功能:  删除flash数据库中所有指纹模板
//参数:  无
//说明:  模块返回确认字
//u8 PS_Empty(void)
u8 PS_Empty(COMM_USART *pcomm)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x03);
	Sendcmd(pcomm,0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//写系统寄存器 PS_WriteReg  //SetSysPara
//功能:  写模块寄存器
//参数:  寄存器序号RegNum:4\5\6
//说明:  模块返回确认字
//
//u8 PS_WriteReg(u8 RegNum,u8 DATA)
u8 PS_WriteReg(COMM_USART *pcomm,u8 RegNum,u8 DATA)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x05);
	Sendcmd(pcomm,0x0E);
	
	//USART1_transmit(RegNum);
	//USART1_transmit(DATA);

	USART_transmit(pcomm,RegNum);
	USART_transmit(pcomm,DATA);
	
	temp = RegNum+DATA+0x01+0x05+0x0E;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//读系统基本参数 PS_ReadSysPara
//功能:  读取模块的基本参数（波特率，包大小等）
//参数:  无
//说明:  返回00---OK, 基本参数（16bytes）
//u8 PS_ReadSysPara(SysPara *p)
u8 PS_ReadSysPara(COMM_USART *pcomm,SysPara *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm); 
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x03);
	Sendcmd(pcomm,0x0F);
	temp = 0x01+0x03+0x0F;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
	{
		ensure = data[9];
		p->PS_max = (data[14]<<8)+data[15];
		p->PS_level = data[17];
		p->PS_addr=(data[18]<<24)+(data[19]<<16)+(data[20]<<8)+data[21];
		p->PS_size = data[23];
		p->PS_N = data[25];
	}		
	else
		ensure=0xff;
	
	pcomm->rx_use += 28;
	
	return ensure;
}
//设置模块地址 PS_SetAddr
//功能:  设置模块地址
//参数:  PS_addr
//说明:  模块返回确认字
//u8 PS_SetAddr(u32 PS_addr)
u8 PS_SetAddr(COMM_USART *pcomm,u32 PS_addr)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x07);
	Sendcmd(pcomm,0x15);
	
	//USART1_transmit(PS_addr>>24);
	//USART1_transmit(PS_addr>>16);
	//USART1_transmit(PS_addr>>8);
	//USART1_transmit(PS_addr);

	USART_transmit(pcomm,PS_addr>>24);
	USART_transmit(pcomm,PS_addr>>16);
	USART_transmit(pcomm,PS_addr>>8);
	USART_transmit(pcomm,PS_addr);
	
	temp = 0x01+0x07+0x15
	+(u8)(PS_addr>>24)+(u8)(PS_addr>>16)
	+(u8)(PS_addr>>8) +(u8)PS_addr;				
	SendCheck(pcomm,temp);
	AS606Addr=PS_addr;//发送完指令，更换地址
  data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;	
		AS606Addr = PS_addr;
	return ensure;
}

//功能： 模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
//	该记事本逻辑上被分成 16 个页。
//参数:  NotePageNum(0~15),Byte32(要写入内容，32个字节)
//说明:  模块返回确认字
//u8 PS_WriteNotepad(u8 NotePageNum,u8 *Byte32)
u8 PS_WriteNotepad(COMM_USART *pcomm,u8 NotePageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,36);
	Sendcmd(pcomm,0x18);
	//USART1_transmit(NotePageNum);
	USART_transmit(pcomm,NotePageNum);
	for(i=0;i<32;i++)
	 {
		 //USART1_transmit(Byte32[i]);
		 USART_transmit(pcomm,Byte32[i]);
		 temp += Byte32[i];
	 }
  temp =0x01+36+0x18+NotePageNum+temp;
	SendCheck(pcomm,temp);
  data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}

//读记事PS_ReadNotepad
//功能：  读取FLASH用户区的128bytes数据
//参数:  NotePageNum(0~15)
//说明:  模块返回确认字+用户信息
//u8 PS_ReadNotepad(u8 NotePageNum,u8 *Byte32)
u8 PS_ReadNotepad(COMM_USART *pcomm,u8 NotePageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x04);
	Sendcmd(pcomm,0x19);
	//USART1_transmit(NotePageNum);
	USART_transmit(pcomm,NotePageNum);
	temp = 0x01+0x04+0x19+NotePageNum;
	SendCheck(pcomm,temp);
  data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
	{
		ensure=data[9];
		for(i=0;i<32;i++)
		{
			Byte32[i]=data[10+i];
		}
	}
	else
		ensure=0xff;
	return ensure;
}


//ZACK Lynn --20170227
//读记事PS_ReadFPFlash
//功能：  读取指纹库列表
//参数:  PageNum(0~3)  //每页32字节，每个bit表示一个位是否有数据。每页32个字节，代表256个存储位
//说明:  确认码00--OK , 
//u8 PS_ReadFPFlash(u8 PageNum,u8 *Byte32)
u8 PS_ReadFPFlash(COMM_USART *pcomm,u8 PageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x04);
	Sendcmd(pcomm,0x1F);   //指令码1F  
	//USART1_transmit(NotePageNum);
	USART_transmit(pcomm,PageNum);   //0~3
	temp = 0x01+0x04+0x1F+PageNum;
	SendCheck(pcomm,temp);
  data=JudgeStr(pcomm,100,AS60X_ACK);
	if(data)
	{
		ensure=data[9];
		for(i=0;i<32;i++)
		{
			Byte32[i]=data[10+i];
		}
	}
	else
		ensure=0xff;

  pcomm->rx_use += 44;	
	
	return ensure;
}


 


//高速搜索PS_HighSpeedSearch
//功能：以 CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹库。
//		  若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质量
//		  很好的指纹，会很快给出搜索结果。
//参数:  BufferID， StartPage(起始页)，PageNum（页数）
//说明:  模块返回确认字+页码（相配指纹模板）
//u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
u8 PS_HighSpeedSearch(COMM_USART *pcomm,u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x08);
	Sendcmd(pcomm,0x1b);
	
	//USART1_transmit(BufferID);
	//USART1_transmit(StartPage>>8);
	//USART1_transmit(StartPage);
	//USART1_transmit(PageNum>>8);
	//USART1_transmit(PageNum);

	USART_transmit(pcomm,BufferID);
	USART_transmit(pcomm,StartPage>>8);
	USART_transmit(pcomm,StartPage);
	USART_transmit(pcomm,PageNum>>8);
	USART_transmit(pcomm,PageNum);
	
	temp = 0x01+0x08+0x1b+BufferID
	+(StartPage>>8)+(u8)StartPage
	+(PageNum>>8)+(u8)PageNum;
	SendCheck(pcomm,temp);
	data=JudgeStr(pcomm,500,AS60X_ACK);
 	if(data)
	{
		ensure=data[9];
		p->pageID 	=(data[10]<<8) +data[11];
		p->mathscore=(data[12]<<8) +data[13];
	}
	else
		ensure=0xff;
	return ensure;
}
//读有效模板个数 PS_ValidTempleteNum
//功能：读有效模板个数
//参数: 无
//说明: 返回 00 -- 成功。有效模板个数=ValidN
//u8 PS_ValidTempleteNum(u16 *ValidN)
u8 PS_ValidTempleteNum(COMM_USART *pcomm,u16 *ValidN)
{
	u16 temp;
  u8  ensure;
	u8  *data;

	RX_Clear(pcomm);  
	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	SendFlag(pcomm,0x01);//命令包标识
	SendLength(pcomm,0x03);
	Sendcmd(pcomm,0x1d);
	temp = 0x01+0x03+0x1d;
	SendCheck(pcomm,temp);
  data=JudgeStr(pcomm,100,AS60X_ACK);
	
	if(data)
	{
		ensure=data[9];
		*ValidN = (data[10]<<8) +data[11];
	}		
	else
		ensure=0xff;

  pcomm->rx_use += 14;	
	
	return ensure;
}

//与AS608握手 PS_HandShake
//参数: PS_Addr地址指针
//说明: 模块返新地址（正确地址）	
//u8 PS_HandShake(u32 *PS_Addr)
u8 PS_HandShake(COMM_USART *pcomm,u32 *PS_Addr)
{
	uint16_t waittime=100;
	u8 *rx_buf = pcomm->rx_buf;

	RX_Clear(pcomm);  
 	
	SendAS606Head(pcomm);
	SendAS606Addr(pcomm);
	
	//USART1_transmit(0X01);
	//USART1_transmit(0X00);
	//USART1_transmit(0X00);	

	USART_transmit(pcomm,0X01);
	USART_transmit(pcomm,0X00);
	USART_transmit(pcomm,0X00);
	
	delay_ms(200);
	//if(USART1_RX_STA&0X8000)//接收到数据 
	//if(if_USART1_has_new_data()>11) 
 

   #if 0		
	 if(pcomm->rx_state==2)
	 {	
 		 if(//判断是不是模块返回的应答包				
					USART1_RX[0]==0XEF
				&&USART1_RX[1]==0X01
				&&USART1_RX[6]==0X07
			)
			{
				*PS_Addr=(USART1_RX[2]<<24) + (USART1_RX[3]<<16)
								+(USART1_RX[4]<<8) + (USART1_RX[5]);
				//USART1_RX_STA=0;
				return 0;
			}
		//USART1_RX_STA=0;					 	
	 }
	 #endif
	 
  while(waittime--)
	{
		delay_ms(10);
		
	  if(pcomm->rx_state==2)
	  {
 		 if(//判断是不是模块返回的应答包 
					rx_buf[0]==0XEF
				&&rx_buf[1]==0X01
				&&rx_buf[6]==0X07
			)
			{
				*PS_Addr=(rx_buf[2]<<24) + (rx_buf[3]<<16)
								+(rx_buf[4]<<8) + (rx_buf[5]);
				//USART1_RX_STA=0;
				return 0;
			}
		//USART1_RX_STA=0;					 	
	 }
 }
	 
	 
	 
	return 1;		
}
