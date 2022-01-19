/*******************************************************************************
 * @file     lcd1602.h
 * @brief    drive the lcd1602
 * @version  V1.1
 * @date     2022.1.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1.	
EXAMPLE CODE:
	LCD1602_Init();
	LCD1602_Display_Char(0,0,'H');
	LCD1602_Display_Char(0,1,'e');
	LCD1602_Display_Char(0,2,'l');
	LCD1602_Display_Char(0,3,'l');
	LCD1602_Display_Char(0,4,'o');
	LCD1602_Display_String(0,6,"LCD1602",7);
	LCD1602_Display_Num(1,0,123456);
	LCD1602_Write_CGRAM(0,MyChar0);
	LCD1602_Write_CGRAM(1,MyChar1);
	LCD1602_Write_CGRAM(2,MyChar2);
	LCD1602_Display_Char(1,7,0);
	LCD1602_Display_Char(1,8,1);
	LCD1602_Display_Char(1,9,2);
*******************************************************************************/
#ifndef __LCD1602_H
#define __LCD1602_H

#include "stm32f1xx_hal.h"

#define LCD_INTERFACE_PARAL8
//#define LCD_INTERFACE_IIC


#ifdef LCD_INTERFACE_PARAL8
#define LCD1602_RS(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_RW(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_EN(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_D0(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_D1(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_D2(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_D3(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_D4(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_D5(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_D6(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LCD1602_D7(x) HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,x?GPIO_PIN_SET:GPIO_PIN_RESET) //Output Open Drain
#define LCD1602_Read_D7 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7)
#endif

#ifdef LCD_INTERFACE_IIC
#define LCD1602_SDA(x) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,x?GPIO_PIN_SET:GPIO_PIN_RESET) //D6
#define LCD1602_SCL(x) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,x?GPIO_PIN_SET:GPIO_PIN_RESET) //D7

#endif

//
extern const char MyChar0[8];
extern const char MyChar1[8];
extern const char MyChar2[8];


void LCD1602_delay			(unsigned int x); //about 10us
void LCD1602_Init			(void);
void LCD1602_Write_Command	(const char *cmd,unsigned char length);


/*
 * LCD1602支持8个自定义字符
 * index指示了自定义字符存放在CGRAM中的位置(0,7)
 * data指示了字符的像素点阵，具体映射关系详见 .c 文件
 * 通过LCD1602_Display_Char(x,y,index)可以显示相应的字符
 */
void LCD1602_Write_CGRAM	(unsigned char index,const char *data);


// x->(0,1), y->(0~15)
void LCD1602_Display_Char	(unsigned char x, unsigned char y,const char data);
void LCD1602_Display_String	(unsigned char x, unsigned char y,const char *data,unsigned char length);
void LCD1602_Display_Num	(unsigned char x, unsigned char y,long num);
void LCD1602_Display_Clear	(void);



#endif
