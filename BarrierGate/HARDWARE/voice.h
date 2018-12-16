
/*************************************************
 *  程序名称：voice.h头文件
 *  程序功能：初始化IO口，设置成为上拉
 *  程序作者：钟太松
 *  创建时间：2017-1-20
 *  修改时间：
 *  程序版本：V0.1
 *************************************************/

#ifndef __VOICE__H__
#define __VOICE__H__

/* 
 *  包含头文件
 */
#include "sys.h"
#include "delay.h"

/* 
 *  宏定义
 */

#define COME_IN                0X01     // 道闸已打开，请尽快通行
#define FINGERPRINT_AGAIN      0X02     // 重新按指纹
#define CHECK_GUN              0X03     // 请验枪
#define CHECK_GUN_AGAIN        0X04     // 请重新验枪
#define GUN_FAIL               0X05     // 枪和人信息不匹配
#define FINGERPRINT_FIST       0X06     // 请先验指纹
#define FP_FAI                 0X07     // 指纹未能识别
#define CHECK_NEXT_GUN         0X08     // 请验下一把枪
#define CANNOT_PASS            0X09     // 不能通行
#define GUNPASS                0x10     //  验枪通过


#define VOICEout  PAout(12)// PA12

#define VOICE_OFF()      GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define VOICE_ON()       GPIO_SetBits(GPIOA,GPIO_Pin_12)



extern void VOICE_Init(void);  // 初始化函数

/******************************************************
 * 函数名称：SendData
 * 函数功能：MCU_voice数据的输出
 * 入口参数：unsigned char ucaddr  地址
 * 出口参数：void
 ******************************************************/ 
extern void SendVoiceData(unsigned char ucaddr);

/******************************************************
 * 函数名称：VoicePrompt
 * 函数功能：语音提示功能
 * 入口参数：unsigned char ucvoices  语音提示种类
 * 出口参数：void
 ******************************************************/ 
extern void VoicePrompt(unsigned char ucvoices);   // 语音提示

#endif
