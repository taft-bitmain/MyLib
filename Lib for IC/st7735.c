/*******************************************************************************
 * @file     st7735.c
 * @brief
 * @version  V1.0
 * @date     2022.2.28
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "st7735.h"

/***************** Basic Interface *****************/

void ST7735_LL_Init( void )
{
    myspi_io_init( &st7735_spi );
}

inline void ST7735_LL_Delay ( uint16_t ms )
{
    HAL_Delay(ms);
}

void ST7735_LL_SendCmd ( uint8_t cmd )
{
	ST7735_LL_PinDC( 0 );
    myspi_transceive_start( &st7735_spi );
    myspi_transmit_byte( &st7735_spi, cmd );
	myspi_transceive_end( &st7735_spi );
}

void ST7735_LL_SendData ( uint8_t data )
{
	ST7735_LL_PinDC( 1 );
    myspi_transceive_start( &st7735_spi );
    myspi_transmit_byte( &st7735_spi, data );
	myspi_transceive_end( &st7735_spi );
}

void ST7735_LL_SendPixel ( uint16_t rgb )
{
	ST7735_LL_PinDC( 1 );
    myspi_transceive_start( &st7735_spi );
    myspi_transmit_byte( &st7735_spi, (uint8_t)(rgb>>8) );
    myspi_transmit_byte( &st7735_spi, (uint8_t)(rgb) );
	myspi_transceive_end( &st7735_spi );
}

//uint8_t ST7735_LL_RecvData ( void )
//{
//    uint8_t dat;
//	ST7735_LL_PinDC( 1 );
//    myspi_transceive_start( &st7735_spi );
//    dat = myspi_receive_byte( &st7735_spi );
//	myspi_transceive_end( &st7735_spi );
//    return dat;
//}


/******************* Main Functions **************************/

