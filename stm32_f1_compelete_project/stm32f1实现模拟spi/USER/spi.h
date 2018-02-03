#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//SPI 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/13 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  
#define MOSI_H GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define MOSI_L GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define SCLK_H GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define SCLK_L GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define MISO GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)
 
 				  	    													  
void SPI1_Init(void);			 //初始化SPI口
//void SPI1_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void delay_us(int);		 
#endif

