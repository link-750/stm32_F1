#include "include.h"

void time_delay_ms(u32 ms)
{
  u16 i;
  while(ms--)
  {
    for(i=0;i<1200;i++);
  }
}//��ʱ����

//������

void main(void)
{
   PLL_Init(PLL100);            //��ʼ��PLLΪ180M
   UART_Init(UART0,115200);//uart0��ʼ����Ƶ��Ϊ115200
   LCD_Init();
   Draw_LQLogo();//��ʾlog��
   time_delay_ms(10000);
   LCD_CLS();
   CCD_Init();
   PIT_Init(PIT0,1);//�����жϼ�ʱ����ʼ��
   while(1)
   {   

   }

}