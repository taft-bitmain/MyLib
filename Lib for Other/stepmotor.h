/*******************************************************************************
 * @file     stepmotor.h
 * @brief
 * @version  V1.0
 * @date     2022.2.24
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE:
    while( 1 )
    {
        StepMotor_Update(1);
        HAL_Delay(2);
    }
	
*******************************************************************************/
#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"

/***************** Basic Interface *************************/

#include "stm32f1xx.h"
#include "base.h"

// For 4 phase 8 pat
void    StepMotor_SetPhase      ( uint8_t phase );

/****************** Main Functions *************************/

// For 4 phase 8 pat
const static uint8_t STEPMOTOR_CW[8] ={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09};
const static uint8_t STEPMOTOR_CCW[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};

void    StepMotor_Update        ( uint8_t clockwise );


#ifdef __cplusplus
}
#endif
#endif
