/*******************************************************************************
 * @file     lcd1602.c
 * @brief    drive the lcd1602
 * @version  V1.1
 * @date     2022.1.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/

#include "lcd1602.h"

const char MyChar0[8]={0X00,0x1B,0x1F,0x1F,0x0E,0x04,0X00,0X00}; //heart
const char MyChar1[8]={0X04,0x04,0x1F,0x04,0x0A,0x0A,0X15,0X11}; //太
const char MyChar2[8]={0X06,0x0B,0x1A,0x0E,0X0B,0x04,0x1F,0X04}; //华

/*
	映射关系
	每个字节的低5位映射到每个字符的每一行，共8行
	MyChar0		display
	0X00	x x x 0 0 0 0 0
	0x1B	x x x 1 1 0 1 1
	0x1F	x x x 1 1 1 1 1
	0x1F	x x x 1 1 1 1 1
	0x0E	x x x 0 1 1 1 0
	0x04	x x x 0 0 1 0 0
	0X00	x x x 0 0 0 0 0
	0X00	x x x 0 0 0 0 0
*/

void LCD1602_delay(unsigned int x) //about 10us
{
	unsigned int y;
	for( ; x ; x--)
		for(y=0;y<160;y++);
}

#ifdef LCD_INTERFACE_PARAL8
void PARAL8_Busy(void)
{
    char try_times;
    try_times = 0;
	LCD1602_D7(1);
	LCD1602_RS(0);
	LCD1602_delay(2);
	LCD1602_RW(1);
	LCD1602_delay(2);
	while(1)
	{
		LCD1602_EN(1);
		LCD1602_delay(2);
		if(LCD1602_Read_D7 == GPIO_PIN_RESET)
			break;
		LCD1602_EN(0);
		LCD1602_delay(2);
        try_times++;
        if(try_times == 10)
            break;
	}
}

void PARAL8_Send(unsigned char data)
{
	LCD1602_D0(0x01&data);
	LCD1602_D1(0x02&data);
	LCD1602_D2(0x04&data);
	LCD1602_D3(0x08&data);
	LCD1602_D4(0x10&data);
	LCD1602_D5(0x20&data);
	LCD1602_D6(0x40&data);
	LCD1602_D7(0x80&data);
}
#endif

#ifdef LCD_INTERFACE_IIC

void IIC_Start(void)
{
	LCD1602_SCL(1);
	LCD1602_delay(2);
	LCD1602_SDA(1);
	LCD1602_delay(2);
	LCD1602_SDA(0);
	LCD1602_delay(2);
}

void IIC_Send(unsigned char data)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		LCD1602_SCL(0);
		LCD1602_delay(2);
		if(data&0x80)
			LCD1602_SDA(1);
		else
			LCD1602_SDA(0);
		LCD1602_SCL(1);
		LCD1602_delay(2);
		LCD1602_SCL(0);
		data<<=1;
	}
	LCD1602_SDA(0);
	LCD1602_SCL(1);
	LCD1602_delay(2);
	LCD1602_SCL(0);
}

void IIC_Stop(void)
{
	LCD1602_SCL(1);
	LCD1602_delay(2);
	LCD1602_SDA(0);
	LCD1602_delay(2);
	LCD1602_SDA(1);
	LCD1602_delay(2);
}

#endif

void LCD1602_Write_Command(const char *cmd,unsigned char length)
{
#ifdef LCD_INTERFACE_PARAL8
	unsigned char i;
	PARAL8_Busy();
	LCD1602_RS(0);
	LCD1602_delay(2);
	LCD1602_RW(0);
	LCD1602_delay(2);
	
	for(i=0;i<length;i++)
	{
		PARAL8_Send(*cmd);
		cmd++;
		LCD1602_EN(1);
		LCD1602_delay(2);
		LCD1602_EN(0);
		LCD1602_delay(2);
	}
	
#endif

#ifdef LCD_INTERFACE_IIC
	unsigned char i;
	IIC_Start();
	IIC_Send(0x78);
	IIC_Send(0x00);
	for(i=0;i<length;i++)
	{
		IIC_Send(*cmd);
		cmd++;
	}
	IIC_Stop();
#endif
}

