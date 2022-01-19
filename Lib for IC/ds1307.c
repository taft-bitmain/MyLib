/*******************************************************************************
 * @file     ds1307.c
 * @brief    drive the ds1307 clock chip
 * @version  V1.0
 * @date     2021.3.31
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/
#include "ds1307.h"

//寄存器地址定义
#define RTC_ADDR_SEC	0x00
#define RTC_ADDR_MIN	0x01
#define RTC_ADDR_HOUR	0x02
#define RTC_ADDR_DATE	0x03
#define RTC_ADDR_MONTH	0x04
#define RTC_ADDR_DAY	0x05
#define RTC_ADDR_YEAR	0x06
#define RTC_ADDR_CTL	0x07

#define RTC_ADDR_RAM_BASE	0x08

MYIIC myiic1 = {.SCL_GPIO = GPIOB,.SCL_Pin = GPIO_PIN_7,
				.SDA_GPIO = GPIOB,.SDA_Pin = GPIO_PIN_5,
				.Speed = 5,.DevAddr = 0xD0};

					
void RTC_Init(void)
{
	MYIIC_Init(&myiic1);
}

void RTC_RAMWrite(char addr,char *data,unsigned char len)
{
	RTC_WriteMem(RTC_ADDR_RAM_BASE+(addr%56), data, len);
}

void RTC_RAMRead(char addr,char *data,unsigned char len)
{
	RTC_ReadMem(RTC_ADDR_RAM_BASE+(addr%56), data, len);
}

void RTC_CLKStop(void)
{
	char temp;
	RTC_ReadReg(RTC_ADDR_SEC,&temp);
	temp |= 0x80;
	RTC_WriteReg(RTC_ADDR_SEC,temp);

}
void RTC_CLKStart(void)
{
	char temp;
	RTC_ReadReg(RTC_ADDR_SEC,&temp);
	temp &= 0x7F;
	RTC_WriteReg(RTC_ADDR_SEC,temp);

}

void RTC_TimeRead(RTC_Time *t)
{
	char tmp[7];
	RTC_ReadMem(RTC_ADDR_SEC,tmp,7);
	t->sec  = ((tmp[0]&0x70)>>4)*10 + (tmp[0]&0x0F);
	t->min  = ((tmp[1]&0x70)>>4)*10 + (tmp[1]&0x0F);
	t->hour = ((tmp[2]&0x30)>>4)*10 + (tmp[2]&0x0F);
	t->date = ((tmp[3]&0x30)>>4)*10 + (tmp[3]&0x0F);
	t->mon  = ((tmp[4]&0x10)>>4)*10 + (tmp[4]&0x0F);
	t->day  = (tmp[5]&0x07);
	t->year = ((tmp[6]&0xF0)>>4)*10 + (tmp[6]&0x0F);
}

void RTC_TimeWrite(RTC_Time *t)
{
	char tmp[7] = {0};
	tmp[0] = 0x7F & (((t->sec/10)<<4) | (t->sec%10));
	tmp[1] = 0x7F & (((t->min/10)<<4) | (t->min%10));
	tmp[2] = 0x3F & (((t->hour/10)<<4) | (t->hour%10));
	tmp[3] = 0x3F & (((t->date/10)<<4) | (t->date%10));
	tmp[4] = 0x1F & (((t->mon/10)<<4) | (t->mon%10));
	tmp[5] = 0x07 & t->day;
	tmp[6] = ((t->year/10)<<4) | (t->year%10);
	RTC_WriteMem(RTC_ADDR_SEC,tmp,7);
}

unsigned char RTC_TimeToString(RTC_Time *t,char *str)
{
	char *start = str;
	*str++ = t->year/10 + '0';
	*str++ = t->year%10 + '0';
	*str++ = '-';
	*str++ = t->mon/10 + '0';
	*str++ = t->mon%10 + '0';
	*str++ = '-';
	*str++ = t->date/10 + '0';
	*str++ = t->date%10 + '0';
	*str++ = ' ';
	switch(t->day)
	{
		case 1:*str++ = 'M';*str++ = 'o';*str++ = 'n';break;
		case 2:*str++ = 'T';*str++ = 'u';*str++ = 'e';*str++ = 's';break;
		case 3:*str++ = 'W';*str++ = 'e';*str++ = 'd';break;
		case 4:*str++ = 'T';*str++ = 'h';*str++ = 'u';*str++ = 'r';break;
		case 5:*str++ = 'F';*str++ = 'r';*str++ = 'i';break;
		case 6:*str++ = 'S';*str++ = 'a';*str++ = 't';break;
		case 7:*str++ = 'S';*str++ = 'u';*str++ = 'n';break;
		default:*str++ = 'x';*str++ = 'x';*str++ = 'x';break;
	}
	*str++ = ' ';
	*str++ = t->hour/10 + '0';
	*str++ = t->hour%10 + '0';
	*str++ = ':';
	*str++ = t->min/10 + '0';
	*str++ = t->min%10 + '0';
	*str++ = ':';
	*str++ = t->sec/10 + '0';
	*str++ = t->sec%10 + '0';
	*str = 0;
	return str-start;
}
