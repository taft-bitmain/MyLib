/*******************************************************************************
 * @file     max262.c
 * @brief
 * @version  V1.0
 * @date     2021.7.11
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "max262.h"

void MAX262_Delay(uint16_t tick)
{
    uint16_t i;
      for( i = 0; i < tick * 16; i++ )
      {
          ;
      }
}

void MAX262_WriteOnce(uint8_t dat)
{
    MAX262_A0(dat & 0x01);
    MAX262_A1(dat & 0x02);
    MAX262_A2(dat & 0x04);
    MAX262_A3(dat & 0x08);
    MAX262_D0(dat & 0x10);
    MAX262_D1(dat & 0x20);
    //MAX262_Delay(5);
    MAX262_WR(0);
    MAX262_Delay(10);   //wait at least 250ns
    MAX262_WR(1);
    MAX262_Delay(10);   //wait at least 10ns
}

void MAX262_Write(uint8_t channel,uint8_t mode,uint8_t FreqWord,uint8_t QWord)
{
    uint8_t ChMask;
    ChMask = channel ? 0x08 : 0x00;
    MAX262_WriteOnce( ( (mode     & 0x03) << 4) | 0x00 | ChMask );
    MAX262_WriteOnce( ( (FreqWord & 0x03) << 4) | 0x01 | ChMask );
    MAX262_WriteOnce( ( (FreqWord & 0x0C) << 2) | 0x02 | ChMask );
    MAX262_WriteOnce( ( (FreqWord & 0x30)     ) | 0x03 | ChMask );
    MAX262_WriteOnce( ( (QWord    & 0x03) << 4) | 0x04 | ChMask );
    MAX262_WriteOnce( ( (QWord    & 0x0C) << 2) | 0x05 | ChMask );
    MAX262_WriteOnce( ( (QWord    & 0x30)     ) | 0x06 | ChMask );
    MAX262_WriteOnce( ( (QWord    & 0x40) >> 1) | 0x07 | ChMask );
}

void MAX262_Shutdown(void)
{
    MAX262_Write(0,0,0,0);
}

