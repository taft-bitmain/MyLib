/*******************************************************************************
 * @file     max30102.c
 * @brief
 * @version  V1.0
 * @date     2022.2.23
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "max30102.h"

/***************** Basic Interface *************************/

inline uint8_t MAX30102_ReadReg ( uint8_t regaddr )
{
    return MyI2C_ReadReg(&max30102_i2c,regaddr);
}

inline uint8_t MAX30102_ReadRegs ( uint8_t regaddr,uint8_t * pdat,uint16_t len )
{
    return MyI2C_ReadMem(&max30102_i2c,regaddr,pdat,len);
}

inline uint8_t MAX30102_WriteReg ( uint8_t regaddr,uint8_t dat )
{
    return MyI2C_WriteReg(&max30102_i2c,regaddr,dat);
}

inline void MAX30102_Delayms ( uint16_t ms )
{
    HAL_Delay(ms);
}


/****************** Main Functions *************************/

uint8_t MAX30102_Init (void)
{
    if(!MAX30102_WriteReg(MAX30102_INT_EN1,0xc0))       // INTR setting
        return 0x00;
    if(!MAX30102_WriteReg(MAX30102_INT_EN2,0x00))
        return 0x00;
    if(!MAX30102_WriteReg(MAX30102_FIFO_WR_PTR,0x00))   // FIFO_WR_PTR[4:0]
        return 0x00;                                       
    if(!MAX30102_WriteReg(MAX30102_FIFO_OVF_CNT,0x00))  // OVF_COUNTER[4:0]
        return 0x00;                                       
    if(!MAX30102_WriteReg(MAX30102_FIFO_RD_PTR,0x00))   // FIFO_RD_PTR[4:0]
        return 0x00;                                       
    if(!MAX30102_WriteReg(MAX30102_FIFO_CFG,0x0f))      // sample avg = 1, fifo rollover=false, fifo almost full = 17
        return 0x00;                                       
    if(!MAX30102_WriteReg(MAX30102_MODE_CFG,0x03))      // 0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
        return 0x00;
    if(!MAX30102_WriteReg(MAX30102_SpO2_CFG,0x27))      // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
        return 0x00;
    if(!MAX30102_WriteReg(MAX30102_LED1_PA,0x24))       // Choose value for ~ 7mA for LED1
        return 0x00;
    if(!MAX30102_WriteReg(MAX30102_LED2_PA,0x24))       // Choose value for ~ 7mA for LED2
        return 0x00;
    if(!MAX30102_WriteReg(MAX30102_PILOT_PA,0x7f))      // Choose value for ~ 25mA for Pilot LED
        return 0x00;
    return 0x01;
}

uint8_t MAX30102_Reset ( void )
{
    if(!MAX30102_WriteReg(MAX30102_MODE_CFG,0x40))
        return 0x00;
    else
        return 0x01;
}

uint8_t MAX30102_ReadFIFO ( uint32_t * pDataRed, uint32_t * pDataIR )
{
    uint8_t IntSta1, IntSta2;
    uint8_t pdata[6];

    //read and clear status register
    IntSta1 = MAX30102_ReadReg( MAX30102_INT_STA1 );
    IntSta2 = MAX30102_ReadReg( MAX30102_INT_STA2 );
    if( IntSta1 == 0xFF || IntSta2 == 0xFF )
    {
        return 0x00;
    }

    if( MAX30102_ReadRegs( MAX30102_FIFO, pdata, 6 ) == 0x00 )
    {
        return 0x00;
    }
    
    *pDataRed = ((uint8_t)pdata[0]<<16) | ((uint8_t)pdata[1]<<8) | ((uint8_t)pdata[2]);
    *pDataIR  = ((uint8_t)pdata[3]<<16) | ((uint8_t)pdata[4]<<8) | ((uint8_t)pdata[5]);
    
    *pDataRed &= 0x03FFFF;
    *pDataIR  &= 0x03FFFF;
    
    return 0x01;
}



