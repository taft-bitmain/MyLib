/*******************************************************************************
 * @file	 ad9959.c
 * @brief
 * @version  V1.0
 * @date	 2021.7.4
 * @author   RainingRabbits 1466586342@qq.com
 * @code	 UTF-8
*******************************************************************************/

#include "ad9959.h"

const uint8_t CSR_CH[4] = {0x10,0x20,0x40,0x80};

uint8_t FR1_DATA[3] = {0xD0,0x00,0x00};         //default Value = 0x000000
uint8_t FR2_DATA[2] = {0x00,0x00};              //default Value = 0x0000
uint8_t CFR_DATA[3] = {0x00,0x23,0x35};         //default Value = 0x000302
uint8_t LSRR_DATA[2] = {0x00,0x00};             //default Value = 0x----
uint8_t RDW_DATA[4] = {0x00,0x00,0x00,0x00};    //default Value = 0x--------
uint8_t FDW_DATA[4] = {0x00,0x00,0x00,0x00};    //default Value = 0x--------

void AD9959_Init(void)
{

	AD9959_IOInit();
    
    AD9959_Reset();
	
    AD9959_WriteData(AD9959_REG_FR1,FR1_DATA,3);
    AD9959_WriteData(AD9959_REG_FR2,FR2_DATA,2);
    AD9959_WriteData(AD9959_REG_CFR,CFR_DATA,3);
//    AD9959_WriteData(AD9959_REG_LSRR,LSRR_DATA,2);
//    AD9959_WriteData(AD9959_REG_RDW,RDW_DATA,4);
//    AD9959_WriteData(AD9959_REG_FDW,FDW_DATA,4);
    
    uint8_t i;
    uint32_t SinFre[4] = {20000000,1000,2000,4000};
    uint32_t SinPhr[4] = {0,0,0,0};
    uint32_t SinAmp[4] = {511,1023,1023,1023};
    
    for( i = 0; i < 4; i++ )
    	AD9959_SetFreq(i,SinFre[i]);
	for( i = 0; i < 4; i++ )
    	AD9959_SetPhase(i,SinPhr[i]);
    for( i = 0; i < 4; i++ )
    	AD9959_SetAmpli(i,SinAmp[i]);
    
} 

void AD9959_Delay (uint32_t length)
{
    length *= 16;
    while(length--){ }
}

void AD9959_IOInit(void)		   
{   
//	AD9959_IO_PDC = 0;
    AD9959_IO_RST = 0;
	AD9959_IO_CS = 1;
	AD9959_IO_SCK = 0;
	AD9959_IO_UP = 0;
    AD9959_IO_SD0 = 0;
//	AD9959_IO_SD1 = 0;
//	AD9959_IO_SD2 = 0;
	AD9959_IO_SD3 = 0;
//	AD9959_IO_P0 = 0;
//	AD9959_IO_P1 = 0;
//	AD9959_IO_P2 = 0;
//	AD9959_IO_P3 = 0;
}

void AD9959_Reset(void)	  
{
	AD9959_IO_RST = 0;
	AD9959_Delay(5);
	AD9959_IO_RST = 1;
	AD9959_Delay(5);
	AD9959_IO_RST = 0;
}


void AD9959_IOUpdate(void)  
{
	AD9959_IO_UP = 0;
	AD9959_Delay(5);
	AD9959_IO_UP = 1;
	AD9959_Delay(5);
	AD9959_IO_UP = 0;
}

void AD9959_WriteData(uint8_t Address, const uint8_t* Data, uint8_t DataLen)
{
	uint8_t	DataTmp = 0;
	uint8_t	DataIndex = 0;
	uint8_t	i = 0;
    
	AD9959_IO_SCK = 0;
	AD9959_IO_CS = 0;
    
    //Write
    Address &= 0x7F;
    
    //address byte
	for(i = 0; i < 8; i++)
	{
		AD9959_IO_SCK = 0;
		if(0x80 == (Address & 0x80))
        {
            AD9959_IO_SD0 = 1;
        }
		else
        {
            AD9959_IO_SD0 = 0;
        }
		AD9959_IO_SCK = 1;
		Address <<= 1;
	}
	AD9959_IO_SCK = 0;
    
    //some data byte
	for (DataIndex = 0; DataIndex < DataLen; DataIndex++)
	{
		DataTmp = Data[DataIndex];
		for (i = 0; i < 8; i++)
		{
			AD9959_IO_SCK = 0;
            if(0x80 == (DataTmp & 0x80))
            {
                AD9959_IO_SD0 = 1;
            }
            else
            {
                AD9959_IO_SD0 = 0;
            }
            AD9959_IO_SCK = 1;
			DataTmp <<= 1;
		}
		AD9959_IO_SCK = 0;
	}
    
    AD9959_IO_CS = 1;
    
    AD9959_IOUpdate();
}
/*---------------------------------------
Freq: Hz, too low or too high is not good
---------------------------------------*/
void AD9959_SetFreq(uint8_t Channel,uint32_t Freq)
{	 
    uint8_t CFTW0_DATA[4];
    uint32_t Temp;			
    Temp = (uint32_t)(Freq * 8.589934592);
    
    CFTW0_DATA[0] = (uint8_t)(Temp>>24);
    CFTW0_DATA[1] = (uint8_t)(Temp>>16);
    CFTW0_DATA[2] = (uint8_t)(Temp>>8);
    CFTW0_DATA[3] = (uint8_t)Temp;
    
    if(Channel < 4)
    {
        AD9959_WriteData(AD9959_REG_CSR,&CSR_CH[Channel],1);
        AD9959_WriteData(AD9959_REG_CFTW0,CFTW0_DATA,4);
    }
}

/*---------------------------------------
Phase：0~16383 -> 0°~360°
---------------------------------------*/
void AD9959_SetPhase(uint8_t Channel,uint16_t Phase)
{
	uint16_t P_temp=0;
    uint8_t CPOW0_DATA[2];
    
	P_temp = (uint16_t)Phase;
    
	CPOW0_DATA[1] = (uint8_t)(P_temp);
	CPOW0_DATA[0] = (uint8_t)(P_temp>>8);
    
    if(Channel < 4)
    {
        AD9959_WriteData(AD9959_REG_CSR,&CSR_CH[Channel],1);
        AD9959_WriteData(AD9959_REG_CPOW0,CPOW0_DATA,2);
    }
    
}

/*---------------------------------------
Ampli:0~1023
---------------------------------------*/
void AD9959_SetAmpli(uint8_t Channel, uint16_t Ampli)
{ 
    uint16_t A_temp;
    uint8_t ACR_DATA[3];
    
    A_temp = Ampli | 0x1000;
    
    ACR_DATA[0] = 0x00;
    ACR_DATA[1] = ((uint8_t)(A_temp>>8) & 0x03) | 0x14;
    ACR_DATA[2] = (uint8_t)(A_temp);
    
    if(Channel < 4)
    {
        AD9959_WriteData(AD9959_REG_CSR,&CSR_CH[Channel],1);
        AD9959_WriteData(AD9959_REG_ACR,ACR_DATA,3);
    }

}

