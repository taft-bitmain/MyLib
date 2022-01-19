/*******************************************************************************
 * @file     mytimer.c
 * @brief
 * @version  V1.0
 * @date     2021.5.2
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "mytim.h"

volatile uint32_t MyTIM_UpdateCount;
volatile uint32_t MyTIM_CountMs;
volatile uint32_t MyTIM_Fre;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM1)
    MyTIM_UpdateCount++;
  if(htim->Instance == TIM2)
  {
    //获得计数值
    uint32_t count;
    count = MyTIM_UpdateCount * 65536 + __HAL_TIM_GET_COUNTER(&htim1);
    MyTIM_Fre = count * (1000/MyTIM_CountMs);
    //清零
    MyTIM_UpdateCount = 0;
    __HAL_TIM_SET_COUNTER(&htim1,0);
  }
}



void MyTIM_FreMeasure_Start(void)
{
  MyTIM_UpdateCount = 0;
  MyTIM_CountMs = 1000;
  //设置测量时间
  htim2.Init.Period = MyTIM_CountMs + 10 - 1;
  HAL_TIM_Base_Init(&htim2);
  
  //清空用来计数的定时器
  __HAL_TIM_SET_COUNTER(&htim1,0);
  
  //启动定时器，先启动用来计数的，在启动用来产生闸门的
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim2);
  
  //使用PWM模式2产生闸门，闸门时间从 CCR1  ->  ARR
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
  
  //while();
//  HAL_Delay(1012);
//  HAL_TIM_Base_Stop(&htim2);
//  
//  //获得计数值
//  count = MyTIM_UpdateCount * 65536 + __HAL_TIM_GET_COUNTER(&htim1);
//  
//  return count * (1000/time_ms);
}

void MyTIM_FreMeasure_Stop(void)
{
  HAL_TIM_Base_Stop_IT(&htim2);
  HAL_TIM_Base_Stop_IT(&htim1);
}

uint32_t MyTIM_FreMeasure_Cali(uint32_t frein)
{
  uint32_t freout;
  freout = (uint32_t)(frein*1.000054 + 0.5);
  return freout;
}
