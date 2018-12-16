
/*************************************************
 *  �������ƣ�voice.hͷ�ļ�
 *  �����ܣ���ʼ��IO�ڣ����ó�Ϊ����
 *  �������ߣ���̫��
 *  ����ʱ�䣺2017-1-20
 *  �޸�ʱ�䣺
 *  ����汾��V0.1
 *************************************************/

#ifndef __VOICE__H__
#define __VOICE__H__

/* 
 *  ����ͷ�ļ�
 */
#include "sys.h"
#include "delay.h"

/* 
 *  �궨��
 */

#define COME_IN                0X01     // ��բ�Ѵ򿪣��뾡��ͨ��
#define FINGERPRINT_AGAIN      0X02     // ���°�ָ��
#define CHECK_GUN              0X03     // ����ǹ
#define CHECK_GUN_AGAIN        0X04     // ��������ǹ
#define GUN_FAIL               0X05     // ǹ������Ϣ��ƥ��
#define FINGERPRINT_FIST       0X06     // ������ָ��
#define FP_FAI                 0X07     // ָ��δ��ʶ��
#define CHECK_NEXT_GUN         0X08     // ������һ��ǹ
#define CANNOT_PASS            0X09     // ����ͨ��
#define GUNPASS                0x10     //  ��ǹͨ��


#define VOICEout  PAout(12)// PA12

#define VOICE_OFF()      GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define VOICE_ON()       GPIO_SetBits(GPIOA,GPIO_Pin_12)



extern void VOICE_Init(void);  // ��ʼ������

/******************************************************
 * �������ƣ�SendData
 * �������ܣ�MCU_voice���ݵ����
 * ��ڲ�����unsigned char ucaddr  ��ַ
 * ���ڲ�����void
 ******************************************************/ 
extern void SendVoiceData(unsigned char ucaddr);

/******************************************************
 * �������ƣ�VoicePrompt
 * �������ܣ�������ʾ����
 * ��ڲ�����unsigned char ucvoices  ������ʾ����
 * ���ڲ�����void
 ******************************************************/ 
extern void VoicePrompt(unsigned char ucvoices);   // ������ʾ

#endif
