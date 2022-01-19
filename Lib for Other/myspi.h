/*******************************************************************************
 * @file     myspi.h
 * @brief    simulation spi interface
 * @version  V1.1
 * @date     2021.8.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:

EXAMPLE CODE:
	MySPI hspi = {
    .SCK_Port  = GPIOE, .SCK_Bit  = GPIO_PIN_11,
    .CS_Port   = GPIOE, .CS_Bit   = GPIO_PIN_10,
    .MOSI_Port = GPIOE, .MOSI_Bit = GPIO_PIN_12,
    .MISO_Port = NULL,  .MISO_Bit = 0,
    .Speed = 2, .CPOL = 0, .CPHA = 0
};
    MySPI_IO_Init(&hspi);
    MySPI_WriteReg(&hspi,0x12,0x34);
	
*******************************************************************************/
#ifndef __MYSPI_H
#define __MYSPI_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx.h"

typedef struct{
	GPIO_TypeDef*   SCK_Port;
	uint32_t		SCK_Bit;
	GPIO_TypeDef*	CS_Port;
	uint32_t		CS_Bit;
	GPIO_TypeDef*	MOSI_Port;
	uint32_t		MOSI_Bit;
	GPIO_TypeDef*	MISO_Port;
	uint32_t		MISO_Bit;
	uint16_t	    Speed;
	uint8_t			CPOL;
	uint8_t			CPHA;
}MySPI;


void        MySPI_IO_Init               (MySPI *hspi);
void        MySPI_TransmitByte          (MySPI *hspi,uint8_t dat);
uint8_t     MySPI_ReceiveByte           (MySPI *hspi);
uint8_t     MySPI_TransmitReceiveByte   (MySPI *hspi,uint8_t dat);


void        MySPI_WriteReg		        (MySPI *hspi,uint8_t RegAddr,uint8_t dat);
uint8_t     MySPI_ReadReg               (MySPI *hspi,uint8_t RegAddr);
void        MySPI_WriteMem		        (MySPI *hspi,uint8_t RegAddr,uint8_t *dat,uint16_t len);
void        MySPI_ReadMem		        (MySPI *hspi,uint8_t RegAddr,uint8_t *dat,uint16_t len);
void        MySPI_Write		            (MySPI *hspi,uint8_t *dat,uint16_t len);
void        MySPI_Read			        (MySPI *hspi,uint8_t *dat,uint16_t len);
void        MySPI_ReadWrite             (MySPI *hspi,uint8_t *dat_in,uint8_t *dat_out,uint16_t len);


#ifdef __cplusplus
}
#endif
#endif
