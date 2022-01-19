/*******************************************************************************
 * @file     gy30.h
 * @brief    
 * @version  V1.0
 * @date     2021.5.13
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __GY30_H
#define __GY30_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "myiic.h"

extern MYIIC gy30;

void       GY30_Init      (void);
uint16_t   GY30_Read      (void);

#ifdef __cplusplus
}
#endif
#endif
