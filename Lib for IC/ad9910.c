/****************************************************************************2***
 * @file	 ad9910.c
 * @brief
 * @version  V1.0
 * @date	 2021.7.5
 * @author   RainingRabbits 1466586342@qq.com
 * @code	 UTF-8
*******************************************************************************/

#include "ad9910.h"


/************************* 寄存器配置参数（高位优先） *************************/
//反Sinc滤波器使能
const uint8_t CFR1_DATA[4] = {0x00, 0x01, 0x00, 0x04};

//使能单音profiles调制幅度
const uint8_t CFR2_DATA[4] = {0x01, 0x00, 0x08, 0x20};

//40M输入，25倍频，PFD工作，输入分频器工作，设置PLL电流，频段，系统时钟1Ghz
const uint8_t CFR3_DATA[4] = {0x0D, 0x3f, 0x41, 0x32};

//01->振幅控制，23->相位控制，4567->频率调谐字
uint8_t profile0[8] = {0x20, 0x00, 0x00, 0x00, 0xCC, 0xCC, 0xCC, 0xCC};


void AD9910_Init(void)
{
	AD9910_IOInit();
    
    AD9910_Reset();
	
    AD9910_WriteData(AD9910_REG_CFR1,CFR1_DATA,4);
    AD9910_WriteData(AD9910_REG_CFR2,CFR2_DATA,4);
    AD9910_WriteData(AD9910_REG_CFR3,CFR3_DATA,4);
//    AD9910_WriteData(AD9910_REG_ADAC,ADAC_DATA,4);
    
    AD9910_SetFreq(100000000);
//    AD9910_SetAmpli(0x3fff);
//    AD9910_SetPhase(0x0000);
    
    AD9910_WriteData(AD9910_REG_PROF0,profile0,8);
} 

void AD9910_Delay (uint32_t length)
{
    length *= 16;
    while(length--){ }
}

void AD9910_IOInit(void)		   
{   
    AD9910_IO_RST = 0;
    AD9910_IO_CSB = 1;
	AD9910_IO_SCLK = 0;
	AD9910_IO_UPD = 0;
    AD9910_IO_SDIO = 0;
}

void AD9910_Reset(void)
{
	AD9910_IO_RST = 0;
	AD9910_Delay(5);
	AD9910_IO_RST = 1;
	AD9910_Delay(20);
	AD9910_IO_RST = 0;
}


void AD9910_IOUpdate(void)
{
	AD9910_IO_UPD = 0;
	AD9910_Delay(5);
	AD9910_IO_UPD = 1;
	AD9910_Delay(5);
	AD9910_IO_UPD = 0;
}

void AD9910_WriteData(uint8_t Address, const uint8_t* Data, uint8_t DataLen)
{
	uint8_t	DataTmp = 0;
	uint8_t	DataIndex = 0;
	uint8_t	i = 0;

    //写入地址
	AD9910_IO_SCLK = 0;
	AD9910_IO_CSB = 0;	 
	for(i = 0; i < 8; i++)
	{
		AD9910_IO_SCLK = 0;
		if( Address & 0x80 )
        {
            AD9910_IO_SDIO = 1;
        }
		else
        {
            AD9910_IO_SDIO = 0;
        }
		AD9910_IO_SCLK = 1;
		Address <<= 1;
	}
	AD9910_IO_SCLK = 0;
    //写入数据
	for (DataIndex = 0; DataIndex < DataLen; DataIndex++)
	{
		DataTmp = Data[DataIndex];
        
		for (i = 0; i < 8; i++)
		{
			AD9910_IO_SCLK = 0;
            if( DataTmp & 0x80 )
            {
                AD9910_IO_SDIO = 1;
            }
            else
            {
                AD9910_IO_SDIO = 0;
            }
            AD9910_IO_SCLK = 1;
			DataTmp <<= 1;
		}
		AD9910_IO_SCLK = 0;
	}
    
    AD9910_IO_CSB = 1;
    
    AD9910_IOUpdate();
}

void AD9910_SetFreq(uint32_t Freq)
{	
    uint32_t Temp;
    
    Freq = (Freq > 500000000) ? 500000000 : Freq;
    
    /* see datasheet page23 */
    /* FTW = fout * (2^32) / 1GHz */
    Temp = (uint32_t)(Freq*4.294967296 +0.5);
    profile0[7] = (uint8_t)(Temp);
    profile0[6] = (uint8_t)(Temp>>8);
    profile0[5] = (uint8_t)(Temp>>16);
    profile0[4] = (uint8_t)(Temp>>24);
}


void AD9910_SetPhase(uint16_t Phase)
{
	uint16_t Temp=0;
    
    /* see datasheet page23 */
    /* delta degree = 360 * POW / 2^16 */
    
	Temp = (uint16_t)Phase;
    
	profile0[3] = (uint8_t)(Temp);
	profile0[2] = (uint8_t)(Temp>>8);
    
}

void AD9910_SetAmpli(uint16_t Ampli)
{ 
    uint16_t Temp;
    
    /* see datasheet page23 */
    /* Amplitude Scale = ASF / 2^14 */
    Temp = Ampli;
    
    Temp &= 0x3FFF;
    
    profile0[1] = (uint8_t)(Temp);
    profile0[0] = (uint8_t)(Temp>>8);
    

}

