/*******************************************************************************
 * @file     esp8266.c
 * @brief    Connect esp8266 module with AT commands via UART.
 * @version  V1.2
 * @date     2021.12.2
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 *******************************************************************************/

#include "esp8266.h"

#include "string.h"
#include <stdio.h>

uint8_t   transparent;
uint8_t * rxbuffer_now;
volatile uint16_t  rx_len_now;


inline void esp8266_ll_send ( uint8_t * txbuffer, uint16_t tx_len )
{
    if( txbuffer && tx_len )
    {
        //myprintf( "\tll send:%s\r\n", ( char * )txbuffer );
        HAL_UART_Transmit_DMA( &ESP8266_LL_HUART, txbuffer, tx_len );
    }
}

inline void esp8266_ll_recv( uint8_t * rxbuffer, uint16_t rx_len )
{
    if( rxbuffer )
    {
        HAL_UART_AbortReceive( &ESP8266_LL_HUART );
        HAL_UARTEx_ReceiveToIdle_DMA( &ESP8266_LL_HUART, rxbuffer, rx_len );
        rxbuffer_now = rxbuffer;
    }
}

inline void esp8266_ll_recv_callback ( UART_HandleTypeDef * huart,
                                       uint16_t Size )
{
    rx_len_now = Size;
    rxbuffer_now[rx_len_now] = 0;
    // todo
}


uint8_t esp8266_cmd_wait ( esp8266 * esp, uint32_t timeout )
{
    uint32_t tick_now;

    if( timeout )
    {
        rx_len_now = 0;
        esp8266_ll_recv( esp->rxbuffer, ESP8266_BUFLEN2 );
    }

    esp8266_ll_send( esp->txbuffer, esp->tx_len );
    esp->tx_len = 0;

    tick_now = HAL_GetTick();

    while( HAL_GetTick() < tick_now + timeout )
    {
        if( rx_len_now )
        {
            //myprintf( "\tll recv:%s\r\n", ( char * )esp->rxbuffer );
            
            if ( strstr( ( char * )esp->rxbuffer, "OK" ) != NULL )
            {
                return 1;
            }

            else if( strstr( ( char * )esp->rxbuffer, "ERROR" ) != NULL )
            {
                return 2;
            }

            else if( strstr( ( char * )esp->rxbuffer, "busy" ) != NULL )
            {
                return 3;
            }

            return 4;
        }
    }

    //HAL_UART_AbortReceive( &ESP8266_LL_HUART );
    return 0;
}


uint8_t esp8266_reset( esp8266 * esp )
{
#if (ESP_USE_PINRST == 1)
    {
        HAL_GPIO_WritePin( GPIOA, GPIO_PIN_1, GPIO_PIN_RESET );
        HAL_Delay( 200 );
        HAL_GPIO_WritePin( GPIOA, GPIO_PIN_1, GPIO_PIN_SET );
        HAL_Delay( 500 );
        return 1;
    }
#else
    esp->tx_len = sprintf( ( char * )esp->txbuffer, "AT+RST\r\n" );
    esp8266_cmd_wait( esp, 0 );
    esp8266_ll_delayms( 1000 );
    return 1;
#endif
}

uint8_t esp8266_at( esp8266 * esp )
{

    esp->tx_len = sprintf( ( char * )esp->txbuffer, "AT\r\n" );

    if( esp8266_cmd_wait( esp, 500 ) == 1 )
        return 1;

    else
    {
        esp8266_debug( "Log:ESP8266 AT test fail\r\n" );
        return 0;
    }
}

uint8_t esp8266_setmode( esp8266 * esp )
{
    esp->tx_len = sprintf( ( char * )esp->txbuffer, "AT+CWMODE=%d\r\n", esp->mode );

    if( esp8266_cmd_wait( esp, 500 ) == 1 )
        return 1;

    else
    {
        esp8266_debug( "Log:ESP8266 set mode fail\r\n" );
        return 0;
    }
}

