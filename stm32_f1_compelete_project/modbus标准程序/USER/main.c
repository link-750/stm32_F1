/*此程序的重点是反复查询这个函数(void) eMBPoll();只有在此函数中查询到modbus上位机
有命令时，才会发送相关的数据，而当其他程序的时间占用的时间比较长，此时相对于查询函数
的时间就比较少，上位机就很难在那查询短时间内发送数据，解决方法是，将查询函数(void) eMBPoll();
执行5000次，此时留在(void) eMBPoll();函数的时间就比较充足，占整个的时间比重才大，上位机在
在查询函数的时间内才能读到相关数据。
 * main.c
 *
 *  Created on: 2015年4月20日
 *      Author: 张品好
 */
#include"stm32f10x_conf.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1000
#define REG_INPUT_NREGS 4
#define REG_HOLDING_START 1000
#define REG_HOLDING_NREGS 4
#define REG_COILS_START     1000
#define REG_COILS_SIZE      16
#define REG_DISC_START     1000
#define REG_DISC_SIZE      16

/* ----------------------- Static variables ---------------------------------*/
static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];
static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];
static UCHAR ucRegCoilsBuf[REG_COILS_SIZE / 8];
static UCHAR ucRegDiscBuf[REG_DISC_SIZE / 8] =
  { 0, 0 };

/* ----------------------- Private functions ---------------------------------*/
void
LED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/* ----------------------- Start implementation -----------------------------*/
int
main(void)
{
  eMBErrorCode eStatus;

  LED_Init();
  ucRegCoilsBuf[0] |= 0x01;

  eStatus = eMBInit(MB_RTU, 0x01, 0, 9600, MB_PAR_NONE);

  /* Enable the Modbus Protocol Stack. */
  eStatus = eMBEnable();

  for (;;)
    {
      (void) eMBPoll();
      if (ucRegCoilsBuf[0] & 0x01)
        {
          GPIOC->ODR |= GPIO_ODR_ODR2; //PC2 输出高
        }
      else
        {
          GPIOC->ODR &= ~GPIO_ODR_ODR2; //PC2输出低
        }
      if (GPIOC->IDR & GPIO_IDR_IDR3)
        {
          ucRegDiscBuf[0] |= 0x01;
        }
      else
        {
          ucRegDiscBuf[0] &= 0xFE;
        }

      /* Here we simply count the number of poll cycles. */
      usRegInputBuf[0]++;
    }

  return eStatus;
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
