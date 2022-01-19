/*******************************************************************************
 * @file     gy30.c
 * @brief
 * @version  V1.0
 * @date     2021.5.13
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "gy30.h"

#define GY30_DEVADDR 0x46

#define GY30_PWRON   0x00
#define GY30_PWROFF  0x01
#define GY30_RESET   0x07
#define GY30_CON_HR  0x10
#define GY30_CON_HR2 0x11
#define GY30_CON_LR  0x13
#define GY30_ONE_HR  0x20
#define GY30_ONE_HR2 0x21
#define GY30_ONE_LR  0x23

#define GY30_CH_TIM_H       0x40
#define GY30_CH_TIM_H_MASK  0x07
#define GY30_CH_TIM_L       0x60
#define GY30_CH_TIM_L_MASK  0x1F

MYIIC gy30 = {.GPIO_SCL = GPIOA,.Pin_SCL = GPIO_PIN_7,
					.GPIO_SDA = GPIOA,.Pin_SDA = GPIO_PIN_5,
					.Speed = 2,.DevAddr = GY30_DEVADDR};

void GY30_Init(void)
{
  MYIIC_Init(&gy30);
  MYIIC_WriteByte(&gy30,GY30_PWRON);
  MYIIC_WriteByte(&gy30,GY30_RESET);
}

uint16_t GY30_Read(void)
{
  char Data[2];
  MYIIC_WriteByte(&gy30,GY30_ONE_LR);
  HAL_Delay(20);
  MYIIC_Read(&gy30,Data,2);
  return ( (Data[0] << 8) | Data[1] );
}

