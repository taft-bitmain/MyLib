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
	static uint32_t red, ir;
    
    if( MAX30102_Reset() )
        myprintf("MAX30102_Reset OK\r\n");
    if( MAX30102_Init() )
        myprintf("MAX30102_Init OK\r\n");
    
    while( 1 )
    {
        // check the interrupt pin
        while( HAL_GPIO_ReadPin( GPIOB, GPIO_PIN_8 ) );
        if( MAX30102_ReadFIFO( &red, &ir ) )
        {
            myprintf( "ch1=%d,", red );
            myprintf( "ch2=%d,", ir );
        }
    }
*******************************************************************************/
#ifndef __MAX30102_H
#define __MAX30102_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"

/***************** Basic Interface *************************/
#include "myi2c.h"

#define     MAX30102_I2CADDR    0xAE 

static MyI2C max30102_i2c = {
	.SCL_Port = GPIOB,.SCL_Bit = GPIO_PIN_10,
	.SDA_Port = GPIOB,.SDA_Bit = GPIO_PIN_0,
	.Speed = 2 ,.DevAddr = MAX30102_I2CADDR
};

uint8_t     MAX30102_ReadReg     ( uint8_t regaddr );
uint8_t     MAX30102_ReadRegs    ( uint8_t regaddr,uint8_t * pdat,uint16_t len );
uint8_t     MAX30102_WriteReg    ( uint8_t regaddr,uint8_t dat );
void        MAX30102_Delayms     ( uint16_t ms );

/****************** Main Functions *************************/

uint8_t     MAX30102_Init        ( void );
uint8_t     MAX30102_Reset       ( void );
uint8_t     MAX30102_ReadFIFO    ( uint32_t * pDataRed, uint32_t * pDataIR );

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
