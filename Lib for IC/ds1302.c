/*******************************************************************************
 * @file     ds1302.c
 * @brief    drive the ds1302 clock chip
 * @version  V1.0
 * @date     2021.2.10
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/
#include "ds1302.h"

//寄存器地址定义
#define RTC_ADDR_SEC	0x80
#define RTC_ADDR_MIN	0x82
#define RTC_ADDR_HOUR	0x84
#define RTC_ADDR_DATE	0x86
#define RTC_ADDR_MONTH	0x88
#define RTC_ADDR_DAY	0x8A
#define RTC_ADDR_YEAR	0x8C
#define RTC_ADDR_CTL	0x8E
#define RTC_ADDR_CHG	0x90
#define RTC_ADDR_RAM		0xC0
#define RTC_ADDR_RAM_MASK	0x3C
#define RTC_ADDR_CLKBST		0xBE
#define RTC_ADDR_RAMBST		0xFE

void RTC_DelayUs(unsigned int x)
{
	uint32_t t = x * 160;
	while(t--);
}

void RTC_IO_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = IO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(IO_GPIO_Port, &GPIO_InitStruct);
}

void RTC_IO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = IO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(IO_GPIO_Port, &GPIO_InitStruct);
}

void RTC_WriteByte(char data)
{
	unsigned char index;
	RTC_IO_OUT();
	for(index = 0; index <8 ;index++)
	{
		DS1302_SCLK(0);		
		DS1302_O(data & (0x01<<index));
							RTC_DELAYUS(2);
		DS1302_SCLK(1);		RTC_DELAYUS(2);
	}
}

void RTC_WriteAddr(char addr,char data)
{
	DS1302_CE(1);	RTC_DELAYUS(4);
	RTC_WriteByte(addr & 0xFE);
	RTC_WriteByte(data);
	DS1302_SCLK(0);	RTC_DELAYUS(2);
	DS1302_CE(0);	RTC_DELAYUS(2);
}

void RTC_WriteBurst(unsigned char addr,char *data, unsigned char len)
{
	unsigned char len_t;
	if(addr == RTC_ADDR_CLKBST)
		len = 8;
	else if(addr == RTC_ADDR_RAMBST)
		len = len>31?31:len;
	else
		return;
	DS1302_CE(1);	RTC_DELAYUS(4);
	RTC_WriteByte(addr & 0xFE);
	for(len_t = 0; len_t < len ;len_t++)
		RTC_WriteByte(*(data + len_t));
	DS1302_SCLK(0);	RTC_DELAYUS(2);
	DS1302_CE(0);	RTC_DELAYUS(2);
	
}

char RTC_ReadByte(void)
{
	unsigned char index,data = 0x00;
	RTC_IO_IN();
	for(index = 0; index <8 ;index++)
	{
		DS1302_SCLK(1);		RTC_DELAYUS(2);
		DS1302_SCLK(0);		RTC_DELAYUS(2);
		data |= (DS1302_I?(0x01<<index):0);
	}
	return data;
}

char RTC_ReadAddr(char addr)
{
	char data;
	DS1302_CE(1);	RTC_DELAYUS(4);
	RTC_WriteByte(addr | 0x01);
	data = RTC_ReadByte();
	DS1302_CE(0);	RTC_DELAYUS(2);
	return data;
}

void RTC_ReadBurst(unsigned char addr,char *data, unsigned char len)
{
	unsigned char len_t;
	if(addr == RTC_ADDR_CLKBST)
		len = 8;
	else if(addr == RTC_ADDR_RAMBST)
		len %= 32;
	else
		return;
	DS1302_CE(1);	RTC_DELAYUS(4);
	RTC_WriteByte(addr | 0x01);
	for(len_t = 0; len_t < len ;len_t++)
		*(data + len_t) = RTC_ReadByte();
	DS1302_CE(0);	RTC_DELAYUS(2);
}

void RTC_Init(void)
{
	DS1302_O(0);
	DS1302_SCLK(0);
	DS1302_CE(0);
	RTC_DELAYUS(1);
}

void RTC_WP(char flag)
{
	if(flag)
		RTC_WriteAddr(RTC_ADDR_CTL,0x80);
	else
		RTC_WriteAddr(RTC_ADDR_CTL,0x00);
}

void RTC_RAMWrite(char addr,char *data,unsigned char len)
{
	RTC_WP(0);
	unsigned char index;	
	for(index = 0; index <len ;index++)
	{
		RTC_WriteAddr( RTC_ADDR_RAM|((addr<<1)&RTC_ADDR_RAM_MASK), *( data+index));
	}
	RTC_WP(1);
}

void RTC_RAMRead(char addr,char *data,unsigned char len)
{
	unsigned char index;	
	for(index = 0; index <len ;index++)
	{
		*(data+index) = RTC_ReadAddr( RTC_ADDR_RAM|((addr<<1)&RTC_ADDR_RAM_MASK));
	}
}

void RTC_CLKStop(void)
{
	char temp = RTC_ReadAddr(RTC_ADDR_SEC);
	temp |= 0x80;
	RTC_WP(0);
	RTC_WriteAddr(RTC_ADDR_SEC,temp);
	RTC_WP(1);
}
void RTC_CLKStart(void)
{
	char temp = RTC_ReadAddr(RTC_ADDR_SEC);
	temp &= 0x7F;
	RTC_WP(0);
	RTC_WriteAddr(RTC_ADDR_SEC,temp);
	RTC_WP(1);
}

void RTC_TimeRead(RTC_Time *t)
{
	char tmp[8];
	RTC_ReadBurst(RTC_ADDR_CLKBST,tmp,8);
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
	char tmp[8] = {0};
	tmp[0] = 0x7F & (((t->sec/10)<<4) | (t->sec%10));
	tmp[1] = 0x7F & (((t->min/10)<<4) | (t->min%10));
	tmp[2] = 0x3F & (((t->hour/10)<<4) | (t->hour%10));
	tmp[3] = 0x3F & (((t->date/10)<<4) | (t->date%10));
	tmp[4] = 0x1F & (((t->mon/10)<<4) | (t->mon%10));
	tmp[5] = 0x07 & t->day;
	tmp[6] = ((t->year/10)<<4) | (t->year%10);
	tmp[7] = 0x80;
	RTC_WP(0);
	RTC_WriteBurst(RTC_ADDR_CLKBST,tmp,8);
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

