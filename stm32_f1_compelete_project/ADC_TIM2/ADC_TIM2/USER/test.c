#include <stm32f10x_lib.h>
#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h"
#include "exti.h"
#include "wdg.h"
#include "timer.h"
#include "lcd.h"	   
#include "rtc.h"
#include "wkup.h"
#include "adc.h"
#include "dma.h"
//Mini STM32�����巶������13
//ADC ʵ��
//����ԭ��@ALIENTEK
//������̳:www.openedv.co
int main(void)
{		
	u16 adcx;
	u8 buffer[10];
	float temp;
	u16 Receive[1];
  	Stm32_Clock_Init(6);//ϵͳʱ������
	delay_init(72);		//��ʱ��ʼ��
	uart_init(72,9600); //����1��ʼ��   
	LED_Init();	
	LCD_Init();  	
	Adc_Init();	
	dma_init(DMA1_Channel1,(u32)&Receive,(u32)&ADC1->DR,sizeof(Receive));
    TIM2_Pwm(4500,0);
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,"Mini STM32");	
	LCD_ShowString(60,70,"ADC TEST");	
	LCD_ShowString(60,90,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,"2010/12/30");	
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,130,"ADC_CH0_VAL:");	      
	LCD_ShowString(60,150,"ADC_CH0_VOL:0.000V");
		      
	while(1)
	{
	//	adcx=Get_Adc(ADC_CH0);
		adcx=Receive[0];
		LCD_ShowNum(156,130,adcx,4,16);//��ʾADC��ֵ
		sprintf(buffer,"%d",adcx); 
		LCD_ShowString2(200,110,buffer);	
		temp=(float)adcx*(3.3/4096);
		adcx=temp;
		LCD_ShowNum(156,150,adcx,1,16);//��ʾ��ѹֵ
		temp-=adcx;
		temp*=1000;
		LCD_ShowNum(172,150,temp,3,16);
		LED0=!LED0;
		delay_ms(250);
	}
}






















