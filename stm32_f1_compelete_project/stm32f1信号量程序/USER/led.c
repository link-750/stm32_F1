#include "led.h"  
void LED1()  
{  
  GPIO_ResetBits(GPIOA,GPIO_Pin_8);   
  OSTimeDly(500);   
  GPIO_SetBits(GPIOA,GPIO_Pin_8);  
  OSTimeDly(500);  
}  
  
void LED2()  
{  
  GPIO_ResetBits(GPIOD,GPIO_Pin_2);   
  OSTimeDly(100);   
  GPIO_SetBits(GPIOD,GPIO_Pin_2);  
  OSTimeDly(100);  
}  
  
