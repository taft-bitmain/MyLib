/*******************************************************************************
 * @file     servo.h
 * @brief
 * @version  V1.0
 * @date     2022.2.24
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
    1.  period = 20ms
        pulse width = 0.5ms ->   0°
        pulse width = 1.5ms ->  90°
        pulse width = 2.5ms -> 180°

EXAMPLE:
    Servo_SetPhase( 0 );
    HAL_Delay( 1000 );
    Servo_SetPhase( 90 );
    HAL_Delay( 1000 );
    Servo_SetPhase( 180 );
    HAL_Delay( 1000 );
    Servo_SetPhase( 90 );
    HAL_Delay( 1000 );
	
*******************************************************************************/
#ifndef __SERVO_H
#define __SERVO_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"

/***************** Basic Interface *************************/

#include "stm32f1xx.h"
#include "base.h"

#define Servo_Pin(state)   WritePin( GPIOB, GPIO_PIN_6, state )
void    Servo_Delay10us     ( uint32_t time );

/****************** Main Functions *************************/

void    Servo_SetPhase      ( uint8_t phase );

#ifdef __cplusplus
}
#endif
#endif
