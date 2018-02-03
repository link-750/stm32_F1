#ifndef __CCD_H__
#define __CCD_H__


#define PIX_START	28       //显示范围
#define PIX_END		100
#define PIX_NUM		(PIX_END-PIX_START)



extern unsigned short int CCD_AD[128];
extern unsigned char TimerFlag20ms,IntegrationTime;
extern int CCD_AVG_E;
extern unsigned int CCD_MAX,CCD_MIN,CCD_AVG;




extern void CCD_Init(void); 
extern void Integration(void);   //曝光函数
extern void CCD_TSL1401(unsigned short int *p);
extern void CalculateIntegrationTime(void); 
extern void Dly_us(unsigned int us);
extern void Display_CCD(unsigned short int * Pst);//显示CCD波形    传递的是128bit的CCD原始数据
extern void UART_Send_CCD(UARTn_e uart,unsigned short int * Pst);
#endif