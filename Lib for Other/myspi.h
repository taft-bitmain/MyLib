/*******************************************************************************
 * @file     myspi.h
 * @brief    simulation spi interface
 * @version  V1.0
 * @date     2021.3.21
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1.使用STM32 HAL库时，需要自行使能相应的GPIO端口时钟，GPIO不用再另行配置
	2.Speed参数说明
		在16MHz的条件下:
			Speed=1，SCL约为 100Khz;
			Speed=2，SCL约为 78Khz;
			Speed=10，SCL约为 27Khz
		该参数指示了I2C延时的时间，值越大，速度越慢。
EXAMPLE CODE:
	MYSPI myspi1 = {    .GPIO_SCK  = GPIOB, .Pin_SCK  = GPIO_PIN_5,
                        .GPIO_MOSI = GPIOB, .Pin_MOSI = GPIO_PIN_6,
                        .GPIO_MISO = GPIOB, .Pin_MISO = GPIO_PIN_7,
                        .GPIO_CS   = GPIOB, .Pin_CS   = GPIO_PIN_8,
                        .Speed = 5, .CPOL = 0, .CPHA = 0};
	MYSPI_Init(&myspi1);
	MYSPI_WriteReg(0x55,0xAA);
	
*******************************************************************************/
#ifndef __MYSPI_H
#define __MYSPI_H
#ifdef __cplusplus
extern "c" {
#endif

//基于STM32HAL库的模拟spi协议
#define MYSPI_USE_STM32HAL

#ifdef MYSPI_USE_STM32HAL
#include "stm32f1xx_hal.h"
#include "stdint.h"
#endif

typedef struct{
//IO定义
#ifdef MYSPI_USE_STM32HAL
	GPIO_TypeDef	*GPIO_SCK;
	uint16_t		Pin_SCK;
	GPIO_TypeDef	*GPIO_MOSI;
	uint16_t		Pin_MOSI;
	GPIO_TypeDef	*GPIO_MISO;
	uint16_t		Pin_MISO;
	GPIO_TypeDef	*GPIO_CS;
	uint16_t		Pin_CS;
#endif
//速度，时钟极性和相位定义
	uint16_t	    Speed;
	uint8_t			CPOL;
	uint8_t			CPHA;
}MYSPI;

//以下函数需要根据不同平台适配
void    MYSPI_WritePinSCK   (MYSPI *hspi,uint8_t state);
void    MYSPI_WritePinMOSI  (MYSPI *hspi,uint8_t state);
void    MYSPI_WritePinCS    (MYSPI *hspi,uint8_t state);
uint8_t MYSPI_ReadPinMISO   (MYSPI *hspi);

void    MYSPI_Delay         (uint16_t x);
void    MYSPI_Init			(MYSPI *hspi);

//以下函数与平台无关，无需修改

void    MYSPI_WriteReg		(MYSPI *hspi,uint8_t RegAddr,uint8_t data);
uint8_t MYSPI_ReadReg       (MYSPI *hspi,uint8_t RegAddr);
void    MYSPI_WriteMem		(MYSPI *hspi,uint8_t RegAddr,uint8_t *data,uint16_t len);
void    MYSPI_ReadMem		(MYSPI *hspi,uint8_t RegAddr,uint8_t *data,uint16_t len);
void    MYSPI_Write		    (MYSPI *hspi,uint8_t *data,uint16_t len);
void    MYSPI_Read			(MYSPI *hspi,uint8_t *data,uint16_t len);
void    MYSPI_ReadWrite     (MYSPI *hspi,uint8_t *data_out,uint8_t *data_in,uint16_t len);


#ifdef __cplusplus
}
#endif
#endif
