/*******************************************************************************
 * @file     myspi.c
 * @brief    simulation spi interface
 * @version  V1.0
 * @date     2021.3.21
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "myspi.h"


void MYSPI_WritePinSCK(MYSPI *hspi,uint8_t state)
{
#ifdef MYSPI_USE_STM32HAL
	HAL_GPIO_WritePin(hspi->GPIO_SCK,hspi->Pin_SCK,state?GPIO_PIN_SET:GPIO_PIN_RESET);
#endif
}
void MYSPI_WritePinMOSI(MYSPI *hspi,uint8_t state)
{
#ifdef MYSPI_USE_STM32HAL
	if(hspi->GPIO_MOSI != NULL)
		HAL_GPIO_WritePin(hspi->GPIO_MOSI,hspi->Pin_MOSI,state?GPIO_PIN_SET:GPIO_PIN_RESET);
#endif
}
void MYSPI_WritePinCS(MYSPI *hspi,uint8_t state)
{
#ifdef MYSPI_USE_STM32HAL
	if(hspi->GPIO_CS != NULL)
		HAL_GPIO_WritePin(hspi->GPIO_CS,hspi->Pin_CS,state?GPIO_PIN_SET:GPIO_PIN_RESET);
#endif
}
uint8_t MYSPI_ReadPinMISO(MYSPI *hspi)
{
#ifdef MYSPI_USE_STM32HAL
	if(hspi->GPIO_MISO != NULL)
		return HAL_GPIO_ReadPin(hspi->GPIO_MISO,hspi->Pin_MISO);
	return 0;
#endif
}

void MYSPI_Delay(uint16_t x)
{
	uint16_t t = x * 8;
	while(t--);
}

void MYSPI_Init(MYSPI *hspi)
{
//GPIO
#ifdef MYSPI_USE_STM32HAL
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = hspi->Pin_SCK;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(hspi->GPIO_SCK, &GPIO_InitStruct);
	if(hspi->GPIO_MOSI != NULL)
	{
		GPIO_InitStruct.Pin = hspi->Pin_MOSI;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(hspi->GPIO_MOSI, &GPIO_InitStruct);
	}
	if(hspi->GPIO_MISO != NULL)
	{
		GPIO_InitStruct.Pin = hspi->Pin_MISO;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(hspi->GPIO_MISO, &GPIO_InitStruct);
	}
	if(hspi->GPIO_CS != NULL)
	{
		GPIO_InitStruct.Pin = hspi->Pin_CS;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(hspi->GPIO_CS, &GPIO_InitStruct);
	}
#endif

	MYSPI_WritePinSCK(hspi,hspi->CPOL);
	MYSPI_WritePinCS(hspi,1);
}

void MYSPI_WriteReg(MYSPI *hspi,uint8_t RegAddr,uint8_t data)
{
	uint8_t i;
	MYSPI_WritePinCS(hspi,0);
	MYSPI_Delay(hspi->Speed);
	for(i = 0; i < 8; i++)
	{
		if(!(hspi->CPHA))
			MYSPI_WritePinMOSI(hspi,RegAddr&(0x80>>i));
		MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
		MYSPI_Delay(hspi->Speed);
		if(hspi->CPHA)
			MYSPI_WritePinMOSI(hspi,RegAddr&(0x80>>i));
		MYSPI_WritePinSCK(hspi,hspi->CPOL);
		MYSPI_Delay(hspi->Speed);
	}
	for(i = 0; i < 8; i++)
	{
		if(!(hspi->CPHA))
			MYSPI_WritePinMOSI(hspi,data&(0x80>>i));
		MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
		MYSPI_Delay(hspi->Speed);
		if(hspi->CPHA)
			MYSPI_WritePinMOSI(hspi,data&(0x80>>i));
		MYSPI_WritePinSCK(hspi,hspi->CPOL);
		MYSPI_Delay(hspi->Speed);
	}
	MYSPI_WritePinCS(hspi,1);
	MYSPI_Delay(hspi->Speed);
}
uint8_t MYSPI_ReadReg(MYSPI *hspi,uint8_t RegAddr)
{
	uint8_t i,data = 0x00;
	MYSPI_WritePinCS(hspi,0);
	MYSPI_Delay(hspi->Speed);
	for(i = 0; i < 8; i++)
	{
		if(!(hspi->CPHA))
			MYSPI_WritePinMOSI(hspi,RegAddr&(0x80>>i));
		MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
		MYSPI_Delay(hspi->Speed);
		if(hspi->CPHA)
			MYSPI_WritePinMOSI(hspi,RegAddr&(0x80>>i));
		MYSPI_WritePinSCK(hspi,hspi->CPOL);
		MYSPI_Delay(hspi->Speed);
	}
	for(i = 0; i < 8; i++)
	{
		MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
		MYSPI_Delay(hspi->Speed);
		if(!(hspi->CPHA))
			data |= (MYSPI_ReadPinMISO(hspi)?(0x80>>i):0);
		MYSPI_WritePinSCK(hspi,hspi->CPOL);
		MYSPI_Delay(hspi->Speed);
		if(hspi->CPHA)
			data |= (MYSPI_ReadPinMISO(hspi)?(0x80>>i):0);
	}
	MYSPI_WritePinCS(hspi,1);
	MYSPI_Delay(hspi->Speed);
	return data;
}
void MYSPI_WriteMem(MYSPI *hspi,uint8_t RegAddr,uint8_t *data,uint16_t len)
{
	uint8_t i;
	MYSPI_WritePinCS(hspi,0);
	MYSPI_Delay(hspi->Speed);
	for(i = 0; i < 8; i++)
	{
		if(!(hspi->CPHA))
			MYSPI_WritePinMOSI(hspi,RegAddr&(0x80>>i));
		MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
		MYSPI_Delay(hspi->Speed);
		if(hspi->CPHA)
			MYSPI_WritePinMOSI(hspi,RegAddr&(0x80>>i));
		MYSPI_WritePinSCK(hspi,hspi->CPOL);
		MYSPI_Delay(hspi->Speed);
	}
	while(len--)
	{
		for(i = 0; i < 8; i++)
		{
			if(!(hspi->CPHA))
				MYSPI_WritePinMOSI(hspi,(*data)&(0x80>>i));
			MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
			MYSPI_Delay(hspi->Speed);
			if(hspi->CPHA)
				MYSPI_WritePinMOSI(hspi,(*data)&(0x80>>i));
			MYSPI_WritePinSCK(hspi,hspi->CPOL);
			MYSPI_Delay(hspi->Speed);
		}
		data++;
	}
	MYSPI_WritePinCS(hspi,1);
	MYSPI_Delay(hspi->Speed);
}
void MYSPI_ReadMem(MYSPI *hspi,uint8_t RegAddr,uint8_t *data,uint16_t len)
{
	uint8_t i;
	MYSPI_WritePinCS(hspi,0);
	MYSPI_Delay(hspi->Speed);
	for(i = 0; i < 8; i++)
	{
		if(!(hspi->CPHA))
			MYSPI_WritePinMOSI(hspi,RegAddr&(0x80>>i));
		MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
		MYSPI_Delay(hspi->Speed);
		if(hspi->CPHA)
			MYSPI_WritePinMOSI(hspi,RegAddr&(0x80>>i));
		MYSPI_WritePinSCK(hspi,hspi->CPOL);
		MYSPI_Delay(hspi->Speed);
	}
	while(len--)
	{
		for(i = 0; i < 8; i++)
		{
			MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
			MYSPI_Delay(hspi->Speed);
			if(!(hspi->CPHA))
				(*data) |= (MYSPI_ReadPinMISO(hspi)?(0x80>>i):0);
			MYSPI_WritePinSCK(hspi,hspi->CPOL);
			MYSPI_Delay(hspi->Speed);
			if(hspi->CPHA)
				(*data) |= (MYSPI_ReadPinMISO(hspi)?(0x80>>i):0);
		}
		data++;
	}
	MYSPI_WritePinCS(hspi,1);
	MYSPI_Delay(hspi->Speed);
}
void MYSPI_Write(MYSPI *hspi,uint8_t *data,uint16_t len)
{
	uint8_t i;
	MYSPI_WritePinCS(hspi,0);
	MYSPI_Delay(hspi->Speed);
	while(len--)
	{
		for(i = 0; i < 8; i++)
		{
			if(!(hspi->CPHA))
				MYSPI_WritePinMOSI(hspi,(*data)&(0x80>>i));
			MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
			MYSPI_Delay(hspi->Speed);
			if(hspi->CPHA)
				MYSPI_WritePinMOSI(hspi,(*data)&(0x80>>i));
			MYSPI_WritePinSCK(hspi,hspi->CPOL);
			MYSPI_Delay(hspi->Speed);
		}
		data++;
	}
	MYSPI_WritePinCS(hspi,1);
	MYSPI_Delay(hspi->Speed);
}
void MYSPI_Read(MYSPI *hspi,uint8_t *data,uint16_t len)
{
	uint8_t i;
	MYSPI_WritePinCS(hspi,0);
	MYSPI_Delay(hspi->Speed);
	while(len--)
	{
		for(i = 0; i < 8; i++)
		{
			MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
			MYSPI_Delay(hspi->Speed);
			if(!(hspi->CPHA))
				(*data) |= (MYSPI_ReadPinMISO(hspi)?(0x80>>i):0);
			MYSPI_WritePinSCK(hspi,hspi->CPOL);
			MYSPI_Delay(hspi->Speed);
			if(hspi->CPHA)
				(*data) |= (MYSPI_ReadPinMISO(hspi)?(0x80>>i):0);
		}
		data++;
	}
	MYSPI_WritePinCS(hspi,1);
	MYSPI_Delay(hspi->Speed);
}
void MYSPI_ReadWrite(MYSPI *hspi,uint8_t *data_out,uint8_t *data_in,uint16_t len)
{
	uint8_t i;
	MYSPI_WritePinCS(hspi,0);
	MYSPI_Delay(hspi->Speed);
	while(len--)
	{
		for(i = 0; i < 8; i++)
		{
			if(!(hspi->CPHA))
				MYSPI_WritePinMOSI(hspi,(*data_out)&(0x80>>i));
			MYSPI_WritePinSCK(hspi,!(hspi->CPOL));
			MYSPI_Delay(hspi->Speed);
			if(!(hspi->CPHA))
				(*data_in) |= (MYSPI_ReadPinMISO(hspi)?(0x80>>i):0);
			
			if(hspi->CPHA)
				MYSPI_WritePinMOSI(hspi,(*data_out)&(0x80>>i));
			MYSPI_WritePinSCK(hspi,hspi->CPOL);
			MYSPI_Delay(hspi->Speed);
			if(hspi->CPHA)
				(*data_in) |= (MYSPI_ReadPinMISO(hspi)?(0x80>>i):0);
		}
		data_out++;
		data_in++;
	}
	MYSPI_WritePinCS(hspi,1);
	MYSPI_Delay(hspi->Speed);
}
