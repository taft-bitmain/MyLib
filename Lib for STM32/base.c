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

#include <stdio.h>
#include <stdarg.h>

char _dat_printf[UART_BUFFER_LEN];

int uart_printf(const char * format, ...)
{
    va_list arg_ptr;
    int nBytes ;
    va_start(arg_ptr, format);
    nBytes = vsprintf(_dat_printf, format, arg_ptr);
    va_end(arg_ptr);


    while ( !(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC)) );
    HAL_UART_Transmit(&huart1,(uint8_t *)_dat_printf, nBytes,1000);
    
    return nBytes;
}

#endif

#if (BASE_USB_PRINTF == 1)

#include "usbd_cdc_if.h"
#include <stdio.h>
#include <stdarg.h>

char _dat_printf[USB_BUFFER_LEN];

int usb_printf(const char * format, ...)
{
    va_list arg_ptr;
    int nBytes ;
    va_start(arg_ptr, format);
    nBytes = vsprintf(_dat_printf, format, arg_ptr);
    va_end(arg_ptr);
    
    int trytimes = 3000;
    while ( trytimes-- && CDC_Transmit_FS((uint8_t *)_dat_printf, nBytes) != USBD_OK )
    {
        //HAL_Delay( 1 );
    }
    
    return nBytes;
}

#endif


/****************** usart dma transmit **************************/
#if (BASE_UART_DMA_TRANSMIT == 1)

#include <stdio.h>
#include <stdarg.h>

#if (UART1_DMA_TRANSMIT == 1)
uint8_t UART1_TxBuffer[UART_BUFFER_LEN];
#endif
#if (UART2_DMA_TRANSMIT == 1)
uint8_t UART2_TxBuffer[UART_BUFFER_LEN];
#endif
#if (UART3_DMA_TRANSMIT == 1)
uint8_t UART3_TxBuffer[UART_BUFFER_LEN];
#endif
#if (UART4_DMA_TRANSMIT == 1)
uint8_t UART4_TxBuffer[UART_BUFFER_LEN];
#endif



void UART_DMA_Transmintf(UART_HandleTypeDef *huart,const char * format, ...)
{
	uint16_t nbytes;
    uint8_t* pData;
	
#if (UART1_DMA_TRANSMIT == 1)
	if(huart->Instance == USART1)
        pData = UART1_TxBuffer;
#endif
#if (UART2_DMA_TRANSMIT == 1)
	if(huart->Instance == USART2)
        pData = UART2_TxBuffer;
#endif
#if (UART3_DMA_TRANSMIT == 1)
	if(huart->Instance == USART3)
        pData = UART3_TxBuffer;
#endif
#if (UART4_DMA_TRANSMIT == 1)
    if(huart->Instance == UART4)
        pData = UART4_TxBuffer;
#endif
    
    if(pData == NULL)
        return ;
    
    va_list arg_ptr;
	va_start(arg_ptr, format);
    nbytes = vsprintf((char*)pData, format, arg_ptr);
	va_end(arg_ptr);
    
    UART_DMA_Transmint(huart,pData,nbytes);
}

void UART_DMA_Transmint(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    // if the DMA is transmition complete, send it. Otherwise waitting for DMA completion.
    while ( !(__HAL_UART_GET_FLAG(huart, UART_FLAG_TC)) );
    HAL_UART_Transmit_DMA(huart,pData,Size);
}

#endif




/****************** usart dma receive ***************************/
#if (BASE_UART_DMA_RECEIVE == 1)

#if (UART1_DMA_RECEIVE == 1)
uint8_t UART1_RxBuffer[UART_BUFFER_LEN];
#endif
#if (UART2_DMA_RECEIVE == 1)
uint8_t UART2_RxBuffer[UART_BUFFER_LEN];
#endif
#if (UART3_DMA_RECEIVE == 1)
uint8_t UART3_RxBuffer[UART_BUFFER_LEN];
#endif
#if (UART4_DMA_RECEIVE == 1)
uint8_t UART4_RxBuffer[UART_BUFFER_LEN];
#endif


void UART_IDLE_Init(void)
{
#if (UART1_DMA_RECEIVE == 1)
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1,UART1_RxBuffer,UART_BUFFER_LEN);
#endif
#if (UART2_DMA_RECEIVE == 1)
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2,UART2_RxBuffer,UART_BUFFER_LEN);
#endif
#if (UART3_DMA_RECEIVE == 1)
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart3,UART3_RxBuffer,UART_BUFFER_LEN);
#endif
#if (UART4_DMA_RECEIVE == 1)
	__HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart4,UART4_RxBuffer,UART_BUFFER_LEN);
#endif
}

void UART_IDLE_Handler(UART_HandleTypeDef *huart)
{
    uint32_t temp = READ_REG(huart->Instance->SR);
    if(temp&UART_FLAG_IDLE)
	{
		__HAL_UART_CLEAR_IDLEFLAG(huart);
		HAL_DMA_Abort(huart->hdmarx);
		huart->RxXferCount = __HAL_DMA_GET_COUNTER(huart->hdmarx);
		
#if (UART1_DMA_RECEIVE == 1)
		if(huart->Instance == USART1)
			UART1_IDLE_Callback(huart->pRxBuffPtr,UART_BUFFER_LEN - huart->RxXferCount);
#endif
#if (UART2_DMA_RECEIVE == 1)
		if(huart->Instance == USART2)
			UART2_IDLE_Callback(huart->pRxBuffPtr,UART_BUFFER_LEN - huart->RxXferCount);
#endif
#if (UART3_DMA_RECEIVE == 1)
		if(huart->Instance == USART3)
			UART3_IDLE_Callback(huart->pRxBuffPtr,UART_BUFFER_LEN - huart->RxXferCount);
#endif
#if (UART4_DMA_RECEIVE == 1)
		if(huart->Instance == UART4)
			UART4_IDLE_Callback(huart->pRxBuffPtr,UART_BUFFER_LEN - huart->RxXferCount);
#endif
		
		huart->RxState = HAL_UART_STATE_READY;
		huart->hdmarx->State = HAL_DMA_STATE_READY;
		HAL_UART_Receive_DMA(huart,huart->pRxBuffPtr,UART_BUFFER_LEN);
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
#if (UART1_DMA_RECEIVE == 1)
	if(huart->Instance == USART1)
	{
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart1,UART1_RxBuffer,UART_BUFFER_LEN);
	}
#endif
#if (UART2_DMA_RECEIVE == 1)
	if(huart->Instance == USART2)
	{
		__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart2,UART2_RxBuffer,UART_BUFFER_LEN);
	}
#endif
#if (UART3_DMA_RECEIVE == 1)
	if(huart->Instance == USART3)
	{
		__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart3,UART3_RxBuffer,UART_BUFFER_LEN);
	}
#endif
#if (UART4_DMA_RECEIVE == 1)
	if(huart->Instance == UART4)
	{
		__HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart4,UART4_RxBuffer,UART_BUFFER_LEN);
	}
#endif

}

__weak void UART1_IDLE_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}
__weak void UART2_IDLE_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}
__weak void UART3_IDLE_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}
__weak void UART4_IDLE_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}

#endif
