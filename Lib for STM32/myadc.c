/*******************************************************************************
 * @file     myadc.c
 * @brief    sample signal with ADC, using HAL lib
 * @version  V1.3
 * @date     2021.5.28
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "myadc.h"

volatile uint8_t ADC1_ConvCplt_Flag;
volatile uint8_t ADC3_ConvCplt_Flag;

#if (MYADC_USE_ADC1 == 1)
uint16_t  ADC1_Data[MYADC_DATAMAX];
#endif
#if (MYADC_USE_ADC3 == 1)
uint16_t  ADC3_Data[MYADC_DATAMAX];
#endif
#if (MYADC_USE_DUALADC == 1)
uint32_t DualADC_Data[MYADC_DATAMAX];
#endif

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
		ADC1_ConvCplt_Flag = 1;
	else if(hadc->Instance == ADC3)
		ADC3_ConvCplt_Flag = 1;
}

void ADC_ChannelInit(ADC_HandleTypeDef *hadc,uint32_t Channel)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = Channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}
/**
  * @brief    初始化定时器的预分频值和重装载值
  * @note     
  * @param    
  * @retval   
  */
void ADC_TimerInit(TIM_HandleTypeDef *htim,uint16_t Pre, uint16_t reload)
{
  HAL_TIM_Base_Stop(htim);
  __HAL_TIM_SET_PRESCALER(htim,Pre);
  __HAL_TIM_SET_AUTORELOAD(htim,reload);
}

/**
  * @brief    设置定时器的溢出频率
  * @note     
  * @param    
  * @retval   
  */
uint8_t ADC_Timer2Fre(TIM_HandleTypeDef *htim,uint32_t Frq)
{
  uint16_t Pre,Reload;
  
  Pre = 0;
  Reload = MYADC_TIMCLK/Frq - 1;
  
  ADC_TimerInit(htim,Pre,Reload);
	return 1;
}
/**
  * @brief    通过定时器溢出频率采样
  * @note     
  * @param    
  * @retval   
  */
void ADC_SampleByTimer(ADC_HandleTypeDef *hadc,TIM_HandleTypeDef *htim,uint32_t Channel,uint16_t *dat, uint32_t len)
{
  
#if (MYADC_USE_ADC1 == 1)
  if(hadc->Instance == ADC1)
    ADC1_ConvCplt_Flag = 0;
#endif
#if (MYADC_USE_ADC3 == 1)
  if(hadc->Instance == ADC3)
    ADC3_ConvCplt_Flag = 0;
#endif
  
  HAL_ADC_Stop_DMA(hadc);
  HAL_TIM_Base_Stop(htim);

  hadc->Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc->Init.ContinuousConvMode = DISABLE;
  HAL_ADC_Init(&hadc1);
  
  HAL_TIM_Base_Start(htim);
  ADC_ChannelInit(hadc,Channel);
  len = (len < MYADC_DATAMAX)?len:MYADC_DATAMAX;
  HAL_ADC_Start_DMA(hadc,(uint32_t*)dat,len);
}
/**
  * @brief    连续转化，根据ADC时钟频率，按照最大采样速度采样
  * @note     
  * @param    
  * @retval   
  */
void ADC_SampleContinuous(ADC_HandleTypeDef *hadc,TIM_HandleTypeDef *htim,uint32_t Channel,uint16_t *dat, uint32_t len)
{
#if (MYADC_USE_ADC1 == 1)
  if(hadc->Instance == ADC1)
    ADC1_ConvCplt_Flag = 0;
#endif
#if (MYADC_USE_ADC3 == 1)
  if(hadc->Instance == ADC3)
    ADC3_ConvCplt_Flag = 0;
#endif
  
  HAL_ADC_Stop_DMA(hadc);
  
  hadc->Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc->Init.ContinuousConvMode = ENABLE;
  HAL_ADC_Init(&hadc1);
  
  ADC_ChannelInit(hadc,Channel);
  len = (len < MYADC_DATAMAX)?len:MYADC_DATAMAX;
  
  //尽管采样频率和定时器无关，但仍然需要定时器的触发信号来启动连续转换
  HAL_TIM_Base_Start(htim);
  HAL_ADC_Start_DMA(hadc,(uint32_t*)dat,len);
}


#if (MYADC_USE_DUALADC == 1 )

void ADC_DualSampleByTimer(TIM_HandleTypeDef *htim,uint32_t *dat, uint32_t len)
{
  ADC1_ConvCplt_Flag = 0;
    
  len = (len < MYADC_DATAMAX)?len:MYADC_DATAMAX;
  
  HAL_TIM_Base_Start(htim);
  
  //作为从模式的定时器，必须先要启动它
  HAL_ADC_Start(&hadc2);
  
  HAL_ADCEx_MultiModeStart_DMA(&hadc1,dat,len);
}

void ADC_DualSampleContinuous(uint32_t *dat, uint32_t len)
{
  ADC1_ConvCplt_Flag = 0;
  
  len = (len < MYADC_DATAMAX)?len:MYADC_DATAMAX;
  
  //作为从模式的定时器，必须先要启动它
  HAL_ADC_Start(&hadc2);
  
  HAL_ADCEx_MultiModeStart_DMA(&hadc1,dat,len);
}

#endif
