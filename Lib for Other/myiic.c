/*******************************************************************************
 * @file     myiic.c
 * @brief    simulation iic interface base on HAL library
 * @version  V1.1
 * @date     2021.7.18
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "myiic.h"

void MYIIC_WritePinSCL(MYIIC *hiic,uint8_t state)
{
#ifdef MYIIC_USE_STM32HAL
	HAL_GPIO_WritePin(hiic->GPIO_SCL,hiic->Pin_SCL,state?GPIO_PIN_SET:GPIO_PIN_RESET);
#endif
}
void MYIIC_WritePinSDA(MYIIC *hiic,uint8_t state)
{
#ifdef MYIIC_USE_STM32HAL
	HAL_GPIO_WritePin(hiic->GPIO_SDA,hiic->Pin_SDA,state?GPIO_PIN_SET:GPIO_PIN_RESET);
#endif
}
uint8_t MYIIC_ReadPinSDA(MYIIC *hiic)
{
#ifdef MYIIC_USE_STM32HAL
	return HAL_GPIO_ReadPin(hiic->GPIO_SDA,hiic->Pin_SDA);
#endif
}
void MYIIC_Init(MYIIC *hiic)
{
#ifdef MYIIC_USE_STM32HAL
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = hiic->Pin_SCL;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(hiic->GPIO_SCL, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = hiic->Pin_SDA;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(hiic->GPIO_SDA, &GPIO_InitStruct);
#endif
	MYIIC_WritePinSCL(hiic,0);
	MYIIC_WritePinSDA(hiic,0);
}


void MYIIC_Delay(uint16_t x)
{
	uint16_t t = x * 8;
	while(t--);
}

void MYIIC_Start(MYIIC *hiic)
{
	MYIIC_WritePinSDA(hiic,1);
	MYIIC_WritePinSCL(hiic,0);
	MYIIC_Delay(hiic->Speed);
	MYIIC_WritePinSCL(hiic,1);
	MYIIC_Delay(hiic->Speed);
	MYIIC_WritePinSDA(hiic,0);
	MYIIC_Delay(hiic->Speed);
	MYIIC_WritePinSCL(hiic,0);
	MYIIC_Delay(hiic->Speed);
}
uint8_t MYIIC_Transmit(MYIIC *hiic,uint8_t dat)
{
	uint8_t index,flag = 0;
	for(index = 0;index < 8;index++)
    {
		MYIIC_WritePinSDA(hiic,dat & (0x80>>index));
		MYIIC_Delay(hiic->Speed);
		MYIIC_WritePinSCL(hiic,1);
		MYIIC_Delay(hiic->Speed*2);
		MYIIC_WritePinSCL(hiic,0);
		MYIIC_Delay(hiic->Speed);
    }
	MYIIC_WritePinSDA(hiic,1);
	MYIIC_Delay(hiic->Speed);
	MYIIC_WritePinSCL(hiic,1);
	MYIIC_Delay(hiic->Speed*2);
	if(MYIIC_ReadPinSDA(hiic) == 0)
		flag = 1;
	MYIIC_WritePinSCL(hiic,0);
	MYIIC_Delay(hiic->Speed);
	return flag;
}
uint8_t MYIIC_Receive(MYIIC *hiic,uint8_t ack)
{
	uint8_t index,dat = 0x00;
	MYIIC_WritePinSDA(hiic,1);
	for(index = 0;index < 8;index++)
    {
		MYIIC_Delay(hiic->Speed);
		MYIIC_WritePinSCL(hiic,1);
		MYIIC_Delay(hiic->Speed*2);
		if(MYIIC_ReadPinSDA(hiic))
			dat |= (0x80>>index);
		MYIIC_WritePinSCL(hiic,0);
		MYIIC_Delay(hiic->Speed);
    }
	MYIIC_WritePinSDA(hiic,!ack);
	MYIIC_Delay(hiic->Speed);
	MYIIC_WritePinSCL(hiic,1);
	MYIIC_Delay(hiic->Speed*2);
	MYIIC_WritePinSCL(hiic,0);
	MYIIC_Delay(hiic->Speed);
	return dat;
}
void MYIIC_End(MYIIC *hiic)
{
	MYIIC_WritePinSDA(hiic,0);
	MYIIC_Delay(hiic->Speed);
	MYIIC_WritePinSCL(hiic,1);
	MYIIC_Delay(hiic->Speed);
	MYIIC_WritePinSDA(hiic,1);
	MYIIC_Delay(hiic->Speed);
	MYIIC_WritePinSCL(hiic,0);
	MYIIC_Delay(hiic->Speed);
}




uint8_t MYIIC_WriteReg(MYIIC *hiic,uint8_t RegAddr,uint8_t dat)
{
	uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,RegAddr);
	flag &= MYIIC_Transmit(hiic,dat);
	MYIIC_End(hiic);
	return flag;
}

uint8_t MYIIC_ReadReg(MYIIC *hiic,uint8_t RegAddr)
{
	uint8_t  flag = 1,dat;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,RegAddr);
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	dat = MYIIC_Receive(hiic,0);
	MYIIC_End(hiic);
	return dat;
}

uint8_t	MYIIC_WriteReg2 (MYIIC *hiic,uint8_t RegAddr,uint16_t dat)
{
    uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,RegAddr);
	flag &= MYIIC_Transmit(hiic,(uint8_t)(dat>>8));
    flag &= MYIIC_Transmit(hiic,(uint8_t)(dat));
	MYIIC_End(hiic);
	return flag;
}

uint16_t MYIIC_ReadReg2 (MYIIC *hiic,uint8_t RegAddr)
{
    uint8_t flag = 1;
    uint16_t dat;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,RegAddr);
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	dat = MYIIC_Receive(hiic,1);
    dat <<= 8;
    dat |= MYIIC_Receive(hiic,0);
	MYIIC_End(hiic);
	return dat;
}




uint8_t MYIIC_WriteMem(MYIIC *hiic,uint8_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,MemAddr);
	while(len--)
		flag &= MYIIC_Transmit(hiic,*dat++);
	MYIIC_End(hiic);
	return flag;
}

uint8_t MYIIC_ReadMem(MYIIC *hiic,uint8_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,MemAddr);
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	while(--len>0)
		*dat++ = MYIIC_Receive(hiic,1);
	*dat = MYIIC_Receive(hiic,0);
	MYIIC_End(hiic);
	return flag;
}

uint8_t MYIIC_WriteMem2(MYIIC *hiic,uint16_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,(uint8_t)(MemAddr>>8));
	flag &= MYIIC_Transmit(hiic,(uint8_t) MemAddr);
	while(len--)
		flag &= MYIIC_Transmit(hiic,*dat++);
	MYIIC_End(hiic);
	return flag;
}

uint8_t MYIIC_ReadMem2(MYIIC *hiic,uint16_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,(uint8_t)(MemAddr>>8));
	flag &= MYIIC_Transmit(hiic,(uint8_t) MemAddr);
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	while(--len>0)
		*dat++ = MYIIC_Receive(hiic,1);
	*dat = MYIIC_Receive(hiic,0);
	MYIIC_End(hiic);
	return flag;
}




uint8_t	MYIIC_WriteByte(MYIIC *hiic,uint8_t dat)
{
  uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MYIIC_Transmit(hiic,dat);
	MYIIC_End(hiic);
	return flag;
}

uint8_t	MYIIC_ReadByte(MYIIC *hiic)
{
  uint8_t dat;
	MYIIC_Start(hiic);
	MYIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	dat = MYIIC_Receive(hiic,0);
	MYIIC_End(hiic);
	return dat;
}

uint8_t MYIIC_WriteBytes(MYIIC *hiic,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	while(len--)
		flag &= MYIIC_Transmit(hiic,*dat++);
	MYIIC_End(hiic);
	return flag;
}

uint8_t MYIIC_ReadBytes(MYIIC *hiic,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MYIIC_Start(hiic);
	flag &= MYIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	while(--len > 0)
		*dat++ = MYIIC_Receive(hiic,1);
	*dat = MYIIC_Receive(hiic,0);
	MYIIC_End(hiic);
	return flag;
}




uint8_t MYIIC_Detect(MYIIC *hiic)
{
	 uint8_t DevAddr = 0x00;
	for(DevAddr = 0x00;DevAddr != 0xFE;DevAddr += 0x02)
	{	
		MYIIC_Start(hiic);
		if(MYIIC_Transmit(hiic,DevAddr))
			break;
		MYIIC_End(hiic);
	}
	return DevAddr;
}
