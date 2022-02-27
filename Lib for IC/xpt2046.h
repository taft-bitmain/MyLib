/*******************************************************************************
 * @file     xpt2046.h
 * @brief
 * @version  V1.0
 * @date     2022.2.27
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1. xpt2046 spi clock frequency max is 2Mhz
EXAMPLE:

    XPT2046_Init();
    
    while ( 1 )
    {
        static uint16_t x, y, pressure;
        
        while( XPT2046_LL_IRQ() == 1 );
        
        XPT2046_GetPosition( &x, &y );
        pressure = XPT2046_GetPressure();
        
        myprintf("x = %d , y = %d\r\n", x, y );
        myprintf("pressure = %d\r\n", pressure );
        
        HAL_Delay(20);
    }
	
*******************************************************************************/
#ifndef __XPT2046_H
#define __XPT2046_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"

/****************** Basic Interface *************************/
 
#include "myspi.h"


static myspi xpt2046_spi = {
	.SCK_port  = GPIOB, .SCK_pin  = GPIO_PIN_7,
    .CS_port   = GPIOB, .CS_pin   = GPIO_PIN_6,
    .MOSI_port = GPIOB, .MOSI_pin = GPIO_PIN_5,
    .MISO_port = GPIOB, .MISO_pin = GPIO_PIN_4,
    .CPOL = 0, .CPHA = 0
};

#define     XPT2046_LL_IRQ()	    (( GPIOB->IDR  & GPIO_PIN_3 ) ? 1 : 0 )

void        XPT2046_LL_Init         ( void );
uint16_t    XPT2046_LL_Transceive   ( uint8_t cmd );

/******************* Main Functions **************************/

void       XPT2046_Init          ( void );
void       XPT2046_GetPosition   ( uint16_t * x, uint16_t * y );
uint16_t   XPT2046_GetPressure   ( void );


/******************* Command Definations *********************/

#define     XPT2046_CMD_RDX     0XD0
#define     XPT2046_CMD_RDY     0X90
#define     XPT2046_CMD_RDZ1    0XB0
#define     XPT2046_CMD_RDZ2    0XC0

#ifdef __cplusplus
}
#endif
#endif
