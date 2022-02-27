/*******************************************************************************
 * @file     xpt2046.c
 * @brief
 * @version  V1.0
 * @date     2022.2.27
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "xpt2046.h"

/****************** Basic Interface *************************/
 
inline void XPT2046_LL_Init ( void )
{
    myspi_io_init( &xpt2046_spi );
}

uint16_t XPT2046_LL_Transceive ( uint8_t cmd )    
{
    uint8_t dat[2];
    uint16_t ret;
    myspi_read_bytes( &xpt2046_spi, cmd, dat, 2 );
    
    ret  = (dat[0] << 5) & 0x0FE0;
    ret |= (dat[1] >> 3) & 0x001F;
    
    return  ret;
}

/******************* Main Functions **************************/


void XPT2046_Init ( void )
{
    XPT2046_LL_Init();
}

void XPT2046_GetPosition ( uint16_t * x, uint16_t * y )
{
    *x = XPT2046_LL_Transceive( XPT2046_CMD_RDX );
    *y = XPT2046_LL_Transceive( XPT2046_CMD_RDY );
}

// Rtouch = Rx * ( x / 4096 ) * ( Z2 / Z1 - 1 )

uint16_t XPT2046_GetPressure ( void )
{
    uint16_t X, Z1, Z2;
    float pressure;
    
    X = XPT2046_LL_Transceive( XPT2046_CMD_RDX );
    Z1 = XPT2046_LL_Transceive( XPT2046_CMD_RDZ1 );
    Z2 = XPT2046_LL_Transceive( XPT2046_CMD_RDZ2 );
    pressure  = (float)512 * X / 4096 * ( (float)Z2 / Z1 - 1 );
    return (uint16_t)pressure;
}

