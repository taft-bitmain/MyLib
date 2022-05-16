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
#include <stdlib.h>


uint8_t _transparent_state;
esp_buffer * _rxbuffer_now;

esp_buffer * esp_buffer_static( uint8_t * mem, uint16_t capicity )
{
    esp_buffer * mybuf = ( esp_buffer * )malloc( sizeof(esp_buffer) );
    
    if( mybuf )
    {
        mybuf->type = 1;
        mybuf->data = mem;
        mybuf->cap = capicity;
        mybuf->size = 0;
    }
    
    return mybuf;
}

esp_buffer * esp_buffer_dynamic( uint16_t capicity )
{
    esp_buffer * mybuf = ( esp_buffer * )malloc( sizeof(esp_buffer) );
    
    if( mybuf )
    {
        mybuf->type = 1;
        mybuf->data = ( uint8_t * )malloc( capicity );
        if( mybuf->data == NULL )
        {
            free( mybuf );
            return NULL;
        }
        mybuf->cap = capicity;
        mybuf->size = 0;
    }
    
    return mybuf;
}

void esp_buffer_free( esp_buffer * buffer )
{
    if( buffer )
    {
        if( buffer->type == 2 )
        {
            free( buffer->data );
        }
        free( buffer );
    }
}

void esp_ll_recv_callback ( UART_HandleTypeDef * huart, uint16_t Size )
{
    _rxbuffer_now->size = Size;
    _rxbuffer_now->data[_rxbuffer_now->size] = 0;
    //esp_debug("ESP8266 : log : get packet\n");
}

void esp_send_buffer(esp_buffer * txbuf)
{
    if( txbuf )
    {
        if ( txbuf->data )
        {
            //esp_debug("ESP8266 : log : send\n");
            esp_ll_send( txbuf->data, txbuf->size );
            txbuf->size = 0;
        }
    }
}

void esp_recv_buffer(esp_buffer * rxbuf)
{
    if( rxbuf )
    {
        if( rxbuf->data != NULL )
        {
            rxbuf->size = 0;
            _rxbuffer_now = rxbuf;
            //esp_debug("ESP8266 : log : recv\n");
            esp_ll_recv( rxbuf->data, rxbuf->cap );
        }
    }
}


uint8_t esp_cmd_wait ( esp_buffer * txbuf, esp_buffer * rxbuf, uint32_t timeout )
{
    if( rxbuf )
    {
        //rxbuf->size = 0;
        esp_recv_buffer(rxbuf);
    }
    if( txbuf )
    {
        esp_send_buffer(txbuf);
        //txbuf->size = 0;
    }
    if( timeout )
    {
        uint32_t tick_now = HAL_GetTick();

        while( HAL_GetTick() < tick_now + timeout )
        {
            if( rxbuf )
            {
                if( rxbuf->size )
                {
                    if ( strstr( ( char * )rxbuf->data, "OK" ) != NULL )
                    {
                        return 1;
                    }

                    else if( strstr( ( char * )rxbuf->data, "ERROR" ) != NULL )
                    {
                        return 2;
                    }

                    else if( strstr( ( char * )rxbuf->data, "busy" ) != NULL )
                    {
                        return 3;
                    }

                    return 4;
                }
            }
        }
    }
    return 0;
}


uint8_t esp_reset( esp * myesp )
{
#ifdef ESP_USE_PINRST

    HAL_GPIO_WritePin( GPIOA, GPIO_PIN_1, GPIO_PIN_RESET );
    HAL_Delay( 200 );
    HAL_GPIO_WritePin( GPIOA, GPIO_PIN_1, GPIO_PIN_SET );
    HAL_Delay( 500 );
    return 0;
    
#else
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT+RST\r\n" );
    esp_cmd_wait( myesp->ptxbuf, NULL, 1000 );
    return 0;
#endif
}

uint8_t esp_at( esp * myesp )
{
    uint8_t ret;
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT\r\n" );
    ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 1000 );
    
    if( ret != 1 )
    {
        esp_debug( "ESP8266 : error : AT test fail\n" );
        return 1;
    }
    return 0;
}

uint8_t esp_at_echo( esp * myesp, uint8_t echo )
{
    
    uint8_t ret;
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "ATE%d\r\n", echo );
    ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 1000 );
    
    if( ret != 1 )
    {
        esp_debug( "ESP8266 : error : Set echo fail\n" );
        return 1;
    }
    return 0;
}

uint8_t esp_setmode( esp * myesp, uint8_t mode )
{
    uint8_t ret;
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT+CWMODE=%d\r\n", mode );
    ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 500 );
    
    if( ret != 1 )
    {
        esp_debug( "ESP8266 : error : Set mode fail\n" );
        return 1;
    }
    return 0;
    
}

