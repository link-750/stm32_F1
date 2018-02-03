#include"dma.h"
u16 m; 
void dma_init(DMA_Channel_TypeDef*dma_number,u32 masteraddr,u32 slaveaddr,u16 number)
{
 u32 p;
p=slaveaddr;
m=number;
RCC->AHBENR|=1<<0;//��dmaʱ��
delay_us(5);
dma_number->CCR|=0<<4; //�������ȡ����
dma_number->CMAR=(u32)masteraddr;	//�洢����ַ
dma_number->CPAR=(u32)slaveaddr; //������ʹ��,�����ַ
dma_number->CNDTR=number;//����dmaͨ��������ݵĸ���
dma_number->CCR|=1<<7;//�洢����ַ����ģʽ����Ϊֻ��Ϊ���������ܹ�ʹ��ÿ��
//dma_number->CCR|=1<<6;//�Ӵ洢����ȡ���ݵ�ʱ����ַָ����Զ���1
dma_number->CCR|=1<<8;//�������ݿ��Ϊ16λ
dma_number->CCR|=1<<10;//�洢�����ݿ��Ϊ16λ
dma_number->CCR|=2<<12;//dma���ȼ���
dma_number->CCR|=1<<5;	//ִ��ѭ��ģʽ
dma_number->CCR|=(1<<0); //����dma����
}					  
void dma_singlemove(DMA_Channel_TypeDef*dma_number)
{
 dma_number->CCR&=~(1<<0);
 dma_number->CNDTR=m;//����dmaͨ���������ݵĸ���
 dma_number->CCR|=(1<<0);
          
}
