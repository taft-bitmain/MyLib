/*******************************************************************************
 * @file     hmc472.c
 * @brief
 * @version  V1.0
 * @date     2021.7.11
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "hmc472.h"

void HMC472(uint8_t para)
{
    HMC472_P0(para & 0x01);
    HMC472_P1(para & 0x02);
    HMC472_P2(para & 0x04);
    HMC472_P3(para & 0x08);
    HMC472_P4(para & 0x10);
    HMC472_P5(para & 0x20);
}
