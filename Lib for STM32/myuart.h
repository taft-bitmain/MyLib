/*******************************************************************************
 * @file     myuart.h
 * @brief    High performance to operate UART with DMA
 * @version  V1.1
 * @date     2021.5.11
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 *******************************************************************************/
/*******************************************************************************
TIPS:
1. 如果你的串口数据量不是特别大的话，请不要使用这个库，否则你将是大炮
   打蚊子，还打不死的那种
2. 使用时，需要在cubeMX中配置相应的串口：
    - 使能串口
    - 使能中断（IDLE需要用到）
    - 使能相应的DMA通道（保持默认配置即可）
3. 使用DMAT功能时，需要：
    - 打开宏定义
    - 在myuart.c中声明MyUart_BufList变量，在myuart.h中extern
    - 根据你的MyUart_BufList和USART的映射关系，修改或者增加myuart.c中
      回调函数应该填入的MyUart_BufList参数
4. 使用DMAR功能时，需要：
    - 打开宏定义
    - 在myuart.c中声明MyUart_BufList变量，在myuart.h中extern
    - 根据你的MyUart_BufList和USART的映射关系，在stm32f1xx_it.c的相应
      的串口中断中(USARTx_IRQHandler),先调用MyUart_IDLEHandler(),参数
      根据你的串口而定
      
EXAMPLE CODE:

  MyUart_Init(&BL4UART1,&huart1);//初始化
  
  MyUart_Printf(&BL4UART1,"Hello %s!\r\n","world");//以printf格式打印数据
  MyUart_Printf(&BL4UART1,"This is a uart echo program\r\n");
  MyUart_Flush(&BL4UART1);//刷新发送数据，每次发送数据都需要调用
  
  int count;
  while (1)
  {
    if(BL4UART1.rcount)//如果接收到数据
    {
      BL4UART1.rcount = 0;//次数清零
      count = MyUart_Available(&BL4UART1);//获取接收到的数据长度
      MyUart_Dump(&BL4UART1,data,count);//读取指定长数据到接收到的数组
      MyUart_Trans(&BL4UART1,data,count);//发送数据
      MyUart_Flush(&BL4UART1);//刷新发送缓冲区，立即发送
    }
  }

*******************************************************************************/
#ifndef __MYUART_H
#define __MYUART_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/*************************** 功能宏定义和结构体声明 ***********************/
#define	  MYUART_DMAT       1
#define		MYUART_DMAR       1
//最大发送和接收长度，根据你的情况修改下列定义
#define   MYUART_MAX_TRANS	64
#define   MYUART_MAX_REC		64
//节点的长度和个数，根据你的情况修改下列定义
#define   MYUART_NODESIZE		1024
#define   MYUART_TXNODE		  3
#define   MYUART_RXNODE  	  3

struct MyUart_BufNode{
	char data[MYUART_NODESIZE];
	volatile unsigned int count;
	volatile unsigned int size;
	volatile char state;
	struct MyUart_BufNode *next;
	struct MyUart_BufNode *prev;
};

typedef struct {
  UART_HandleTypeDef *huart;
#if	(MYUART_DMAT == 1)
  struct MyUart_BufNode TBufList[MYUART_TXNODE];
  struct MyUart_BufNode *write;
  struct MyUart_BufNode *trans;
#endif
#if	(MYUART_DMAR == 1)
  struct MyUart_BufNode RBufList[MYUART_RXNODE];
  struct MyUart_BufNode *read;
  struct MyUart_BufNode *rec;
  volatile unsigned int rcount;
#endif
}MyUart_BufList;

/*************************** 变量及函数声明 ***********************/

extern  MyUart_BufList      BL4UART1;

void		MyUart_Init	        (MyUart_BufList *BL,UART_HandleTypeDef *huart);

/**************************** DMAT ****************************/
#if	(MYUART_DMAT == 1)

void		MyUart_DMAT_Init	  (MyUart_BufList *BL);
void    MyUart_DMAT_Callback(MyUart_BufList *BL);
int			MyUart_Printf	      (MyUart_BufList *BL,const char * format, ...);
int			MyUart_Trans		    (MyUart_BufList *BL,const char *data,unsigned int len);
void		MyUart_Flush	      (MyUart_BufList *BL);

#endif

/**************************** DMAR ***************************/
#if	(MYUART_DMAR == 1)

void		MyUart_DMAR_Init    (MyUart_BufList *BL);
void		MyUart_IDLEHandler  (MyUart_BufList *BL);
int			MyUart_Available	  (MyUart_BufList *BL);
int			MyUart_Dump	        (MyUart_BufList *BL,char *data,unsigned int len);

#endif


#ifdef __cplusplus
}
#endif
#endif
