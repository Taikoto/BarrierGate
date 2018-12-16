
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


unsigned char ucTemp = 0;         

unsigned char ucHc595Out_update = 0;   //  刷新变量，每次更改LED的状态都要更新一次。
unsigned char ucHc595Out_Temp = 0xfe;     //  代表底层74HC595输出状态的中间变量

unsigned char ucHc595Out_Q0 = 0;       //  代表8个输出口状态，0 代表灭，1代表亮
unsigned char ucHc595Out_Q1 = 0; 
unsigned char ucHc595Out_Q2 = 0; 
unsigned char ucHc595Out_Q3 = 0; 
unsigned char ucHc595Out_Q4 = 0; 
unsigned char ucHc595Out_Q5 = 0; 
unsigned char ucHc595Out_Q6 = 0; 
unsigned char ucHc595Out_Q7 = 0; 


unsigned char ucBarrierGate_Control_flag = 0;
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
void HC595_IO_Out( void )
{
  if(ucHc595Out_update ==1)
	{
	  ucHc595Out_update = 0;
		
//		if(ucHc595Out_Q0 == 1)
//		{
//		  ucHc595Out_Temp = ucHc595Out_Temp | 0x01;
//		}
//		else
//		{
//		  ucHc595Out_Temp = ucHc595Out_Temp & 0xfe;
//		}
		
		if(ucHc595Out_Q1 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xfc;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x02;
		}
		
		if(ucHc595Out_Q2 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xfa;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x04;
		}
		
		if(ucHc595Out_Q3 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xf6;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x08;
		}
		
		if(ucHc595Out_Q4 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xee;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x10;
		}
		
		if(ucHc595Out_Q5 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xde;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x20;
		}
		
		if(ucHc595Out_Q6 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xbe;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x40;
		}
		
		if(ucHc595Out_Q7 == 1)
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0x7e;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x80;
		}

		
		if(ucHc595Out_Q0 == 1)     // 为什么一定要放到最后，难道只是因为 只有它是高电平触发吗？
		{
		  ucHc595Out_Temp = ucHc595Out_Temp | 0x01;
		}
		else
		{
		  ucHc595Out_Temp = ucHc595Out_Temp & 0xfe;
		}
		
	}
	
	HC595SendData(ucHc595Out_Temp);
	
//  USART3_transmit(ucHc595Out_Temp);
}
	
void HC595_IO_Out_Updata_Status(unsigned char ucStatus)
{
  switch(ucStatus)
	{
	  case Beep_on:       ucHc595Out_Q0 = 0;  ucHc595Out_update = 1; break;
		case Beep_off:      ucHc595Out_Q0 = 1;  ucHc595Out_update = 1; break;
		
	  case Relay1_2_on:   ucHc595Out_Q1 = 0;  ucHc595Out_update = 1; break;
		case Relay1_2_off:  ucHc595Out_Q1 = 1;  ucHc595Out_update = 1; break;		
		
		case Relay3_4_on:   ucHc595Out_Q2 = 1;  ucHc595Out_update = 1; break;
		case Relay3_4_off:  ucHc595Out_Q2 = 0;  ucHc595Out_update = 1; break;	
		
//		case Lock_on:       ucHc595Out_Q3 = 1;  ucHc595Out_update = 1; break;    // 外部电磁铁接口
//		case Lock_off:      ucHc595Out_Q3 = 0;  ucHc595Out_update = 1; break;
		
		case W25X16_H:      ucHc595Out_Q3 = 0;  ucHc595Out_update = 1; break;        // 按此配置即可。
		case W25X16_L:      ucHc595Out_Q3 = 1;  ucHc595Out_update = 1; break;	 
    		
		case Led6_on:       ucHc595Out_Q4 = 1;  ucHc595Out_update = 1; break;
		case Led6_off:      ucHc595Out_Q4 = 0;  ucHc595Out_update = 1; break;	
		
		case Led7_on:       ucHc595Out_Q5 = 1;  ucHc595Out_update = 1; break;
  	case Led7_off:      ucHc595Out_Q5 = 0;  ucHc595Out_update = 1; break;	
		
		case Led8_on:       ucHc595Out_Q6 = 1;  ucHc595Out_update = 1; break;
		case Led8_off:      ucHc595Out_Q6 = 0;  ucHc595Out_update = 1; break;	
		
		case Led9_on:       ucHc595Out_Q7 = 1;  ucHc595Out_update = 1; break;
		case Led9_off:      ucHc595Out_Q7 = 0;  ucHc595Out_update = 1; break;		

		default: 
		{
//		  ucHc595Out_Q0 = 0;
//			ucHc595Out_Q1 = 0;
//			ucHc595Out_Q2 = 0;
//			ucHc595Out_Q3 = 0;
//			ucHc595Out_Q4 = 0;
//			ucHc595Out_Q5 = 0;
//			ucHc595Out_Q6 = 0;
//			ucHc595Out_Q7 = 0;
//			ucHc595Out_update = 1;
			break;
		}
	}
	
	HC595_IO_Out( );
}

void BarrierGate_Open(void)     //  道闸开启
{
  if(ucBarrierGate_Control_flag == 1)
	{
	  ucBarrierGate_Control_flag = 0;
		HC595_IO_Out_Updata_Status(Relay1_2_on);
		HC595_IO_Out_Updata_Status(Led6_on);	      // D6	作为继电器指示
	}

}

void BarrierGate_Close(void)   // 道闸关闭        道闸开启后延时5ms 马上执行关闭命令。延时时间不宜超过10ms
{
  if(ucBarrierGate_Control_flag != 1)
	{
	  HC595_IO_Out_Updata_Status(Relay1_2_off);
		HC595_IO_Out_Updata_Status(Led6_off);	
	}
}


