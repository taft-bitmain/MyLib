/*******************************************************************************
 * @file     base.c
 * @brief    Basic operations for IO and UART.
 * @version  V1.3
 * @date     2021.8.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "base.h"


/********************** printf ****************************/
#if (BASE_UART_PRINTF == 1)

#include "usart.h"
#include <stdio.h>
#include <stdarg.h>

char _dat_printf[UART_BUFFER_LEN];

int uart_printf( const char * format, ... )
{
    va_list arg_ptr;
    int nBytes ;
    va_start( arg_ptr, format );
    nBytes = vsprintf( _dat_printf, format, arg_ptr );
    va_end( arg_ptr );


    while ( !( __HAL_UART_GET_FLAG( &huart1, UART_FLAG_TC ) ) );

    HAL_UART_Transmit( &huart1, ( uint8_t * )_dat_printf, nBytes, 1000 );

    return nBytes;
}

#endif

#if (BASE_USB_PRINTF == 1)

#include "usbd_cdc_if.h"
#include <stdio.h>
#include <stdarg.h>

char _dat_printf[USB_BUFFER_LEN];

int usb_printf( const char * format, ... )
{
    va_list arg_ptr;
    int nBytes ;
    va_start( arg_ptr, format );
    nBytes = vsprintf( _dat_printf, format, arg_ptr );
    va_end( arg_ptr );

    int trytimes = 3000;

    while ( trytimes--
            && CDC_Transmit_FS( ( uint8_t * )_dat_printf, nBytes ) != USBD_OK )
    {
        //HAL_Delay( 1 );
    }

    return nBytes;
}

#endif

