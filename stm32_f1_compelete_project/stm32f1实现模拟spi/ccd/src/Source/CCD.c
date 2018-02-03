#include "include.h"
#include "CCD.h"
/******************** LQ_K60_������ v1.0 ********************
 * �ļ���           ��CCD.c
 * ����             ����ʼ��CCD���Լ��ɼ�CCD
 * ��ע             �����������ع����
 * ����             ��2015-12-13
 * ʵ��ƽ̨         ������ k60DN512Z���İ� 
 * ��������         ��IAR 7.3
 * ����             �������� 005
 * �Ա���           ��https://longqiu.taobao.com
 * �����������ܿƼ�������Ⱥ ��517926609
*************************************************************/
#define TSL_SI   PTE5_OUT   //E5
#define TSL_CLK  PTE4_OUT   //E4    
//ADC 9->

unsigned short int CCD_AD[128];//128�����ص�
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

void CCD_Init(void) //ccd��ʼ��
{
  GPIO_Init(PORTE,4,GPO,1);
  GPIO_Init(PORTE,5,GPO,1);
  ADC_Init(ADC1);//ADC1_DM1   adc1������
}


void Integration(void)   //�ع⺯��
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
    // �������ص��ƽ��ADֵ 
    for(i=PIX_START;i<PIX_END;i++)
    {
    	CCD_SUM += CCD_AD[i];
    }
    CCD_AVG = (unsigned short int) (CCD_SUM / PIX_NUM);   //��ƽ��ֵ

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

void Display_CCD(unsigned short int * Pst)//��ʾCCD����    ���ݵ���128bit��CCDԭʼ����
{
        unsigned char tdat;
        unsigned char i;
        for(i=PIX_START;i<PIX_END;i++)
  	{
          tdat = (unsigned char) (Pst[i]>>8); //���ư�λ ���ǲ��õ���12bit��ADC  ʣ4 ��ô һ��0--15
          if(tdat>15)	
          tdat=15;      			
          LCD_ClrDot(i);
          LCD_PutPixel((unsigned char)i ,(unsigned char)(63-tdat));  //������OLED�Ĵ������ҪCCD.hҪ��LQ12864.h֮��
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
         UART_Put_Char(uart,(unsigned char)(SendData>>8)); //���͸�8λ
         UART_Put_Char(uart,(unsigned char)SendData);  //���͸ߵ�8λ 
    }
  
}
