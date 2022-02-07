/*******************************************************************************
 * @file     dht11.c
 * @brief    Drive the DHT11 module
 * @version  V1.0
 * @date     2021.3.12
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/
#include "dht11.h"
#include "stdio.h"

void DHT11_DelayUs(unsigned int x)
{
	uint32_t t = x * 48;
	while(t--);
}

void DHT11_DelayMs(unsigned int x)
{
	HAL_Delay(x*1000);
}

void DHT11_DATA_OUT(void)
{
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.Pin = DATA_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//	HAL_GPIO_Init(DATA_GPIO_Port, &GPIO_InitStruct);
}

void DHT11_DATA_IN(void)
{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	GPIO_InitStruct.Pin = DATA_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//	HAL_GPIO_Init(DATA_GPIO_Port, &GPIO_InitStruct);
}

char DHT11_ReadBits(char *data)
{
	unsigned char index=0;
	unsigned int threshold=0,i=0;
	
	//clear data memory
	for(i=0;i<5;i++)
		data[i] = 0;
	
	//Start Signal
	DHT11_DATA_OUT();
	DHT11_DATA_WRITE(0);
	HAL_Delay(22);
	DHT11_DATA_WRITE(1);
	
	//Wait Echo
	DHT11_DATA_IN();
	for(i=0;DHT11_DATA_READ == 1;i++)
	{
		DHT11_DelayUs(1);
		if(i == 0xffff)	//return if wait for a long time
			return 0;
	}
	for(i=0;DHT11_DATA_READ == 0;i++)
	{
		DHT11_DelayUs(1);
		if(i == 0xffff)
			return 0;
	}
	for(i=0;DHT11_DATA_READ == 1;i++)
	{
		DHT11_DelayUs(1);
		if(i == 0xffff)
			return 0;
	}
	
	//Recieve 40 bits
	for(index=0;index<40;index++)
    {
		for(i=0;DHT11_DATA_READ == 0;i++)
		{
			DHT11_DelayUs(1);
			if(i == 0xffff)
				return 0;
		}
		threshold = i;
		for(i=0;DHT11_DATA_READ == 1;i++)
		{
			DHT11_DelayUs(1);
			if(i == 0xffff)
				return 0;
		}
		data[index/8] |= ((i>threshold? 0x80 : 0)>>(index%8));
	}
	
	//Wait End
	for(i=0;DHT11_DATA_READ == 0;i++)
	{
		DHT11_DelayUs(1);
		if(i == 0xffff)
			return 0;
	}
	
	return 1;
}

char DHT11_ReadData(DHT11_Data *data)
{
	char tmp[5];
	if(DHT11_ReadBits(tmp) == 0)
		return 0;
	if(tmp[1] != 0x00)
		return 0;
	//sum check
	if((tmp[0]+tmp[2]+tmp[3]) != tmp[4])
		return 0;
	
	data->humi = tmp[0];
	data->temp_i = tmp[2] * ((tmp[3]&0x80) ? (-1) : (1));
	data->temp_f = tmp[3] & 0x7F;
	data->temp = data->temp_i + data->temp_f*0.1;
	
    sprintf(data->string,"HUMI:%d%% TEMP:%dC",data->humi,(int)data->temp);
  
	return 1;
}
