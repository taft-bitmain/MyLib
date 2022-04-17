/*******************************************************************************
 * @file     aht10.c
 * @brief    Drive the IC aht10
 * @version  V1.1
 * @date     2022.4.17
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/

#include "aht10.h"

uint8_t CMD_INIT[3]    = { 0xE1 ,0x08 ,0x00 };
uint8_t CMD_SWRST[1]   = { 0xBA };
uint8_t CMD_TRIGGER[3] = { 0xAC ,0x33 ,0x00 };


/****************** Main Functions *************************/

uint8_t AHT10_Init ( void )
{
    uint8_t dat[3] = { 0xE1 ,0x08 ,0x00 };
    
#ifdef AHT10_I2C_HARDWARE

    

#endif
    
#ifdef AHT10_I2C_SOFTWARE

    myi2c_io_init(&aht10_i2c);
    
#endif
    
    // wait for 20 ms after power on
    AHT10_DelayMs(40);
    
    AHT10_I2C_Read(dat,1);
    if( dat[0] & 0x08 )
        return 0;
        
    AHT10_I2C_Write(CMD_INIT,sizeof(CMD_INIT));
    
    return 1;
}

uint8_t AHT10_SoftReset ( void )
{
    return AHT10_I2C_Write(CMD_SWRST,sizeof(CMD_SWRST));
}

uint8_t AHT10_Trigger ( void )
{
    return AHT10_I2C_Write(CMD_TRIGGER,sizeof(CMD_TRIGGER));
}

uint8_t AHT10_MeasureGetRaw ( uint32_t *RH , uint32_t *Temp )
{
    uint8_t dat[6];
    AHT10_I2C_Read(dat,6);
    
    if( dat[0] & 0x80 ) // busy
        return 0;
    *RH = (dat[1] << 12) | (dat[2] << 4) |( (dat[3]&0xf0) >> 4);
    *Temp = ((dat[3]&0x0f) << 16) | (dat[4] << 8) |(dat[5] );
    return 1;
}

uint8_t AHT10_MeasureGet ( uint8_t *RH , uint8_t *Temp )
{
    uint32_t rh,temp;
    if ( AHT10_MeasureGetRaw(&rh,&temp) )
    {
        *RH = (uint8_t)(rh*100/0xfffff);
        *Temp = (uint8_t)(temp*200/0xfffff-50);
        return 1;
    }
    *RH = 0;
    *Temp = 0;
    return 0;
}

uint8_t AHT10_MeasureGetf ( float *RH , float *Temp )
{
    uint32_t rh,temp;
    if ( AHT10_MeasureGetRaw(&rh,&temp) )
    {
        *RH = (uint8_t)(rh*100.0/0xfffff);
        *Temp = (uint8_t)(temp*200.0/0xfffff-50);
        return 1;
    }
    *RH = 0;
    *Temp = 0;
    return 0;
}

