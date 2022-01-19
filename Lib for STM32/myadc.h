/*******************************************************************************
 * @file     myadc.h
 * @brief    sample signal with ADC, using HAL lib
 * @version  V1.3
 * @date     2021.5.28
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:

-----------------------------CubeMX配置非常关键----------------------------

  1. 使用单ADC模式，配置Independent mode，DMA配置为HalfWord,HalfWord
  2. 使用单通道的定时器的触发转换，需要
    - 设置定时器输出信号
    - 配置外部触发源
    - 关掉连续转化模式
  3. 使用单通道的连续转换，需要
    - 设置定时器输出信号
    - 设置软件触发转换源（如果配置外部触发源，必须要产生这样一个触发来启动）
    - 打开连续转化模式
  4. 使用双ADC模式，配置Dual xxx模式，DMA配置为word,word
  5. 使用双ADC的定时器的触发转换，需要
    - 设置定时器输出信号
    - 配置外部触发源
    - 关掉连续转化模式（主从都关闭）
    - 启动从ADC（先）和主ADC（后）
  6. 使用双ADC的连续转换，需要
    - 设置定时器输出信号
    - 设置软件触发转换源（如果配置外部触发源，必须要产生这样一个触发来启动）
    - 打开连续转化模式（主从都打开）
    - 启动从ADC（先）和主ADC（后）
EXAMPLE CODE:


  ADC_SampleByTimer(&hadc1,&htim3,ADC_CHANNEL_5,ADC1_Data,1024);
  while(ADC1_ConvCplt_Flag == 0);
  
  //or
  
  ADC_SampleContinuous(&hadc1,&htim3,ADC_CHANNEL_5,ADC1_Data,1024);
  while(ADC1_ConvCplt_Flag == 0);
  
  //or
  
  ADC_DualSampleByTimer(&htim3,DualADC_Data,1024);
  while(ADC1_ConvCplt_Flag == 0);
  
  //or
  
  ADC_DualSampleContinuous(DualADC_Data,1024);
  while(ADC1_ConvCplt_Flag == 0);
  
*******************************************************************************/
#ifndef __MYADC_H
#define __MYADC_H

#define MYADC_USE_ADC1     0
#define MYADC_USE_ADC3     0
#define MYADC_USE_DUALADC  1

#define MYADC_TIMCLK       56000000

#define MYADC_DATAMAX 4096

#include "stm32f1xx_hal.h"
#include "adc.h"
#include "tim.h"
#include "stdint.h"

extern volatile uint8_t ADC1_ConvCplt_Flag;
extern volatile uint8_t ADC3_ConvCplt_Flag;

#if (MYADC_USE_ADC1 == 1)
extern uint16_t ADC1_Data[MYADC_DATAMAX];
#endif
#if (MYADC_USE_ADC3 == 1)
extern uint16_t  ADC3_Data[MYADC_DATAMAX];
#endif
#if (MYADC_USE_DUALADC == 1)
extern uint32_t DualADC_Data[MYADC_DATAMAX];
#endif

void			ADC_ChannelInit (ADC_HandleTypeDef *hadc,uint32_t Channel);
void			ADC_TimerInit		(TIM_HandleTypeDef *htim,uint16_t Pre, uint16_t reload);
uint8_t		ADC_Timer2Fre		(TIM_HandleTypeDef *htim,uint32_t Frq);

//单ADC采样
void			ADC_SampleByTimer	    (ADC_HandleTypeDef *hadc,TIM_HandleTypeDef *htim,uint32_t Channel,uint16_t *dat, uint32_t len);
void			ADC_SampleContinuous	(ADC_HandleTypeDef *hadc,TIM_HandleTypeDef *htim,uint32_t Channel,uint16_t *dat, uint32_t len);

//双ADC采样
#if (MYADC_USE_DUALADC == 1 )
void			ADC_DualSampleByTimer	    (TIM_HandleTypeDef *htim,uint32_t *dat, uint32_t len);
void			ADC_DualSampleContinuous	(uint32_t *dat, uint32_t len);
#endif


#endif
