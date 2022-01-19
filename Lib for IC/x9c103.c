/*******************************************************************************
 * @file     x9c103.c
 * @brief
 * @version  V1.0
 * @date     2021.5.28
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "x9c103.h"



void X9C103_Delay(uint16_t n)
{
  uint16_t i;
  for( i = 0; i < n*55; i++ )
  {
  	;
  }
}

void X9C103_Store(void)
{
  X9C103_SetPin(INC,1);
  X9C103_SetPin(CS,0);
  X9C103_Delay(100);
  X9C103_SetPin(CS,1);
}

void X9C103_Increase(uint16_t n)
{
  uint16_t i;
  X9C103_SetPin(CS,0);
  X9C103_SetPin(UD,1);
  X9C103_Delay(100);
  
  for( i = 0; i < n; i++ )
  {
  	X9C103_SetPin(INC,1);
    X9C103_Delay(100);
    X9C103_SetPin(INC,0);
    X9C103_Delay(100);
  }
  X9C103_SetPin(CS,1);
  X9C103_Store();
}

void X9C103_Decrease(uint16_t n)
{
  uint16_t i;
  X9C103_SetPin(CS,0);
  X9C103_SetPin(UD,0);
  X9C103_Delay(100);
  
  for( i = 0; i < n; i++ )
  {
  	X9C103_SetPin(INC,1);
    X9C103_Delay(100);
    X9C103_SetPin(INC,0);
    X9C103_Delay(100);
  }
  X9C103_SetPin(CS,1);
  X9C103_Store();
}

void X9C103_SetZero(void)
{
  X9C103_Decrease(100);
  X9C103_Store();
}

