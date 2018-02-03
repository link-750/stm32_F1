/*�����ܣ�ͨ��modbusЭ�飬��usRegHoldingBuf[1]����ֵΪ0x0000ʱ��������
1��2��3��4ͨ������ѭ���ɼ���ֵ���������ֵ��Ƶ�ʣ�����������20����ֵ
���Ƶ�ʣ����꣩��20����ֵ�ŵ�usRegInputBuf�У���usRegHoldingBuf[1]����ֵΪ
0x0101ʱ���Ӵ��ڷ���ͨ��1�ɼ���4000��ԭʼ���ݣ���usRegHoldingBuf[1]����ֵΪ
0x0102ʱ���Ӵ��ڷ���ͨ��2�ɼ���4000��ԭʼ���ݣ�����3��4ͨ��ͬ���������������1600
��ѹ����������1100,�����Ǵ��ڷ��͸ߵͰ�λ��֮ǰ�Ǵ��ڴ�ӡ����printf���������ܣ��ڴ��ڷ���֮ǰ
����һ����������ֹ��һ�ֽڶ�ʧ�����м���4000��������򣬷�ֹ���ݴ�����̴��������
"\n"����������������λ���жϽ��������ַ�
 * main.c
 *
 *  Created on: 2016��10��20��
 *      Created by chuan
 */
#include"stm32f10x_conf.h"
#include "stdio.h"
#include"arm_math.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"
/* ----------------------- Defines ------------------------------------------*/

#define REG_INPUT_START 0001
#define REG_INPUT_NREGS 640
#define REG_HOLDING_START 0001
#define REG_HOLDING_NREGS 8
#define REG_COILS_START     0001
#define REG_COILS_SIZE      16
#define REG_DISC_START     0001
#define REG_DISC_SIZE      16

/* ----------------------- Static variables ---------------------------------*/
static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
static UCHAR ucRegCoilsBuf[REG_COILS_SIZE / 8];
static UCHAR ucRegDiscBuf[REG_DISC_SIZE / 8] =
  { 0, 0 };
/* ----------------------- ���������������� ---------------------------------*/
#define numstages 2 
unsigned char k=0x00;//����һ��DMAȫ�ֱ�־��
u16 ADCData[4000]={0};//adc�ɼ�����,��������4096����ֵ
float32_t ADCData1[4096]={0};
int *kk;//��x�ĵ�ַǿ��ת����Ϊint*��ָ�������ֵ,���ں���ĳ���
const float32_t IIRCoeffs32LP[5*numstages] = 
    {
     1.0f,  2.0f,  1.0f, 1.9492159580258415f,  -0.95306989532789055f ,      
     1.0f,  2.0f,  1.0f, 1.8866095826215064f,  -0.89033973628402419f                         
    };
float32_t ScaleValue=0.00096348432551229075f * 0.00093253841562947442f;
float32_t IIRStateF32[4*numstages];
arm_rfft_fast_instance_f32 fft_state;
arm_biquad_casd_df1_inst_f32 iir_state;
/* ----------------------- �������� ---------------------------------*/
 void DMA_Configuration();
 void RCC_Configuration();
 void ADC_Configuration(); 
 void TIM2_Configuration();
 
 void data_change(int);
 void front20(int);
 void peek_data();
 void app_rfft();
 void arm_iir_lp();  
 void channeldata(); 
 void holdbuffervalue();//�������ּĴ�����⺯��
