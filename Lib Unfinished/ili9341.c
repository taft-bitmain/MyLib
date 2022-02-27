/*******************************************************************************
 * @file     ili9341.c
 * @brief    drive the TFT with ILI9341
 * @version  V1.0
 * @date     2022.2.25
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "ili9341.h"

/****************** Basic Interface *************************/

inline void ILI9341_LL_DelayMs ( uint32_t ms )
{
    HAL_Delay( ms );
}

void ILI9341_LL_Init ( void )
{
    MySPI_IO_Init( &ili9341_spi );
}

void ILI9341_LL_Write ( uint8_t cmd, uint8_t * data, uint16_t len )
{
    uint32_t i;
    ILI9341_SetPinDC( 0 );
    MySPI_TransceiveStart( &ili9341_spi );
    MySPI_TransmitByte( &ili9341_spi,cmd );
    ILI9341_SetPinDC( 1 );
    for( i = 0; i < len; i++ )
    {
        MySPI_TransmitByte( &ili9341_spi,data[i] );
    }
    MySPI_TransceiveEnd( &ili9341_spi );
}

void ILI9341_LL_WriteColor ( uint16_t RGB )
{
    ILI9341_SetPinDC( 1 );
    MySPI_TransceiveStart( &ili9341_spi );
    MySPI_TransmitByte( &ili9341_spi, (uint8_t)(RGB >> 8) );
    MySPI_TransmitByte( &ili9341_spi, (uint8_t)(RGB & 0xFF) );
    MySPI_TransceiveEnd( &ili9341_spi );
}

void ILI9341_LL_Read ( uint8_t cmd, uint8_t * data, uint16_t len )   
{
    uint32_t i;
    ILI9341_SetPinDC( 0 );
    MySPI_TransceiveStart( &ili9341_spi );
    MySPI_TransmitByte( &ili9341_spi,cmd );
    ILI9341_SetPinDC( 1 );
    for( i = 0; i < len; i++ )
    {
        data[i] = MySPI_ReceiveByte( &ili9341_spi );
    }
    MySPI_TransceiveEnd( &ili9341_spi );
}

