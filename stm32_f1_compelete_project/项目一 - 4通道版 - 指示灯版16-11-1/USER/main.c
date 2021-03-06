/*程序功能：通过modbus协议，当usRegHoldingBuf[1]里面值为0x0000时，反复对
1、2、3、4通道进行循环采集数值并计算出幅值和频率，将最后的最大的20个极值
点的频率（坐标）和20个极值放到usRegInputBuf中；当usRegHoldingBuf[1]里面值为
0x0101时，从串口发出通道1采集的4000个原始数据，当usRegHoldingBuf[1]里面值为
0x0102时，从串口发出通道2采集的4000个原始数据，后面3、4通道同理电流传感器除以1600
电压传感器除以1100,现在是串口发送高低八位，之前是串口打印函数printf，新增功能：在串口发送之前
加了一个函数，防止第一字节丢失，还有加了4000个数的异或，防止数据传输过程错误，最后有
"\n"，结束符，用于上位机判断结束接收字符
 * main.c
 *
 *  Created on: 2016年10月20日
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
/* ----------------------- 自有数据声明声明 ---------------------------------*/
#define numstages 2 
unsigned char k=0x00;//定义一个DMA全局标志量
u16 ADCData[4000]={0};//adc采集数组,必须满足4096个数值
float32_t ADCData1[4096]={0};
int *kk;//将x的地址强制转换成为int*的指针变量的值,用于后面的程序
const float32_t IIRCoeffs32LP[5*numstages] = 
    {
     1.0f,  2.0f,  1.0f, 1.9492159580258415f,  -0.95306989532789055f ,      
     1.0f,  2.0f,  1.0f, 1.8866095826215064f,  -0.89033973628402419f                         
    };
float32_t ScaleValue=0.00096348432551229075f * 0.00093253841562947442f;
float32_t IIRStateF32[4*numstages];
arm_rfft_fast_instance_f32 fft_state;
arm_biquad_casd_df1_inst_f32 iir_state;
/* ----------------------- 函数声明 ---------------------------------*/
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
 void holdbuffervalue();//声明保持寄存器检测函数
