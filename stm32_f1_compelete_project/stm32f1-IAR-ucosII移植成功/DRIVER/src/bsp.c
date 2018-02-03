    #include "BSP.h"  
      
    void BSP_Init()  
    {  
      SystemInit(); 
      
      LED_GPIO_Init();  
      NVIC_Config();  
    }  
    //�����ǳ�ʼ������LED�Ƶ�GPIO�ĳ���  
    //�����õ�GPIOA�˿ڣ����У�  
    //GPIO_Pin_10 ����LED1��  
    //GPIO_Pin_11 ����LED2��  
    //GPIO_Pin_12 ����LED3��  
    //GPIO_Pin_13 ����LED4.  
    void LED_GPIO_Init()  
    {  
      GPIO_InitTypeDef  GPIO_InitStructure;
 	
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PA,PD�˿�ʱ��
      	
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 //LED0-->PA.8 �˿�����
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
      GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA.8
      //GPIO_SetBits(GPIOA,GPIO_Pin_8);						 //PA.8 �����
      
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	    		 //LED1-->PD.2 �˿�����, �������
      GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
      //GPIO_SetBits(GPIOD,GPIO_Pin_2); 						 //PD.2 ����� 
      
      
    }  
      
      
    void NVIC_Config()  
    {  
      NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);  
    }  