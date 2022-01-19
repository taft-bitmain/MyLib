/*******************************************************************************
 * @file     myuart.c
 * @brief    High performance to operate UART with DMA
 * @version  V1.1
 * @date     2021.5.11
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 *******************************************************************************/

#include "myuart.h"
#include "main.h"

MyUart_BufList BL4UART1;

void MyUart_Init(MyUart_BufList *BL,UART_HandleTypeDef *huart)
{
  BL->huart = huart;
  
#if	(MYUART_DMAT == 1)
  MyUart_DMAT_Init(BL);
#endif
  
#if	(MYUART_DMAR == 1)
  MyUart_DMAR_Init(BL);
#endif
  
}

#if	(MYUART_DMAT == 1)

/*********************************************
- 发送完成和发送出错的回调函数，根据你的设置，
  来选择调用相应的回调函数的参数
*********************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		MyUart_DMAT_Callback(&BL4UART1);
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		MyUart_DMAT_Callback(&BL4UART1);
	}
}

/****************************************************************
state    |      0      |      1      |      2      |     3      |
function |     free    |   writing   |ready to send|   sending  |
****************************************************************/

void MyUart_DMAT_Init(MyUart_BufList *BL)
{
  int i;
	for( i = 0; i < MYUART_TXNODE; i++ )
	{
		BL->TBufList[i].count = 0;
		BL->TBufList[i].size = 0;
		BL->TBufList[i].state = 0;
		BL->TBufList[i].next = &(BL->TBufList[(i+1)%MYUART_TXNODE]);
		BL->TBufList[i].prev = &(BL->TBufList[(i+MYUART_TXNODE-1)%MYUART_TXNODE]);
	}
	BL->write = BL->trans = &(BL->TBufList[0]);
  
}

void MyUart_DMAT_Callback(MyUart_BufList *BL)
{
	HAL_UART_AbortTransmit(BL->huart);
	BL->trans->state = 0;
	BL->trans->count = 0;
	BL->trans->size = 0;
	BL->trans = BL->trans->next;
	if(BL->trans->state == 2)
	{
		BL->trans->state = 3;
		if(BL->trans->size > MYUART_NODESIZE)
			BL->trans->size = MYUART_NODESIZE;
		HAL_UART_Transmit_DMA(BL->huart, (uint8_t*)BL->trans->data,BL->trans->size);
	}
}



int MyUart_Printf(MyUart_BufList *BL,const char * format, ...)
{
	va_list arg_ptr;
	int nBytes ;
	
	while(BL->write->state >= 2)
	{
		BL->write = BL->write->next;
	}
	if(BL->write->state == 0)
	{
		BL->write->size = 0;
		BL->write->state = 1;
	}
	
	va_start(arg_ptr, format);
	nBytes = vsprintf(BL->write->data + BL->write->size, format, arg_ptr);
	va_end(arg_ptr);
	
	BL->write->size += nBytes;
	
	if(BL->write->size >= MYUART_NODESIZE - MYUART_TXNODE)
	{
		MyUart_Flush(BL);
	}
	return nBytes;
}

int MyUart_Trans(MyUart_BufList *BL,const char *data,unsigned int len)
{
	int total = 0,transmitLen;
	while(total < len)
	{
		while(BL->write->state >= 2)
		{
			BL->write = BL->write->next;
		}
		if(BL->write->state == 0)
		{
			BL->write->size = 0;
			BL->write->state = 1;
		}
		
		transmitLen = MYUART_NODESIZE - BL->write->size;
		
		if(total + transmitLen <= len)
		{
			memcpy(BL->write->data + BL->write->size,data + total,transmitLen);
			BL->write->size += transmitLen;
			total += transmitLen;
			MyUart_Flush(BL);
		}
		else
		{
			transmitLen = len - total;
			memcpy(BL->write->data + BL->write->size,data + total,transmitLen);
			BL->write->size += transmitLen;
			total += transmitLen;
			//DMAPrintf_Flush();
		}
	}
	return 0;
}

