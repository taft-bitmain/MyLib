/*******************************************************************************
 * @file     base.c
 * @brief    Basic operations, such as uart, detecting key delection and lighting the LED.
 * @version  V1.2
 * @date     2021.7.4
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "base.h"

/************************* Key ****************************/
#if (BASE_KEY == 1)
char iSPin_10(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin)==GPIO_PIN_RESET)
	{
		HAL_Delay(10);
		return 1;
	}
	return 0;
	
}
char iSPin_01(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin)==GPIO_PIN_SET)
	{
		HAL_Delay(10);
		return 1;
	}
	return 0;
}
char iSPin_101(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin)==GPIO_PIN_RESET)
	{
		HAL_Delay(10);
		while(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin)==GPIO_PIN_RESET);
		HAL_Delay(10);
		return 1;
	}
	return 0;
}
char iSPin_010(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin)==GPIO_PIN_SET)
	{
		HAL_Delay(10);
		while(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin)==GPIO_PIN_SET);
		HAL_Delay(10);
		return 1;
	}
	return 0;
}
#endif


/********************* Key Timer Interrupt *********************/
#if (BASE_KEY_TIMER == 1)

void Key_SetPin(Key_TypeDef *key ,GPIO_TypeDef *Port,uint16_t Pin,uint16_t PullUpDown)
{
	key->Port = Port;
	key->Pin = Pin;
	key->PP = PullUpDown;
	key->Count = 0;
	key->CountTmp = 0;
	key->PrevState = 0;
	key->Press = 0;
	key->Relase = 0;
}

uint16_t Key_IsPress(Key_TypeDef *key)
{
	if(key->Press)
	{
		key->Press--;
		return 1;
	}
	return 0;
}
uint16_t Key_IsRelase(Key_TypeDef *key)
{
	if(key->Relase)
	{
		key->Relase--;
		if(key->Count > KEY_LONG_COUNT)
			return 2;
		else
			return 1;
	}
	return 0;
}

void Key_TimerHandle(Key_TypeDef *key)
{
	uint16_t PinState;
	PinState = HAL_GPIO_ReadPin(key->Port,key->Pin);
	//如果按键按下
    if( PinState != key->PP)
	{
		//去除抖动
		if(key->PrevState == 0)
		{
			key->CountTmp++;
			if(key->CountTmp >= KEY_SHORT_COUNT)
			{
				key->PrevState = 1;
				key->Press++;
				key->Count = 0;
				key->CountTmp = 0;
			}	
		}
		else
		{
			key->Count++;
		}	
	}
    //如果按键没有按下
	else
	{
		if(key->PrevState == 1)
		{
			key->CountTmp++;
			if(key->CountTmp >= KEY_SHORT_COUNT)
			{
				key->PrevState = 0;
				key->Relase++;
				key->CountTmp = 0;
			}
		}
	}
}

#endif


/********************** printf ****************************/
#if (BASE_USART_PRINTF == 1)

#include <stdio.h>
#include <stdarg.h>

//#include "usbd_cdc_if.h"

char _dat_printf[1024];

int Myprintf(const char * format, ...)
{
    va_list arg_ptr;
    int nBytes ;
    va_start(arg_ptr, format);
    nBytes = vsprintf(_dat_printf, format, arg_ptr);
    va_end(arg_ptr);

    //todo
    //send it by your function
    HAL_UART_Transmit(&huart1,(uint8_t *)_dat_printf, nBytes,1000);
    //CDC_Transmit_FS((uint8_t *)_dat_printf, nBytes);
    return nBytes;
}

#endif


/****************** usart dma transmit **************************/
#if (BASE_USART_DMA_TRANSMIT == 1)
#include <stdio.h>
#include <stdarg.h>

#if (USART1_DMA_TRANSMIT == 1)
uint8_t USART1_TxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART2_DMA_TRANSMIT == 1)
uint8_t USART2_TxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART3_DMA_TRANSMIT == 1)
uint8_t USART3_TxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART4_DMA_TRANSMIT == 1)
uint8_t USART4_TxBuffer[USART_DMA_BUFFER_LEN];
#endif



