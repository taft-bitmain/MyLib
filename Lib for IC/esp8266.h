/*******************************************************************************
 * @file     esp8266.h
 * @brief    Connect esp8266 module with AT commands via UART.
 * @version  V1.3
 * @date     2022.3.4
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
    1. Recv link data when cipmode=1 is not finished, it will receive the packet header
        +IPD<[,id]>,[length]:[payload]
EXAMPLE CODE:

    esp8266 myesp;
    esp8266_link mylink;

    void setup( void )
    {
        esp8266_init( &myesp );
        esp8266_setup( &myesp, 1, 0, "yourssid", "yourpswd", NULL, NULL, 1 ,1 ,0 ); 
        esp8266_link_setup( &myesp, &mylink, 0, 1, "192.168.x.x", xxxx, xxxx, xx );
        esp8266_link_connect( &myesp, &mylink );
        
    }

    void loop( void )
    {
        esp8266_link_recv( &myesp, &mylink );
        if( mylink.rx_len )
        {
            mylink.tx_len = sprintf( (char *)mylink.txbuffer, "recv:%d\r\n\t%s\r\n", mylink.rx_len, mylink.rxbuffer );
            esp8266_link_send( &myesp, &mylink );
            mylink.rx_len = 0;
        }
    }


*******************************************************************************/
#ifndef __ESP8266_H
#define __ESP8266_H

#include "stdint.h"
#include "stm32f1xx.h"

#include "usart.h"
#include "stm32f1xx_hal_uart.h"

//#include "base.h"

// #define     ESP8266_USE_HWRESET	    1

#define     ESP8266_BUFLEN1             128 // for AT cmd send
#define     ESP8266_BUFLEN2             256 // for AT cmd receive
#define     ESP8266_BUFLEN3             512 // for link data send
#define     ESP8266_BUFLEN4             512 // for link data receive

#define     ESP8266_LL_HUART            huartx

//#define     esp8266_debug(expr)         myprintf(expr)
#define     esp8266_debug(expr)         ((void)0U)


typedef struct
{
    uint8_t     id;         // 0,1,2,3,4
    uint8_t     netpro;     // 1:TCP 2:UDP 3:SSL
    uint8_t     ip[13];
    uint8_t     linked;
    uint16_t    port;
    uint16_t    localport;
    uint16_t    keepalive;
    uint8_t     txbuffer[ESP8266_BUFLEN3];
    uint16_t    tx_len;
    uint8_t     rxbuffer[ESP8266_BUFLEN4];
    volatile uint16_t rx_len;
} esp8266_link;

typedef struct
{
    uint8_t     mode;       // 1:Station 2:SoftAP 3:SoftAP+Station
    uint8_t     mode_ap;    // 0：OPEN 2：WPA_PSK 3：WPA2_PSK 4：WPA_WPA2_PSK
    uint8_t     ssid[32];
    uint8_t     pswd[64];
    uint8_t     ssid_ap[32];
    uint8_t     pswd_ap[64];
    uint8_t     auto_connect;
    uint8_t     cipmode;
    uint8_t     cipmux;
    uint8_t     connected;
    uint8_t     txbuffer[ESP8266_BUFLEN1];
    uint16_t    tx_len;
    uint8_t     rxbuffer[ESP8266_BUFLEN2];
    uint16_t    rx_len;
    esp8266_link  *  link0;
    esp8266_link  *  link1;
    esp8266_link  *  link2;
    esp8266_link  *  link3;
    esp8266_link  *  link4;
} esp8266;


#define     esp8266_ll_delayms(ms)      HAL_Delay( ms )

void        esp8266_ll_send             ( uint8_t * txbuffer, uint16_t tx_len );
void        esp8266_ll_recv             ( uint8_t * rxbuffer, uint16_t rx_len );

void        esp8266_ll_recv_callback    ( UART_HandleTypeDef * huart,
        uint16_t Size );

/* return   1: OK
            2: ERROR
            3: busy
            4: recvived something but not 1,2,3
            5: timeout
*/
uint8_t     esp8266_cmd_wait            ( esp8266 * esp, uint32_t timeout );


uint8_t     esp8266_reset               ( esp8266 * esp );
uint8_t     esp8266_at                  ( esp8266 * esp );
uint8_t     esp8266_setmode             ( esp8266 * esp );
uint8_t     esp8266_join_ap             ( esp8266 * esp );
uint8_t     esp8266_set_ap              ( esp8266 * esp );
uint8_t     esp8266_autocon             ( esp8266 * esp );
uint8_t     esp8266_cipmode             ( esp8266 * esp );
uint8_t     esp8266_cipmux              ( esp8266 * esp );
uint8_t     esp8266_transparent         ( esp8266 * esp );
void        esp8266_transparent_end     ( esp8266 * esp );
uint8_t     esp8266_mulitransparent     ( esp8266 * esp );

uint8_t     esp8266_init                ( esp8266 * esp );

uint8_t     esp8266_setup               ( esp8266 * esp, uint8_t mode,
        uint8_t mode_ap, const char * ssid, const char * pswd, \
        const char * ssid_ap, const char * pswd_ap, \
        uint8_t auto_connect, uint8_t cipmode, uint8_t cipmux );

uint8_t     esp8266_link_setup          ( esp8266 * esp, esp8266_link * link,
        uint8_t id, uint8_t netpro, const char * ip, uint16_t port, \
        uint16_t localport, uint16_t keepalive );
uint8_t     esp8266_link_connect        ( esp8266 * esp, esp8266_link * link );
uint8_t     esp8266_link_close          ( esp8266 * esp, esp8266_link * link );
uint8_t     esp8266_link_send           ( esp8266 * esp, esp8266_link * link );
uint8_t     esp8266_link_recv           ( esp8266 * esp, esp8266_link * link );



#endif
