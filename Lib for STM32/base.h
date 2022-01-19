/*******************************************************************************
 * @file     base.h
 * @brief    Basic operations, such as uart, detecting key delection and lighting the LED.
 * @version  V1.2
 * @date     2021.7.4
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1.使用 LED 和 KEY 功能时
      - 提前配置好IO，并且Enter your label。
	2.使用 KEY_TIMER 功能时
      - 需要提前配置好IO，Enter your label
      - 调用KEY_SET_PIN进行初始化。
  3.使用 USART_PRINTF 功能时
      - 配置相应的串口
      - 修改宏定义为你使用的串口
  4.使用 USART_DMA_TRANSMIT 功能时
      - 配置相应的串口
      - 打开相应的DMA通道
  5.使用 BASE_USART_DMA_RECEIVE 功能时
      - 配置相应的串口
      - 打开相应的DMA通道
      - 使能串口全局中断（接收需要用到IDLE中断）
      - 调用 UART_IDLE_Init() 进行初始化
      - 在相应的中断函数中添加IDLE的回调函数 UART_IDLE_Handler()
*******************************************************************************/
#ifndef __BASE_H
#define __BASE_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "main.h"
#include "usart.h"

/********************* 功能启动宏定义 ****************************/
#define		BASE_IO              		1
#define		BASE_LED              		0
#define		BASE_KEY              		0
#define		BASE_KEY_TIMER         		0
#define		BASE_USART_PRINTF     		1
#define		BASE_USART_DMA_TRANSMIT		0
#define		BASE_USART_DMA_RECEIVE		0

//dma usart 缓冲Buffer长度
#define USART_DMA_BUFFER_LEN         1024

//DMA串口发送功能启用
#if (BASE_USART_DMA_TRANSMIT == 1 )
#define USART1_DMA_TRANSMIT          0
#define USART2_DMA_TRANSMIT          0
#define USART3_DMA_TRANSMIT          0
#define USART4_DMA_TRANSMIT          0
#endif
//DMA串口接收功能启用
#if (BASE_USART_DMA_RECEIVE == 1)
#define USART1_DMA_RECEIVE           0
#define USART2_DMA_RECEIVE           0
#define USART3_DMA_RECEIVE           0
#define USART4_DMA_RECEIVE           0
#endif

#if (BASE_IO == 1)

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


//使用下面宏定义，可以将CubeMX定义的IO名字转化为Port和Pin
#define IO_Name2Port(x)			x##_GPIO_Port
#define IO_Name2Pin(x)			x##_Pin
#define SetPin(x,y)             HAL_GPIO_WritePin(IO_Name2Port(x),IO_Name2Pin(x),y?GPIO_PIN_SET:GPIO_PIN_RESET)
#define GetPin(x)               HAL_GPIO_ReadPin(IO_Name2Port(x),IO_Name2Pin(x))
#define TogglePin(x)            HAL_GPIO_TogglePin(IO_Name2Port(x),IO_Name2Pin(x))
#endif

/************************* LED ****************************/
#if (BASE_LED == 1)
#define LED(x,y)				HAL_GPIO_WritePin(IO_Name2Port(x),IO_Name2Pin(x),y?GPIO_PIN_RESET:GPIO_PIN_SET)
#define LEDON(x)				HAL_GPIO_WritePin(IO_Name2Port(x),IO_Name2Pin(x),GPIO_PIN_RESET)
#define LEDOFF(x)				HAL_GPIO_WritePin(IO_Name2Port(x),IO_Name2Pin(x),GPIO_PIN_SET)
#define LEDTOGGLE(x)		    HAL_GPIO_TogglePin(IO_Name2Port(x),IO_Name2Pin(x))
#endif