void LCD1602_Write_CGRAM(unsigned char index,const char *data)
{
	unsigned char i;
	char cmd = 0x40 + (index%8)*0x08;
	LCD1602_Write_Command(&cmd,1);
	
#ifdef LCD_INTERFACE_PARAL8

	PARAL8_Busy();
	LCD1602_RS(1);
	LCD1602_delay(2);
	LCD1602_RW(0);
	LCD1602_delay(2);
	
	for(i=0;i<8;i++)
	{
		PARAL8_Send(*data++);
		LCD1602_EN(1);
		LCD1602_delay(2);
		LCD1602_EN(0);
		LCD1602_delay(2);
	}
#endif

#ifdef LCD_INTERFACE_IIC

	IIC_Start();
	IIC_Send(0x78);
	IIC_Send(0x40);
	for(i=0;i<8;i++)
	{
		IIC_Send(*data);
		data++;
	}
	IIC_Stop();

#endif
}

void LCD1602_Display_Char(unsigned char x, unsigned char y,const char data)
{
	char cmd = 0x80 + (x%2)*0x40 + (y%16);
	LCD1602_Write_Command(&cmd,1);
	
#ifdef LCD_INTERFACE_PARAL8
	PARAL8_Busy();
	LCD1602_RS(1);
	LCD1602_delay(2);
	LCD1602_RW(0);
	LCD1602_delay(2);
	PARAL8_Send(data);
	LCD1602_EN(1);
	LCD1602_delay(2);
	LCD1602_EN(0);
	LCD1602_delay(2);
#endif

#ifdef LCD_INTERFACE_IIC

	IIC_Start();
	IIC_Send(0x78);
	IIC_Send(0xC0);
	IIC_Send(data);
	IIC_Stop();

#endif
}

void LCD1602_Display_String(unsigned char x, unsigned char y,const char *data,unsigned char length)
{
	unsigned char i;
	char cmd = 0x80 + (x%2)*0x40 + (y%16);
	LCD1602_Write_Command(&cmd,1);
	
#ifdef LCD_INTERFACE_PARAL8
	PARAL8_Busy();
	LCD1602_RS(1);
	LCD1602_delay(2);
	LCD1602_RW(0);
	LCD1602_delay(2);
	
	for(i=0;i<length;i++)
	{
		PARAL8_Send(*data++);
		LCD1602_EN(1);
		LCD1602_delay(2);
		LCD1602_EN(0);
		LCD1602_delay(2);
	}
#endif

#ifdef LCD_INTERFACE_IIC

	IIC_Start();
	IIC_Send(0x78);
	IIC_Send(0x40);
	for(i=0;i<length;i++)
	{
		IIC_Send(*data);
		data++;
	}
	IIC_Stop();
#endif
}

void LCD1602_Display_Num(unsigned char x, unsigned char y,long num)
{
	signed char i=0;
	char data[10];
	if(num == 0)
	{
		LCD1602_Display_Char(x,y,'0');
		return;
	}
	else if(num < 0)
	{
		num = -num;
		LCD1602_Display_Char(x,y++,'-');
	}
	for(i=9;i>=0;i--)
    {
    	data[i] = num%10 + '0';
		num /=10;
		if(num == 0)
			break;
    }
	LCD1602_Display_String(x,y,data+i,10-i);
}

void LCD1602_Init(void)
{
	char cmd[]={0x38,0x0C,0x01,0x06};
	LCD1602_Write_Command(cmd,sizeof(cmd));
}

void LCD1602_Display_Clear(void)
{
	char cmd[]={0x01};
	LCD1602_Write_Command(cmd,sizeof(cmd));
}