/* ----------------------- Start implementation -----------------------------*/
int
main(void)
{
  
  usRegHoldingBuf[1]=0x0000;//初始化为0x0000
  RCC_Configuration();//时钟配置
  TIM2_Configuration();//tim2，adc触发开关配置
  DMA_Configuration();  //dma配置  
  
  eMBErrorCode eStatus;
  eStatus = eMBInit(MB_RTU, 0x01, 0, 9600, MB_PAR_NONE); 
  eStatus = eMBEnable();
  while(1)
    {
       GPIO_SetBits(GPIOC, GPIO_Pin_2);//定义a0口的高电平      
       if(usRegHoldingBuf[1]==0x0000)//当保持寄存器为原始数值时那就直接计算八通道前20个数值
       {
         ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);
         ADC_Configuration();         
         data_change(1);//对7大功能进行变换         
         
         ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,ADC_SampleTime_239Cycles5);
         ADC_Configuration();             
         data_change(2);//对7大功能进行变换
         
         ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1,ADC_SampleTime_239Cycles5);
         ADC_Configuration();             
         data_change(3);//对7大功能进行变换
         
         ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1,ADC_SampleTime_239Cycles5);
         ADC_Configuration();              
         data_change(4);//对7大功能进行变换
         
     //     ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1,ADC_SampleTime_239Cycles5);
     //     ADC_Configuration();             
     //     data_change(5);//对7大功能进行变换
     //     
     //     ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1,ADC_SampleTime_239Cycles5);
     //     ADC_Configuration();              
     //     data_change(6);//对7大功能进行变换
     //     
     //     ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1,ADC_SampleTime_239Cycles5);
     //     ADC_Configuration();              
     //     data_change(7);//对7大功能进行变换
     //     
     //     ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1,ADC_SampleTime_239Cycles5);
     //     ADC_Configuration();              
     //     data_change(8);//对7大功能进行变换
       }
            
    }

  return eStatus;
}
    int fputc(int ch,FILE *f)
   {
     //ch送给USART1
      USART_SendData(USART1, ch);
      //等待发送完毕
      while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET) { }
      //返回ch
      return(ch);
    }
    void RCC_Configuration()
    {
      //SystemInit();
      
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);//使能时钟a
      GPIO_InitTypeDef GPIO_InitStructure;//定义结构体
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_8;//定义0和1口
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义端口速度
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
      GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
  
      RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO,ENABLE);//外部复用功能时钟
      RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);/*使能a口gpio的rcc时钟*/
     // RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2时钟开启 
      RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12M
    }
     void TIM2_Configuration()
    {
      TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//时钟结构体配置
      TIM_TimeBaseStructure.TIM_Period =0x3E8;//时钟的周期  979us采集一次
      TIM_TimeBaseStructure.TIM_Prescaler = 71;    //设置预分频：预分频=，即为72/6=12MHz  
      TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分频系数：不分频  
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数溢出模式  
      TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //初始化 
      
      TIM_OCInitTypeDef  TIM_OCInitStructure;
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //配置为PWM模式1  
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//定义输出使能                
      TIM_OCInitStructure.TIM_Pulse = 500;  //设置跳变值，当计数器计数到这个值时，电平发生跳变  
      TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_Low;//当定时器计数值小于CCR1时为高电平  ,pwm1时为低，pwm2时为高
      TIM_OC2Init(TIM2, &TIM_OCInitStructure); //使能通道2,oc1为通道1，oc2为通道2 output Chanel X（1.2.3.4），通道2在没有重映射的情况下对应pa7口，所以在gpio的配置中要配置pa7口
      TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);//使能TIM3在ccr2上的预装载寄存器 
      TIM_ARRPreloadConfig(TIM2, ENABLE); //使能TIM3重载寄存器ARR  
      /* TIM3 enable counter */  
      TIM_Cmd(TIM2, ENABLE);//使能TIM3 
      TIM_InternalClockConfig(TIM2);
      TIM_UpdateDisableConfig(TIM2,DISABLE);
      
    }
   void ADC_Configuration()
    {
      ADC_InitTypeDef ADC_InitStructure;//模式配置
      ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
      ADC_InitStructure.ADC_ScanConvMode = DISABLE;//单通道的单次模式
      ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//循环模式
      ADC_InitStructure.ADC_ExternalTrigConv =ADC_ExternalTrigConv_T2_CC2;//软件触发
      
      ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
      ADC_InitStructure.ADC_NbrOfChannel = 1;//规定了顺序进行规则转换的 ADC 通道的数目。这个数目的取值范围是 1 到 16。
      ADC_Init(ADC1, &ADC_InitStructure);//
      
      //ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1,ADC_SampleTime_28Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
      
      ADC_Cmd(ADC1, ENABLE);//
      ADC_DMACmd(ADC1, ENABLE);//重点使能或者失能指定的 ADC 的 DMA 请求
      ADC_ResetCalibration(ADC1);//重置指定的 ADC 的校准寄存器
      while(ADC_GetResetCalibrationStatus(ADC1));//直到重置成功    
      ADC_StartCalibration(ADC1);//开始指定 ADC 的校准状态
      while(ADC_GetCalibrationStatus(ADC1));//获取指定 ADC 的校准程序    
      ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能或者失能指定的 ADC 的软件转换启动功能 
      ADC_ExternalTrigConvCmd(ADC1,ENABLE);
    }
   void DMA_Configuration()
    {
      DMA_DeInit(DMA1_Channel1);//DMA1正好连接adc1
      DMA_InitTypeDef DMA_InitStructure;
      DMA_InitStructure.DMA_PeripheralBaseAddr=(u32)&ADC1->DR;//相当于51的buffer（缓存器）,将地址赋给前方结构体，定义外设基地址
      DMA_InitStructure.DMA_MemoryBaseAddr =(u32)&ADCData[0];//定义内存基地址，即数组的首地址
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设数据来源
      DMA_InitStructure.DMA_BufferSize = 4000;//应与数组大小相同
      DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增
      DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_HalfWord;//定义数据位数，因为数组的位数为16位
      DMA_InitStructure.DMA_MemoryDataSize =DMA_MemoryDataSize_HalfWord;//定义内存位数
      DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环缓存模式
      DMA_InitStructure.DMA_Priority = DMA_Priority_High;//优先级为中
      DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//外设到内存
      DMA_Init(DMA1_Channel1, &DMA_InitStructure);//初始化
      DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);//使能dma通道1发送完成中断
      //DMA_Cmd(DMA1_Channel1, ENABLE);//使能外设,dma的使能放到通道选择之后
      
    }
  void data_change(int channel)//7大功能变换函数,将通道名传向最终极值函数
     {
        u16 i;
        float32_t sub=0,ave=0;        
        DMA_Cmd(DMA1_Channel1, ENABLE);
        while(k==0x00)//等待dma传输完成，k标志位就编程0xff
        {
          (void) eMBPoll();//反复查询，等待串口发送的中断产生          
        }
        DMA_Cmd(DMA1_Channel1, DISABLE);//关闭dma
        k=0x00;//k归位  
        if(usRegHoldingBuf[1]!=0x0000)//当在等待中改变保持寄存器40001的值时，直接进入到原始数据发送函数
        {
          holdbuffervalue();//进入到发送原始值函数
        }       
        
      for(i=0;i<4000;i++)
      {
        ADCData1[i]=ADCData[i];//将采集到的adc原始值转成浮点型电压数值放到内存中，ADCData的使用不受影响
        sub=ADCData1[i]+sub;//求实际电压值得总值           
      }                
      ave=sub/4000;//求平均值         
     for(i=0;i<4000;i++)//将4096个数放入rfft
     {
       ADCData1[i]=(ADCData1[i]-ave)/1600;//将原始值减去平均值除以1100之后的数覆盖原始值，电压传感器1100，电流传感器1600
       //printf("%d\t", channel);
     } 
     for(i=4000;i<4096;i++)//将后面的96个b补0
     {
       ADCData1[i]=0;
     }
     //usRegInputBuf[1]=100;
     app_rfft();//傅里叶函数
     arm_iir_lp();//低通滤波函数        
     peek_data();//求极值函数
     front20(channel);//求极值前20个
         return;
     }
   /////////////////////傅里叶变换函数///////////////////////////////
  void app_rfft()//在傅里叶变换中必须设定一个局部变量，否则在求模的过程中会出错
  {    
    //u16 j;
    float32_t *pp,testoutput[4096];//float32_t的pp指针指向ADCData1，复用该数组
    pp=&ADCData1[0];//pp指针指向ADCData1
    arm_rfft_fast_init_f32(&fft_state,4096);//初始化，后面是进行运算的数量
    arm_rfft_fast_f32(&fft_state,&ADCData1[0],&testoutput[0],0);//input是运算的数，output是虚部的值，最后的那个0表示是正变换    
    arm_cmplx_mag_f32(&testoutput[0],pp,2048);//此函数用于求复数的模值，求排列的虚数的模值，output是存的虚数，pp所指的数组是求出的模值
  }
  /////////////////////巴特沃兹低通滤波函数///////////////////////////////
  void arm_iir_lp()
  {
    
      float32_t *pi;
      pi=&ADCData1[0];
     /* 初始化 */
     arm_biquad_cascade_df1_init_f32(&iir_state, numstages, (float32_t *)&IIRCoeffs32LP[0], (float32_t*)&IIRStateF32[0]);
    /* IIR滤波 */
     arm_biquad_cascade_df1_f32(&iir_state, & ADCData1[0], pi, 2048);     
     /*放缩系数 */
    // ScaleValue = 0.00096348432551229075f * 0.00093253841562947442f;
  }
  void peek_data()//求极值函数
  {
    u16 k;  
    for(k=0;k<2048;k++) //将ADCData1数组后面2048到4096利用起来
    {
       ADCData1[2048+k]=ADCData1[k]*ScaleValue;//用一个局部变量将储存有滤波值得数据腾出来
      // printf("%f\t", ADCData1[k+2048]);//滤波值
    }
    for(k=1;k<2048;k++)//比较极值点,此时ADCData1里面第一个是滤波值得第一个点，1-1999才会真正是极值或0
    {
      ADCData1[0]=0;//将坐标为0的地方直接赋值，为直流分量
      if((ADCData1[k+2048]>ADCData1[k-1+2048])&&(ADCData1[k+2048]>ADCData1[k+1+2048]))
       ADCData1[k]=ADCData1[k+2048];
     else
       ADCData1[k]=0; 
       //printf("%f\t",  ADCData1[k]);//发出极值
    }
    return;
  }
  void front20(int channel)//求极值的前20个数
  {
    u16 t,y;   
    float32_t pdat[20]={0},max=0,fe[20]={0};//用于储存极值点的数值
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
      ADCData1[(int)fe[y]]=0;//每次清除里面的最大值，下一次找第二大值        
    }  
    for(t=0;t<20;t++)
    {
      fe[t]=fe[t]*0.244140625;
    }
    switch(channel)
  {
  case 1://通道1，将数值存到0-79
    {                
      u16 j;
      for(j=0;j<80;)//坐标放到偶数位
      {         
          kk=(int *)&fe[(j-0)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=2;j<80;)//极值放到奇数位
      {
          kk=(int *)&pdat[(j-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }
    break;
  case 2://通道2，将数值存到40-79
    {
       u16 j;
      for(j=80;j<160;)//坐标放到偶数位
      {         
          kk=(int *)&fe[(j-80)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=82;j<160;)//极值放到奇数位
      {
          kk=(int *)&pdat[(j-80-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  case 3://通道3，将数值存到80-119
    {
      u16 j;
      for(j=160;j<240;)//坐标放到偶数位
      {         
          kk=(int *)&fe[(j-160)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=162;j<240;)//极值放到奇数位
      {
          kk=(int *)&pdat[(j-160-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  case 4://通道4，将数值存到120-159
    {
     u16 j;
      for(j=240;j<320;)//坐标放到偶数位
      {         
          kk=(int *)&fe[(j-240)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=242;j<320;)//极值放到奇数位
      {
          kk=(int *)&pdat[(j-240-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
     
    }   
    break;
  case 5://通道5，将数值存到160-199
    {
      u16 j;
      for(j=320;j<400;)//坐标放到偶数位
      {         
          kk=(int *)&fe[(j-320)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=322;j<400;)//极值放到奇数位
      {
          kk=(int *)&pdat[(j-320-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  case 6://通道6，将数值存到200-239
    {
     u16 j;
      for(j=400;j<480;)//坐标放到偶数位
      {         
          kk=(int *)&fe[(j-400)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=402;j<480;)//极值放到奇数位
      {
          kk=(int *)&pdat[(j-400-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }   
    break;
  case 7://通道7，将数值存到240-279
    {
      u16 j;
      for(j=480;j<560;)//坐标放到偶数位
      {         
          kk=(int *)&fe[(j-480)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=482;j<560;)//极值放到奇数位
      {
          kk=(int *)&pdat[(j-480-2)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;             
          j=j+4;
      }
    }    
    break;
  case 8://通道8，将数值存到280-319
    {
      u16 j;
      for(j=560;j<640;)//坐标放到偶数位
      {         
          kk=(int *)&fe[(j-560)/4];  
          usRegInputBuf[j]=*kk>>16;
          usRegInputBuf[j+1]=*kk;                         
          j=j+4;
      }
      for(j=562;j<640;)//极值放到奇数位
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
 
  
  void holdbuffervalue()//检测保持寄存器值函数
  {
     if(usRegHoldingBuf[1]==0x0101)
      {
        ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_239Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
        ADC_Configuration();
        channeldata();
        
      }
      else if(usRegHoldingBuf[1]==0x0102)
      {
        ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1,ADC_SampleTime_239Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
        ADC_Configuration();         
        channeldata();
      }
      else if(usRegHoldingBuf[1]==0x0103)
      {
        
        ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1,ADC_SampleTime_239Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
        ADC_Configuration();        
         channeldata();
      }
      else if(usRegHoldingBuf[1]==0x0104)
      {
        
        ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1,ADC_SampleTime_239Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
        ADC_Configuration();        
        channeldata();
      }
     // else if(usRegHoldingBuf[1]==0x0105)
     // {
     //   
     //   ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1,ADC_SampleTime_239Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
     //   ADC_Configuration();         
     //   DMA_Cmd(DMA1_Channel1, ENABLE);//使能外设dma
     //    channeldata();
     // }
     // else if(usRegHoldingBuf[1]==0x0106)
     // {
     //   
     //   ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1,ADC_SampleTime_239Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
     //   ADC_Configuration();
     //    channeldata();
     // }
     // else if(usRegHoldingBuf[1]==0x0107)
     // {
     //   ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1,ADC_SampleTime_239Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
     //   ADC_Configuration();         
     //    channeldata();
     // }
     // else if(usRegHoldingBuf[1]==0x0108)
     // {
     //   
     //   ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1,ADC_SampleTime_239Cycles5);//设置指定 ADC 的规则组通道，设置它们的转化顺序和采样时间
     //   ADC_Configuration();
     //    channeldata();
     // }
     else 
     {
       printf("eerror");
       usRegHoldingBuf[1]=0x0000;//归位
       return;
     }
     
     return ;//返回原函数
  }
  void channeldata()
  {
         u16 l,y,xor=0;//异或局部变量
         usRegHoldingBuf[1]=0x0000;//归位
         DMA_Cmd(DMA1_Channel1, ENABLE);//使能外设dma
         while(k==0x00);//等待dma传输完成，k标志位就编程0xff
         DMA_Cmd(DMA1_Channel1, DISABLE);//关闭dma
         k=0x00;//k归位   
         
         for(l=0;l<4000;l++)
         {       
           y++;
           
           //printf("%d\t",ADCData[l]);//发送第一通道原始数据
            USART_ClearFlag(USART1,USART_FLAG_TC);//作用是防止串口发送第一字节丢失
            USART_SendData(USART1,ADCData[l]>>8);//这后面加上下面判断
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));//等待发送完才可以发下一个，不然会被冲掉的。
            USART_SendData(USART1,ADCData[l]);//这后面加上下面判断
            while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));//等待发送完才可以发下一个，不然会被冲掉的。
           
           if(y<50)
           {              
              GPIO_SetBits(GPIOC, GPIO_Pin_2);//定义a0口的高电平    
           }
           else if((y>=50)&&(y<100))
           {
             GPIO_ResetBits(GPIOC, GPIO_Pin_2);//定义a0口的高电平 
           }
           while(y>100)
           {
             y=0;//归位
           }
         }
         xor=ADCData[0];
         for(l=1;l<4000;l++)
         {
           xor=xor^ADCData[l];//将数值异或运算作为校验码              
         }
         USART_SendData(USART1,xor>>8);//这后面加上下面判断
         while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));//等待发送完才可以发下一个，不然会被冲掉的。
         USART_SendData(USART1,xor);//这后面加上下面判断
         while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));//等待发送完才可以发下一个，不然会被冲掉的。//发送异或之后的校验码
         printf("\n"); //用于上位机判断结束
  }
   void DMA1_Channel1_IRQHandler()//dma通道1传输中断处理函数
{  
  if(DMA_GetITStatus(DMA1_IT_TC1))
  {    
    k=0xff;
    DMA_ClearITPendingBit(DMA1_IT_TC1);//清除中断标志位
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