uint8_t esp8266_join_ap( esp8266 * esp )
{
    uint8_t i;

    if( esp->mode == 1 || esp->mode == 3 )
    {
        esp->tx_len = sprintf( ( char * )esp->txbuffer, "AT+CWJAP?\r\n" );
        esp8266_cmd_wait( esp, 500 );

        if( strstr( ( char * )esp->rxbuffer, ( char * )esp->ssid ) != NULL )
        {
            esp8266_debug( "Log:ESP8266 has been joined AP\r\n" );
            esp->connected = 1;
            return 1;
        }

        esp8266_debug( "Log:ESP8266 join AP...\r\n" );

        esp->connected = 0;

        esp->tx_len = sprintf( ( char * )esp->txbuffer, "AT+CWJAP=\"%s\",\"%s\"\r\n",
                               esp->ssid, esp->pswd );

        for( i = 0; i < 5 ; i++ )
        {
            if( esp8266_cmd_wait( esp, 5000 ) == 0 )
                return 0;

            if( strstr( ( char * )esp->rxbuffer, "OK" ) )
            {
                esp->connected = 1;
                esp8266_debug( "Log:ESP8266 Join AP success\r\n" );
                return 1;
            }

        }

        esp8266_debug( "Log:ESP8266 Join AP fail\r\n" );
        return 0;
    }

    else
    {
        return 1;
    }
}

uint8_t esp8266_set_ap( esp8266 * esp )
{
    if( esp->mode == 2 || esp->mode == 3 )
    {
        esp->tx_len = sprintf( ( char * )esp->txbuffer,
                               "AT+CWSAP=\"%s\",\"%s\",1,%d\r\n",
                                esp->ssid_ap, esp->pswd_ap, esp->mode_ap );

        if( esp8266_cmd_wait( esp, 2000 ) == 1 )
        {
            esp8266_debug( "Log:ESP8266 Set AP success\r\n" );
            return 1;
        }

        else
        {
            esp8266_debug( "Log:ESP8266 Set AP fail\r\n" );
            return 0;
        }
    }

    else
    {
        return 1;
    }
}

uint8_t esp8266_at_echo( esp8266 * esp, uint8_t echo )
{
    esp->tx_len = sprintf( ( char * )esp->txbuffer, "ATE%d\r\n",
                           echo );

    if( esp8266_cmd_wait( esp, 1000 ) == 1 )
    {
        return 1;
    }

    else
    {
        esp8266_debug( "Log:ESP8266 set at echo fail\r\n" );
        return 0;
    }
}

uint8_t esp8266_autocon( esp8266 * esp )
{
    esp->tx_len = sprintf( ( char * )esp->txbuffer, "AT+CWAUTOCONN=%d\r\n",
                           esp->auto_connect );

    if( esp8266_cmd_wait( esp, 1000 ) == 1 )
    {
        return 1;
    }

    else
    {
        esp8266_debug( "Log:ESP8266 set auto connect fail\r\n" );
        return 0;
    }
}

uint8_t esp8266_cipmode( esp8266 * esp )
{
    esp->tx_len = sprintf( ( char * )esp->txbuffer, "AT+CIPMODE=%d\r\n",
                           esp->cipmode );

    if( esp8266_cmd_wait( esp, 500 ) == 1 )
    {
        return 1;
    }

    else
    {
        esp8266_debug( "Log:ESP8266 set cipmode fail\r\n" );
        return 0;
    }
}

uint8_t esp8266_cipmux( esp8266 * esp )
{
    esp->tx_len = sprintf( ( char * )esp->txbuffer, "AT+CIPMUX=%d\r\n",
                           esp->cipmux );

    if( esp8266_cmd_wait( esp, 500 ) == 1 )
    {
        return 1;
    }

    else
    {
        esp8266_debug( "Log:ESP8266 set cipmux fail\r\n" );
        return 0;
    }
}


uint8_t esp8266_init ( esp8266 * esp )
{
    // register callback function for HAL_UARTEx_ReceiveToIdle_DMA
    HAL_UART_RegisterRxEventCallback( &ESP8266_LL_HUART, esp8266_ll_recv_callback );

    if( !esp8266_reset( esp ) )
        return 0;

    if( !esp8266_at_echo( esp, 0 ) )
        return 0;

    if( !esp8266_at( esp ) )
        return 0;

    return 1;
}

