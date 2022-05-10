/*******************************************************************************
 * @file     bh1750.c
 * @brief
 * @version  V1.0
 * @date     2022.5.9
 * @author   auther email@example.com
 * @code     UTF-8
*******************************************************************************/

#include "bh1750.h"

uint8_t BH1750_Init ( void )
{
    BH1750_I2C_Write(BH1750_CMD_PWRON);
    BH1750_I2C_Write(BH1750_CMD_CHRES);
    return 0x00;
}    

uint16_t BH1750_Get ( void )
{
    uint8_t dat[2];
    uint16_t light;
    BH1750_I2C_Read( dat, 2 );
    light = ( dat[0] << 8 ) | dat[1];
    return light;
}

