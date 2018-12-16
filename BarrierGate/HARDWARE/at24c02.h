
/*************************************************
 *  程序名称：at24c02.h头文件
 *  程序功能：at24c02驱动配置及应用文件
 *  程序作者：钟太松
 *  创建时间：2017-2-9
 *  修改时间：
 *  程序版本：V0.1
 *************************************************/
 
#ifndef __AT24C02__H__
#define __AT24C02__H__

/* 
 *  包含头文件
 */
#include "stm32f10x.h"
#include "sys.h"
#include "myiic.h"
#include "delay.h"
/* 
 *  宏定义
 */
 
#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767   

#define EE_TYPE AT24C02

/*************************************************
 *  函数名称：At24c02_Init
 *  函数功能：at24c02初始化函数
 *  入口参数：void
 *  出口参数：void
 ************************************************/
extern void At24c02_Init(void);

/*************************************************
 *  函数名称：AT24CXX_ReadOneByte
 *  函数功能：在AT24CXX指定地址读出一个数据
 *  入口参数：ReadAddr
 *  出口参数：读到的数
 ************************************************/
extern u8 AT24CXX_ReadOneByte(u16 ReadAddr);

/*************************************************
 *  函数名称：AT24CXX_WriteOneByte
 *  函数功能：在AT24CXX指定地址写入一个数据
 *  入口参数：WriteAddr  :写入数据的目的地址 
 *            DataToWrite:要写入的数据 
 *  出口参数：void
 ************************************************/
extern void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);

/*************************************************
 *  函数名称：AT24CXX_WriteLenByte
 *  函数功能：在AT24CXX指定地址写入长度为Len的数据
 *  入口参数：WriteAddr  :写入数据的目的地址 
 *            DataToWrite:要写入的数据 
 *            Len        :要写入数据的长度2,4
 *  出口参数：void
 ************************************************/
extern void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);

/*************************************************
 *  函数名称：AT24CXX_ReadLenByte
 *  函数功能：在AT24CXX指定地址读出长度为Len的数据
 *            该函数用于读出16bit或者32bit的数据.
 *  入口参数：ReadAddr   :开始读出的地址 
 *            Len        :要写入数据的长度2,4
 *  出口参数：读到的数
 ************************************************/
extern u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);

/*************************************************
 *  函数名称：AT24CXX_Check
 *  函数功能：检查AT24CXX是否正常
 *            这里用了24XX的最后一个地址(255)来存储标志字.
 *            如果用其他24C系列,这个地址要修改 
 *  入口参数：void
 *  出口参数：返回1:检测失败, 返回0:检测成功
 ************************************************/
extern u8 AT24CXX_Check(void);

/*************************************************
 *  函数名称：AT24CXX_Read
 *  函数功能：在AT24CXX里面的指定地址开始读出指定个数的数据
 *  入口参数：ReadAddr :开始读出的地址 对24c02为0~255
 *            pBuffer  :数据数组首地址
 *            NumToRead:要读出数据的个数
 *  出口参数：读到的数据
 ************************************************/
extern void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);

/*************************************************
 *  函数名称：AT24CXX_Read
 *  函数功能：在AT24CXX里面的指定地址开始写入指定个数的数据
 *  入口参数：WriteAddr :开始写入的地址 对24c02为0~255
 *            pBuffer   :数据数组首地址
 *            NumToWrite:要写入数据的个数
 *  出口参数：读到的数据
 ************************************************/
extern void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);
#endif


