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
    1. Config the pin as GPIO Ouptput Open Drain and pull-up.
    2. Reading the DHT11 too frequently may occur errors, 500 ms is suitable.
EXAMPLE CODE:
	
	while (1)
	{
        static DHT11_Data d;
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

#include "stm32f1xx_hal.h"


#define DATA_GPIO_Port		    GPIOC
#define DATA_Pin			    GPIO_PIN_0

#define DHT11_DATA_READ         HAL_GPIO_ReadPin (DATA_GPIO_Port,DATA_Pin)
#define DHT11_DATA_WRITE(x)     HAL_GPIO_WritePin(DATA_GPIO_Port,DATA_Pin,x?GPIO_PIN_SET:GPIO_PIN_RESET)


typedef struct{
	char humi;
	char temp_i;
	char temp_f;
	float temp;
	char string[16];
	char strlen;
}DHT11_Data;


void	DHT11_DelayUs  	(unsigned int x); 
void	DHT11_DelayMs	(unsigned int x);
char	DHT11_ReadBits	(char *data);
char	DHT11_ReadData	(DHT11_Data *data);

#ifdef __cplusplus
}
#endif
#endif
