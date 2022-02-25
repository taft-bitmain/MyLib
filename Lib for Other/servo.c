/*******************************************************************************
 * @file     servo.c
 * @brief
 * @version  V1.0
 * @date     2022.2.24
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "servo.h"

// about delay time*10us @48MHz sysclk
void Servo_Delay10us ( uint32_t time) 
{
    uint32_t i, j;
    i = 0;
    do{
        j = 0;
        do{
            j++;
        }
        while( j < 480/5-2);
        i++;
    }
    while( i < time);
}

/****************** Main Functions *************************/

/*
period = 20ms
pulse width = 0.5ms ->   0°
pulse width = 1.5ms ->  90°
pulse width = 2.5ms -> 180°
*/

void Servo_SetPhase ( uint8_t phase )
{
    uint32_t width_pos, width_neg;
    
    phase = ( phase > 180) ? 180 : phase;
    width_pos = 50 + 10 * phase / 9;
    width_neg = 2000 - width_neg;
    
    Servo_Pin( 1 );
    Servo_Delay10us( width_pos );
    Servo_Pin( 0 );
    Servo_Delay10us( width_neg );
}