uint8_t esp_join_ap( esp * myesp, uint8_t * ssid, uint8_t * pswd  )
{
    uint8_t ret;
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT+CWJAP?\r\n" );
    ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 500 );
    
    if( strstr( ( char * )myesp->prxbuf->data, ( char * )ssid ) != NULL )
    {
        esp_debug( "ESP8266 : log : Has been joined AP\n" );
        return 0;
    }
    
    esp_debug( "ESP8266 : log : join AP ...\n" );
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pswd );
    
    
    for( uint8_t i = 0; i < 5 ; i++ )
    {
        ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 5000 );
        
        if( ret == 1 )
        {
            esp_debug( "ESP8266 : log : Join AP success\r\n" );
            myesp->connected = 1;
            return 0;
        }

    }
    
    return 1;
}

uint8_t esp_set_ap( esp * myesp, uint8_t * ssid, uint8_t * pswd, uint8_t mode )
{
    uint8_t ret;
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT+CWSAP=\"%s\",\"%s\",1,%d\r\n", (char *)ssid, (char *)pswd, mode );
    ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 2000 );
    
    if( ret != 1 )
    {
        esp_debug( "ESP8266 : error : Set AP mode fail\n" );
        return 1;
    }
    return 0;
}



uint8_t esp_autocon( esp * myesp, uint8_t autocon )
{
    uint8_t ret;
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT+CWAUTOCONN=%d\r\n", autocon );
    ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 500 );
    
    if( ret != 1 )
    {
        esp_debug( "ESP8266 : error : Set auto connect fail\n" );
        return 1;
    }
    return 0;
    
}

uint8_t esp_cipmode( esp * myesp, uint8_t cipmode )
{
    uint8_t ret;
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT+CIPMODE=%d\r\n", cipmode );
    ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 500 );
    
    if( ret != 1 )
    {
        esp_debug( "ESP8266 : error : Set cipmode fail\n" );
        return 1;
    }
    return 0;
}

uint8_t esp_cipmux( esp * myesp, uint8_t cipmux )
{
    uint8_t ret;
    myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "AT+CIPMUX=%d\r\n", cipmux );
    ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 500 );
    
    if( ret != 1 )
    {
        esp_debug( "ESP8266 : error : Set cipmux fail\n" );
        return 1;
    }
    return 0;
}


uint8_t esp_setup ( esp * myesp, esp_buffer * txbuf,esp_buffer * rxbuf )
{
    esp_ll_init();
    
    if( myesp == NULL || txbuf == NULL || rxbuf == NULL )
        return 1;
    
    myesp->ptxbuf = txbuf;
    myesp->prxbuf = rxbuf;
    
    if( myesp->mode > 3 || myesp->mode == 0 )
        return 1;

    if( myesp->mode_ap > 4 || myesp->mode_ap == 1 )
        return 1;

    if( myesp->mode == 1 || myesp->mode == 3 )
    {
        if( myesp->ssid[0] == 0 || myesp->pswd[0] == 0 )
            return 1;
    }
    
    if( myesp->mode == 2 || myesp->mode == 3 )
    {
        if( myesp->ssid_ap[0] == 0 || myesp->pswd_ap[0] == 0 )
            return 1;
    }

    myesp->auto_connect = myesp->auto_connect ? 1 : 0;

    myesp->cipmux = myesp->cipmux ? 1 : 0;
    
    myesp->cipmode = myesp->cipmux ? 0 : (myesp->cipmode ? 1 : 0);
    
    
    
    if( esp_reset( myesp ) )
        return 2;
    
    if( esp_at( myesp ) )
        return 3;
    
    if( esp_at_echo( myesp,0 ) )
        return 4;

    if( esp_autocon( myesp, myesp->auto_connect ) )
        return 5;

    if( esp_setmode( myesp, myesp->mode ) )
        return 6;

    if( myesp->mode == 1 || myesp->mode == 3 )
    {
        if( esp_join_ap( myesp ,myesp->ssid,myesp->pswd ) )
            return 7;
    }
    
    if( myesp->mode == 2 || myesp->mode == 3 )
    {
        if( esp_set_ap( myesp, myesp->ssid_ap, myesp->pswd_ap, myesp->mode_ap ) )
            return 8;
    }

    if( esp_cipmode( myesp, myesp->cipmode ) )
        return 9;

    if( esp_cipmux( myesp, myesp->cipmux ) )
        return 10;

    return 0;
}

uint8_t esp_link_setup ( esp * myesp, esp_link * mylink, esp_buffer * txbuf, esp_buffer * rxbuf )
{
    if( myesp == NULL || mylink == NULL || txbuf == NULL || rxbuf == NULL )
        return 1;
    
    mylink->ptxbuf = txbuf;
    mylink->prxbuf = rxbuf;
    
    if( mylink->id > 4 )
        return 2;
    
    switch( mylink->id )
    {
        case 0:
            myesp->link0 = mylink;
            break;

        case 1:
            myesp->link1 = mylink;
            break;

        case 2:
            myesp->link2 = mylink;
            break;

        case 3:
            myesp->link3 = mylink;
            break;

        case 4:
            myesp->link4 = mylink;
            break;
    }
    
    if( mylink->netpro > 3 && mylink->netpro == 0 )
        return 2;
    
    mylink->linked = 0;
    mylink->keepalive = (  mylink->keepalive > 7200 ) ? 7200 :  mylink->keepalive;
    
    return 0;
}

