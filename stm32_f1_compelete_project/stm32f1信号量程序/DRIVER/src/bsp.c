    #include "BSP.h"  
      
    void BSP_Init()  
    {  
      SystemInit(); 
      
      LED_GPIO_Init();  
      NVIC_Config();  
    }  
    //下面是初始化控制LED灯的GPIO的程序，  
    //我是用的GPIOA端口，其中，  
    //GPIO_Pin_10 控制LED1，  
    //GPIO_Pin_11 控制LED2，  
    //GPIO_Pin_12 控制LED3，  
    //GPIO_Pin_13 控制LED4.  
    void LED_GPIO_Init()  
    {  
      GPIO_InitTypeDef  GPIO_InitStructure;
 	
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PA,PD端口时钟
      	
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 端口配置
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
      GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.8
      //GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 输出高
      
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 端口配置, 推挽输出
      GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
      //GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 输出高 
      
      
    }  
      
      
    void NVIC_Config()  
    {  
      NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  
    }  