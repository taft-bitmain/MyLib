/*******************************************************************************
 * @file     ws2812b.h
 * @brief
 * @version  V1.0
 * @date     2022.2.21
 * @author   RainingCats 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
    1. This is for 48MHz system clock
	
EXAMPLE:

    HAL_Delay(1000);
    WS2812B_SetColor( 0x800000 );
    HAL_Delay(1000);
    WS2812B_SetColor( 0x008000 );
    HAL_Delay(1000);
    WS2812B_SetColor( 0x000080 );

*******************************************************************************/

#ifndef __WS2812B_H
#define __WS2812B_H

#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"
#include "stm32f1xx.h"

/***************** Basic Interface *************************/

#define     WS2812B_PinDIN(bit) ( bit ? (GPIOB->BSRR = GPIO_PIN_8):(GPIOB->BRR = GPIO_PIN_8))

/****************** Main Functions *************************/

void        WS2812B_SetColor    ( uint32_t GRB ); // only the last 24 bits is valid
void        WS2812B_SetColors   ( uint32_t * GRBs, uint16_t len );


#ifdef __cplusplus
}
#endif
#endif