/* ----------------------- Start implementation -----------------------------*/
int
main(void)
{
  
  usRegHoldingBuf[1]=0x0000;//��ʼ��Ϊ0x0000
  RCC_Configuration();//ʱ������
  TIM2_Configuration();//tim2��adc������������
  DMA_Configuration();  //dma����  
  
  eMBErrorCode eStatus;
  eStatus = eMBInit(MB_RTU, 0x01, 0, 9600, MB_PAR_NONE); 
  eStatus = eMBEnable();
  while(1)
    {
       GPIO_SetBits(GPIOC, GPIO_Pin_2);//����a0�ڵĸߵ�ƽ      
       if(usRegHoldingBuf[1]==0x0000)//�����ּĴ���Ϊԭʼ��ֵʱ�Ǿ�ֱ�Ӽ����ͨ��ǰ20����ֵ
       {
         ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);
         ADC_Configuration();         
         data_change(1);//��7���ܽ��б任         
         
         ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,ADC_SampleTime_239Cycles5);
         ADC_Configuration();             
         data_change(2);//��7���ܽ��б任
         
         ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1,ADC_SampleTime_239Cycles5);
         ADC_Configuration();             
         data_change(3);//��7���ܽ��б任
         
         ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1,ADC_SampleTime_239Cycles5);
         ADC_Configuration();              
         data_change(4);//��7���ܽ��б任
         
     //     ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1,ADC_SampleTime_239Cycles5);
     //     ADC_Configuration();             
     //     data_change(5);//��7���ܽ��б任
     //     
     //     ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1,ADC_SampleTime_239Cycles5);
     //     ADC_Configuration();              
     //     data_change(6);//��7���ܽ��б任
     //     
     //     ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1,ADC_SampleTime_239Cycles5);
     //     ADC_Configuration();              
     //     data_change(7);//��7���ܽ��б任
     //     
     //     ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1,ADC_SampleTime_239Cycles5);
     //     ADC_Configuration();              
     //     data_change(8);//��7���ܽ��б任
       }
            
    }

  return eStatus;
}
    int fputc(int ch,FILE *f)
   {
     //ch�͸�USART1
      USART_SendData(USART1, ch);
      //�ȴ��������
      while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET) { }
      //����ch
      return(ch);
    }
    void RCC_Configuration()
    {
      //SystemInit();
      
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);//ʹ��ʱ��a
      GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_8;//����0��1��
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����˿��ٶ�
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
      GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
  
      RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,ENABLE);//�ⲿ���ù���ʱ��
      RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);/*ʹ��a��gpio��rccʱ��*/
     // RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2ʱ�ӿ��� 
      RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M
    }
     void TIM2_Configuration()
    {
      TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//ʱ�ӽṹ������
      TIM_TimeBaseStructure.TIM_Period =0x3E8;//ʱ�ӵ�����  979us�ɼ�һ��
      TIM_TimeBaseStructure.TIM_Prescaler = 71;    //����Ԥ��Ƶ��Ԥ��Ƶ=����Ϊ72/6=12MHz  
      TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷ�Ƶϵ��������Ƶ  
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ������ģʽ  
      TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //��ʼ�� 
      
      TIM_OCInitTypeDef  TIM_OCInitStructure;
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //����ΪPWMģʽ1  
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�������ʹ��                
      TIM_OCInitStructure.TIM_Pulse = 500;  //��������ֵ�������������������ֵʱ����ƽ��������  
      TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_Low;//����ʱ������ֵС��CCR1ʱΪ�ߵ�ƽ  ,pwm1ʱΪ�ͣ�pwm2ʱΪ��
      TIM_OC2Init(TIM2, &TIM_OCInitStructure); //ʹ��ͨ��2,oc1Ϊͨ��1��oc2Ϊͨ��2 output Chanel X��1.2.3.4����ͨ��2��û����ӳ�������¶�Ӧpa7�ڣ�������gpio��������Ҫ����pa7��
      TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//ʹ��TIM3��ccr2�ϵ�Ԥװ�ؼĴ��� 
      TIM_ARRPreloadConfig(TIM2, ENABLE); //ʹ��TIM3���ؼĴ���ARR  
      /* TIM3 enable counter */  
      TIM_Cmd(TIM2, ENABLE);//ʹ��TIM3 
      TIM_InternalClockConfig(TIM2);
      TIM_UpdateDisableConfig(TIM2,DISABLE);
      
    }
   void ADC_Configuration()
    {
      ADC_InitTypeDef ADC_InitStructure;//ģʽ����
      ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
      ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ͨ���ĵ���ģʽ
      ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//ѭ��ģʽ
      ADC_InitStructure.ADC_ExternalTrigConv =ADC_ExternalTrigConv_T2_CC2;//�������
      
      ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
      ADC_InitStructure.ADC_NbrOfChannel = 1;//�涨��˳����й���ת���� ADC ͨ������Ŀ�������Ŀ��ȡֵ��Χ�� 1 �� 16��
      ADC_Init(ADC1, &ADC_InitStructure);//
      
      //ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1,ADC_SampleTime_28Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
      
      ADC_Cmd(ADC1, ENABLE);//
      ADC_DMACmd(ADC1, ENABLE);//�ص�ʹ�ܻ���ʧ��ָ���� ADC �� DMA ����
      ADC_ResetCalibration(ADC1);//����ָ���� ADC ��У׼�Ĵ���
      while(ADC_GetResetCalibrationStatus(ADC1));//ֱ�����óɹ�    
      ADC_StartCalibration(ADC1);//��ʼָ�� ADC ��У׼״̬
      while(ADC_GetCalibrationStatus(ADC1));//��ȡָ�� ADC ��У׼����    
      ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ���� ADC �����ת���������� 
      ADC_ExternalTrigConvCmd(ADC1,ENABLE);
    }
   void DMA_Configuration()
    {
      DMA_DeInit(DMA1_Channel1);//DMA1��������adc1
      DMA_InitTypeDef DMA_InitStructure;
      DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//�൱��51��buffer����������,����ַ����ǰ���ṹ�壬�����������ַ
      DMA_InitStructure.DMA_MemoryBaseAddr =(u32)&ADCData[0];//�����ڴ����ַ����������׵�ַ
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//����������Դ
      DMA_InitStructure.DMA_BufferSize = 4000;//Ӧ�������С��ͬ
      DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ����
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ����
      DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_HalfWord;//��������λ������Ϊ�����λ��Ϊ16λ
      DMA_InitStructure.DMA_MemoryDataSize =DMA_MemoryDataSize_HalfWord;//�����ڴ�λ��
      DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ������ģʽ
      DMA_InitStructure.DMA_Priority = DMA_Priority_High;//���ȼ�Ϊ��
      DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//���赽�ڴ�
      DMA_Init(DMA1_Channel1, &DMA_InitStructure);//��ʼ��
      DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);//ʹ��dmaͨ��1��������ж�
      //DMA_Cmd(DMA1_Channel1, ENABLE);//ʹ������,dma��ʹ�ܷŵ�ͨ��ѡ��֮��
      
    }
  void data_change(int channel)//7���ܱ任����,��ͨ�����������ռ�ֵ����
     {
        u16 i;
        float32_t sub=0,ave=0;        
        DMA_Cmd(DMA1_Channel1, ENABLE);
        while(k==0x00)//�ȴ�dma������ɣ�k��־λ�ͱ��0xff
        {
          (void) eMBPoll();//������ѯ���ȴ����ڷ��͵��жϲ���          
        }
        DMA_Cmd(DMA1_Channel1, DISABLE);//�ر�dma
        k=0x00;//k��λ  
        if(usRegHoldingBuf[1]!=0x0000)//���ڵȴ��иı䱣�ּĴ���40001��ֵʱ��ֱ�ӽ��뵽ԭʼ���ݷ��ͺ���
        {
          holdbuffervalue();//���뵽����ԭʼֵ����
        }       
        
      for(i=0;i<4000;i++)
      {
        ADCData1[i]=ADCData[i];//���ɼ�����adcԭʼֵת�ɸ����͵�ѹ��ֵ�ŵ��ڴ��У�ADCData��ʹ�ò���Ӱ��
        sub=ADCData1[i]+sub;//��ʵ�ʵ�ѹֵ����ֵ           
      }                
      ave=sub/4000;//��ƽ��ֵ         
     for(i=0;i<4000;i++)//��4096��������rfft
     {
       ADCData1[i]=(ADCData1[i]-ave)/1600;//��ԭʼֵ��ȥƽ��ֵ����1100֮���������ԭʼֵ����ѹ������1100������������1600
       //printf("%d\t", channel);
     } 
     for(i=4000;i<4096;i++)//�������96��b��0
     {
       ADCData1[i]=0;
     }
     //usRegInputBuf[1]=100;
     app_rfft();//����Ҷ����
     arm_iir_lp();//��ͨ�˲�����        
     peek_data();//��ֵ����
     front20(channel);//��ֵǰ20��
         return;
     }
   /////////////////////����Ҷ�任����///////////////////////////////
  void app_rfft()//�ڸ���Ҷ�任�б����趨һ���ֲ���������������ģ�Ĺ����л����
  {    
    //u16 j;
    float32_t *pp,testoutput[4096];//float32_t��ppָ��ָ��ADCData1�����ø�����
    pp=&ADCData1[0];//ppָ��ָ��ADCData1
    arm_rfft_fast_init_f32(&fft_state,4096);//��ʼ���������ǽ������������
    arm_rfft_fast_f32(&fft_state,&ADCData1[0],&testoutput[0],0);//input�����������output���鲿��ֵ�������Ǹ�0��ʾ�����任    
    arm_cmplx_mag_f32(&testoutput[0],pp,2048);//�˺�������������ģֵ�������е�������ģֵ��output�Ǵ��������pp��ָ�������������ģֵ
  }
  /////////////////////�������ȵ�ͨ�˲�����///////////////////////////////
  void arm_iir_lp()
  {
    
      float32_t *pi;
      pi=&ADCData1[0];
     /* ��ʼ�� */
     arm_biquad_cascade_df1_init_f32(&iir_state, numstages, (float32_t *)&IIRCoeffs32LP[0], (float32_t*)&IIRStateF32[0]);
    /* IIR�˲� */
     arm_biquad_cascade_df1_f32(&iir_state, & ADCData1[0], pi, 2048);     
     /*����ϵ�� */
    // ScaleValue = 0.00096348432551229075f * 0.00093253841562947442f;
  }
  void peek_data()//��ֵ����
  {
    u16 k;  
    for(k=0;k<2048;k++) //��ADCData1�������2048��4096��������
    {
       ADCData1[2048+k]=ADCData1[k]*ScaleValue;//��һ���ֲ��������������˲�ֵ�������ڳ���
      // printf("%f\t", ADCData1[k+2048]);//�˲�ֵ
    }
    for(k=1;k<2048;k++)//�Ƚϼ�ֵ��,��ʱADCData1�����һ�����˲�ֵ�õ�һ���㣬1-1999�Ż������Ǽ�ֵ��0
    {
      ADCData1[0]=0;//������Ϊ0�ĵط�ֱ�Ӹ�ֵ��Ϊֱ������
      if((ADCData1[k+2048]>ADCData1[k-1+2048])&&(ADCData1[k+2048]>ADCData1[k+1+2048]))
       ADCData1[k]=ADCData1[k+2048];
     else
       ADCData1[k]=0; 
       //printf("%f\t",  ADCData1[k]);//������ֵ
    }
    return;
  }
  void front20(int channel)//��ֵ��ǰ20����
  {
    u16 t,y;   
    float32_t pdat[20]={0},max=0,fe[20]={0};//���ڴ��漫ֵ�����ֵ
    for(y=0;y<20;y++)
    {
       for(t=0;t<2048;t++)
       {
         if(max<ADCData1[t])
          {
            max=ADCData1[t];
            pdat[y]=max;
            fe[y]=t;
          }
      }
      max=0;
      ADCData1[(int)fe[y]]=0;//ÿ�������������ֵ����һ���ҵڶ���ֵ        
    }  
    for(t=0;t<20;t++)
    {
      fe[t]=fe[t]*0.244140625;
    }
    switch(channel)
  {
  case 1://ͨ��1������ֵ�浽0-79
    {                
      u16 j;
      for(j=0;j<80;)//����ŵ�ż��λ
      {         
          kk=(int *)&fe[(j-0)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=2;j<80;)//��ֵ�ŵ�����λ
      {
          kk=(int *)&pdat[(j-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }
    break;
  case 2://ͨ��2������ֵ�浽40-79
    {
       u16 j;
      for(j=80;j<160;)//����ŵ�ż��λ
      {         
          kk=(int *)&fe[(j-80)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=82;j<160;)//��ֵ�ŵ�����λ
      {
          kk=(int *)&pdat[(j-80-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  case 3://ͨ��3������ֵ�浽80-119
    {
      u16 j;
      for(j=160;j<240;)//����ŵ�ż��λ
      {         
          kk=(int *)&fe[(j-160)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=162;j<240;)//��ֵ�ŵ�����λ
      {
          kk=(int *)&pdat[(j-160-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  case 4://ͨ��4������ֵ�浽120-159
    {
     u16 j;
      for(j=240;j<320;)//����ŵ�ż��λ
      {         
          kk=(int *)&fe[(j-240)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=242;j<320;)//��ֵ�ŵ�����λ
      {
          kk=(int *)&pdat[(j-240-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
     
    }   
    break;
  case 5://ͨ��5������ֵ�浽160-199
    {
      u16 j;
      for(j=320;j<400;)//����ŵ�ż��λ
      {         
          kk=(int *)&fe[(j-320)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=322;j<400;)//��ֵ�ŵ�����λ
      {
          kk=(int *)&pdat[(j-320-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  case 6://ͨ��6������ֵ�浽200-239
    {
     u16 j;
      for(j=400;j<480;)//����ŵ�ż��λ
      {         
          kk=(int *)&fe[(j-400)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=402;j<480;)//��ֵ�ŵ�����λ
      {
          kk=(int *)&pdat[(j-400-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  case 7://ͨ��7������ֵ�浽240-279
    {
      u16 j;
      for(j=480;j<560;)//����ŵ�ż��λ
      {         
          kk=(int *)&fe[(j-480)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=482;j<560;)//��ֵ�ŵ�����λ
      {
          kk=(int *)&pdat[(j-480-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }    
    break;
  case 8://ͨ��8������ֵ�浽280-319
    {
      u16 j;
      for(j=560;j<640;)//����ŵ�ż��λ
      {         
          kk=(int *)&fe[(j-560)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=562;j<640;)//��ֵ�ŵ�����λ
      {
          kk=(int *)&pdat[(j-560-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  
  }     
          
    return;
  }
 
  
  void holdbuffervalue()//��Ᵽ�ּĴ���ֵ����
  {
     if(usRegHoldingBuf[1]==0x0101)
      {
        ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
        ADC_Configuration();
        channeldata();
        
      }
      else if(usRegHoldingBuf[1]==0x0102)
      {
        ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,ADC_SampleTime_239Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
        ADC_Configuration();         
        channeldata();
      }
      else if(usRegHoldingBuf[1]==0x0103)
      {
        
        ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1,ADC_SampleTime_239Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
        ADC_Configuration();        
         channeldata();
      }
      else if(usRegHoldingBuf[1]==0x0104)
      {
        
        ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1,ADC_SampleTime_239Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
        ADC_Configuration();        
        channeldata();
      }
     // else if(usRegHoldingBuf[1]==0x0105)
     // {
     //   
     //   ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1,ADC_SampleTime_239Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
     //   ADC_Configuration();         
     //   DMA_Cmd(DMA1_Channel1, ENABLE);//ʹ������dma
     //    channeldata();
     // }
     // else if(usRegHoldingBuf[1]==0x0106)
     // {
     //   
     //   ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1,ADC_SampleTime_239Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
     //   ADC_Configuration();
     //    channeldata();
     // }
     // else if(usRegHoldingBuf[1]==0x0107)
     // {
     //   ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1,ADC_SampleTime_239Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
     //   ADC_Configuration();         
     //    channeldata();
     // }
     // else if(usRegHoldingBuf[1]==0x0108)
     // {
     //   
     //   ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1,ADC_SampleTime_239Cycles5);//����ָ�� ADC �Ĺ�����ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
     //   ADC_Configuration();
     //    channeldata();
     // }
     else 
     {
       printf("eerror");
       usRegHoldingBuf[1]=0x0000;//��λ
       return;
     }
     
     return ;//����ԭ����
  }
  void channeldata()
  {
         u16 l,y,xor=0;//���ֲ�����
         usRegHoldingBuf[1]=0x0000;//��λ
         DMA_Cmd(DMA1_Channel1, ENABLE);//ʹ������dma
         while(k==0x00);//�ȴ�dma������ɣ�k��־λ�ͱ��0xff
         DMA_Cmd(DMA1_Channel1, DISABLE);//�ر�dma
         k=0x00;//k��λ   
         
         for(l=0;l<4000;l++)
         {       
           y++;
           
           //printf("%d\t",ADCData[l]);//���͵�һͨ��ԭʼ����
            USART_ClearFlag(USART1,USART_FLAG_TC);//�����Ƿ�ֹ���ڷ��͵�һ�ֽڶ�ʧ
            USART_SendData(USART1,ADCData[l]>>8);//�������������ж�
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));//�ȴ�������ſ��Է���һ������Ȼ�ᱻ����ġ�
            USART_SendData(USART1,ADCData[l]);//�������������ж�
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));//�ȴ�������ſ��Է���һ������Ȼ�ᱻ����ġ�
           
           if(y<50)
           {              
              GPIO_SetBits(GPIOC, GPIO_Pin_2);//����a0�ڵĸߵ�ƽ    
           }
           else if((y>=50)&&(y<100))
           {
             GPIO_ResetBits(GPIOC, GPIO_Pin_2);//����a0�ڵĸߵ�ƽ 
           }
           while(y>100)
           {
             y=0;//��λ
           }
         }
         xor=ADCData[0];
         for(l=1;l<4000;l++)
         {
           xor=xor^ADCData[l];//����ֵ���������ΪУ����              
         }
         USART_SendData(USART1,xor>>8);//�������������ж�
         while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));//�ȴ�������ſ��Է���һ������Ȼ�ᱻ����ġ�
         USART_SendData(USART1,xor);//�������������ж�
         while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));//�ȴ�������ſ��Է���һ������Ȼ�ᱻ����ġ�//�������֮���У����
         printf("\n"); //������λ���жϽ���
  }
   void DMA1_Channel1_IRQHandler()//dmaͨ��1�����жϴ�����
{  
  if(DMA_GetITStatus(DMA1_IT_TC1))
  {    
    k=0xff;
    DMA_ClearITPendingBit(DMA1_IT_TC1);//����жϱ�־λ
  }
}

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
  eMBErrorCode eStatus = MB_ENOERR;
  int iRegIndex;
  //printf("%d\n", usAddress);
  if ((usAddress >= REG_INPUT_START)
      && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS))
    {
      iRegIndex = (int) (usAddress - usRegInputStart);
      while (usNRegs > 0)
        {
          *pucRegBuffer++ = (unsigned char) (usRegInputBuf[iRegIndex] >> 8);
          *pucRegBuffer++ = (unsigned char) (usRegInputBuf[iRegIndex] & 0xFF);
          iRegIndex++;
          usNRegs--;
        }
    }
  else
    {
      eStatus = MB_ENOREG;
    }

  return eStatus;
}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
  eMBErrorCode eStatus;
  int iRegIndex;
  if ((usAddress >= REG_HOLDING_START)
      && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS))
    {
      iRegIndex = (int) (usAddress - usRegHoldingStart);
      switch (eMode)
        {
      case MB_REG_READ:
        while (usNRegs > 0)
          {
            *pucRegBuffer++ = (UCHAR) (usRegHoldingBuf[iRegIndex] >> 8);
            *pucRegBuffer++ = (UCHAR) (usRegHoldingBuf[iRegIndex] & 0xFF);
            iRegIndex++;
            usNRegs--;
          }
        break;
      case MB_REG_WRITE:
        while (usNRegs > 0)
          {
            usRegHoldingBuf[iRegIndex] = (USHORT) (*pucRegBuffer++ << 8);
            usRegHoldingBuf[iRegIndex] |= (USHORT) (*pucRegBuffer++);
            iRegIndex++;
            usNRegs--;
          }
        }
    }
  else
    {
      eStatus = MB_ENOREG;
    }
  return eStatus;
}

eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
  eMBErrorCode eStatus = MB_ENOERR;
  int iNCoils = (int) usNCoils;
  unsigned short usBitOffset;

  /* Check if we have registers mapped at this block. */
  if ((usAddress >= REG_COILS_START)
      && (usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE))
    {
      usBitOffset = (unsigned short) (usAddress - REG_COILS_START);
      switch (eMode)
        {
      /* Read current values and pass to protocol stack. */
      case MB_REG_READ:
        while (iNCoils > 0)
          {
            *pucRegBuffer++ = xMBUtilGetBits(ucRegCoilsBuf, usBitOffset,
                (unsigned char) (iNCoils > 8 ? 8 : iNCoils));
            iNCoils -= 8;
            usBitOffset += 8;
          }
        break;

        /* Update current register values. */
      case MB_REG_WRITE:
        while (iNCoils > 0)
          {
            xMBUtilSetBits(ucRegCoilsBuf, usBitOffset,
                (unsigned char) (iNCoils > 8 ? 8 : iNCoils), *pucRegBuffer++);
            iNCoils -= 8;
            usBitOffset += 8;
          }
        break;
        }

    }
  else
    {
      eStatus = MB_ENOREG;
    }
  return eStatus;
}

eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
  eMBErrorCode eStatus = MB_ENOERR;
  short iNDiscrete = (short) usNDiscrete;
  unsigned short usBitOffset;

  /* Check if we have registers mapped at this block. */
  if ((usAddress >= REG_DISC_START)
      && (usAddress + usNDiscrete <= REG_DISC_START + REG_DISC_SIZE))
    {
      usBitOffset = (unsigned short) (usAddress - REG_DISC_START);
      while (iNDiscrete > 0)
        {
          *pucRegBuffer++ = xMBUtilGetBits(ucRegDiscBuf, usBitOffset,
              (unsigned char) (iNDiscrete > 8 ? 8 : iNDiscrete));
          iNDiscrete -= 8;
          usBitOffset += 8;
        }
    }
  else
    {
      eStatus = MB_ENOREG;
    }
  return eStatus;
}
