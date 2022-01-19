/*******************************************************************************
 * @file     dds.h
 * @brief    Generate sine signal with DAC and DMA on STM32F103ZET6, using HAL lib
 * @version  V1.0
 * @date     2021.3.27
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	CubeMX Configuration：
		see the pdf:
			STM32 HAL ADC和DAC使用指南.pdf
EXAMPLE CODE:
	
	DDS_SinOutput(&hdac,&htim6,DAC_CHANNEL_1,FRE_TAB[index]);
	printf("DDS Freq = %d \r\n",FRE_TAB[index]);
	
*******************************************************************************/
#ifndef __DDS_H
#define __DDS_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "dac.h"
#include "stdint.h"
#include "math.h"

//extern uint16_t dac_data[1024];
//extern uint16_t dac_data_len;

void			DDS_SinCalculation(uint16_t num, uint16_t Vpp,float offset);
void			DDS_TimerInit			(TIM_HandleTypeDef *htim,uint16_t Pre, uint16_t reload);
uint32_t	DDS_SinOutput			(DAC_HandleTypeDef *hdac,TIM_HandleTypeDef *htim,uint32_t Channel,uint32_t Frq,uint16_t Vpp);
void			DDS_SinOutputStop	(DAC_HandleTypeDef *hdac,TIM_HandleTypeDef *htim,uint32_t Channel);



#ifdef __cplusplus
}
#endif
#endif
