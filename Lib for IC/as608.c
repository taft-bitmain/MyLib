/*******************************************************************************
 * @file     as608.c
 * @brief
 * @version  V1.0
 * @date     2022.2.20
 * @author   RainingCats 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "as608.h"

#include "usart.h"

inline void AS608_LowLevelSend( uint8_t * dat, uint16_t len )
{
    HAL_UART_Transmit( &huart1, dat, len, 1000 );
}

inline uint8_t AS608_LowLevelRecv( uint8_t * dat, uint16_t len, uint16_t timeout )
{
    if( HAL_UART_Receive( &huart1, dat, len, timeout ) == HAL_OK )
    {
        return 1;
        
    }
    else
    {
        HAL_UART_AbortReceive(&huart1);
        return 0;        
    }
}

#include "stdlib.h"
void AS608_SendPacket ( uint8_t index, uint8_t * payload, uint16_t payload_len )
{
    uint16_t i, j, stream_len = payload_len + 11;
    uint8_t * stream = ( uint8_t * ) malloc( stream_len );
    uint16_t sum;

    i = 0;
    stream[i++] = u16_u8h( AS608_DEDAULT_HEADER );
    stream[i++] = u16_u8l( AS608_DEDAULT_HEADER );
    stream[i++] = u32_u8_4( AS608_DEDAULT_ADDR );
    stream[i++] = u32_u8_3( AS608_DEDAULT_ADDR );
    stream[i++] = u32_u8_2( AS608_DEDAULT_ADDR );
    stream[i++] = u32_u8_1( AS608_DEDAULT_ADDR );
    stream[i++] = index;
    stream[i++] = u16_u8h( payload_len + 2 );
    stream[i++] = u16_u8l( payload_len + 2 );
    for( j = 0 ; j < payload_len ; j++ )
        stream[i++] = payload[j];
    for( sum = 0,j = 6 ; j < i ; j++ )
        sum += stream[j];
    stream[i++] = u16_u8h( sum );
    stream[i++] = u16_u8l( sum );

    AS608_LowLevelSend( stream, stream_len );
    free( stream );
}

uint8_t AS608_RecvPacket ( AS608_Packet * pack, uint16_t payload_len, uint16_t timeout )
{
    uint16_t stream_len = payload_len + 11;
    static uint8_t stream[80];
    uint16_t i, sum_temp;

    if( ! AS608_LowLevelRecv( stream, stream_len, timeout ) )
        return 0;

    pack->header = u8_u16( stream[0], stream[1] );

    pack->addr = u8_u32( stream[2], stream[3], stream[4], stream[5] );

    pack->index = stream[6];

    pack->len = u8_u16( stream[7], stream[8] );
    if( pack->len != payload_len + 2 )
        return 0;

    pack->payload_len = payload_len;

    for( i = 0 ; i < pack->payload_len ; i++ )
        pack->payload[i] = stream[9+i];

    pack->sum = u8_u16( stream[stream_len-2], stream[stream_len-1] );

    for( sum_temp = 0,i = 6 ; i < stream_len - 2 ; i++ )
        sum_temp += stream[i];
    if( pack->sum == sum_temp )
        return 1;
    return 0;
}




uint8_t AS608_GetImage( void )
{
    uint8_t dat[1] = {0x01};
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 1 );
    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_GenChar( uint8_t BufferID )
{
    uint8_t dat[2] = { 0x02, BufferID };
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 2 );
    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_Match( uint16_t * Socre )
{
    uint8_t dat[1] = {0x03};
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 1 );
    if( AS608_RecvPacket( &RecvPacket, 3, 2000 ) )
    {
        *Socre = u8_u16( RecvPacket.payload[1], RecvPacket.payload[2] );
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_Search( uint8_t BufferID,uint16_t StartPage,uint16_t PageNum, AS608_SearchResult *p )
{
    uint8_t dat[6] = { 0x04, BufferID, u16_u8h( StartPage ), u16_u8l( StartPage ), u16_u8h( PageNum ), u16_u8l( PageNum ) };
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 6 );

    if( AS608_RecvPacket( &RecvPacket, 5, 2000 ) )
    {
        p->pageID = u8_u16( RecvPacket.payload[1], RecvPacket.payload[2] );
        p->score  = u8_u16( RecvPacket.payload[3], RecvPacket.payload[4] );
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_RegModel( void )
{
    uint8_t dat[1] = {0x05};
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 1 );
    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_StoreChar( uint8_t BufferID,uint16_t PageID )
{
    uint8_t dat[4] = { 0x06, BufferID, u16_u8h( PageID ), u16_u8l( PageID ) };
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 4 );

    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}

uint8_t AS608_LoadChar( uint8_t BufferID,uint16_t PageID )
{
    uint8_t dat[4] = { 0x07, BufferID, u16_u8h( PageID ), u16_u8l( PageID ) };
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 4 );

    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_DeletChar( uint16_t PageID,uint16_t N )
{
    uint8_t dat[5] = { 0x0C, u16_u8h( PageID ), u16_u8l( PageID ), u16_u8h( N ), u16_u8l( N ) };
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 5 );

    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_Empty( void )
{
    uint8_t dat[1] = {0x0D};
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 1 );

    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_WriteReg( uint8_t RegNum,uint8_t RegData )
{
    uint8_t dat[3] = {0x0D, RegNum, RegData };
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 3 );

    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_ReadSysPara( AS608_SysPara *p )
{
    uint8_t dat[1] = {0x0F};
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 1 );

    if( AS608_RecvPacket( &RecvPacket, 17, 2000 ) )
    {
        p->StateReg      = u8_u16( RecvPacket.payload[1], RecvPacket.payload[2] );
        p->SensorType    = u8_u16( RecvPacket.payload[3], RecvPacket.payload[4] );
        p->DatabaseMax   = u8_u16( RecvPacket.payload[5], RecvPacket.payload[6] );
        p->SecurityLevel = u8_u16( RecvPacket.payload[7], RecvPacket.payload[8] );
        p->DeviceAddr    = u8_u32( RecvPacket.payload[9], RecvPacket.payload[10], RecvPacket.payload[11], RecvPacket.payload[12] );
        p->PacketSize    = u8_u16( RecvPacket.payload[13], RecvPacket.payload[14] );
        p->N_bps         = u8_u16( RecvPacket.payload[15], RecvPacket.payload[16] );
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_SetChipAddr( uint32_t addr )
{
    uint8_t dat[5] = { 0x015, u32_u8_4( addr ), u32_u8_3( addr ), u32_u8_2( addr ), u32_u8_1( addr ) };
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 5 );

    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_WriteNotepad( uint8_t NotePageNum,uint8_t * Byte32 )
{
    uint8_t dat[34] = { 0x18, NotePageNum,};

    AS608_Packet RecvPacket;

    for( uint8_t i = 0 ; i < 32 ; i++ )
        dat[i+2] = Byte32[i];

    AS608_SendPacket( AS608_PKT_CMD, dat, 34 );

    if( AS608_RecvPacket( &RecvPacket, 1, 2000 ) )
    {
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_ReadNotepad( uint8_t NotePageNum, uint8_t * Byte32 )
{
    uint8_t dat[2] = { 0x19, NotePageNum };
    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 2 );

    if( AS608_RecvPacket( &RecvPacket, 33, 2000 ) )
    {
        Byte32 = RecvPacket.payload + 1;
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_HighSpeedSearch( uint8_t BufferID,uint16_t StartPage,uint16_t PageNum,AS608_SearchResult *p )
{
    uint8_t dat[6] = { 0x1B, BufferID, u16_u8h( StartPage ), u16_u8l( StartPage ), u16_u8h( PageNum ), u16_u8l( PageNum ) };

    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 6 );

    if( AS608_RecvPacket( &RecvPacket, 5, 2000 ) )
    {
        p->pageID = u8_u16( RecvPacket.payload[1], RecvPacket.payload[2] );
        p->score  = u8_u16( RecvPacket.payload[3], RecvPacket.payload[4] );
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}


uint8_t AS608_ValidTempleteNum( uint16_t *ValidN )
{
    uint8_t dat[1] = { 0x1D };

    AS608_Packet RecvPacket;

    AS608_SendPacket( AS608_PKT_CMD, dat, 1 );

    if( AS608_RecvPacket( &RecvPacket, 3, 2000 ) )
    {
        *ValidN = u8_u16( RecvPacket.payload[1], RecvPacket.payload[2] );
        return RecvPacket.payload[0];
    }
    else
    {
        return 0xFF;
    }
}

//#include "base.h"

uint8_t AS608_InputFingerprint ( uint16_t ID )
{
    uint8_t i, ret, step;
	uint16_t socre; 
    step = 0;
	while(1)
	{
		switch (step)
		{
			case 0:
                //myprintf( "InputFingerprint step 0\r\n" );
                do
                {
                    ret = AS608_GetImage();
                    HAL_Delay( 10 );
                }
                while( ret != 0x00 );
                
                ret = AS608_GenChar( AS608_CHARBUFFER1 );                
                if( ret == 0x00 )
                {
                    step = 1;
                }
                break;
			
			case 1:
                //myprintf( "InputFingerprint step 1\r\n" );
                do
                {
                    ret = AS608_GetImage();
                    HAL_Delay( 10 );
                }
                while( ret != 0x00 );
                
                ret = AS608_GenChar( AS608_CHARBUFFER2 );                
                if( ret == 0x00 )
                {
                    step = 2;
                }
                break;

			case 2:
                //myprintf( "InputFingerprint step 2\r\n" );
				ret = AS608_Match( &socre );                
                if( ret == 0x00 )
                {
                    step = 3;
                }
                else
                {
                    step = 0;
                }
                break;

			case 3:
                //myprintf( "InputFingerprint step 3\r\n" );
                ret = AS608_RegModel();                
                if( ret == 0x00 )
                {
                    step = 4;
                }
                else
                {
                    step = 0;
                }
                break;
				
			case 4:   
                //myprintf( "InputFingerprint step 4\r\n" );                
                ret = AS608_StoreChar( AS608_CHARBUFFER2, ID );
            
				if( ret == 0x00 )
                {
                    return 0x00;
                }
                break;
        }
	}
}

uint8_t AS608_CheckFingerprint ( uint16_t * ID  )
{
    AS608_SearchResult result;
	uint8_t ret;
    
    do
    {
        ret = AS608_GetImage();
        HAL_Delay( 10 );
    }
    while( ret != 0x00 );
    
    ret = AS608_GenChar( AS608_CHARBUFFER2 );
    if( ret != 0x00 )
    {
        return 0x01;
    }
    
    ret = AS608_GenChar( AS608_CHARBUFFER1 );
    if( ret != 0x00 )
    {
        return 0x02;
    }
    
    ret = AS608_HighSpeedSearch( AS608_CHARBUFFER1, 0 , 300, &result );
    if( ret != 0x00 )
    {
        return 0x03;
    }
    
    *ID = result.pageID;
    
    return 0x00;
}
