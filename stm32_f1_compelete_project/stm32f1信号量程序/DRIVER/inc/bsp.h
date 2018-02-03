#ifndef __BSP_H  
#define __BSP_H  
  
#include "stm32f10x.h"   
#include "ucos_ii.h"   
  
void BSP_Init(void);  
void LED_GPIO_Init(void);  
void NVIC_Config(void);  
  
#endif // __BSP_H