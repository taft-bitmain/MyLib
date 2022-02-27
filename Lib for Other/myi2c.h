/*******************************************************************************
 * @file     myi2c.h
 * @brief    simulation i2c interface
 * @version  V1.4
 * @date     2022.2.27
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

    static myi2c i2c1 = {
        .SCL_port = GPIOx,.SCL_pin = GPIO_PIN_x,
        .SDA_port = GPIOx,.SDA_pin = GPIO_PIN_x,
        .Speed = 1 ,.DevAddr = 0x00
    };
    
	if( myi2c_detect(&i2c1) )
        myprintf("Detect: 0x%X\r\n",i2c1.slaver_addr);
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
    GPIO_TypeDef* SCL_port;
    uint32_t      SCL_pin;
    GPIO_TypeDef* SDA_port;
    uint32_t      SDA_pin;
    uint16_t      speed;
    uint8_t       slaver_addr; // 7 bits
}myi2c;

                                          
void	    myi2c_start         ( myi2c * hi2c );
uint8_t	    myi2c_transmit      ( myi2c * hi2c, uint8_t dat );
uint8_t	    myi2c_receive       ( myi2c * hi2c, uint8_t ack );
void	    myi2c_end           ( myi2c * hi2c );

uint8_t	    myi2c_write_byte   ( myi2c * hi2c, uint16_t addr, uint8_t addr_len, uint8_t dat );
uint8_t	    myi2c_write_bytes  ( myi2c * hi2c, uint16_t addr, uint8_t addr_len, const uint8_t * dat, uint32_t len );
uint8_t	    myi2c_read_byte    ( myi2c * hi2c, uint16_t addr, uint8_t addr_len, uint8_t * dat );
uint8_t	    myi2c_read_bytes   ( myi2c * hi2c, uint16_t addr, uint8_t addr_len, uint8_t * dat, uint32_t len );

uint8_t	    myi2c_detect        ( myi2c * hi2c );

#ifdef __cplusplus
}
#endif
#endif
