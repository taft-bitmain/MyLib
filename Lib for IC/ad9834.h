/*******************************************************************************
 * @file     ad9834.h
 * @brief    
 * @version  V1.0
 * @date     2021.7.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:

EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __AD9834_H
#define __AD9834_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "base.h"


/************************* IO接口定义 ******************************/
#define AD9834_IO_SCLK		PAout(4)
#define AD9834_IO_DATA		PAout(2)
#define AD9834_IO_SYNC		PAout(0)
#define AD9834_IO_RESET		PAout(6)

/************************* 函数功能声明 ******************************/
void    AD9834_Delay        (uint32_t length);
void    AD9834_IOInit       (void);
void    AD9834_WriteReg     (uint16_t Data);
void    AD9834_Init         (void);
void    AD9834_SetFreq      (uint8_t which,uint32_t Freq_Hz);
void    AD9834_SetPhase     (uint8_t which,uint16_t Phase);


#ifdef __cplusplus
}
#endif
#endif