uint8_t esp_link_connect ( esp * esp, esp_link * link )
{
    char * p_txbuffer = ( char * )esp->ptxbuf->data;

    if( esp == NULL || link == NULL )
    {
        return 1;
    }

    if( !esp->connected )
    {
        esp_debug( "ESP8266 : error : connect fail, not connect\n" );
        return 2;
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

    esp->ptxbuf->size = ( uint8_t * )p_txbuffer - esp->ptxbuf->data;
    
    esp_cmd_wait( esp->ptxbuf, esp->prxbuf, 1000 );

    if( strstr( ( char * )esp->prxbuf->data, "CONNECT" )
            || strstr( ( char * )esp->prxbuf->data, "ALREADY CONNECTED" ) )
    {
        link->linked = 1;
        return 0;
    }

    else
    {
        esp_debug( "ESP8266 : error : link connect fail\r\n" );
        return 3;
    }
}

uint8_t esp_link_close ( esp * myesp, esp_link * mylink )
{
    uint8_t ret;
    char * p_txbuffer = ( char * )myesp->ptxbuf->data;

    if( mylink->linked )
    {
        esp_cmd_wait( NULL, NULL, 500 );
        
        if( _transparent_state )
        {
            myesp->ptxbuf->size = sprintf( ( char * )myesp->ptxbuf->data, "+++" );
            esp_cmd_wait( myesp->ptxbuf, NULL, 0 );
            
            esp_cmd_wait( NULL, NULL, 1100 );
        }

        p_txbuffer += sprintf( ( char * )p_txbuffer, "AT+CIPCLOSE" );

        if( myesp->cipmux )
            p_txbuffer += sprintf( ( char * )p_txbuffer, "=%d", mylink->id );

        p_txbuffer += sprintf( ( char * )p_txbuffer, "\r\n" );

        myesp->ptxbuf->size = ( uint8_t * )( p_txbuffer ) - myesp->ptxbuf->data;

        ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 500 );
        
        if( ret == 1 || strstr( ( char * )myesp->prxbuf->data, "CLOSED" ) )
        {
            return 0;
        }

        else
        {
            esp_debug( "ESP8266 : error : close failed\n" );
            return 1;
        }
    }

    return 0;
}

uint8_t esp_link_send ( esp * myesp, esp_link * mylink )
{
    uint8_t i;
    uint8_t ret;
    char * p_txbuffer = ( char * )myesp->ptxbuf->data;

    if( mylink == NULL )
        return 1;

    if( mylink->ptxbuf->size == 0 )
        return 2;

    if( mylink->linked == 0 )
    {
        esp_debug( "ESP8266 : error : link send fail for no connection\n" );
        return 3;
    }

    if( myesp->cipmode == 0 )
    {
        p_txbuffer += sprintf( p_txbuffer, "AT+CIPSEND=" );

        if( myesp->cipmux )
            p_txbuffer += sprintf( p_txbuffer, "%d,", mylink->id );

        p_txbuffer += sprintf( p_txbuffer, "%d\r\n", mylink->ptxbuf->size );

        myesp->ptxbuf->size = ( uint8_t * )( p_txbuffer ) - myesp->ptxbuf->data;

        if( esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 500 ) == 1 )
        {
            
            esp_send_buffer( mylink->ptxbuf );
            
            for( i = 0; i < 4; i++ )
            {
                if( esp_cmd_wait( NULL, myesp->prxbuf, 500 ) == 1 )
                    return 0;
            }
            return 4;
        }
        else
        {
            esp_debug( "ESP8266 : error : link send start fail\n" );
            return 5;
        }

        
    }
    else
    {
        if( !_transparent_state )
        {

            myesp->ptxbuf->size = sprintf( (char *)myesp->ptxbuf->data, "AT+CIPSEND\r\n" );
            ret = esp_cmd_wait( myesp->ptxbuf, myesp->prxbuf, 500 );
            if( ret && strstr((char*)myesp->prxbuf->data , ">") != NULL )
            {
                _transparent_state = 1;
                esp_send_buffer( mylink->ptxbuf );
                return 0;
            }
            else
            {
                esp_debug( "ESP8266 : error : link send fail\n" );
                return 6; 
            }            
        }

        else
        {
            esp_send_buffer( mylink->ptxbuf );
            return 0;
        }
    }
}

uint8_t esp_link_recv ( esp * myesp, esp_link * mylink )
{

    if( myesp == NULL || mylink == NULL )
    {
        return 1;
    }

    if( mylink->linked == 0 )
    {
        esp_debug( "ESP8266 : error : link recv fail for no connection\n" );
        return 2;
    }
    
    esp_recv_buffer( mylink->prxbuf );

    return 0;
}


