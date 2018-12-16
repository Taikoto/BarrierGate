
/*************************************************
 *  程序名称：74hc595.h头文件
 *  程序功能：74hc595驱动配置及应用文件
 *  程序作者：钟太松
 *  创建时间：2017-2-9
 *  修改时间：
 *  程序版本：V0.1
 *************************************************/

#ifndef __HC595__H__
#define __HC595__H__

/* 
 *  包含头文件
 */
#include "stm32f10x.h"

/* 
 *  宏定义
 */
#define S_CLK_RelayPart    GPIO_Pin_15
#define R_CLK_RelayPart    GPIO_Pin_14
#define MOSI_RelayPart     GPIO_Pin_13

#define S_CLK_DigitalPart  GPIO_Pin_8
#define R_CLK_DigitalPart  GPIO_Pin_9
#define MOSI_DigitalPart   GPIO_Pin_5

//#define RelayPart          GPIOC
//#define DigitalPart        GPIOB

#define Beep_on         0x01         //  Q0
#define Beep_off        0xfe

#define Relay1_2_on     0xfd         //  Q1
#define Relay1_2_off    0x02

#define Relay3_4_on     0xfb         //  Q2
#define Relay3_4_off    0x04

#define Lock_on         0xf7         //  Q3
#define Lock_off        0x08

#define Led6_on         0xef         //  Q4
#define Led6_off        0x10

#define Led7_on         0xdf         //  Q5
#define Led7_off        0x20         

#define Led8_on         0xbf         //  Q6
#define Led8_off        0x40   

#define Led9_on         0x7f         //  Q7
#define Led9_off        0x80

/*************************************************
 *  函数名称：74HC595_Init_RelayPart
 *  函数功能：继电器部分74HC595初始化函数
 *  入口参数：void
 *  出口参数：void
 ************************************************/
extern void HC595_Init_RelayPart(void);

/*************************************************
 *  函数名称：74HC595_Init_DigitalPart
 *  函数功能：数码管部分74HC595初始化函数
 *  入口参数：void
 *  出口参数：void
 ************************************************/
extern void HC595_Init_DigitalPart(void);

/*************************************************
 *  函数名称：HC595SendData
 *  函数功能：HC595向SPI总线发送数据
 *  入口参数：unsigned char ucSendValue 要发送的数据 
 *  出口参数：void
 ************************************************/
extern void HC595SendData(unsigned char ucSendValue);

/*************************************************
 *  函数名称：HC595_IO_Out
 *  函数功能：HC595输出口输出数据
 *  入口参数：unsigned char ucHc595Status 要发送的数据 
 *  出口参数：void
 ************************************************/
extern void HC595_IO_Out(unsigned char ucHc595Status);

//void NOP();

#endif
