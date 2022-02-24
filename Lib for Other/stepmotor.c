/*******************************************************************************
 * @file     stepmotor.c
 * @brief
 * @version  V1.0
 * @date     2022.2.24
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "stepmotor.h"

/***************** Basic Interface *************************/
 
 // For 4 phase 8 pat
void StepMotor_SetPhase ( uint8_t phase )
{
    WritePin( GPIOB, GPIO_PIN_5, phase&0x01 );
    WritePin( GPIOB, GPIO_PIN_6, phase&0x02 );
    WritePin( GPIOB, GPIO_PIN_7, phase&0x04 );
    WritePin( GPIOB, GPIO_PIN_8, phase&0x08 );
}

/****************** Main Functions *************************/

// For 4 phase 8 pat
void StepMotor_Update ( uint8_t clockwise )
{
    static uint8_t phase_index;
    phase_index &= 0x07;
    if( clockwise )
        StepMotor_SetPhase(STEPMOTOR_CW[phase_index++]);
    else
        StepMotor_SetPhase(STEPMOTOR_CCW[phase_index++]);
}