/************************* Key ****************************/
#if (BASE_KEY == 1)
#define KEY_PU_PRESS(x) 	iSPin_101(IO_Name2Port(x),IO_Name2Pin(x))
#define KEY_PD_PRESS(x) 	iSPin_010(IO_Name2Port(x),IO_Name2Pin(x))
char		iSPin_10(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
char		iSPin_01(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
char		iSPin_101(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
char		iSPin_010(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
#endif

/********************* Key Timer Interrupt *********************/
/*
EXAMPLE CODE:
		Key_TypeDef mykey1;
		KEY_SET_PIN(mykey1,K1,1);
		if(mykey1.Press)
		{
			mykey1.Press = 0;
			LED(D1,0);
		}
*/
#if (BASE_KEY_TIMER == 1)

//去抖动时间阈值和长短按时间阈值，以中断周期单位
#define KEY_SHORT_COUNT    10
#define KEY_LONG_COUNT    300

typedef struct {
	GPIO_TypeDef          *Port;
	uint16_t              Pin;
	uint8_t               PP;
	__IO uint16_t         Count;
	uint16_t              CountTmp;
	uint8_t               PrevState;
	__IO uint8_t          Press;
	__IO uint8_t          Relase;
}Key_TypeDef;

//设置按键的属性
#define KEY_SET_PIN(key,io,pp)     Key_SetPin(&key,IO_Name2Port(io),IO_Name2Pin(io),pp)
void 		Key_SetPin        (Key_TypeDef *key ,GPIO_TypeDef *Port,uint16_t Pin,uint16_t PullUpDown);
//按键按下检测
uint16_t 	Key_IsPress       (Key_TypeDef *key);
//按键释放检测 返回  1->短按  2->长按
uint16_t 	Key_IsRelase      (Key_TypeDef *key);

//将下面的函数放入你的定时器中断中，定时器中断频率1KHz最佳
void		Key_TimerHandle   (Key_TypeDef *key);

#endif


#if (BASE_USART_PRINTF == 1)
int Myprintf(const char * format, ...);

#endif

/****************** usart dma transmit **************************/
#if (BASE_USART_DMA_TRANSMIT == 1)

//下面是发送缓冲Buffer
#if (USART1_DMA_TRANSMIT == 1)
extern uint8_t USART1_TxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART2_DMA_TRANSMIT == 1)
extern uint8_t USART2_TxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART3_DMA_TRANSMIT == 1)
extern uint8_t USART3_TxBuffer[USART_DMA_BUFFER_LEN];
#endif
#if (USART4_DMA_TRANSMIT == 1)
extern uint8_t USART4_TxBuffer[USART_DMA_BUFFER_LEN];
#endif

//使用类似printf函数的格式
void UART_DMA_Transmintf(UART_HandleTypeDef *huart,const char * format, ...);
#endif

/****************** usart dma receive ***************************/
#if (BASE_USART_DMA_RECEIVE == 1)

//下面是接收缓冲Buffer和中断回调函数（WEAK）
#if (USART1_DMA_RECEIVE == 1)
extern uint8_t USART1_RxBuffer[USART_DMA_BUFFER_LEN];
void USART1_IDLE_Callback(uint8_t *data,uint16_t len);
#endif
#if (USART2_DMA_RECEIVE == 1)
extern uint8_t USART2_RxBuffer[USART_DMA_BUFFER_LEN];
void USART2_IDLE_Callback(uint8_t *data,uint16_t len);
#endif
#if (USART3_DMA_RECEIVE == 1)
extern uint8_t USART3_RxBuffer[USART_DMA_BUFFER_LEN];
void USART3_IDLE_Callback(uint8_t *data,uint16_t len);
#endif
#if (USART4_DMA_RECEIVE == 1)
extern uint8_t USART4_RxBuffer[USART_DMA_BUFFER_LEN];
void USART4_IDLE_Callback(uint8_t *data,uint16_t len);
#endif

//使用前先初始化
void UART_IDLE_Init(void);

//将这个函数放入串口中断中
void UART_IDLE_Handler(UART_HandleTypeDef *huart);

#endif


#ifdef __cplusplus
}
#endif
#endif
