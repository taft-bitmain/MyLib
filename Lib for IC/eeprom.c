/*******************************************************************************
 * @file     eeprom.c
 * @brief    drive the 24cxx EERROM memory chip
 * @version  V1.0
 * @date     2021.4.1
 * @author   RainingRabbits 1466586342@qq.com
******************************************************************************/
#include "eeprom.h"

/* EEPROM地址
	MSB							LSB	
	1	0	1	0	A2	A1	A0	R/W
*/

MYIIC eeprom = {.SCL_GPIO = GPIOB,.SCL_Pin = GPIO_PIN_7,
				.SDA_GPIO = GPIOB,.SDA_Pin = GPIO_PIN_5,
				.Speed = 5,.DevAddr = 0xA0};

					
void EEPROM_Init(void)
{
	MYIIC_Init(&eeprom);
}

void EEPROM_Write(int addr,char *data,unsigned char len)
{
	RTC_WriteMem(addr, data, len);
}

void EEPROM_Read(int addr,char *data,unsigned char len)
{
	RTC_ReadMem(addr, data, len);
}
