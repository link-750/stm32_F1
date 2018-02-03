#include "include.h"

void time_delay_ms(u32 ms)
{
  u16 i;
  while(ms--)
  {
    for(i=0;i<1200;i++);
  }
}//延时函数

//主函数

void main(void)
{
   PLL_Init(PLL100);            //初始化PLL为180M
   UART_Init(UART0,115200);//uart0初始化，频率为115200
   LCD_Init();
   Draw_LQLogo();//显示log的
   time_delay_ms(10000);
   LCD_CLS();
   CCD_Init();
   PIT_Init(PIT0,1);//周期中断计时器初始化
   while(1)
   {   

   }

}