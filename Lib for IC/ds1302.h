/*******************************************************************************
 * @file     ds1302.h
 * @brief    drive the ds1302 clock chip
 * @version  V1.0
 * @date     2021.2.10
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1.	当VCC(VCC2)连接5V电源时，上电会导致内部CLK的数据重置为 
		00-01-01 Mon 00:00:00
		当VCC(VCC2)连接3V3电源时，不存在上述问题
	2.	对于不同频率的系统，延时函数（L64）保证在大于1us即可 
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
*******************************************************************************/

#ifndef __DS1302_H
#define __DS1302_H

#include "stm32f1xx_hal.h"

//接口定义
#define CE_GPIO_Port	GPIOB
#define IO_GPIO_Port	GPIOB
#define SCLK_GPIO_Port	GPIOB
#define CE_Pin			GPIO_PIN_3
#define IO_Pin			GPIO_PIN_5
#define SCLK_Pin		GPIO_PIN_7

//基本IO操作定义
#define DS1302_CE(x)	HAL_GPIO_WritePin(CE_GPIO_Port,CE_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define DS1302_I		HAL_GPIO_ReadPin (IO_GPIO_Port,IO_Pin)
#define DS1302_O(x)		HAL_GPIO_WritePin(IO_GPIO_Port,IO_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define DS1302_SCLK(x)	HAL_GPIO_WritePin(SCLK_GPIO_Port,SCLK_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET)

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

//延时函数 当时钟频率过高时注意适当增加延时 大于1us即可 
#define RTC_DELAYUS(x)		RTC_DelayUs(x)
void	RTC_DelayUs			(unsigned int x);


//函数接口定义
void	RTC_Init			(void);
void	RTC_WP				(char flag);
void	RTC_RAMWrite		(char addr,char *data,unsigned char len);
void	RTC_RAMRead			(char addr,char *data,unsigned char len);
void	RTC_CLKStop			(void);
void	RTC_CLKStart		(void);
void	RTC_TimeRead		(RTC_Time *t);
void	RTC_TimeWrite		(RTC_Time *t);
unsigned char	RTC_TimeToString	(RTC_Time *t,char *str);

#endif
