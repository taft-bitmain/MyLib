/*******************************************************************************
 * @file     max30102.h
 * @brief
 * @version  V1.0
 * @date     2022.2.23
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE:

    uint16_t LED_Data[1024];
    uint16_t IR_Data[1024];

	static uint32_t red, ir;
    
    MAX30102_Reset();
    MAX30102_Init();
    
    while( 1 )
    {
        static uint16_t size;

        MAX30102_Sample( LED_Data, IR_Data, 1000, &size );
        if(size == 1000)
        {
            for(uint16_t i = 0; i < size ; i++)
            myprintf("LED=%d,IR=%d,",LED_Data[i],IR_Data[i]);
            size = 0;
        }
    }
*******************************************************************************/
#ifndef __MAX30102_H
#define __MAX30102_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"

#define MAX30102_I2C_ADDR      (0x57)

#define MAX30102_I2C_SOFTWARE
//#define MAX30102_I2C_HARDWARE

/***************** Basic Interface *************************/

#ifdef MAX30102_I2C_SOFTWARE /* software i2c */

#include "myi2c.h"

static myi2c max30102_i2c = {
	.SCL_port = GPIOB,.SCL_pin = GPIO_PIN_6,
	.SDA_port = GPIOB,.SDA_pin = GPIO_PIN_7,
	.speed = 2 ,.slaver_addr = MAX30102_I2C_ADDR
};


static inline uint8_t MAX30102_ReadReg( uint8_t regaddr )
{
    uint8_t dat;
    myi2c_read_byte(&max30102_i2c, regaddr , 1 , &dat );
    return dat;
}

static inline uint8_t MAX30102_ReadRegs( uint8_t regaddr,uint8_t * pdat,uint16_t len )
{
    return myi2c_read_bytes(&max30102_i2c, regaddr , 1 , pdat, len);
}

static inline uint8_t MAX30102_WriteReg( uint8_t regaddr,uint8_t dat )
{
    return myi2c_write_byte(&max30102_i2c, regaddr , 1 , dat );
}


#endif

#ifdef MAX30102_I2C_HARDWARE   /* hardware i2c */

#define MAX30102_I2C_HANDLE        hi2c1

#include "i2c.h"

static inline uint8_t MAX30102_ReadReg( uint8_t regaddr )
{
    uint8_t dat;
    return HAL_I2C_Mem_Read( &MAX30102_I2C_HANDLE , MAX30102_I2C_ADDR<<1, regaddr, I2C_MEMADD_SIZE_8BIT , &dat, 1, 500 ) == HAL_OK ? dat : 0x00;
}

static inline uint8_t MAX30102_ReadRegs( uint8_t regaddr,uint8_t * pdat,uint16_t len )
{
    return HAL_I2C_Mem_Read( &MAX30102_I2C_HANDLE , MAX30102_I2C_ADDR<<1, regaddr, I2C_MEMADD_SIZE_8BIT , pdat, len, 500 ) == HAL_OK ? 1 : 0;
}

static inline uint8_t MAX30102_WriteReg( uint8_t regaddr,uint8_t dat )
{
    return HAL_I2C_Mem_Write( &MAX30102_I2C_HANDLE , MAX30102_I2C_ADDR<<1, regaddr, I2C_MEMADD_SIZE_8BIT , &dat, 1, 500 ) == HAL_OK ? 1 : 0;
}


#endif


static inline void MAX30102_Delayms ( uint16_t ms )
{
    HAL_Delay(ms);
}

/****************** Main Functions *************************/

uint8_t     MAX30102_Init        ( void );
uint8_t     MAX30102_Reset       ( void );
uint8_t     MAX30102_ReadFIFO    ( uint16_t * pDataRed, uint16_t * pDataIR );

// Call this function frequently, base on the sample rate and fifo size
uint16_t    MAX30102_Sample      ( uint16_t * pDataRed, uint16_t * pDataIR, uint16_t count, uint16_t *size  );

/***************** Registers Definition ********************/

#define     MAX30102_INT_STA1            0x00
#define     MAX30102_INT_STA2            0x01
#define     MAX30102_INT_EN1             0x02
#define     MAX30102_INT_EN2             0x03
#define     MAX30102_FIFO_WR_PTR         0x04
#define     MAX30102_FIFO_OVF_CNT        0x05
#define     MAX30102_FIFO_RD_PTR         0x06
#define     MAX30102_FIFO                0x07

#define     MAX30102_FIFO_CFG            0x08
#define     MAX30102_MODE_CFG            0x09
#define     MAX30102_SpO2_CFG            0x0A
#define     MAX30102_LED1_PA             0x0C
#define     MAX30102_LED2_PA             0x0D
#define     MAX30102_PILOT_PA            0x10
#define     MAX30102_MUL_MODE12          0x11
#define     MAX30102_MUL_MODE34          0x12

#define     MAX30102_TEMP_INT            0x1F
#define     MAX30102_TEMP_FRAC           0x20
#define     MAX30102_TEMP_CFG            0x21

#define     MAX30102_PROX_INT_THRESH     0x30

#define     MAX30102_REV_ID              0xFE
#define     MAX30102_PART_ID             0xFF

#ifdef __cplusplus
}
#endif
#endif
