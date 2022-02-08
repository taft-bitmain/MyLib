/*******************************************************************************
 * @file     aht10.c
 * @brief    Drive the IC aht10
 * @version  V1.0
 * @date     2022.2.8
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/

#include "aht10.h"

/***************** Basic Interface *************************/

inline uint8_t AHT10_I2C_Write ( uint8_t * pdatain, uint8_t len )
{
    return MyI2C_WriteBytes( &aht10_i2c, pdatain, len );
}

inline uint8_t AHT10_I2C_Read ( uint8_t * pdataout, uint8_t len )
{
    return MyI2C_ReadBytes( &aht10_i2c, pdataout, len );
}

inline void AHT10_DelayMs ( uint16_t ms )
{
    HAL_Delay(ms);
}


/****************** Main Functions *************************/

uint8_t AHT10_Init ( void )
{
    uint8_t cmd[1] = { 0xE1 };
    AHT10_I2C_Write(cmd,1);
    
    // wait for 20 ms at most
    AHT10_DelayMs(20);
    
    AHT10_I2C_Read(cmd,1);
    
    if( cmd[0] & 0x08 )
        return 1;
    else
        return 0;
    
}

uint8_t AHT10_SoftReset ( void )
{
    uint8_t cmd[1] = { 0xBA };
    return AHT10_I2C_Write(cmd,1);
}

uint8_t AHT10_Trigger ( void )
{
    uint8_t cmd[3] = { 0xAC ,0x33 ,0x00 };
    return AHT10_I2C_Write(cmd,3);
}

uint8_t AHT10_MeasureGet ( uint32_t *RH , uint32_t *Temp )
{
    uint8_t dat[6];
    AHT10_I2C_Read(dat,6);
    
    if( dat[0] & 0x80 ) // busy
        return 0;
    *RH = (dat[1] << 12) | (dat[2] << 4) |( (dat[3]&0xf0) >> 4);
    *Temp = ((dat[3]&0x0f) << 16) | (dat[4] << 8) |(dat[5] );
    return 1;
}

