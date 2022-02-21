/*******************************************************************************
 * @file     ws2812b.c
 * @brief
 * @version  V1.0
 * @date     2022.2.21
 * @author   RainingCats 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "ws2812b.h"

/***************** Basic Interface *************************/


/****************** Main Functions *************************/

void WS2812B_SendBit0 ( void )
{
    WS2812B_PinDIN(1);          // 400ns +/- 150ns
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();
    WS2812B_PinDIN(0);          // 850ns +/- 150ns
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
}

void WS2812B_SendBit1 ( void )
{
    WS2812B_PinDIN(1);          // 800ns +/- 150ns
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();__NOP();__NOP();
    WS2812B_PinDIN(0);          // 450ns +/- 150ns
}

void WS2812B_SetColor ( uint32_t GRB )
{
    uint8_t i;
    for( i = 0; i < 24; i++ )
    {
        if( GRB & 0x00800000)
        {
            WS2812B_SendBit1();
        }
        else
        {
            WS2812B_SendBit0();
        }
        GRB <<= 1;
    }
}

void WS2812B_SetColors ( uint32_t * GRBs, uint16_t len )
{
    uint8_t i;
    for( i = 0; i < len; i++ )
    {
        WS2812B_SetColor( GRBs[i] );
    }
}


