/*******************************************************************************
 * @file     dht11.c
 * @brief    Drive the DHT11 module
 * @version  V1.0
 * @date     2021.3.12
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:

EXAMPLE CODE:
	DHT11_Data d;
	while (1)
	{
		if(DHT11_ReadData(&d))
			HAL_UART_Transmit(&huart1,(uint8_t*)d.string,d.strlen,1000);
		else
			HAL_UART_Transmit(&huart1,(uint8_t*)"READ FAIL",11,1000);
		HAL_Delay(1000);
	}
*******************************************************************************/

#ifndef __DHT11_H
#define __DHT11_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f4xx_hal.h"


#define DATA_GPIO_Port		GPIOA
#define DATA_Pin			    GPIO_PIN_4

#define DHT11_DATA_READ		   HAL_GPIO_ReadPin (DATA_GPIO_Port,DATA_Pin)
#define DHT11_DATA_WRITE(x)	 HAL_GPIO_WritePin(DATA_GPIO_Port,DATA_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET)


typedef struct{
	char humi;
	char temp_i;
	char temp_f;
	float temp;
	char string[16];
	char strlen;
}DHT11_Data;


void	DHT11_DelayUs  	(unsigned int x); 
void	DHT11_DelayMs	  (unsigned int x);
char	DHT11_ReadBits	(char *data);
char	DHT11_ReadData	(DHT11_Data *data);

#ifdef __cplusplus
}
#endif
#endif
