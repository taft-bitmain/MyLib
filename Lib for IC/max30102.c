/*******************************************************************************
 * @file     max30102.c
 * @brief
 * @version  V1.0
 * @date     2022.2.23
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "max30102.h"


/****************** Main Functions *************************/

uint8_t MAX30102_Init (void)
{
#ifdef MAX30102_I2C_SOFTWARE
    myi2c_io_init(&max30102_i2c);
#endif
    
    if(!MAX30102_WriteReg(MAX30102_INT_EN1,0x80))       // INTR setting
        return 0x01;
    if(!MAX30102_WriteReg(MAX30102_INT_EN2,0x00))
        return 0x02;
    if(!MAX30102_WriteReg(MAX30102_FIFO_WR_PTR,0x00))   // FIFO_WR_PTR[4:0]
        return 0x03;                                       
    if(!MAX30102_WriteReg(MAX30102_FIFO_OVF_CNT,0x00))  // OVF_COUNTER[4:0]
        return 0x04;                                       
    if(!MAX30102_WriteReg(MAX30102_FIFO_RD_PTR,0x00))   // FIFO_RD_PTR[4:0]
        return 0x05;                                       
    if(!MAX30102_WriteReg(MAX30102_FIFO_CFG,0x0f))      // sample avg = 1, fifo rollover=false, fifo almost full = 17
        return 0x06;                                       
    if(!MAX30102_WriteReg(MAX30102_MODE_CFG,0x03))      // 0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
        return 0x07;
    if(!MAX30102_WriteReg(MAX30102_SpO2_CFG,0x27))      // SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)
        return 0x08;
    if(!MAX30102_WriteReg(MAX30102_LED1_PA,0x24))       // Choose value for ~ 7mA for LED1
        return 0x09;
    if(!MAX30102_WriteReg(MAX30102_LED2_PA,0x24))       // Choose value for ~ 7mA for LED2
        return 0x0A;
    if(!MAX30102_WriteReg(MAX30102_PILOT_PA,0x7f))      // Choose value for ~ 25mA for Pilot LED
        return 0x0B;
    return 0x00;
}

uint8_t MAX30102_Reset ( void )
{
    if( MAX30102_WriteReg(MAX30102_MODE_CFG,0x40) )
        return 0x00;
    else
        return 0x01;
}

uint8_t MAX30102_ReadFIFO ( uint16_t * pDataRed, uint16_t * pDataIR )
{
    uint32_t tmp1, tmp2;
    uint8_t pdata[6];

    if( MAX30102_ReadRegs( MAX30102_FIFO, pdata, 6 ) == 0 )
    {
        *pDataRed = 0;
        *pDataIR = 0;
        return 0x00;
    }
    
    tmp1 = (((uint8_t)pdata[0]<<16) | ((uint8_t)pdata[1]<<8) | ((uint8_t)pdata[2])) & 0x03FFFF;
    tmp2 = (((uint8_t)pdata[3]<<16) | ((uint8_t)pdata[4]<<8) | ((uint8_t)pdata[5])) & 0x03FFFF;
    
    
    *pDataRed = tmp1 >> 2;
    *pDataIR = tmp2 >> 2;
    
    return 0x01;
}

uint16_t MAX30102_Sample ( uint16_t * pDataRed, uint16_t * pDataIR, uint16_t count, uint16_t *size )
{
    uint16_t smaple_count;
    uint8_t wrptr,overflow,rdptr;

    wrptr = MAX30102_ReadReg( MAX30102_FIFO_WR_PTR );
    overflow = MAX30102_ReadReg( MAX30102_FIFO_OVF_CNT );
    rdptr = MAX30102_ReadReg( MAX30102_FIFO_RD_PTR );
  
    if( wrptr >= rdptr )
    {
        smaple_count =  wrptr - rdptr;
        if( overflow )
            smaple_count =  0x20;
    }
    else
        smaple_count = wrptr + 0x20 - rdptr;
    uint16_t size_tmp = *size;
    for( ; smaple_count && size_tmp < count; smaple_count--)
    {
        MAX30102_ReadFIFO( pDataRed + size_tmp, pDataIR + size_tmp );
        size_tmp++;
    }
    *size = size_tmp;
    return size_tmp;
}



