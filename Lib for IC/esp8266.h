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
    1. In STM32CubeMX, you need enable the uart and it's global interrupt and DMA, and
        enable [Project Menager->Advanced Settings->Register Callback->UART].
    2. Recv link data when cipmode=1 is not finished, it will receive the packet header
        +IPD<[,id]>,[length]:[payload]
EXAMPLE CODE:

    uint8_t pdata1[128];
    uint8_t pdata2[128];
    uint8_t pdata3[128];
    uint8_t pdata4[128];

    esp myesp =
    {
        .mode = 1,
        .ssid = "117-middle",
        .pswd = "zxcv1234",
        .auto_connect = 1,
        .cipmode = 1,
        .cipmux = 0
    };

    esp_link mylink = 
    {
        .id = 0,
        .netpro = 1,
        .ip = "192.168.31.17",
        .port = 8080,
        .localport = 8089,
        .keepalive = 60
    };
    
    esp_setup( &myesp , esp_buffer_static(pdata1,128), esp_buffer_static(pdata2,128));
    esp_link_setup( &myesp, &mylink, esp_buffer_static(pdata3,128), esp_buffer_static(pdata4,128) );
    esp_link_connect( &myesp, &mylink );
    
    void ESP_LoopTask( void )
    {
        static uint32_t state;
        switch(state)
        {
            case 0:
                esp_link_recv( &myesp, &mylink );
                state = 1;
                break;
            case 1:
                if( mylink.prxbuf->size )
                {
                    mylink.ptxbuf->size = sprintf( (char *)mylink.ptxbuf->data, "recv:%d\r\n\t%s\r\n", mylink.prxbuf->size, mylink.prxbuf->data );
                    esp_link_send( &myesp, &mylink );
                    state = 0;
                }
                break;
            default : state = 0;break;
        }
    }
    

*******************************************************************************/
#ifndef __ESP8266_H
#define __ESP8266_H

#include "stdint.h"
#include "stm32f1xx.h"

#include "usart.h"
#include "stm32f1xx_hal_uart.h"

#include "base.h"
#include "stdio.h"

#define     ESP_LL_HUART            huartx
#define     ESP_USE_HWRESET         0

//#define     esp_debug(expr)         myprintf(expr)
#define     esp_debug(expr)         ((void)0U)

typedef struct _esp_buffer
{
    uint8_t type;       // 0:not ready  1:static(stack or global mem)  2:dynamic(heap)
    uint8_t * data;     // data pointer
    uint16_t cap;       // total capocity
    volatile uint16_t size;      // actual length
}esp_buffer;

esp_buffer *    esp_buffer_static   ( uint8_t * mem, uint16_t capicity );
esp_buffer *    esp_buffer_dynamic  ( uint16_t capicity );
void            esp_buffer_free     ( esp_buffer * buffer );

typedef struct
{
    uint8_t     id;         // 0,1,2,3,4
    uint8_t     netpro;     // 1:TCP 2:UDP 3:SSL
    uint8_t     ip[128];
    uint16_t    port;
    uint16_t    localport;
    uint16_t    keepalive;
    esp_buffer * ptxbuf;
    esp_buffer * prxbuf;
    uint8_t     linked;
} esp_link;

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
    esp_buffer * ptxbuf;
    esp_buffer * prxbuf;
    esp_link  *  link0;
    esp_link  *  link1;
    esp_link  *  link2;
    esp_link  *  link3;
    esp_link  *  link4;
} esp;


void  esp_ll_recv_callback ( UART_HandleTypeDef * huart, uint16_t Size );

static inline void esp_ll_send ( uint8_t * pdat, uint16_t len )
{
    HAL_UART_Transmit_DMA( &ESP_LL_HUART, pdat, len );
}

static inline void esp_ll_recv ( uint8_t * pdat, uint16_t len )
{
    //HAL_UART_AbortReceive( &ESP_LL_HUART );
    HAL_UARTEx_ReceiveToIdle_DMA( &ESP_LL_HUART, pdat, len );
}

static inline void esp_ll_init ( void )
{
    // register callback function for HAL_UARTEx_ReceiveToIdle_DMA
    HAL_UART_RegisterRxEventCallback( &ESP_LL_HUART, esp_ll_recv_callback );
}

/* return   1: OK
            2: ERROR
            3: busy
            4: recvived something but not 1,2,3
            5: timeout
*/
uint8_t     esp_cmd_wait            ( esp_buffer * txbuf, esp_buffer * rxbuf, uint32_t timeout );


uint8_t     esp_reset               ( esp * myesp );
uint8_t     esp_at                  ( esp * myesp );
uint8_t     esp_at_echo             ( esp * myesp, uint8_t echo );
uint8_t     esp_setmode             ( esp * myesp, uint8_t mode);
uint8_t     esp_join_ap             ( esp * myesp, uint8_t * ssid, uint8_t * pswd );
uint8_t     esp_set_ap              ( esp * myesp, uint8_t * ssid, uint8_t * pswd, uint8_t mode );

uint8_t     esp_autocon             ( esp * myesp, uint8_t autocon );
uint8_t     esp_cipmode             ( esp * myesp, uint8_t cipmode );
uint8_t     esp_cipmux              ( esp * myesp, uint8_t cipmux );

uint8_t     esp_transparent         ( esp * myesp );
void        esp_transparent_end     ( esp * myesp );
uint8_t     esp_mulitransparent     ( esp * myesp );

uint8_t     esp_setup               ( esp * myesp, esp_buffer * txbuf,esp_buffer * rxbuf );
uint8_t     esp_link_setup          ( esp * myesp, esp_link * mylink, esp_buffer * txbuf, esp_buffer * rxbuf );

uint8_t     esp_link_connect        ( esp * myesp, esp_link * mylink );
uint8_t     esp_link_close          ( esp * myesp, esp_link * mylink );
uint8_t     esp_link_send           ( esp * myesp, esp_link * mylink );
uint8_t     esp_link_recv           ( esp * myesp, esp_link * mylink );



#endif
