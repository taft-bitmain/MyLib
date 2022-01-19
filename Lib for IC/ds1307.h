/*******************************************************************************
 * @file     ds1307.h
 * @brief    drive the ds1307 clock chip
 * @version  V1.0
 * @date     2021.3.31
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1.RAM has 56 bytes.
	2.Volitage range is 4.5V~5.5V, batttery is 3V.
EXAMPLE CODE:
	//21-1-1 1 12:00:00
	char data[30],len;
	RTC_Time t;
	t.sec = 00;
	t.min = 00;
	t.hour= 12;
	t.date= 1;
	t.mon = 1;
	t.day = 1;
	t.year= 21;
	RTC_Init();
	RTC_TimeWrite(&t);
	RTC_TimeRead(&t);
	len = RTC_TimeToString(&t,data);
	HAL_UART_Transmit(&huart1,(uint8_t*)data,len,1000);
	
	RTC_RAMWrite(0,"I love you!",11);
	RTC_RAMRead(0,data,11);
	HAL_UART_Transmit(&huart1,(uint8_t*)data,11,1000);
	HAL_Delay(1000);
*******************************************************************************/

#ifndef __DS1307_H
#define __DS1307_H
#ifdef __cplusplus
extern "c" {
#endif


//接口定义
#include "myiic.h"

#define RTC_ReadReg(ADDR,data)			MYIIC_ReadReg(&myiic1,ADDR,data)
#define RTC_WriteReg(ADDR,data)			MYIIC_WriteReg(&myiic1,ADDR,data)
#define RTC_WriteMem(ADDR,data,len)		MYIIC_WriteMem(&myiic1,ADDR,data,len)
#define RTC_ReadMem(ADDR,data,len)		MYIIC_ReadMem(&myiic1,ADDR,data,len)




//存储时间结构体 函数不会进行参数检查，使用者耗子尾汁
typedef struct{
	unsigned char sec;		//00~59
	unsigned char min;		//00~59
	unsigned char hour;		//00~23
	unsigned char date;		//1~31
	unsigned char mon;		//1~12
	unsigned char day;		//1~7
	unsigned char year;		//00~99
}RTC_Time;



//函数接口定义
void	RTC_Init			(void);
void	RTC_RAMWrite		(char addr,char *data,unsigned char len);
void	RTC_RAMRead			(char addr,char *data,unsigned char len);
void	RTC_CLKStop			(void);
void	RTC_CLKStart		(void);
void	RTC_TimeRead		(RTC_Time *t);
void	RTC_TimeWrite		(RTC_Time *t);
unsigned char	RTC_TimeToString	(RTC_Time *t,char *str);
unsigned char	RTC_TimeToHex		(RTC_Time *t,char *str);


#ifdef __cplusplus
}
#endif
#endif
