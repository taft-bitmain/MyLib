/*******************************************************************************
 * @file     rc522.c
 * @brief    Simple funtions to drive MFRC522
 * @version  V1.0
 * @date     2022.1.10
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "rc522.h"

/*********************** RC522 Interface **********************/

inline void RC522_Delay( uint32_t count ) // about 1 us
{
    uint8_t i = 48;
    while(count--)
        while(i--);
}

inline void RC522_PinRst( uint8_t state )
{
    WritePin(GPIOB,GPIO_PIN_12,state);
}

static MySPI hspi;

void RC522_InterfaceInit( void )
{
    hspi.SCK_Port  = GPIOD;
    hspi.SCK_Bit  = GPIO_PIN_12;
    hspi.CS_Port   = GPIOD;
    hspi.CS_Bit   = GPIO_PIN_14;
    hspi.MOSI_Port = GPIOD;
    hspi.MOSI_Bit = GPIO_PIN_10;
    hspi.MISO_Port = GPIOD;
    hspi.MISO_Bit = GPIO_PIN_8;
    hspi.Speed = 5;
    hspi.CPOL = 0;
    hspi.CPHA = 0;
    MySPI_IO_Init(&hspi);
}

inline uint8_t RC522_RegRead( uint8_t reg )
{
    return MySPI_ReadReg(&hspi, ( reg << 1 ) | 0x80);
}


inline void RC522_RegWrite( uint8_t reg, uint8_t value )
{
    MySPI_WriteReg(&hspi,( reg << 1 ), value);
}

inline void RC522_SetBitMask ( uint8_t ucReg, uint8_t mask )
{
    RC522_RegWrite ( ucReg, RC522_RegRead ( ucReg ) | mask );
}

inline void RC522_ClearBitMask ( uint8_t ucReg, uint8_t mask )
{
    RC522_RegWrite ( ucReg, RC522_RegRead ( ucReg ) & ( ~ mask) );
}

/********************** RC522 Main Functions  ********************/

void RC522_Reset ( void )
{
    uint8_t count;
    RC522_PinRst(1);
    RC522_Delay(5);
    RC522_PinRst(0);
    RC522_Delay(5);
    RC522_PinRst(1);
    RC522_Delay(5);
    RC522_RegWrite ( CommandReg, CMD_SOFTRESET );
    count = 10;
    do {
        RC522_Delay(10);
        count--;
    } while ( RC522_RegRead ( CommandReg ) & 0x10  && count );
}

inline void RC522_AntennaOn ( void )
{
    RC522_SetBitMask( TxControlReg, 0x03);
}

inline void RC522_AntennaOff ( void )
{
    RC522_ClearBitMask( TxControlReg, 0x03 );
}

inline void RC522_SoftPowerDown ( void )
{
    RC522_SetBitMask( CommandReg, 0x10);
}

void RC522_SoftPowerUp ( void )
{
    RC522_ClearBitMask( CommandReg, 0x10 );
    uint8_t count;
    count = 10;
    do {
        RC522_Delay(10);
        count--;
    } while ( RC522_RegRead ( CommandReg ) & 0x10  && count );
}

void RC522_ConfigISOType ( uint8_t ucType )
{
    //ISO14443_A
    if ( ucType == 'A')
    {
        RC522_RegWrite ( RxSelReg, 0x86 );

        RC522_RegWrite ( TModeReg, 0x80 );          // 6.78M / 3390(0x0D3E) = 2K
        RC522_RegWrite ( TPrescalerReg, 0xA9 );
        RC522_RegWrite ( TReloadRegH, 0x03 );       // Timer Reload = 0x03E8 = 1000
        RC522_RegWrite ( TReloadRegL, 0xE8 );

        RC522_RegWrite ( TxASKReg, 0x40 );		    // force 100% ASK modulation
        RC522_RegWrite ( ModeReg, 0x3D );           // CRC preset 0x6363

        RC522_RegWrite ( RFCfgReg, 0x7F );          // 48dB Rx Gain
    }
}


void RC522_Init ( void )
{
    RC522_InterfaceInit();

    RC522_Reset();

    RC522_ConfigISOType( 'A' );

    RC522_AntennaOn();
}

void RC522_CalulateCRC ( uint8_t * pdatain, uint8_t bytein, uint8_t * pdataout )
{
    uint8_t i, ret;

    RC522_ClearBitMask(DivIrqReg,0x04);
    RC522_RegWrite(CommandReg,CMD_IDLE);
    RC522_SetBitMask(FIFOLevelReg,0x80);

    for ( i = 0; i < bytein; i ++)
        RC522_RegWrite ( FIFODataReg, pdatain [ i ] );

    RC522_RegWrite ( CommandReg, CMD_CALCCRC );
    i = 0xFF;
    do {
        ret = RC522_RegRead ( DivIrqReg );
        i --;
    }
    while ( ( i != 0 ) && ! ( ret & 0x04 ) );

    pdataout [ 0 ] = RC522_RegRead ( CRCResultRegL );
    pdataout [ 1 ] = RC522_RegRead ( CRCResultRegM );
}

