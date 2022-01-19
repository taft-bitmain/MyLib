/*******************************************************************************
 * @file     adf4351.c
 * @brief
 * @version  V1.0
 * @date     2021.7.16
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "adf4351.h"

uint32_t ADF4351Reg[6] = {
    0x00000000|350<<15,
    0x08008011,
    0x0D003Fc2|125<<14,
    0x006004b3,
    0x0060a43c,
    0x00580005
};
    

void ADF4351_Delay(uint32_t length)
{
    length *= 16;
    while(length--){ }
}

void ADF4351_WriteReg(uint32_t dat)
{
    uint8_t i;
    ADF4351_LE = 0;
    ADF4351_Delay(1);
    for(i = 0; i < 32; i++)
	{
		ADF4351_CLK = 0;
		ADF4351_DATA = (dat & 0x80000000) ? 1 : 0;
        ADF4351_Delay(1);
		ADF4351_CLK = 1;
		dat <<= 1;
        ADF4351_Delay(1);
	}
    ADF4351_CLK = 0;
    ADF4351_DATA = 0;
    ADF4351_Delay(1);
    ADF4351_LE = 1;
}

void ADF4351_Init(void)
{
    ADF4351_CLK  = 0;
    ADF4351_DATA = 0;
    ADF4351_LE   = 0;
    ADF4351_CE   = 1;
    HAL_Delay(10);
    ADF4351_WriteReg(ADF4351Reg[5]);
    ADF4351_WriteReg(ADF4351Reg[4]);
    ADF4351_WriteReg(ADF4351Reg[3]);
    ADF4351_WriteReg(ADF4351Reg[2]);
    ADF4351_WriteReg(ADF4351Reg[1]);
    ADF4351_WriteReg(ADF4351Reg[0]);
}

void ADF4351_SetFreq(uint32_t freq_100KHz)
{
    if (     freq_100KHz >= 690  && freq_100KHz <= 1370 )
        ADF4351_WriteReg(0x0050443c);
    else if (freq_100KHz > 1370  && freq_100KHz <= 2740 )
        ADF4351_WriteReg(0x0040443c);
    else if (freq_100KHz > 2740  && freq_100KHz <= 5490 )
        ADF4351_WriteReg(0x0030143c);
    else if (freq_100KHz > 5490  && freq_100KHz <= 10990)
        ADF4351_WriteReg(0x0020143c); 
    else if (freq_100KHz > 10990 && freq_100KHz <= 21990)
        ADF4351_WriteReg(0x0010143c); 
    else if (freq_100KHz > 21990)
        ADF4351_WriteReg(0x0000143c); 
    else
        ADF4351_WriteReg(0x0060443c);	
                                                            
    ADF4351_WriteReg(0x00000000 | freq_100KHz << 15);
}
