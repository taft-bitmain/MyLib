/*******************************************************************************
 * @file     myiic.h
 * @brief    simulation iic interface
 * @version  V1.3
 * @date     2021.8.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
    1. Using the STM32CubeMX to config pin (SCL and SDA) as
        output level            : High
        mode                    : Output Open Drain
        Pull-up/Poll-down       : Pull-up
        Maximum output speed    : Low or Medium
        
    2. Speed parameter dexcription:
        when the SYSCLK is 48MHz,
			Speed =  0, SCL frequeny is about 500 Khz
			Speed =  1, SCL frequeny is about 190 Khz
			Speed =  5, SCL frequeny is about  53 Khz
            Speed = 10, SCL frequeny is about  28 Khz
		
EXAMPLE CODE:

MyIIC myiic1 = {
	.SCL_Port = GPIOG,.SCL_Bit = GPIO_PIN_2,
	.SDA_Port = GPIOG,.SDA_Bit = GPIO_PIN_4,
	.Speed = 1 ,.DevAddr = 0x12
};
	MyIIC_Detect(&myiic1);
	MyIIC_WriteReg(&myiic1,0x34,0x56);
    
*******************************************************************************/
#ifndef __MYIIC_H
#define __MYIIC_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx.h"

typedef struct{
    GPIO_TypeDef* SCL_Port;
    uint32_t      SCL_Bit;
    GPIO_TypeDef* SDA_Port;
    uint32_t      SDA_Bit;
    uint16_t      Speed;
    uint8_t       DevAddr;
}MyIIC;


void	    MyIIC_Start         (MyIIC *hiic);
uint8_t	    MyIIC_Transmit      (MyIIC *hiic,uint8_t dat);
uint8_t	    MyIIC_Receive       (MyIIC *hiic,uint8_t ack);
void	    MyIIC_End           (MyIIC *hiic);

uint8_t	    MyIIC_WriteReg      (MyIIC *hiic,uint8_t RegAddr,uint8_t dat);
uint8_t	    MyIIC_ReadReg       (MyIIC *hiic,uint8_t RegAddr);
uint8_t	    MyIIC_WriteReg2     (MyIIC *hiic,uint8_t RegAddr,uint16_t dat);
uint16_t    MyIIC_ReadReg2      (MyIIC *hiic,uint8_t RegAddr);

uint8_t	    MyIIC_WriteMem      (MyIIC *hiic,uint8_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	    MyIIC_ReadMem       (MyIIC *hiic,uint8_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	    MyIIC_WriteMem2     (MyIIC *hiic,uint16_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	    MyIIC_ReadMem2      (MyIIC *hiic,uint16_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	    MyIIC_WriteByte     (MyIIC *hiic,uint8_t dat);
uint8_t	    MyIIC_ReadByte      (MyIIC *hiic);
uint8_t	    MyIIC_WriteBytes    (MyIIC *hiic,uint8_t *dat,uint16_t len);
uint8_t	    MyIIC_ReadBytes     (MyIIC *hiic,uint8_t *dat,uint16_t len);

uint8_t	    MyIIC_Detect        (MyIIC *hiic);

#ifdef __cplusplus
}
#endif
#endif
