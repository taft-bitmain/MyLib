/*******************************************************************************
 * @file     myi2c.h
 * @brief    simulation i2c interface
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

    static MyI2C i2c1 = {
        .SCL_Port = GPIOB,.SCL_Bit = GPIO_PIN_10,
        .SDA_Port = GPIOB,.SDA_Bit = GPIO_PIN_0,
        .Speed = 1 ,.DevAddr = 0x00
    };
    
	if( MyI2C_Detect(&i2c1) && i2c1.DevAddr != 0 )
        myprintf("Detect: 0x%X\r\n",i2c1.DevAddr);
    else
        myprintf("Detect fail\r\n");
    
*******************************************************************************/
#ifndef __MYI2C_H
#define __MYI2C_H
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
}MyI2C;


void	    MyI2C_Start         (MyI2C *hi2c);
uint8_t	    MyI2C_Transmit      (MyI2C *hi2c,uint8_t dat);
uint8_t	    MyI2C_Receive       (MyI2C *hi2c,uint8_t ack);
void	    MyI2C_End           (MyI2C *hi2c);

uint8_t	    MyI2C_WriteReg      (MyI2C *hi2c,uint8_t RegAddr,uint8_t dat);
uint8_t	    MyI2C_ReadReg       (MyI2C *hi2c,uint8_t RegAddr);
uint8_t	    MyI2C_WriteReg2     (MyI2C *hi2c,uint8_t RegAddr,uint16_t dat);
uint16_t    MyI2C_ReadReg2      (MyI2C *hi2c,uint8_t RegAddr);

uint8_t	    MyI2C_WriteMem      (MyI2C *hi2c,uint8_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	    MyI2C_ReadMem       (MyI2C *hi2c,uint8_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	    MyI2C_WriteMem2     (MyI2C *hi2c,uint16_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	    MyI2C_ReadMem2      (MyI2C *hi2c,uint16_t MemAddr,uint8_t *dat,uint16_t len);
uint8_t	    MyI2C_WriteByte     (MyI2C *hi2c,uint8_t dat);
uint8_t	    MyI2C_ReadByte      (MyI2C *hi2c);
uint8_t	    MyI2C_WriteBytes    (MyI2C *hi2c,uint8_t *dat,uint16_t len);
uint8_t	    MyI2C_ReadBytes     (MyI2C *hi2c,uint8_t *dat,uint16_t len);

uint8_t	    MyI2C_Detect        (MyI2C *hi2c);

#ifdef __cplusplus
}
#endif
#endif
