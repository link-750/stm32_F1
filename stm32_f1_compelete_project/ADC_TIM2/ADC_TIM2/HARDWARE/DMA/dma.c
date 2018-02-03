#include"dma.h"
u16 m; 
void dma_init(DMA_Channel_TypeDef*dma_number,u32 masteraddr,u32 slaveaddr,u16 number)
{
 u32 p;
p=slaveaddr;
m=number;
RCC->AHBENR|=1<<0;//打开dma时钟
delay_us(5);
dma_number->CCR|=0<<4; //从外设读取数据
dma_number->CMAR=(u32)masteraddr;	//存储器地址
dma_number->CPAR=(u32)slaveaddr; //做缓冲使用,外设地址
dma_number->CNDTR=number;//代表dma通道传数输据的个数
dma_number->CCR|=1<<7;//存储器地址增量模式，因为只有为增量，才能够使得每次
//dma_number->CCR|=1<<6;//从存储器读取数据的时，地址指针可自动加1
dma_number->CCR|=1<<8;//外设数据宽度为16位
dma_number->CCR|=1<<10;//存储器数据宽度为16位
dma_number->CCR|=2<<12;//dma优先级高
dma_number->CCR|=1<<5;	//执行循环模式
dma_number->CCR|=(1<<0); //开启dma传输
}					  
void dma_singlemove(DMA_Channel_TypeDef*dma_number)
{
 dma_number->CCR&=~(1<<0);
 dma_number->CNDTR=m;//代表dma通道传输数据的个数
 dma_number->CCR|=(1<<0);
          
}
