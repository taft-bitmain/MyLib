/****************************************************************************2***
 * @file	 ad9834.c
 * @brief
 * @version  V1.0
 * @date	 2021.7.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code	 UTF-8
*******************************************************************************/

#include "ad9834.h"

/************************* 寄存器配置参数（高位优先） *************************/

//#define MODE 0


uint16_t AD9834_REG[] = 
{   
    0x2000,         /* ctrl_word */
    0x4000,0x4000,  /* freq0_word */
    0x4000,0x4000,  /* freq1_word */
    0x4000,         /* phase0_word */
    0x4000          /* phase1_word */
};


void AD9834_Init(void)
{
    
	AD9834_IOInit();
    
    /* Software Reset */
    AD9834_WriteReg(0x0100);
    AD9834_Delay(10);
    AD9834_WriteReg(0x0000);
	
    AD9834_SetFreq(0,10000000);
    AD9834_SetPhase(0,0x0000);
    
//    AD9834_WriteReg(0x2100);
//    AD9834_WriteReg(0x50C7);
//    AD9834_WriteReg(0x4000);
//    AD9834_WriteReg(0xC000);
//    AD9834_WriteReg(0x2000);
} 

void AD9834_Delay(uint32_t length)
{
    length *= 16;
    while(length--){ }
}

void AD9834_IOInit(void)		   
{   
    AD9834_IO_RESET = 0;
    AD9834_IO_SYNC = 1;
	AD9834_IO_SCLK = 1;
    AD9834_IO_DATA = 0;
}


void AD9834_WriteReg(uint16_t Data)
{
	uint8_t i = 0;
    AD9834_IO_SYNC = 0;
    for (i = 0; i < 16; i++)
    {
        AD9834_IO_SCLK = 1;
        if( Data & 0x8000 )
        {
            AD9834_IO_DATA = 1;
        }
        else
        {
            AD9834_IO_DATA = 0;
        }
        AD9834_IO_SCLK = 0;
        Data <<= 1;
    }
    AD9834_IO_SCLK = 1;
    AD9834_IO_SYNC = 1;
}

void AD9834_SetFreq(uint8_t which,uint32_t Freq)
{	
    uint32_t Temp;
    
    /* FTW = fout * (2^28) / Fsys */    
#define  Fsys 75000000

    Temp = (uint32_t)(Freq*3.579139413 +0.5);
    
    Temp &= 0x0FFFFFFF;
    
    if(which == 0)
    {
        AD9834_REG[0] = 0x2000;
        AD9834_REG[1] = 0x4000 | (0x3FFF & Temp);
        AD9834_REG[2] = 0x4000 | (0x3FFF & ( Temp>>14 ));
        AD9834_WriteReg(AD9834_REG[0]);
        AD9834_WriteReg(AD9834_REG[1]);
        AD9834_WriteReg(AD9834_REG[2]);
    }
    else
    {
        AD9834_REG[0] = 0x2000;
        AD9834_REG[1] = 0x8000 | (0x3FFF & Temp);
        AD9834_REG[2] = 0x8000 | (0x3FFF & ( Temp>>14 ));
        AD9834_WriteReg(AD9834_REG[0]);
        AD9834_WriteReg(AD9834_REG[3]);
        AD9834_WriteReg(AD9834_REG[4]);
    }
    
}


void AD9834_SetPhase(uint8_t which,uint16_t Phase)
{
    
    /* degree = 360 * PHASE / 2^12 */
    
    if(which == 0)
    {
        AD9834_REG[5] = 0xC000 | (0x0FFF & Phase);
        AD9834_WriteReg(AD9834_REG[5]);
    }
    else
    {
        AD9834_REG[5] = 0xE000 | (0x0FFF & Phase);
        AD9834_WriteReg(AD9834_REG[6]);
    }
    
}
