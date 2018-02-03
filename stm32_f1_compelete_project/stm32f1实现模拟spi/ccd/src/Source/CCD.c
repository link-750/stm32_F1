#include "include.h"
#include "CCD.h"
/******************** LQ_K60_函数库 v1.0 ********************
 * 文件名           ：CCD.c
 * 功能             ：初始化CCD，以及采集CCD
 * 备注             ：适用于自曝光程序
 * 日期             ：2015-12-13
 * 实验平台         ：龙丘 k60DN512Z核心板 
 * 开发环境         ：IAR 7.3
 * 作者             ：龙丘技术 005
 * 淘宝店           ：https://longqiu.taobao.com
 * 北京龙丘智能科技车讨论群 ：517926609
*************************************************************/
#define TSL_SI   PTE5_OUT   //E5
#define TSL_CLK  PTE4_OUT   //E4    
//ADC 9->

unsigned short int CCD_AD[128];//128个像素点
int CCD_AVG_E=0;
unsigned int CCD_MAX=0,CCD_MIN,CCD_AVG=0;
unsigned char TimerFlag20ms,IntegrationTime;

void Dly_us(unsigned int us)//
{
  while(us--)
  {
    for(u16 i=0;i<180;i++)
     asm("nop");  
  }
}

void CCD_Init(void) //ccd初始化
{
  GPIO_Init(PORTE,4,GPO,1);
  GPIO_Init(PORTE,5,GPO,1);
  ADC_Init(ADC1);//ADC1_DM1   adc1的引脚
}


void Integration(void)   //曝光函数
{
    unsigned char i;
    
    TSL_SI=1;            /* SI  = 1 */
    Dly_us(1);
    
    TSL_CLK = 1;         /* CLK = 1 */
    Dly_us(1);
    
    TSL_SI=0;            /* SI  = 0 */
    Dly_us(1);
    
    TSL_CLK = 0;         /* CLK = 0 */

    for(i=0; i<127; i++) 
    {
        Dly_us(1);
        TSL_CLK = 1;       /* CLK = 1 */
        Dly_us(1);
        TSL_CLK = 0;       /* CLK = 0 */
    }
    Dly_us(1);
    TSL_CLK = 1;           /* CLK = 1 */
    Dly_us(1);
    TSL_CLK = 0;           /* CLK = 0 */
}



void CCD_TSL1401(unsigned short int *p) 
{
	unsigned char i;
	unsigned int  temp = 0;

	TSL_SI=1;;            //SI  = 1 
	Dly_us(1);
	TSL_CLK = 1;           // CLK = 1 
	Dly_us(1);
	TSL_SI=0;           // SI  = 0 
	Dly_us(1);

	for(i=0; i<128; i++) 
	{
		Dly_us(1);
		TSL_CLK = 1;       // CLK = 1 
		Dly_us(1);
		//Sampling Pixel 2~128
		temp= ADC_Ave(ADC1,ADC1_DM1,ADC_12bit,5);       
		*p++ = temp;     
		TSL_CLK = 0;       // CLK = 0 
	}
	Dly_us(1);
	TSL_CLK = 1;           // CLK = 1 
	Dly_us(1);
	TSL_CLK = 0;           // CLK = 0 
}
void CalculateIntegrationTime(void)
{
    unsigned char i;
    unsigned int CCD_SUM=0;
    // 计算像素点的平均AD值 
    for(i=PIX_START;i<PIX_END;i++)
    {
    	CCD_SUM += CCD_AD[i];
    }
    CCD_AVG = (unsigned short int) (CCD_SUM / PIX_NUM);   //求平均值

    CCD_AVG_E = (int)2500 - (int)CCD_AVG;
    
    if(CCD_AVG_E < -500)
        IntegrationTime--;
    if(CCD_AVG_E > 200)
        IntegrationTime++;
    if(IntegrationTime <= 1)
        IntegrationTime = 1;
    if(IntegrationTime >= 20)
        IntegrationTime = 20;
}

void Display_CCD(unsigned short int * Pst)//显示CCD波形    传递的是128bit的CCD原始数据
{
        unsigned char tdat;
        unsigned char i;
        for(i=PIX_START;i<PIX_END;i++)
  	{
          tdat = (unsigned char) (Pst[i]>>8); //右移八位 我们采用的是12bit的ADC  剩4 那么 一共0--15
          if(tdat>15)	
          tdat=15;      			
          LCD_ClrDot(i);
          LCD_PutPixel((unsigned char)i ,(unsigned char)(63-tdat));  //包含有OLED的代码因此要CCD.h要在LQ12864.h之后
  	}    
}

void UART_Send_CCD(UARTn_e uart,unsigned short int * Pst)
{
    unsigned char i;
    int SendData = 0;
    UART_Put_Char(uart,'T');
    for(i=0; i<128; i++) 
    {
         SendData  =  Pst[i];
         UART_Put_Char(uart,(unsigned char)(SendData>>8)); //发送高8位
         UART_Put_Char(uart,(unsigned char)SendData);  //发送高低8位 
    }
  
}
