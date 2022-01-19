/*******************************************************************************
 * @file     max262.h
 * @brief    
 * @version  V1.0
 * @date     2021.7.11
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __MAX262_H
#define __MAX262_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"


#define MAX262_A0(x)        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4 ,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define MAX262_A1(x)        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6 ,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define MAX262_A2(x)        HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4 ,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define MAX262_A3(x)        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0 ,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define MAX262_D0(x)        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2 ,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define MAX262_D1(x)        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2 ,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define MAX262_WR(x)        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0 ,x?GPIO_PIN_SET:GPIO_PIN_RESET)


void MAX262_Write(uint8_t channel,uint8_t mode,uint8_t FreqWord,uint8_t QWord);

void MAX262_Shutdown(void);


#ifdef __cplusplus
}
#endif
#endif
