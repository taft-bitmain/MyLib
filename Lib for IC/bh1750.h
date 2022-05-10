/*******************************************************************************
 * @file     bh1750.h
 * @brief
 * @version  V1.0
 * @date     2022.5.9
 * @author   auther email@example.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE:
	
*******************************************************************************/
#ifndef __BH1750_H
#define __BH1750_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"
#include "stm32f1xx.h"


#define BH1750_I2C_ADDR      0x23 // or 0x5C when ADDR = 1

//#define BH1750_I2C_SOFTWARE
#define BH1750_I2C_HARDWARE

/***************** Basic Interface *************************/

static inline void BH1750_DelayMs ( uint16_t ms )
{
    HAL_Delay(ms);
}

#ifdef BH1750_I2C_SOFTWARE /* software i2c */

#include "myi2c.h"

static myi2c aht10_i2c = {
	.SCL_port = GPIOB,.SCL_pin = GPIO_PIN_0,
	.SDA_port = GPIOB,.SDA_pin = GPIO_PIN_1,
	.speed = 2 ,.slaver_addr = BH1750_I2C_ADDR
};

static inline uint8_t BH1750_I2C_Write ( uint8_t * pdatain, uint8_t len )
{
    return myi2c_write( &aht10_i2c, pdatain, len );
}

static inline uint8_t BH1750_I2C_Read ( uint8_t * pdataout, uint8_t len )
{
    return myi2c_read( &aht10_i2c, pdataout, len );
}

#endif


#ifdef BH1750_I2C_HARDWARE   /* hardware i2c */

#define BH1750_I2C_HANDLE        hi2c1

#include "i2c.h"

static inline uint8_t BH1750_I2C_Write ( uint8_t datain )
{
    return HAL_I2C_Master_Transmit(&BH1750_I2C_HANDLE,BH1750_I2C_ADDR<<1,&datain,1,500);
}

static inline uint8_t BH1750_I2C_Read ( uint8_t * pdataout, uint8_t len )
{
    return HAL_I2C_Master_Receive(&BH1750_I2C_HANDLE,BH1750_I2C_ADDR<<1,pdataout,len,500);
}

#endif

/****************** Main Functions *************************/


uint8_t     BH1750_Init      ( void );  

uint16_t    BH1750_Get       ( void );


/****************** Command Table ***************************/

#define     BH1750_CMD_PWROFF       0x00
#define     BH1750_CMD_PWRON        0x01
#define     BH1750_CMD_RESET        0x07

#define     BH1750_CMD_CHRES        0x10
#define     BH1750_CMD_CHRES2       0x11
#define     BH1750_CMD_CLRES        0x13

#define     BH1750_CMD_SHRES        0x20
#define     BH1750_CMD_SHRES2       0x21
#define     BH1750_CMD_SLRES        0x23

#define     BH1750_CMD_MTH          0x40
#define     BH1750_CMD_MTH_MASK     0x03
#define     BH1750_CMD_MTL          0x60
#define     BH1750_CMD_MTL_MASK     0x1F

#ifdef __cplusplus
}
#endif
#endif
