/*******************************************************************************
 * @file     eeprom.h
 * @brief    drive the 24Cxx EERROM memory chip
 * @version  V1.0
 * @date     2021.4.1
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1.存储结构
		24Cxx中的xx表示内存大小，例如:
			24C02有2K(bits)空间，8位地址，范围(0x00~0xFF）
			24C32有32K(bits)空间，16位地址，范围(0x0000~0x0FFF)
	2.页(Page)
		芯片内部的存储空间(EEPROM array)按照page组织，不同的芯片的page的大小不同
		24C02C one page has 16 Bytes.
		24C32 one page has 64 Bytes
	3.cache
		cache size is equal to page sizes, 每次写入数据，页内地址自动加一，页地址不变
		也就是说，越过页地址写入数据会在页内地址循环
	4.写入周期
		每一次写操作（芯片内部把cache的内容写入到EEPROM array中），不同芯片写入周期不同，大概2ms至5ms
	5.读取
		读取数据没有3，4限制，可以连续读，不需要延时
EXAMPLE CODE:
	char data[30];
	EEPROM_Init();
	EEPROM_Write(0,"hello world!",12);
	HAL_Delay(5);
	EEPROM_Read(0,data,12);
	HAL_UART_Transmit(&huart1,(uint8_t*)data,12,1000);
*******************************************************************************/

#ifndef __EEPROM_H
#define __EEPROM_H
#ifdef __cplusplus
extern "c" {
#endif


//接口定义
#include "myiic.h"

//#define EEPROM_ADDR_LEN8
#define EEPROM_ADDR_LEN16

#ifdef EEPROM_ADDR_LEN8
#define RTC_WriteMem(ADDR,data,len)		MYIIC_WriteMem(&eeprom,ADDR&0xFF,data,len)
#define RTC_ReadMem(ADDR,data,len)		MYIIC_ReadMem(&eeprom,ADDR&0xFF,data,len)
#endif

#ifdef EEPROM_ADDR_LEN16
#define RTC_WriteMem(ADDR,data,len)		MYIIC_WriteMem2(&eeprom,(ADDR>>8)&0xFF,ADDR&0xFF,data,len)
#define RTC_ReadMem(ADDR,data,len)		MYIIC_ReadMem2(&eeprom,(ADDR>>8)&0xFF,ADDR&0xFF,data,len)
#endif


//函数接口定义
void	EEPROM_Init		(void);
void	EEPROM_Write	(int addr,char *data,unsigned char len);
void	EEPROM_Read		(int addr,char *data,unsigned char len);


#ifdef __cplusplus
}
#endif
#endif