uint8_t esp8266_setup ( esp8266 * esp, uint8_t mode, uint8_t mode_ap,
                        const char * ssid, const char * pswd, const char * ssid_ap,
                        const char * pswd_ap, uint8_t auto_connect, uint8_t cipmode, uint8_t cipmux )
{
    if( mode > 3 || mode == 0 )
        return 0;

    esp->mode = mode;

    if( mode_ap > 4 || mode_ap == 1 )
        return 0;

    esp->mode_ap = mode_ap;

    strcpy( ( char * )esp->ssid, ( char * )ssid );
    strcpy( ( char * )esp->pswd, ( char * )pswd );
    strcpy( ( char * )esp->ssid_ap, ( char * )ssid_ap );
    strcpy( ( char * )esp->pswd_ap, ( char * )pswd_ap );

    esp->auto_connect = auto_connect ? 1 : 0;

    esp->cipmux = cipmux ? 1 : 0;
    
    esp->cipmode = cipmux ? 0 : (cipmode ? 1 : 0);

    if( !esp8266_autocon( esp ) )
        return 0;

    if( !esp8266_setmode( esp ) )
        return 0;

    if( !esp8266_join_ap( esp ) )
        return 0;

    if( !esp8266_set_ap( esp ) )
        return 0;

    if( !esp8266_cipmode( esp ) )
        return 0;

    if( !esp8266_cipmux( esp ) )
        return 0;

    return 1;
}

uint8_t esp8266_link_setup ( esp8266 * esp, esp8266_link * link, uint8_t id,
                             uint8_t netpro, const char * ip, uint16_t port,	uint16_t localport,
                             uint16_t keepalive )
{
    if( esp == NULL || link == NULL )
        return 0;

    if( id > 4 )
        return 0;

    if( netpro > 3 && netpro == 0 )
        return 0;

    keepalive = ( keepalive > 7200 ) ? 7200 : keepalive;

    link->id = id;
    link->netpro = netpro;
    strcpy( ( char * )link->ip, ip );
    link->linked = 0;
    link->port = port;
    link->localport = localport;
    link->keepalive = keepalive;
    link->tx_len = 0;
    link->rx_len = 0;

    switch( id )
    {
        case 0:
            esp->link0 = link;
            break;

        case 1:
            esp->link1 = link;
            break;

        case 2:
            esp->link2 = link;
            break;

        case 3:
            esp->link3 = link;
            break;

        case 4:
            esp->link4 = link;
            break;
    }

    return 1;
}

uint8_t esp8266_link_connect ( esp8266 * esp, esp8266_link * link )
{
    char * p_txbuffer = ( char * )esp->txbuffer;

    if( esp == NULL || link == NULL )
    {
        esp8266_debug( "Log:ESP8266 link connect fail for wrong parameters\r\n" );
        return 0;
    }

    if( !esp->connected )
    {
        esp8266_debug( "Log:ESP8266 link connect fail as it is not connect to AP\r\n" );
        return 0;
    }

    p_txbuffer += sprintf( p_txbuffer, "AT+CIPSTART=" );

    if( esp->cipmux )
        p_txbuffer += sprintf( p_txbuffer, "%d,", link->id );


    switch( link->netpro )
    {
        case 1:
            p_txbuffer += sprintf( p_txbuffer, "\"TCP\",\"%s\",%d,%d\r\n", link->ip,
                                   link->port, link->keepalive );
            break;

        case 2:
            p_txbuffer += sprintf( p_txbuffer, "\"UDP\",\"%s\",%d,%d,0\r\n", link->ip,
                                   link->port, link->localport );
            break;

        case 3:
            p_txbuffer += sprintf( p_txbuffer, "\"SSL\",\"%s\",%d,%d\r\n", link->ip,
                                   link->port, link->keepalive );
            break;

        default :
            return 0;
    }

    esp->tx_len = ( uint8_t * )p_txbuffer - esp->txbuffer;
    esp8266_cmd_wait( esp, 1000 );

    if( strstr( ( char * )esp->rxbuffer, "CONNECT" )
            || strstr( ( char * )esp->rxbuffer, "ALREADY CONNECTED" ) )
    {
        link->linked = 1;
        return 1;
    }

    else
    {
        esp8266_debug( "Log:ESP8266 link connect fail\r\n" );
        return 0;
    }
}

