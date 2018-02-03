/******************** LQ_K60_������ v1.0 ********************
 * �ļ���           ��PIT.C
 * ����             ������PIT����ģʽ
 * ��ע             ���ٷ��������޸�
 * ����             ��2015-10-16
 * ʵ��ƽ̨         ������ k60DN512Z���İ� 
 * ��������         ��IAR 7.3
 * ����             �������� 006
 * �Ա���           ��https://longqiu.taobao.com
 * �������ܳ�����Ⱥ ��202949437
*************************************************************/

#include "include.h"
#include "PIT.h"

//-------------------------------------------------------------------------*
//������: pit_init                                                        
//��  ��: ��ʼ��PIT                                                       
//��  ��: pitn:ģ����PIT0��PIT1��PIT2��PIT3
//        cnt �ж�ʱ�䣬��λ1ms
//��  ��: ��                                                              
//��  ��: pit_init(PIT0,1000); PIT0�жϣ�1000ms����1s����PIT0_interrupt()һ��                                  
//-------------------------------------------------------------------------*
void PIT_Init(PITn pitn, u32 cnt)
{
    //PIT �õ��� Bus Clock ����Ƶ��

    /* ����ʱ��*/
    SIM_SCGC6       |= SIM_SCGC6_PIT_MASK;                            //ʹ��PITʱ��

    /* PITģ����� PIT Module Control Register (PIT_MCR) */
    PIT_MCR         &= ~(PIT_MCR_MDIS_MASK | PIT_MCR_FRZ_MASK );      //ʹ��PIT��ʱ��ʱ�� ������ģʽ�¼�������

    /* ��ʱ������ֵ���� Timer Load Value Register (PIT_LDVALn) */
    PIT_LDVAL(pitn)  = cnt*bus_clk*1000;                                          //��������ж�ʱ��

    //��ʱʱ�䵽�˺�TIF �� 1 ��д1��ʱ��ͻ���0
    PIT_Flag_Clear(pitn);                                             //���жϱ�־λ

    /* ��ʱ�����ƼĴ��� Timer Control Register (PIT_TCTRL0) */
    PIT_TCTRL(pitn) |= ( PIT_TCTRL_TEN_MASK | PIT_TCTRL_TIE_MASK );   //ʹ�� PITn��ʱ��,����PITn�ж�

    enable_irq(pitn + 68);			                                //���������ŵ�IRQ�ж�
}

//-------------------------------------------------------------------------*
//������: PIT0_interrupt                                                        
//��  ��: PIT�жϺ���                                                       
//��  ��: ��
//��  ��: ��                                                              
//��  ��: �ɳ�ʼ���������೤ʱ�����һ��                                  
//-------------------------------------------------------------------------*

void PIT0_Interrupt()
{
  static unsigned char TimerCnt20ms = 0; 
  unsigned char integration_piont;
  PIT_Flag_Clear(PIT0);       //���жϱ�־λ
 /*�û�����������*/  
    TimerCnt20ms++;
    /* �����ع�ʱ�����5ms�����ڵ��ع�� */
    integration_piont = (unsigned char)(20 - IntegrationTime);
    if(integration_piont >= 2)
    {   /* �ع��С��2(�ع�ʱ�����18ms)�򲻽������ع� */
            if(integration_piont == TimerCnt20ms)
                    Integration();           /* �ع⿪ʼ */
    }
    if(TimerCnt20ms == 20)
    {
            TimerCnt20ms = 0;
            CCD_TSL1401(CCD_AD);
            CalculateIntegrationTime();
            UART_Send_CCD(UART0,CCD_AD);
            Display_CCD(CCD_AD);         //��������ʾCCD����
    }  
}

void PIT1_Interrupt()
{
  PIT_Flag_Clear(PIT1);       //���жϱ�־λ
 /*�û�����������*/
}

void PIT2_Interrupt()
{
  PIT_Flag_Clear(PIT1);       //���жϱ�־λ
  /*�û�����������*/
}

void PIT3_Interrupt()
{
  PIT_Flag_Clear(PIT3);       //���жϱ�־λ
  /*�û�����������*/
}