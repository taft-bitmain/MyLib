/*******************************************************************************
 * @file     hmc472.h
 * @brief
 * @version  V1.0
 * @date     2021.7.11
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1. 需要先在CubeMX中配置IO
EXAMPLE CODE:
	HMC472(32);
*******************************************************************************/
#ifndef __xxxx_H
#define __xxxx_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"


#define HMC472_P5(x)        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define HMC472_P4(x)        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define HMC472_P3(x)        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define HMC472_P2(x)        HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8 ,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define HMC472_P1(x)        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,x?GPIO_PIN_SET:GPIO_PIN_RESET)
#define HMC472_P0(x)        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,x?GPIO_PIN_SET:GPIO_PIN_RESET)

// para 0~63 -> actually 0~31.5
void HMC472(uint8_t para);

#ifdef __cplusplus
}
#endif
#endif