uint8_t esp8266_link_close ( esp8266 * esp, esp8266_link * link )
{
    uint8_t ret;
    char * p_txbuffer = ( char * )esp->txbuffer;

    if( link->linked )
    {
        esp8266_ll_delayms( 500 );
        if( transparent )
        {
            esp->tx_len = sprintf( ( char * )esp->txbuffer, "+++" );
            esp8266_cmd_wait( esp, 0 );
            esp8266_ll_delayms( 1100 );
        }

        p_txbuffer += sprintf( ( char * )p_txbuffer, "AT+CIPCLOSE" );

        if( esp->cipmux )
            p_txbuffer += sprintf( ( char * )p_txbuffer, "=%d", link->id );

        p_txbuffer += sprintf( ( char * )p_txbuffer, "\r\n" );

        esp->tx_len = ( uint8_t * )( p_txbuffer ) - esp->txbuffer;

        ret = esp8266_cmd_wait( esp, 500 );
        
        if( ret == 1 || strstr( ( char * )esp->rxbuffer, "CLOSED" ) )
        {
            return 1;
        }

        else
        {
            esp8266_debug( "Log:ESP8266 link close failed\r\n" );
            return 0;
        }
    }

    return 1;
}

uint8_t esp8266_link_send ( esp8266 * esp, esp8266_link * link )
{
    uint8_t i;
    char * p_txbuffer = ( char * )esp->txbuffer;

    if( link == NULL )
        return 0;

    if( link->tx_len == 0 )
        return 0;

    if( link->linked == 0 )
    {
        esp8266_debug( "Log:ESP8266 link send fail for no connection \r\n" );
        return 0;
    }

    if( esp->cipmode == 0 )
    {
        p_txbuffer += sprintf( p_txbuffer, "AT+CIPSEND=" );

        if( esp->cipmux )
            p_txbuffer += sprintf( p_txbuffer, "%d,", link->id );

        p_txbuffer += sprintf( p_txbuffer, "%d\r\n", link->tx_len );

        esp->tx_len = ( uint8_t * )( p_txbuffer ) - esp->txbuffer;

        if( esp8266_cmd_wait( esp, 500 ) == 1 )
        {
            esp8266_ll_send( link->txbuffer, link->tx_len );
            link->tx_len = 0;
            
            for( i = 0; i < 4; i++ )
            {
                if( esp8266_cmd_wait( esp, 500 ) == 1 )
                  break;
            }
        }

        else
        {
            esp8266_debug( "Log:ESP8266 link send start fail\r\n" );
        }

        return 0;
    }

    else
    {
        if( !transparent )
        {
            p_txbuffer += sprintf( p_txbuffer, "AT+CIPSEND\r\n" );
            esp->tx_len = ( uint8_t * )p_txbuffer - esp->txbuffer;

            if( esp8266_cmd_wait( esp, 500 ) == 1 )
            {
                transparent = 1;
                esp8266_ll_send( link->txbuffer, link->tx_len );
                return 1;
            }

            esp8266_debug( "Log:ESP8266 link send fail\r\n" );
            return 0;
        }

        else
        {
            esp8266_ll_send( link->txbuffer, link->tx_len );
            return 1;
        }
    }
}

uint8_t esp8266_link_recv ( esp8266 * esp, esp8266_link * link )
{
    static uint8_t state;
    if( esp == NULL || link == NULL )
        return 0;

    if( link->linked == 0 )
    {
        esp8266_debug( "Log:ESP8266 link recv fail for no connection \r\n" );
        return 0;
    }
    
    switch( state )
    {
        case 0:
            
            rx_len_now = 0;
            esp8266_ll_recv( link->rxbuffer + link->rx_len, ESP8266_BUFLEN4 - link->rx_len );
            state = 1;
            break;
        case 1:
            
            if( rx_len_now )
            {
//                if( esp->cipmode == 0 )
//                {
//                    if( esp->cipmux )
//                }
//                else
//                {
//                    
//                }
                link->rx_len += rx_len_now;
                state = 0;
            }
            break;
        
        default:
            state = 0;
        break;
    }
    return 1;
}


