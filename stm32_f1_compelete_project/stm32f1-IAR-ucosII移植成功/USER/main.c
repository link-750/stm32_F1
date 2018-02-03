#include "include.h" 
INT32U OS_CPU_SysTickClkFreq(void);
OS_STK startup_stk[STARTUP_TASK_STK_SIZE];   
OS_STK task1_stk[TASK1_STK_SIZE];   
OS_STK task2_stk[TASK2_STK_SIZE];   

static void startup(void *p_arg); 
  
static void task1(void *p_arg);  
static void task2(void *p_arg); 
u8 os_err;   
  
int main(void)   
{   
  OSInit();  
  OSTaskCreate(startup,   
               (void *)0,   
               &startup_stk[STARTUP_TASK_STK_SIZE-1],   
               STARTUP_TASK_PRIO);   
  OSStart();   
  return 0;   
}  
  
static void startup(void *p_arg)  
{  
  uint32_t cnt=0;
  BSP_Init();
  cnt=OS_CPU_SysTickClkFreq();
  OS_CPU_SysTickInit(cnt/1000); //初始化时钟滴答 ,传递的数值是systick时钟产生中断的频率，公式是：cpu主频/ 理想HZ数，如，主频72Mhz，理想频率为1000hz，那么为72M/1000
  os_err = OSTaskCreate(task1,    
                        (void *)0,   
                        (OS_STK *)&task1_stk[TASK1_STK_SIZE - 1],  
                        (INT8U)TASK1_PRIO);  
    
  os_err = OSTaskCreate(task2,  
                        (void *)0,  
                        (OS_STK *)&task2_stk[TASK2_STK_SIZE - 1],  
                        (INT8U)TASK2_PRIO); 
  
  OSTaskDel(OS_PRIO_SELF);   
}  
  
static void task1(void *p_arg)   
{  
  while(1)  
  {  
    LED1();  
  }  
}  
  
static void task2(void *p_arg)   
{  
  while(1)  
  {  
    LED2();  
  }  
}  
 /*
*********************************************************************************************************
*                                          获得cpu频率
*
* Description: 获取cpu频率，确定系统滴答的时间
*
* Arguments  : None
*
* Note(s)    : 
*********************************************************************************************************
*/
  INT32U OS_CPU_SysTickClkFreq(void)     
{      
    RCC_ClocksTypeDef   rcc_clocks;       
    RCC_GetClocksFreq(&rcc_clocks);       
    return ((INT32U)rcc_clocks.HCLK_Frequency);   //return SYSCLK_FREQ_72MHz;  
} 