void MyUart_Flush(MyUart_BufList *BL)
{
	if(BL->write->size > 0)
	{
		BL->write->state = 2;
		BL->write = BL->write->next;
	}
	while(BL->huart->TxXferCount > 0);
	if(BL->trans->state == 2)
	{
		BL->trans->state = 3;
		if(BL->trans->size > MYUART_NODESIZE)
			BL->trans->size = MYUART_NODESIZE;
		HAL_UART_Transmit_DMA(BL->huart,\
			(uint8_t *)BL->trans->data,BL->trans->size);
	}
}

#endif



#if	(MYUART_DMAR == 1)

/****************************************************************
state    |      0      |      1      |      2      |     3      |
function |     free    |  recieving  |  rec finish |   reading  |
****************************************************************/

void MyUart_DMAR_Init(MyUart_BufList *BL)
{
	int i;
	for( i = 0; i < MYUART_TXNODE; i++ )
	{
		BL->RBufList[i].count = 0;
		BL->RBufList[i].size = 0;
		BL->RBufList[i].state = 0;
		BL->RBufList[i].next = &(BL->RBufList[(i+1)%MYUART_RXNODE]);
		BL->RBufList[i].prev = &(BL->RBufList[(i+MYUART_RXNODE-1)%MYUART_RXNODE]);
	}
	BL->rec = BL->read = &(BL->RBufList[0]);
  
	BL->rec->state = 1;
	HAL_UART_Receive_DMA(BL->huart,(uint8_t *)(BL->rec->data),MYUART_NODESIZE);
	__HAL_UART_ENABLE_IT(BL->huart,UART_IT_IDLE);
}

void MyUart_IDLEHandler(MyUart_BufList *BL)
{
	uint32_t temp = READ_REG(BL->huart->Instance->SR);
  if(temp&UART_FLAG_IDLE)
	{
		__HAL_UART_CLEAR_IDLEFLAG(BL->huart);
		BL->rec->size = MYUART_NODESIZE - __HAL_DMA_GET_COUNTER(BL->huart->hdmarx);
		BL->rcount++;
    if(BL->rec->size >= (MYUART_NODESIZE - MYUART_MAX_REC))
		{
			BL->rec->state = 2;
			HAL_UART_AbortReceive(BL->huart);
			if(BL->rec->next->state == 0)
			{
				BL->rec = BL->rec->next;
				BL->rec->count = 0;
				BL->rec->size = 0;
				BL->rec->state = 1;
				HAL_UART_Receive_DMA(BL->huart,(uint8_t *)BL->rec->data,MYUART_NODESIZE);
			}
		}
	}
}


int	MyUart_Available(MyUart_BufList *BL)
{
	struct MyUart_BufNode *pTmp = BL->read;
	int total = 0;
	while((pTmp->state == 2) || (pTmp->state == 1))
	{
		total += pTmp->size - pTmp->count;
		if(pTmp == BL->rec)
			break;
    else
      pTmp = pTmp->next;
	}
	return total;
}

int	MyUart_Dump(MyUart_BufList *BL,char *data,unsigned int len)
{
	int total;
  total = MyUart_Available(BL);
	if(len > total)
		len = total;
  
	for(total = 0;total < len; )
	{
		int pDumpLen = BL->read->size - BL->read->count;
		if(total + pDumpLen <= len)	//this node all dump
		{
			memcpy(data + total,BL->read->data + BL->read->count,pDumpLen);
			total += pDumpLen;
			BL->read->count += pDumpLen;
			if(BL->read->state == 2)
			{
				BL->read->state = 0;
				BL->read->size = 0;
				BL->read->count = 0;
				BL->read = BL->read->next;
			}
		}
		else	//this node partly dump
		{
			pDumpLen = len - total;
			memcpy(data + total,BL->read->data + BL->read->count,pDumpLen);
			total += pDumpLen;
			BL->read->count += pDumpLen;
			BL->read->state = 3;
			//break;
		}
	}
	return len;
}


#endif
