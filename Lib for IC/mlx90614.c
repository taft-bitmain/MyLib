/*******************************************************************************
 * @file     mlx90614.c
 * @brief    Drive the IC mlx90614
 * @version  V1.1
 * @date     2022.4.17
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/

#include "mlx90614.h"


/****************** Main Functions *************************/

uint8_t MLX90614_Init ( void )
{
#ifdef MLX90614_I2C_SOFTWARE
    myi2c_io_init(&mlx90614_i2c);
#endif
    return 0;
}

uint8_t MLX90614_PEC_Cal(const uint8_t * data,uint16_t len, uint16_t polynomial)
{
    uint16_t tmp,i;
    uint16_t byte = 1;
    tmp = data[0];
    while(--len)
    {
        tmp = (tmp<<8) | data[byte++];
        for( i = 16; i>=8; i--)
        {
            if ( tmp & (1<<i) )
                tmp ^=  (polynomial<<(i-8));
        }
    }
    return (uint8_t)tmp;
}

uint16_t MLX90614_RegRead ( uint8_t addr )
{
    uint8_t data[6];
    data[0] = (MLX90614_I2C_ADDR<<1);
    data[1] = addr;
    data[2] = (MLX90614_I2C_ADDR<<1)|0x01;
    MLX90614_I2C_Read(addr , data + 3 , 3);
    if( MLX90614_PEC_Cal(data, 6, 0x107) == 0x00 )
        return  (data[4] << 8) | data[3];
    else
        return 0x0000;
}

uint8_t MLX90614_RegWrite ( uint8_t addr , uint16_t dat)
{
    uint8_t data[5];
    data[0] = (MLX90614_I2C_ADDR<<1);
    data[1] = addr;
    data[2] = dat;
    data[3] = (dat >> 8) & 0xff;
    data[4] = 0x00;
    data[4] = MLX90614_PEC_Cal(data, 4, 0x107);
    return MLX90614_I2C_Write(addr , data + 2 , 3);
}

float MLX90614_GetTemp( void )
{
    return MLX90614_RegRead(MLX90614_CMD_RAM|MLX90614_RAM_TOBJ1) * 0.02 - 273.15;
}
