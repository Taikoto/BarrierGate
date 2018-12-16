
/*************************************************
 *  程序名称：at24c02.c文件
 *  程序功能：at24c02驱动配置及应用文件
 *  程序作者：钟太松
 *  创建时间：2017-2-9
 *  修改时间：
 *  程序版本：V0.1
 *************************************************/
 
/* 
 *  包含头文件
 */
#include "at24c02.h"

/*************************************************
 *  函数名称：At24c02_Init
 *  函数功能：at24c02初始化函数
 *  入口参数：void
 *  出口参数：void
 ************************************************/
void At24c02_Init(void)
{
  IIC_Init();
}

/*************************************************
 *  函数名称：AT24CXX_ReadOneByte
 *  函数功能：在AT24CXX指定地址读出一个数据
 *  入口参数：ReadAddr
 *  出口参数：读到的数
 ************************************************/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}

/*************************************************
 *  函数名称：AT24CXX_WriteOneByte
 *  函数功能：在AT24CXX指定地址写入一个数据
 *  入口参数：WriteAddr  :写入数据的目的地址 
 *            DataToWrite:要写入的数据 
 *  出口参数：void
 ************************************************/
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

/*************************************************
 *  函数名称：AT24CXX_WriteLenByte
 *  函数功能：在AT24CXX指定地址写入长度为Len的数据
 *  入口参数：WriteAddr  :写入数据的目的地址 
 *            DataToWrite:要写入的数据 
 *            Len        :要写入数据的长度2,4
 *  出口参数：void
 ************************************************/
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/*************************************************
 *  函数名称：AT24CXX_ReadLenByte
 *  函数功能：在AT24CXX指定地址读出长度为Len的数据
 *            该函数用于读出16bit或者32bit的数据.
 *  入口参数：ReadAddr   :开始读出的地址 
 *            Len        :要写入数据的长度2,4
 *  出口参数：读到的数
 ************************************************/
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/*************************************************
 *  函数名称：AT24CXX_Check
 *  函数功能：检查AT24CXX是否正常
 *            这里用了24XX的最后一个地址(255)来存储标志字.
 *            如果用其他24C系列,这个地址要修改 
 *  入口参数：void
 *  出口参数：返回1:检测失败, 返回0:检测成功
 ************************************************/
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/*************************************************
 *  函数名称：AT24CXX_Read
 *  函数功能：在AT24CXX里面的指定地址开始读出指定个数的数据
 *  入口参数：ReadAddr :开始读出的地址 对24c02为0~255
 *            pBuffer  :数据数组首地址
 *            NumToRead:要读出数据的个数
 *  出口参数：读到的数据
 ************************************************/
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
} 

/*************************************************
 *  函数名称：AT24CXX_Read
 *  函数功能：在AT24CXX里面的指定地址开始写入指定个数的数据
 *  入口参数：WriteAddr :开始写入的地址 对24c02为0~255
 *            pBuffer   :数据数组首地址
 *            NumToWrite:要写入数据的个数
 *  出口参数：读到的数据
 ************************************************/
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
 

