
/*************************************************
 *  程序名称：voice.c
 *  程序功能：初始化IO口，设置成为上拉
 *  程序作者：钟太松
 *  创建时间：2017-1-20
 *  修改时间：
 *  程序版本：V0.1
 *************************************************/

/*
 *  包含头文件
 */
#include "voice.h" 
 
/*************************************************
 *  函数名称：VOICE_Init
 *  函数功能：VOICE口初始化函数
 *  入口参数：void
 *  出口参数：void
 ************************************************/
void VOICE_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				   //PA.12 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化PA.12
  GPIO_SetBits(GPIOA,GPIO_Pin_12);						 //PA.12 输出高
	
	
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PA端口时钟
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				   //PA.5 端口配置
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
//  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化PA.5
//  GPIO_SetBits(GPIOA,GPIO_Pin_5);						 //PA.5 输出高
}

/******************************************************
 * 函数名称：SendData
 * 函数功能：MCU_voice数据的输出
 * 入口参数：unsigned char ucaddr  地址
 * 出口参数：void
 ******************************************************/ 
void SendVoiceData(unsigned char ucaddr)
{
  unsigned char i;
	
	VOICEout = 1;         // 开始拉高
	delay_us(1200);
	VOICEout = 0;         // 开始引导码
	delay_us(3000);    // 此处延时最少要大于2ms，此参数延时为310ms
	
  for( i = 0; i <8; i++)     // 8位数据
	{
	  VOICEout = 1; 
		
		if(ucaddr & 0x01)    // 3:1 表示数据位1，每个位用两个脉冲表示
		{
		  delay_us(800);
			VOICEout = 0;
			delay_us(210);
		}
		else
		{
		  delay_us(210);
			VOICEout = 0;
			delay_us(800);
		}
		
		ucaddr >>= 1;
	}
	VOICEout = 1;
}

/******************************************************
 * 函数名称：VoicePrompt
 * 函数功能：语音提示功能
 * 入口参数：unsigned char ucvoices  语音提示种类
 * 出口参数：void
 ******************************************************/ 
void VoicePrompt(unsigned char ucvoices)   // 语音提示
{
	if(ucvoices<=0x09)     // 提示数量少于10
	{
    SendVoiceData(0x0a);
	  SendVoiceData(0x00);
	  SendVoiceData(ucvoices);
	  SendVoiceData(0x0b);
	}
	else if((ucvoices>=0x10) && (ucvoices<0x19))   // 提示数量大于10，且少于20
	{
	  SendVoiceData(0x0a);
	  SendVoiceData(0x01);
	  SendVoiceData(ucvoices - 0x10);
	  SendVoiceData(0x0b);
	}
	else if((ucvoices>=0x20) && (ucvoices<0x29))   // 提示数量大于20，且少于30
	{
	  SendVoiceData(0x0a);
	  SendVoiceData(0x02);
	  SendVoiceData(ucvoices - 0x20);
	  SendVoiceData(0x0b);
	}
	
	else if((ucvoices>=0x30) && (ucvoices<0x39))   // 提示数量大于30，且少于40
	{
	  SendVoiceData(0x0a);
	  SendVoiceData(0x03);
	  SendVoiceData(ucvoices - 0x30);
	  SendVoiceData(0x0b);
	}
}

