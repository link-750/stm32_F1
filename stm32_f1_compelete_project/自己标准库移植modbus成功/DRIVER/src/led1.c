#include"led1.h"

void led_gpio_init(void)
{
      RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
    /*使能gpio的rcc时钟*/
      
     GPIOSpeed_TypeDef.GPIO_Speed_50MHZ;
     
     GPIOSpeed_TypeDef.GPIO_Pin_15;
      GPIO_InitStructure.GPIO_Mode_Out_PP;
      GPIO_Init(GPIO,&GPIO_InitStructure);
}

 