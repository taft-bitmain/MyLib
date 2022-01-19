/*******************************************************************************
 * @file     myiic.h
 * @brief    simulation iic interface base on HAL library
 * @version  V1.1
 * @date     2021.7.18
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
  1.使用STM32 HAL库时，需要自行使能相应的GPIO端口时钟，GPIO不用再另行配置
  2.Speed参数说明
		在16MHz的条件下:
			Speed=1，SCL约为 82Khz;
			Speed=2，SCL约为 55Khz;
			Speed=10，SCL约为 15Khz
		该参数指示了I2C延时的时间，值越大，速度越慢。
		
EXAMPLE CODE:
	MYIIC myiic1 = {.GPIO_SCL = GPIOB,.Pin_SCL = GPIO_PIN_0,
					.GPIO_SDA = GPIOB,.Pin_SDA = GPIO_PIN_1,
					.Speed = 2,.DevAddr = 0x00};
	MYIIC_Init(&myiic1);
	myiic1.DevAddr = MYIIC_Detect(&myiic1);
	MYIIC_WriteReg(&myiic1,0x34,0x56);
*******************************************************************************/
#ifndef __MYIIC_H
#define __MYIIC_H
#ifdef __cplusplus
extern "c" {
#endif

#define MYIIC_USE_STM32HAL

#ifdef MYIIC_USE_STM32HAL
#include "stm32f1xx_hal.h"
#include "stdint.h"
#endif

typedef struct{
#ifdef MYIIC_USE_STM32HAL
  GPIO_TypeDef* GPIO_SCL;
  uint16_t      Pin_SCL;
  GPIO_TypeDef* GPIO_SDA;
  uint16_t      Pin_SDA;
#endif
  uint16_t      Speed;
  uint8_t       DevAddr;
}MYIIC;

void    MYIIC_WritePinSCL	(MYIIC *hiic,uint8_t state);
void    MYIIC_WritePinSDA	(MYIIC *hiic,uint8_t state);
uint8_t MYIIC_ReadPinSDA	(MYIIC *hiic);
void    MYIIC_Init          (MYIIC *hiic);


void 	MYIIC_Delay     (uint16_t x);
void	MYIIC_Start     (MYIIC *hiic);
uint8_t	MYIIC_Transmit  (MYIIC *hiic,uint8_t dat);
uint8_t	MYIIC_Receive   (MYIIC *hiic,uint8_t ack);
void	MYIIC_End       (MYIIC *hiic);


uint8_t	MYIIC_WriteReg  (MYIIC *hiic,uint8_t RegAddr,uint8_t dat);
uint8_t	MYIIC_ReadReg   (MYIIC *hiic,uint8_t RegAddr);
uint8_t	MYIIC_WriteReg2 (MYIIC *hiic,uint8_t RegAddr,uint16_t dat);
uint16_t MYIIC_ReadReg2 (MYIIC *hiic,uint8_t RegAddr);

uint8_t	MYIIC_WriteMem  (MYIIC *hiic,uint8_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	MYIIC_ReadMem   (MYIIC *hiic,uint8_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	MYIIC_WriteMem2 (MYIIC *hiic,uint16_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	MYIIC_ReadMem2  (MYIIC *hiic,uint16_t MemAddr,uint8_t *dat,uint16_t len);

uint8_t	MYIIC_WriteByte (MYIIC *hiic,uint8_t dat);
uint8_t	MYIIC_ReadByte  (MYIIC *hiic);
uint8_t	MYIIC_WriteBytes(MYIIC *hiic,uint8_t *dat,uint16_t len);
uint8_t	MYIIC_ReadBytes (MYIIC *hiic,uint8_t *dat,uint16_t len);

uint8_t	MYIIC_Detect    (MYIIC *hiic);

#ifdef __cplusplus
}
#endif
#endif
