/*******************************************************************************
 * @file     adf4351.h
 * @brief    
 * @version  V1.0
 * @date     2021.7.16
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE CODE:
	ADF4351_Init();
    ADF4351_SetFreq(25000);
*******************************************************************************/
#ifndef __ADF4351_H
#define __ADF4351_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "base.h"

/************************* IO接口定义 ******************************/
#define ADF4351_CLK		PAout(6)
#define ADF4351_DATA    PAout(4)
#define ADF4351_LE		PAout(2)
#define ADF4351_CE		PAout(0)


/************************* 函数功能声明 ******************************/
void        ADF4351_Delay           (uint32_t length);
void        ADF4351_WriteReg        (uint32_t dat);
void        ADF4351_Init            (void);
void        ADF4351_SetFreq         (uint32_t freq_100KHz);

#ifdef __cplusplus
}
#endif
#endif