uint8_t RC522_Communication( uint8_t cmd, uint8_t * pdatain, uint8_t bytein, uint8_t * pdataout, uint32_t * bitout )
{
    uint8_t irqen,waitfor;
    uint8_t lastbits,fifolen;
    uint32_t i;

    switch ( cmd )
    {
    case CMD_MFAUTHENT:	    // Mifare Authenticate
        irqen   = 0x12;		// Enable Idle and Err interrupt
        waitfor = 0x10;		// Wait IdleIRq
        break;

    case CMD_TRANSCEIVE:    // Tansmit and recieve
        irqen   = 0x77;		// Enable all interrupt(Tx,Rx,Idle,HiAlert,LoAlert,Err,Timer)
        waitfor = 0x30;		// Wait RxIRq
        break;

    default:
        break;
    }

    RC522_RegWrite ( ComIEnReg, irqen | 0x80 );		        // Pin IRQ signal is inverted with IRq in Status1Reg
    RC522_ClearBitMask ( ComIrqReg, 0x80 );			        // When IRq occured, the related bit will set 1
    RC522_RegWrite ( CommandReg, CMD_IDLE );		        // Write IDLE command

    RC522_SetBitMask ( FIFOLevelReg, 0x80 );			    // Flush FIFO
    for ( i = 0; i < bytein; i ++ )                         // Write data in FIFO
        RC522_RegWrite ( FIFODataReg, pdatain [ i ] );

    RC522_RegWrite ( CommandReg, cmd );				        // Write command
    if ( cmd == CMD_TRANSCEIVE )                            // Start transmission
        RC522_SetBitMask(BitFramingReg,0x80);

    i = 1000;                                               // Make sure max time to operate is 25 ms
    while ( i-- )
    {
        uint8_t ret;
        ret = RC522_RegRead ( ComIrqReg );
        if( ret & waitfor )
            break;
        if( ret & 0x01 )
            return STATUS_TIMEOUT;
    }

    if ( i == 0 )
        return STATUS_TIMEOUT;
    if ( RC522_RegRead ( ErrorReg ) & 0x1B )
        return STATUS_ERR;

    if ( cmd == CMD_TRANSCEIVE )
    {
        fifolen = RC522_RegRead ( FIFOLevelReg );		    // Read byte length in FIFO

        lastbits = RC522_RegRead ( ControlReg ) & 0x07;	    // Get the total number of bits
        if ( lastbits )
            * bitout = ( fifolen - 1 ) * 8 + lastbits;
        else
            * bitout = fifolen * 8;

        if ( fifolen == 0 )
            fifolen = 1;
        if ( fifolen > MAXRLEN )
            fifolen = MAXRLEN;

        for ( i = 0; i < fifolen; i ++ )
            pdataout [ i ] = RC522_RegRead ( FIFODataReg );
    }

    //RC522_SetBitMask ( ControlReg, 0x80 );                  // Stop timer now
    //RC522_RegWrite ( CommandReg, CMD_IDLE );

    return STATUS_OK;
}

uint8_t RC522_Halt( void )
{
    uint8_t buf [ MAXRLEN ];
    uint32_t  len;

    buf [ 0 ] = PICC_HALT;
    buf [ 1 ] = 0;

    RC522_CalulateCRC ( buf, 2, & buf [2] );
    RC522_Communication ( CMD_TRANSCEIVE, buf, 4, buf, &len );

    return STATUS_OK;
}


uint8_t RC522_Request( uint8_t reqcode, uint8_t * ptagtype )
{
    uint8_t status;
    uint8_t buf [ MAXRLEN ];
    uint32_t bitlen;

    RC522_ClearBitMask ( Status2Reg, 0x08 );	// Clear MIFARE Crypto state
    RC522_RegWrite ( BitFramingReg, 0x07 );	    // Sent the 7 bits of last byte
    RC522_SetBitMask ( TxControlReg, 0x03 );	// Send 13.56Mhz signal on pin TX1,TX2

    buf [ 0 ] = reqcode;

    status = RC522_Communication ( CMD_TRANSCEIVE,	buf, 1, buf, & bitlen );

    if ( ( status == STATUS_OK ) && ( bitlen == 0x10 ) )	// return tag type
    {
        ptagtype [ 0 ] = buf [ 0 ];
        ptagtype [ 1 ] = buf [ 1 ];
    }
    else
        status = STATUS_ERR;

    return status;
}

