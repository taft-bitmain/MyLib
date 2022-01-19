/*******************************************************************************
 * @file     x9c103.h
 * @brief    
 * @version  V1.0
 * @date     2021.5.28
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __X9C103_H
#define __X9C103_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "main.h"

#define IO_Name2Port(x)			    x##_GPIO_Port
#define IO_Name2Pin(x)			    x##_Pin

#define X9C103_SetPin(x,y) HAL_GPIO_WritePin(IO_Name2Port(x),IO_Name2Pin(x),y?GPIO_PIN_SET:GPIO_PIN_RESET)

void X9C103_Store(void);
void X9C103_Delay(uint16_t n);
void X9C103_Increase(uint16_t n);
void X9C103_Decrease(uint16_t n);
void X9C103_SetZero(void);

#ifdef __cplusplus
}
#endif
#endif
