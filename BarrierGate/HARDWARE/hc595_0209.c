
/*************************************************
 *  程序名称：74hc595.c文件
 *  程序功能：74hc595驱动配置及应用文件
 *  程序作者：钟太松
 *  创建时间：2017-2-9
 *  修改时间：
 *  程序版本：V0.1
 *************************************************/
 
/*
 *  包含头文件
 */
#include "hc595.h"

/*************************************************
 *  函数名称：74HC595_Init_RelayPart
 *  函数功能：继电器部分74HC595初始化函数
 *  入口参数：void
 *  出口参数：void
 ************************************************/
void HC595_Init_RelayPart(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				   //PC.13/PC.14 /PC.15 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化PC.13/PC.14 /PC.15
  GPIO_SetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);						 //PC.13/PC.14 /PC.15 输出高
}

/*************************************************
 *  函数名称：74HC595_Init_DigitalPart
 *  函数功能：74HC595初始化函数
 *  入口参数：void
 *  出口参数：void
 ************************************************/
void HC595_Init_DigitalPart(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9;				   //PB.5/PB.8 /PB.9 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PB.5/PB.8 /PB.9
  GPIO_SetBits(GPIOB,GPIO_Pin_5|GPIO_Pin_8|GPIO_Pin_9);						 //PB.5/PB.8 /PB.9 输出高
}

void NOP()
{
	unsigned char i;
  for(i=0; i<30; i++); 
}

/*************************************************
 *  函数名称：HC595SendData
 *  函数功能：HC595向SPI总线发送数据
 *  入口参数：unsigned char ucSendValue 要发送的数据 
 *  出口参数：void
 ************************************************/
void HC595SendData(unsigned char ucSendValue)
{
  unsigned char i;
	for(i = 0; i<8; i++)
	{
	  if((ucSendValue <<i)&0x80)
		{
		  GPIO_SetBits(GPIOC,MOSI_RelayPart);
		}
		else
		{
		  GPIO_ResetBits(GPIOC,MOSI_RelayPart);
		}
		GPIO_ResetBits(GPIOC,S_CLK_RelayPart);      // 先移位
		NOP();
		NOP();
		GPIO_SetBits(GPIOC,S_CLK_RelayPart);	
	}
	
	GPIO_ResetBits(GPIOC,R_CLK_RelayPart);         // 再存储
	NOP();
	NOP();
	GPIO_SetBits(GPIOC,R_CLK_RelayPart);	
}

/*************************************************
 *  函数名称：HC595_IO_Out
 *  函数功能：HC595输出口输出数据
 *  入口参数：unsigned char ucHc595Status 要发送的数据 
 *  出口参数：void
 ************************************************/
void HC595_IO_Out(unsigned char ucHc595Status)
{
  static unsigned char ucTemp = 0;
	ucTemp = ucTemp | ucHc595Status;     // 按位或
	
	HC595SendData(ucTemp);

}
	