uint8_t RC522_Anticoll ( uint8_t * puid )
{
    uint8_t status;
    uint8_t uc, uidcheck = 0;
    uint8_t buf [ MAXRLEN ];
    uint32_t ulLen;

    RC522_ClearBitMask ( Status2Reg, 0x08 );    // Clear MIFARE Crypto state
    RC522_RegWrite ( BitFramingReg, 0x00);      // Clear transmission
    RC522_ClearBitMask ( CollReg, 0x80 );       // Bits received after collision are cleared
    buf [ 0 ] = PICC_ANTICOLL1;
    buf [ 1 ] = 0x20;

    status = RC522_Communication ( CMD_TRANSCEIVE, buf, 2, buf, & ulLen);

    if ( status == STATUS_OK)
    {
        for ( uc = 0; uc < 4; uc ++ )
        {
            * ( puid + uc )  = buf [ uc ];
            uidcheck ^= buf [ uc ];
        }

        if ( uidcheck != buf [ uc ] )
            status = STATUS_ERR;
    }

    RC522_SetBitMask ( CollReg, 0x80 );

    return status;
}

uint8_t RC522_Select ( uint8_t * puid )
{
    uint8_t status,i;
    uint8_t buf [ MAXRLEN ];
    uint32_t  bitlen;

    buf [ 0 ] = PICC_ANTICOLL1;
    buf [ 1 ] = 0x70;
    buf [ 6 ] = 0;

    for ( i = 0; i < 4; i ++ )
    {
        buf [ i + 2 ] = * ( puid + i );
        buf [ 6 ] ^= * ( puid + i );
    }
    RC522_ClearBitMask (CollReg, 0x80);
    RC522_CalulateCRC ( buf, 7, & buf [ 7 ] );
    RC522_ClearBitMask ( Status2Reg, 0x08 );
    status = RC522_Communication ( CMD_TRANSCEIVE, buf, 9, buf, & bitlen );

    if ( ( status == STATUS_OK ) && ( bitlen == 0x18 ) )
        status = STATUS_OK;
    else
        status = STATUS_ERR;

    return status;
}


uint8_t RC522_AuthState ( uint8_t authmode, uint8_t blockaddr, uint8_t * pkey, uint8_t * puid )
{
    uint8_t status,i;
    uint8_t buf [ MAXRLEN ];
    uint32_t bitlen;

    buf [ 0 ] = authmode;
    buf [ 1 ] = blockaddr;

    for ( i = 0; i < 6; i ++ )
        buf [ i + 2 ] = * ( pkey + i );

    for ( i = 0; i< 6; i ++ )
        buf [ i + 8 ] = * ( puid + i );

    status = RC522_Communication ( CMD_MFAUTHENT, buf, 12, buf, &bitlen );

    if ( ( status != STATUS_OK ) || ( ! ( RC522_RegRead ( Status2Reg ) & 0x08 ) ) ) {
        status = STATUS_ERR;
    }
    return status;
}

uint8_t RC522_Write ( uint8_t blockaddr, uint8_t * pdata )
{
    uint8_t status,i;
    uint8_t buf [ MAXRLEN ];
    uint32_t bitlen;

    buf [ 0 ] = PICC_WRITE;
    buf [ 1 ] = blockaddr;

    RC522_CalulateCRC ( buf, 2, & buf [ 2 ] );

    status = RC522_Communication ( CMD_TRANSCEIVE, buf, 4, buf, & bitlen );

    if ( ( status != STATUS_OK ) || ( bitlen != 4 ) || ( ( buf [ 0 ] & 0x0F ) != 0x0A ) )
        status = STATUS_ERR;

    if ( status == STATUS_OK )
    {
        for ( i = 0; i < 16; i ++ )
            buf [ i ] = * ( pdata + i );

        RC522_CalulateCRC ( buf, 16, & buf [ 16 ] );

        status = RC522_Communication ( CMD_TRANSCEIVE, buf, 18, buf, & bitlen );

        if ( ( status != STATUS_OK ) || ( bitlen != 4 ) || ( ( buf [ 0 ] & 0x0F ) != 0x0A ) )
            status = STATUS_ERR;

    }

    return status;

}

uint8_t RC522_Read ( uint8_t blockaddr, uint8_t * pdata )
{
    uint8_t status,i;
    uint8_t buf [ MAXRLEN ];
    uint32_t bitlen;

    buf [ 0 ] = PICC_READ;
    buf [ 1 ] = blockaddr;

    RC522_CalulateCRC ( buf, 2, & buf [ 2 ] );

    status = RC522_Communication ( CMD_TRANSCEIVE, buf, 4, buf, & bitlen );

    if ( ( status == STATUS_OK ) && ( bitlen == 0x90 ) )
    {
        for ( i = 0; i < 16; i ++ )
            pdata [ i ] = buf [ i ];
    }

    else
        status = STATUS_ERR;

    return status;

}


