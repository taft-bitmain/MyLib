/*******************************************************************************
 * @file     aht10.h
 * @brief    Drive the IC aht10
 * @version  V1.0
 * @date     2022.2.8
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE:
    static uint32_t rh,temp;
    AHT10_Trigger();
    HAL_Delay(100);
    AHT10_MeasureGet(&rh,&temp);
    myprintf("aht10 read :%d %d\r\n",rh*100/0xfffff,temp*200/0xfffff-50);
	
*******************************************************************************/
#ifndef __AHT10_H
#define __AHT10_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"
#include "stm32f1xx.h"

/***************** Basic Interface *************************/

#include "myi2c.h"

#define AHT10_I2C_ADDR      0x38

static MyI2C aht10_i2c = {
	.SCL_Port = GPIOB,.SCL_Bit = GPIO_PIN_10,
	.SDA_Port = GPIOB,.SDA_Bit = GPIO_PIN_0,
	.Speed = 2 ,.DevAddr = AHT10_I2C_ADDR << 1
};

uint8_t     AHT10_I2C_Write ( uint8_t * pdatain, uint8_t len );
uint8_t     AHT10_I2C_Read  ( uint8_t * pdataout, uint8_t len );
void        AHT10_DelayMs   ( uint16_t ms );

/****************** Main Functions *************************/

uint8_t     AHT10_Init      ( void );
uint8_t     AHT10_SoftReset ( void );
uint8_t     AHT10_Trigger   ( void );
uint8_t     AHT10_MeasureGet( uint32_t *RH , uint32_t *Temp  );   // wait for 80 ms at least after trigger



#ifdef __cplusplus
}
#endif
#endif