void ILI9341_Init ( void )
{
    uint8_t buff[15];
    
    ILI9341_LL_Init();
    
    ILI9341_SetPinRST( 1 );
    ILI9341_LL_DelayMs( 1 );
    ILI9341_SetPinRST( 0 );
    ILI9341_LL_DelayMs(10);
    ILI9341_SetPinRST( 1 );
    ILI9341_LL_DelayMs(120);
    
    //************* Start Initial Sequence **********//
    
    buff[ 0 ] = 0x39;
    buff[ 1 ] = 0x2C;
    buff[ 2 ] = 0x00;
    buff[ 3 ] = 0x34;
    buff[ 4 ] = 0x02;
    ILI9341_LL_Write( ILI9341_CMD_PowerCtrlA, buff, 5 );
    
    buff[ 0 ] = 0x00;
    buff[ 1 ] = 0xD9;
    buff[ 2 ] = 0X30;
    ILI9341_LL_Write( ILI9341_CMD_PowerCtrlB, buff, 3 );

    buff[ 0 ] = 0x85;
    buff[ 1 ] = 0x10;
    buff[ 2 ] = 0x7A;
    ILI9341_LL_Write( ILI9341_CMD_DriverTimingCtrlA, buff, 3 );

    buff[ 0 ] = 0x00;
    buff[ 1 ] = 0x00;
    ILI9341_LL_Write( ILI9341_CMD_DriverTimingCtrlB, buff, 2 );

    buff[ 0 ] = 0x64;
    buff[ 1 ] = 0x03;
    buff[ 2 ] = 0X12;
    buff[ 3 ] = 0X81;
    ILI9341_LL_Write( ILI9341_CMD_PowerOnSeqCtrl, buff, 4 );

    buff[ 0 ] = 0x20;
    ILI9341_LL_Write( ILI9341_CMD_PumpRatioCtrl, buff, 1 );

    buff[ 0 ] = 0x1B;  // VRH[5:0]
    ILI9341_LL_Write( ILI9341_CMD_PowerCtrl1, buff, 1 );

    buff[ 0 ] = 0x12;  // SAP[2:0];BT[3:0]
    ILI9341_LL_Write( ILI9341_CMD_PowerCtrl2, buff, 1 );

    buff[ 0 ] = 0x08;
    buff[ 1 ] = 0x26;
    ILI9341_LL_Write( ILI9341_CMD_VCOMCtrl1, buff, 2 );

    buff[ 0 ] = 0XB7;
    ILI9341_LL_Write( ILI9341_CMD_VCOMCtrl2, buff, 1 );

    buff[ 0 ] = 0x36;
    ILI9341_LL_Write( ILI9341_CMD_MemAccessCtrl, buff, 1 );

//    buff[ 0 ] = 0x00;   // start
//    buff[ 1 ] = 0x00;
//    buff[ 2 ] = 0x01;   // end
//    buff[ 3 ] = 0x3F;
//    ILI9341_LL_Write( ILI9341_CMD_ColumnAddrSet, buff, 4 );

//    buff[ 0 ] = 0x00;   // start
//    buff[ 1 ] = 0x00;
//    buff[ 2 ] = 0x00;   // end
//    buff[ 3 ] = 0xEF;
//    ILI9341_LL_Write( ILI9341_CMD_PageAddrSet, buff, 4 );

    buff[ 0 ] = 0x55;
    ILI9341_LL_Write( ILI9341_CMD_PixFormatSet, buff, 1 );

    buff[ 0 ] = 0x00;
    buff[ 1 ] = 0x1A;
    ILI9341_LL_Write( ILI9341_CMD_FrameCtrlNormal, buff, 2 );

    buff[ 0 ] = 0x0A;
    buff[ 1 ] = 0xA2;
    //buff[ 2 ] = 0x27;
    ILI9341_LL_Write( ILI9341_CMD_DispFuncCtrl, buff, 2 );

    buff[ 0 ] = 0x00;
    ILI9341_LL_Write( ILI9341_CMD_Enable3G, buff, 1 );

    buff[ 0 ] = 0x01;
    ILI9341_LL_Write( ILI9341_CMD_GammaSet, buff, 1 );

    buff[ 0 ]  = 0x0F;
    buff[ 1 ]  = 0x1D;
    buff[ 2 ]  = 0x1A;
    buff[ 3 ]  = 0x0A;
    buff[ 4 ]  = 0x0D;
    buff[ 5 ]  = 0x07;
    buff[ 6 ]  = 0x49;
    buff[ 7 ]  = 0X66;
    buff[ 8 ]  = 0x3B;
    buff[ 9 ]  = 0x07;
    buff[ 10 ] = 0x11;
    buff[ 11 ] = 0x01;
    buff[ 12 ] = 0x09;
    buff[ 13 ] = 0x05;
    buff[ 14 ] = 0x04;
    ILI9341_LL_Write( ILI9341_CMD_PosiGammaCorrection, buff,15 );

    buff[ 0 ]  = 0x00;
    buff[ 1 ]  = 0x18;
    buff[ 2 ]  = 0x1D;
    buff[ 3 ]  = 0x02;
    buff[ 4 ]  = 0x0F;
    buff[ 5 ]  = 0x04;
    buff[ 6 ]  = 0x36;
    buff[ 7 ]  = 0x13;
    buff[ 8 ]  = 0x4C;
    buff[ 9 ]  = 0x07;
    buff[ 10 ] = 0x13;
    buff[ 11 ] = 0x0F;
    buff[ 12 ] = 0x2E;
    buff[ 13 ] = 0x2F;
    buff[ 14 ] = 0x05;
    ILI9341_LL_Write( ILI9341_CMD_NegGammaCorrection, buff, 15 );
    
    ILI9341_LL_Write( ILI9341_CMD_SleepOut, NULL, 0 );
    
    ILI9341_LL_DelayMs( 120 );
    
    ILI9341_LL_Write( ILI9341_CMD_DisplayON, NULL, 0 );
}

void ILI9341_SetArea( uint16_t Xstart, uint16_t Xend, uint16_t Ystart, uint16_t Yend )
{
    uint8_t buff[4];
    
    buff[ 0 ] = Xstart >> 8;
    buff[ 1 ] = Xstart & 0xff;
    buff[ 2 ] = Xend >> 8;
    buff[ 3 ] = Xend & 0xff;
    ILI9341_LL_Write( ILI9341_CMD_ColumnAddrSet, buff, 4 );

    buff[ 0 ] = Ystart >> 8;
    buff[ 1 ] = Ystart & 0xff;
    buff[ 2 ] = Yend >> 8;
    buff[ 3 ] = Yend & 0xff;
    ILI9341_LL_Write( ILI9341_CMD_PageAddrSet, buff, 4 );
}

void ILI9341_Clear ( uint16_t RGB )
{
    uint32_t i;
    ILI9341_SetArea(0,0,239,319);
    ILI9341_LL_Write( ILI9341_CMD_MemWrite, NULL, 0 );
    
    MySPI_TransceiveStart( &ili9341_spi );
    
    for( i = 0; i < TFTLCD_X_MAX*TFTLCD_Y_MAX; i++ )
        ILI9341_LL_WriteColor( RGB );
    
    MySPI_TransceiveEnd( &ili9341_spi );
}


void ILI9341_RefreshAll(  uint16_t * RGBs )
{  
	uint32_t i;
    ILI9341_SetArea(0,0,239,319);
    ILI9341_LL_Write( ILI9341_CMD_MemWrite, NULL, 0 );
    for( i = 0; i < TFTLCD_X_MAX*TFTLCD_Y_MAX; i++ )
        ILI9341_LL_WriteColor( *RGBs++ );
    
}
