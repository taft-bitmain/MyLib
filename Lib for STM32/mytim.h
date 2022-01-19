/*******************************************************************************
 * @file     mytim.h
 * @brief    
 * @version  V1.0
 * @date     2021.5.2
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __MYTIM_H
#define __MYTIM_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "tim.h"

extern volatile uint32_t MyTIM_CountMs;
extern volatile uint32_t MyTIM_Fre;

//使用定时器2产生不同脉宽的闸门信号，用来控制定时器1的采样周期,闸门可取1ms~1s
void MyTIM_FreMeasure_Start(void);
void MyTIM_FreMeasure_Stop(void);
uint32_t MyTIM_FreMeasure_Cali(uint32_t frein);

#ifdef __cplusplus
}
#endif
#endif
