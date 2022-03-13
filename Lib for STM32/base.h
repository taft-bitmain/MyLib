/*******************************************************************************
 * @file     base.h
 * @brief    Basic operations for IO and UART.
 * @version  V1.3
 * @date     2021.8.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
    1. GPIO is using regiter operation for efficiency.
    2. UART is using HAL lib.

*******************************************************************************/
#ifndef __BASE_H
#define __BASE_H

#ifdef __cplusplus
extern "c"
{
#endif

/**************** basic functions enable macros ************************/

#define     BASE_IO              		1
#define     BASE_UART_PRINTF     		0
#define     BASE_USB_PRINTF     		0


#if ( BASE_UART_PRINTF == 1 )
#define     UART_HANDLE                 huart1
#define     UART_BUFFER_LEN             256
#endif

#if ( BASE_USB_PRINTF == 1 )
#define     USB_BUFFER_LEN              256
#endif

/************************* BASE_IO ******************************/

#if (BASE_IO == 1)

#define     SetPin(port,bit)            ( port->BSRR = bit )
#define     ResetPin(port,bit)          ( port->BRR  = bit )

#define     WritePin(port,bit,state)    ( state ? SetPin(port,bit) : ResetPin(port,bit) )
#define     ReadPin(port,bit)           (( port->IDR  & bit ) ? 1 : 0 )

#define     SetPort(port,val)           ( port->ODR  = val )
#define     GetPort(port)               ( port->IDR )

#endif

/******************** BASE_DEFAULT_PRINTF ************************/

#if     (BASE_USB_PRINTF == 1)
#define     myprintf                    usb_printf
#elif   (BASE_UART_PRINTF == 1)
#define     myprintf                    uart_printf
#endif

/******************** BASE_USB_PRINTF ****************************/

#if (BASE_USB_PRINTF == 1)
    int    usb_printf             ( const char * format, ... );
#endif

/******************** BASE_UART_PRINTF ***************************/

#if (BASE_UART_PRINTF == 1)
    int    uart_printf            ( const char * format, ... );
#endif



#ifdef __cplusplus
}
#endif
#endif
