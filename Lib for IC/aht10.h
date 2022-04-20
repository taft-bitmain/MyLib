/*******************************************************************************
 * @file     aht10.h
 * @brief    Drive the IC aht10
 * @version  V1.1
 * @date     2022.4.17
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE:

AHT10_Init();

    AHT10_Trigger();
    HAL_Delay(100);
    AHT10_MeasureGet(&rh,&temp);
	
*******************************************************************************/
#ifndef __AHT10_H
#define __AHT10_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"
#include "stm32f1xx.h"

#define AHT10_I2C_ADDR      0x38

#define AHT10_I2C_SOFTWARE
//#define AHT10_I2C_HARDWARE

/***************** Basic Interface *************************/

static inline void AHT10_DelayMs ( uint16_t ms )
{
    HAL_Delay(ms);
}

#ifdef AHT10_I2C_SOFTWARE /* software i2c */

#include "myi2c.h"

static myi2c aht10_i2c = {
	.SCL_port = GPIOB,.SCL_pin = GPIO_PIN_0,
	.SDA_port = GPIOB,.SDA_pin = GPIO_PIN_1,
	.speed = 2 ,.slaver_addr = AHT10_I2C_ADDR
};

static inline uint8_t AHT10_I2C_Write ( uint8_t * pdatain, uint8_t len )
{
    return myi2c_write( &aht10_i2c, pdatain, len );
}

static inline uint8_t AHT10_I2C_Read ( uint8_t * pdataout, uint8_t len )
{
    return myi2c_read( &aht10_i2c, pdataout, len );
}

#endif


#ifdef AHT10_I2C_HARDWARE   /* hardware i2c */

#define AHT10_I2C_HANDLE        hi2cx

#include "i2c.h"

static inline uint8_t AHT10_I2C_Write ( uint8_t * pdatain, uint8_t len )
{
    return HAL_I2C_Master_Transmit(&AHT10_I2C_HANDLE,AHT10_I2C_ADDR<<1,pdatain,len,500);
}

static inline uint8_t AHT10_I2C_Read ( uint8_t * pdataout, uint8_t len )
{
    return HAL_I2C_Master_Receive(&AHT10_I2C_HANDLE,AHT10_I2C_ADDR<<1,pdataout,len,500);
}

#endif

/****************** Main Functions *************************/

uint8_t     AHT10_Init      ( void );                           // return 0 if initial successfully
uint8_t     AHT10_SoftReset ( void );
uint8_t     AHT10_Trigger   ( void );
uint8_t     AHT10_MeasureGet( uint8_t *RH , uint8_t *Temp  );   // wait for 80 ms at least after trigger
uint8_t     AHT10_MeasureGetf   ( float *RH , float *Temp );


#ifdef __cplusplus
}
#endif
#endif
