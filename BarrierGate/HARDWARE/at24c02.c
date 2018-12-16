
/*************************************************
 *  �������ƣ�at24c02.c�ļ�
 *  �����ܣ�at24c02�������ü�Ӧ���ļ�
 *  �������ߣ���̫��
 *  ����ʱ�䣺2017-2-9
 *  �޸�ʱ�䣺
 *  ����汾��V0.1
 *************************************************/
 
/* 
 *  ����ͷ�ļ�
 */
#include "at24c02.h"

/*************************************************
 *  �������ƣ�At24c02_Init
 *  �������ܣ�at24c02��ʼ������
 *  ��ڲ�����void
 *  ���ڲ�����void
 ************************************************/
void At24c02_Init(void)
{
  IIC_Init();
}

/*************************************************
 *  �������ƣ�AT24CXX_ReadOneByte
 *  �������ܣ���AT24CXXָ����ַ����һ������
 *  ��ڲ�����ReadAddr
 *  ���ڲ�������������
 ************************************************/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}

/*************************************************
 *  �������ƣ�AT24CXX_WriteOneByte
 *  �������ܣ���AT24CXXָ����ַд��һ������
 *  ��ڲ�����WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ 
 *            DataToWrite:Ҫд������� 
 *  ���ڲ�����void
 ************************************************/
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/*************************************************
 *  �������ƣ�AT24CXX_WriteLenByte
 *  �������ܣ���AT24CXXָ����ַд�볤��ΪLen������
 *  ��ڲ�����WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ 
 *            DataToWrite:Ҫд������� 
 *            Len        :Ҫд�����ݵĳ���2,4
 *  ���ڲ�����void
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
 *  �������ƣ�AT24CXX_ReadLenByte
 *  �������ܣ���AT24CXXָ����ַ��������ΪLen������
 *            �ú������ڶ���16bit����32bit������.
 *  ��ڲ�����ReadAddr   :��ʼ�����ĵ�ַ 
 *            Len        :Ҫд�����ݵĳ���2,4
 *  ���ڲ�������������
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
 *  �������ƣ�AT24CXX_Check
 *  �������ܣ����AT24CXX�Ƿ�����
 *            ��������24XX�����һ����ַ(255)���洢��־��.
 *            ���������24Cϵ��,�����ַҪ�޸� 
 *  ��ڲ�����void
 *  ���ڲ���������1:���ʧ��, ����0:���ɹ�
 ************************************************/
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/*************************************************
 *  �������ƣ�AT24CXX_Read
 *  �������ܣ���AT24CXX�����ָ����ַ��ʼ����ָ������������
 *  ��ڲ�����ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
 *            pBuffer  :���������׵�ַ
 *            NumToRead:Ҫ�������ݵĸ���
 *  ���ڲ���������������
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
 *  �������ƣ�AT24CXX_Read
 *  �������ܣ���AT24CXX�����ָ����ַ��ʼд��ָ������������
 *  ��ڲ�����WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
 *            pBuffer   :���������׵�ַ
 *            NumToWrite:Ҫд�����ݵĸ���
 *  ���ڲ���������������
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
 

