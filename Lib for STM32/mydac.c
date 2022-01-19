/*******************************************************************************
 * @file     mydac.c
 * @brief    Generate sine signal with DAC and DMA on STM32F103ZET6, using HAL lib
 * @version  V1.0
 * @date     2021.3.27
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "mydac.h"

#define PI 3.1415926

uint16_t dac_data[1024];
uint16_t dac_data_len;

void DDS_SinCalculation(uint16_t len, uint16_t Vpp,float offset)
{
	uint16_t i;
	dac_data_len = len;
	Vpp = (uint16_t)Vpp*offset/2;
	if(Vpp > 2048)
		Vpp = 2048;
	for(i=0;i<dac_data_len;i++)
	{
		dac_data[i] = sin(2*PI*i/dac_data_len)*Vpp + 2047;
	}
}

void DDS_TimerInit(TIM_HandleTypeDef *htim,uint16_t Pre, uint16_t reload)
{
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim->Init.Prescaler = Pre;
	htim->Init.CounterMode = TIM_COUNTERMODE_UP;
	htim->Init.Period = reload;
	htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	HAL_TIM_Base_Init(htim);
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(htim, &sMasterConfig);
}

uint32_t DDS_SinOutput(DAC_HandleTypeDef *hdac,TIM_HandleTypeDef *htim,uint32_t Channel,uint32_t Frq,uint16_t Vpp)
{
	
	if(Frq > 200000) 	Frq = 200000;
	if(Frq < 20)		Frq = 20;
	
	if(Frq >= 2000)
	{
		//clk freq = 1MHz;
		DDS_TimerInit(htim,0,71);
		DDS_SinCalculation(1000000/Frq ,Vpp ,1);
	}
	else if(Frq >= 200)
	{
		//clk freq = 100KHz;
		DDS_TimerInit(htim,0,719);
		DDS_SinCalculation(100000/Frq ,Vpp ,1);
	}
	else if(Frq >= 20)
	{
		//clk freq = 10KHz;
		DDS_TimerInit(htim,9,719);
		DDS_SinCalculation(10000/Frq ,Vpp ,1);
	}
	else
		return 0;
	
	HAL_TIM_Base_Start(htim);
	HAL_DAC_Start_DMA(hdac,Channel,(uint32_t *)dac_data,dac_data_len,DAC_ALIGN_12B_R);
	__HAL_DMA_DISABLE_IT(hdac->DMA_Handle1, (DMA_IT_TC | DMA_IT_HT | DMA_IT_TE));
	return 1;
}

void DDS_SinOutputStop(DAC_HandleTypeDef *hdac,TIM_HandleTypeDef *htim,uint32_t Channel)
{
	HAL_DAC_Stop_DMA(hdac,Channel);
	HAL_TIM_Base_Stop(htim);
}
