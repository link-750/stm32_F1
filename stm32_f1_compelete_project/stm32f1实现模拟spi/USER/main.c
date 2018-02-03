#include "stm32f10x.h"
#include <stdio.h>
#include "spi.h"
#include "flash.h"

  void USART_gpio_init();//led�Ƶ�gpio����������
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
  SPI_Flash_Init();  		//SPI FLASH ��ʼ�� 
  while(SPI_Flash_ReadID()!=W25Q64)//��ⲻ��W25Q64
	{
		printf("11111");
	}
  FLASH_SIZE=64*1024*1024;	//FLASH ��СΪ8M�ֽ�
  SPI_Flash_Write((u8*)TEXT_Buffer,FLASH_SIZE-100,SIZE);		//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
  while(1)
  {
    SPI_Flash_Read(datatemp,FLASH_SIZE-100,SIZE);				//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
    for(l=0;l<7;l++)
    {
      printf("%c\t",datatemp[l]);
    }
  }
 }
  int fputc(int ch,FILE *f)
{
//ch�͸�USART1
USART_SendData(USART1, ch);
//�ȴ��������
while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET) { }
//����ch
return(ch);
}
   void RCC_Configuration()
    {
        SystemInit();
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,ENABLE);//�ⲿ���ù���ʱ��
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);/*ʹ��a��gpio��rccʱ��*/
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
        RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M
    }
    void USART_gpio_init()
{      
       
       GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��
       //pa9Ϊua1��tx��  
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//��������
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�������ģʽ
       GPIO_Init(GPIOA,&GPIO_InitStructure);//��ʼ��
       //pa10Ϊua1��rx��
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//��������
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//�������ģʽ
       GPIO_Init(GPIOA,&GPIO_InitStructure);//��ʼ��
       //ad��
       GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//���������ٶ�
       GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;//��������
       GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;//�������ģʽ-ģ������
       GPIO_Init(GPIOA,&GPIO_InitStructure);//��ʼ��
            
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
    USART_ITConfig(USART1,USART_IT_TXE,ENABLE);//ʹ�ܷ����ж�
    USART_ClearFlag(USART1,USART_FLAG_TC);//����������־λ
    USART_Cmd(USART1,ENABLE);//ʹ������
   }