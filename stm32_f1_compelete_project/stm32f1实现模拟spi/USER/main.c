#include "stm32f10x.h"
#include <stdio.h>
#include "spi.h"
#include "flash.h"

  void USART_gpio_init();//led灯的gpio函数的声明
  void USART_Configuration();
  void RCC_Configuration(); 

const u8 TEXT_Buffer[]={"abcdefg"};
#define SIZE sizeof(TEXT_Buffer)
void main()
{
  u8 datatemp[SIZE],l;
  u32 FLASH_SIZE;
  RCC_Configuration();
  USART_gpio_init();
  USART_Configuration();  
  SPI_Flash_Init();  		//SPI FLASH 初始化 
  while(SPI_Flash_ReadID()!=W25Q64)//检测不到W25Q64
	{
		printf("11111");
	}
  FLASH_SIZE=64*1024*1024;	//FLASH 大小为8M字节
  SPI_Flash_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//从倒数第100个地址处开始,写入SIZE长度的数据
  while(1)
  {
    SPI_Flash_Read(datatemp,FLASH_SIZE-100,SIZE);				//从倒数第100个地址处开始,读出SIZE个字节
    for(l=0;l<7;l++)
    {
      printf("%c\t",datatemp[l]);
    }
  }
 }
  int fputc(int ch,FILE *f)
{
//ch送给USART1
USART_SendData(USART1, ch);
//等待发送完毕
while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET) { }
//返回ch
return(ch);
}
   void RCC_Configuration()
    {
        SystemInit();
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,ENABLE);//外部复用功能时钟
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);/*使能a口gpio的rcc时钟*/
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M
    }
    void USART_gpio_init()
{      
       
       GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
       //pa9为ua1的tx端  
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义引脚速度
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//定义引脚
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//定义输出模式
       GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
       //pa10为ua1的rx端
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义引脚速度
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//定义引脚
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//定义输出模式
       GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
       //ad口
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义引脚速度
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//定义引脚
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//定义输出模式-模拟输入
       GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化
            
}
   
   void USART_Configuration()
   {
   
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1,&USART_InitStructure);  
    USART_ITConfig(USART1,USART_IT_TXE,ENABLE);//使能发送中断
    USART_ClearFlag(USART1,USART_FLAG_TC);//清除待处理标志位
    USART_Cmd(USART1,ENABLE);//使能外设
   }