void ST7735_Init ( void)
{
    uint8_t buff[15];
    
    ST7735_LL_Init();
    
    ST7735_LL_PinBLK( 0 );
    
    ST7735_LL_PinRST( 0 );
    ST7735_LL_Delay( 5 );
    ST7735_LL_PinRST( 1 );
    ST7735_LL_Delay( 120 );
    
    ST7735_LL_SendCmd( ST7735_CMD_SLPOUT );
    ST7735_LL_Delay( 120 );
    
    // Start Initial Sequence
    
    ST7735_LL_SendCmd( 0xB1 );      // 帧率
    ST7735_LL_SendData( 0x05 );
    ST7735_LL_SendData( 0x3A );
    ST7735_LL_SendData( 0x3A );

    ST7735_LL_SendCmd( 0xB2 );      // 帧率
    ST7735_LL_SendData( 0x05 );
    ST7735_LL_SendData( 0x3A );
    ST7735_LL_SendData( 0x3A );

    ST7735_LL_SendCmd( 0xB3 );      // 帧率
    ST7735_LL_SendData( 0x05 );
    ST7735_LL_SendData( 0x3A );
    ST7735_LL_SendData( 0x3A );
    ST7735_LL_SendData( 0x05 );
    ST7735_LL_SendData( 0x3A );
    ST7735_LL_SendData( 0x3A );
    
    ST7735_LL_SendCmd( 0xB4 );
    ST7735_LL_SendData( 0x03 );
       
    ST7735_LL_SendCmd( 0xC0 );
    ST7735_LL_SendData( 0x62 );
    ST7735_LL_SendData( 0x02 );
    ST7735_LL_SendData( 0x04 );

    ST7735_LL_SendCmd( 0xC1 );
    ST7735_LL_SendData( 0xC0 );

    ST7735_LL_SendCmd( 0xC2 );
    ST7735_LL_SendData( 0x0D );
    ST7735_LL_SendData( 0x00 );

    ST7735_LL_SendCmd( 0xC3 );
    ST7735_LL_SendData( 0x8D );
    ST7735_LL_SendData( 0x6A );

    ST7735_LL_SendCmd( 0xC4 );
    ST7735_LL_SendData( 0x8D );
    ST7735_LL_SendData( 0xEE );

    ST7735_LL_SendCmd( 0xC5 );
    ST7735_LL_SendData( 0x0E );

    ST7735_LL_SendCmd( 0xE0 );
    ST7735_LL_SendData( 0x10 );
    ST7735_LL_SendData( 0x0E );
    ST7735_LL_SendData( 0x02 );
    ST7735_LL_SendData( 0x03 );
    ST7735_LL_SendData( 0x0E );
    ST7735_LL_SendData( 0x07 );
    ST7735_LL_SendData( 0x02 );
    ST7735_LL_SendData( 0x07 );
    ST7735_LL_SendData( 0x0A );
    ST7735_LL_SendData( 0x12 );
    ST7735_LL_SendData( 0x27 );
    ST7735_LL_SendData( 0x37 );
    ST7735_LL_SendData( 0x00 );
    ST7735_LL_SendData( 0x0D );
    ST7735_LL_SendData( 0x0E );
    ST7735_LL_SendData( 0x10 );

    ST7735_LL_SendCmd( 0xE1 );
    ST7735_LL_SendData( 0x10 );
    ST7735_LL_SendData( 0x0E );
    ST7735_LL_SendData( 0x03 );
    ST7735_LL_SendData( 0x03 );
    ST7735_LL_SendData( 0x0F );
    ST7735_LL_SendData( 0x06 );
    ST7735_LL_SendData( 0x02 );
    ST7735_LL_SendData( 0x08 );
    ST7735_LL_SendData( 0x0A );
    ST7735_LL_SendData( 0x13 );
    ST7735_LL_SendData( 0x26 );
    ST7735_LL_SendData( 0x36 );
    ST7735_LL_SendData( 0x00 );
    ST7735_LL_SendData( 0x0D );
    ST7735_LL_SendData( 0x0E );
    ST7735_LL_SendData( 0x10 );

    ST7735_LL_SendCmd( 0x3A );      // color mode : 16-bit/pixel
    ST7735_LL_SendData( 0x05 );

    ST7735_LL_SendCmd( 0x36 );      // 刷新方向
     ST7735_LL_SendData( 0x08 );
    // ST7735_LL_SendData( 0xC8 );
    // ST7735_LL_SendData( 0x78 );
    // ST7735_LL_SendData( 0xA8 );

    //ST7735_LL_SendCmd( 0x13 );      // 一般模式，默认开启
    
    ST7735_LL_SendCmd( 0x21 );      // 颜色反转
    
    ST7735_LL_SendCmd( 0x29 );      // display on
    ST7735_LL_PinBLK( 1 );
}

void ST7735_SetAddress( uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end)
{
    ST7735_LL_SendCmd( 0x2A );
    ST7735_LL_SendData( 0x00 );
    ST7735_LL_SendData( x_start + ST7735_COL_OFFSET);
    ST7735_LL_SendData( 0x00 );
    ST7735_LL_SendData( x_end + ST7735_COL_OFFSET);

    ST7735_LL_SendCmd( 0x2B );
    ST7735_LL_SendData( 0x00 );
    ST7735_LL_SendData( y_start + ST7735_ROW_OFFSET);
    ST7735_LL_SendData( 0x00 );
    ST7735_LL_SendData( y_end + ST7735_ROW_OFFSET);
}

void ST7735_Clear( uint16_t color )
{
    uint16_t i;
    ST7735_SetAddress( 0, 0, ST7735_COL_MAX-1, ST7735_ROW_MAX-1 );
    
    ST7735_LL_SendCmd( 0x2C );
    for (i = 0; i < ST7735_COL_MAX * ST7735_ROW_MAX; i++)
        ST7735_LL_SendPixel( color );
}

void ST7735_RefreshPart ( uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint16_t * colors )
{
    uint16_t i;
    ST7735_SetAddress( x, y, width-1, height-1 );
    
    ST7735_LL_SendCmd( 0x2C );
    for (i = 0; i < width * height; i++)
        ST7735_LL_SendPixel( *colors++ );
}

