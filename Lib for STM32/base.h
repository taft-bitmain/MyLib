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
    3. To use BASE_UART_DMA_RECEIVE, you need:
        - turn on the UART global interrupt ;
        - config DMA USART_RX ;
        - put the UART_IDLE_Handler() in UARTx_IRQHandler() funcition, which
          belong to stm32f1xx_it.c ;
        - run UART_IDLE_Init() at the begining of your code ;
        - MCU will call UARTx_IDLE_Callback() if the IDLE interrupt occured,
          so rewrite your own callback functions as it is __weak type.

*******************************************************************************/
#ifndef __BASE_H
#define __BASE_H

#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx.h"


    /**************** basic functions enable macros ************************/

#define     BASE_IO              		1
#define     BASE_UART_PRINTF     		0
#define     BASE_USB_PRINTF     		1
#define     BASE_UART_DMA_TRANSMIT      0
#define	   	BASE_UART_DMA_RECEIVE		0

#if ( BASE_UART_PRINTF == 1 ||  BASE_UART_DMA_TRANSMIT== 1 || BASE_UART_DMA_RECEIVE == 1 )
#include "usart.h"
#include "stm32f1xx_hal_uart.h"
#define     UART_BUFFER_LEN             256
#endif

#if ( BASE_USB_PRINTF == 1 )
#define     USB_BUFFER_LEN              256
#endif


#if (BASE_UART_DMA_TRANSMIT == 1 )
#define     UART1_DMA_TRANSMIT          0
#define     UART2_DMA_TRANSMIT          0
#define     UART3_DMA_TRANSMIT          0
#define     UART4_DMA_TRANSMIT          0
#endif

#if (BASE_UART_DMA_RECEIVE == 1)
#define     UART1_DMA_RECEIVE           0
#define     UART2_DMA_RECEIVE           0
#define     UART3_DMA_RECEIVE           0
#define     UART4_DMA_RECEIVE           0
#endif


    /************************* BASE_IO ****************************/

#if (BASE_IO == 1)

#define     SetPin(port,bit)            ( port->BSRR = bit )
#define     ResetPin(port,bit)          ( port->BRR  = bit )

#define     WritePin(port,bit,state)    ( state ? SetPin(port,bit) : ResetPin(port,bit) )
#define     ReadPin(port,bit)           (( port->IDR  & bit ) ? 1 : 0 )

#define     SetPort(port,val)           ( port->ODR  = val )
#define     GetPort(port)               ( port->IDR )

#endif

    /******************** BASE_DEFAULT_PRINTF ****************************/
#if     (BASE_USB_PRINTF == 1)
#define     myprintf                    usb_printf
#elif (BASE_UART_PRINTF == 1)
#define     myprintf                    uart_printf
#endif

    /******************** BASE_USB_PRINTF ****************************/
#if (BASE_USB_PRINTF == 1)
    int         usb_printf             (const char * format, ...);
#endif
    /******************** BASE_UART_PRINTF ****************************/

#if (BASE_UART_PRINTF == 1)
    int         uart_printf            (const char * format, ...);
#endif





    /****************** BASE_UART_DMA_TRANSMIT ************************/
#if (BASE_UART_DMA_TRANSMIT == 1)

#if (UART1_DMA_TRANSMIT == 1)
    extern uint8_t UART1_TxBuffer[UART_BUFFER_LEN];
#endif
#if (UART2_DMA_TRANSMIT == 1)
    extern uint8_t UART2_TxBuffer[UART_BUFFER_LEN];
#endif
#if (UART3_DMA_TRANSMIT == 1)
    extern uint8_t UART3_TxBuffer[UART_BUFFER_LEN];
#endif
#if (UART4_DMA_TRANSMIT == 1)
    extern uint8_t UART4_TxBuffer[UART_BUFFER_LEN];
#endif

    void        UART_DMA_Transmintf (UART_HandleTypeDef *huart,const char * format, ...);
    void        UART_DMA_Transmint  (UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
#endif

    /***************** BASE_UART_DMA_RECEIVE *************************/
#if (BASE_UART_DMA_RECEIVE == 1)

#if (UART1_DMA_RECEIVE == 1)
    extern uint8_t UART1_RxBuffer[UART_BUFFER_LEN];
    void UART1_IDLE_Callback(uint8_t *data,uint16_t len);
#endif
#if (UART2_DMA_RECEIVE == 1)
    extern uint8_t UART2_RxBuffer[UART_BUFFER_LEN];
    void UART2_IDLE_Callback(uint8_t *data,uint16_t len);
#endif
#if (UART3_DMA_RECEIVE == 1)
    extern uint8_t UART3_RxBuffer[UART_BUFFER_LEN];
    void UART3_IDLE_Callback(uint8_t *data,uint16_t len);
#endif
#if (UART4_DMA_RECEIVE == 1)
    extern uint8_t UART4_RxBuffer[UART_BUFFER_LEN];
    void UART4_IDLE_Callback(uint8_t *data,uint16_t len);
#endif

    void        UART_IDLE_Init      (void);
    void        UART_IDLE_Handler   (UART_HandleTypeDef *huart);

#endif


#ifdef __cplusplus
}
#endif
#endif