void UART_DMA_Transmintf(UART_HandleTypeDef *huart,const char * format, ...)
{
	uint16_t nbytes;
	
	va_list arg_ptr;
	va_start(arg_ptr, format);
#if (USART1_DMA_TRANSMIT == 1)
	if(huart->Instance == USART1)
	{
		nbytes = vsprintf((char*)USART1_TxBuffer, format, arg_ptr);
		HAL_UART_Transmit_DMA(&huart1,USART1_TxBuffer,nbytes);
	}
#endif
#if (USART2_DMA_TRANSMIT == 1)
	if(huart->Instance == USART2)
	{
		nbytes = vsprintf((char*)USART2_TxBuffer, format, arg_ptr);
		HAL_UART_Transmit_DMA(&huart2,USART2_TxBuffer,nbytes);
	}
#endif
#if (USART3_DMA_TRANSMIT == 1)
	if(huart->Instance == USART3)
	{
		nbytes = vsprintf((char*)USART3_TxBuffer, format, arg_ptr);
		HAL_UART_Transmit_DMA(&huart3,USART3_TxBuffer,nbytes);
	}
#endif
#if (USART4_DMA_TRANSMIT == 1)
	if(huart->Instance == UART4)
	{
		nbytes = vsprintf((char*)USART4_TxBuffer, format, arg_ptr);
		HAL_UART_Transmit_DMA(&huart4,USART4_TxBuffer,nbytes);
	}
#endif
	
	va_end(arg_ptr);
}

#endif




/****************** usart dma receive ***************************/
#if (BASE_USART_DMA_RECEIVE == 1)

#if (USART1_DMA_RECEIVE == 1)
uint8_t USART1_RxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART2_DMA_RECEIVE == 1)
uint8_t USART2_RxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART3_DMA_RECEIVE == 1)
uint8_t USART3_RxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART4_DMA_RECEIVE == 1)
uint8_t USART4_RxBuffer[USART_DMA_BUFFER_LEN];
#endif


void UART_IDLE_Init(void)
{
#if (USART1_DMA_RECEIVE == 1)
	__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1,USART1_RxBuffer,USART_DMA_BUFFER_LEN);
#endif
#if (USART2_DMA_RECEIVE == 1)
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2,USART2_RxBuffer,USART_DMA_BUFFER_LEN);
#endif
#if (USART3_DMA_RECEIVE == 1)
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart3,USART3_RxBuffer,USART_DMA_BUFFER_LEN);
#endif
#if (USART4_DMA_RECEIVE == 1)
	__HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart4,USART4_RxBuffer,USART_DMA_BUFFER_LEN);
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
		
#if (USART1_DMA_RECEIVE == 1)
		if(huart->Instance == USART1)
			USART1_IDLE_Callback(huart->pRxBuffPtr,USART_DMA_BUFFER_LEN - huart->RxXferCount);
#endif
#if (USART2_DMA_RECEIVE == 1)
		if(huart->Instance == USART2)
			USART2_IDLE_Callback(huart->pRxBuffPtr,USART_DMA_BUFFER_LEN - huart->RxXferCount);
#endif
#if (USART3_DMA_RECEIVE == 1)
		if(huart->Instance == USART3)
			USART3_IDLE_Callback(huart->pRxBuffPtr,USART_DMA_BUFFER_LEN - huart->RxXferCount);
#endif
#if (USART4_DMA_RECEIVE == 1)
		if(huart->Instance == UART4)
			USART4_IDLE_Callback(huart->pRxBuffPtr,USART_DMA_BUFFER_LEN - huart->RxXferCount);
#endif
		
		huart->RxState = HAL_UART_STATE_READY;
		huart->hdmarx->State = HAL_DMA_STATE_READY;
		HAL_UART_Receive_DMA(huart,huart->pRxBuffPtr,USART_DMA_BUFFER_LEN);
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
#if (USART1_DMA_RECEIVE == 1)
	if(huart->Instance == USART1)
	{
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart1,USART1_RxBuffer,USART_DMA_BUFFER_LEN);
	}
#endif
#if (USART2_DMA_RECEIVE == 1)
	if(huart->Instance == USART2)
	{
		__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart2,USART2_RxBuffer,USART_DMA_BUFFER_LEN);
	}
#endif
#if (USART3_DMA_RECEIVE == 1)
	if(huart->Instance == USART3)
	{
		__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart3,USART3_RxBuffer,USART_DMA_BUFFER_LEN);
	}
#endif
#if (USART4_DMA_RECEIVE == 1)
	if(huart->Instance == UART4)
	{
		__HAL_UART_ENABLE_IT(&huart4,UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart4,USART4_RxBuffer,USART_DMA_BUFFER_LEN);
	}
#endif

}

__weak void USART1_IDLE_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}
__weak void USART2_IDLE_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}
__weak void USART3_IDLE_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}
__weak void USART4_IDLE_Callback(uint8_t *data,uint16_t len)
{
	UNUSED(data);
	UNUSED(len);
}

#endif
