#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//SPI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/13 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
#define MOSI_H GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define MOSI_L GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define SCLK_H GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define SCLK_L GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define MISO GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)
 
 				  	    													  
void SPI1_Init(void);			 //��ʼ��SPI��
//void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
void delay_us(int);		 
#endif

