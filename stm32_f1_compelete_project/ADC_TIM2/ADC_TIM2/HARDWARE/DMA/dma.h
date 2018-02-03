#ifndef _DMA_H 
#define _DMA_H
#include <stm32f10x_lib.h>
#include "sys.h"
void dma_init(DMA_Channel_TypeDef*dma_number,u32 masteraddr,u32 slaveaddr,u16 number);
void dma_singlemove(DMA_Channel_TypeDef*dma_number);
#endif 
