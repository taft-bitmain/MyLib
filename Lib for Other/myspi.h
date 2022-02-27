/*******************************************************************************
 * @file     myspi.h
 * @brief    simulation spi interface
 * @version  V1.2
 * @date     2022.2.27
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
    
EXAMPLE CODE:
	myspi hspi = {
        .SCK_port  = GPIOx, .SCK_pin  = GPIO_PIN_x,
        .CS_port   = GPIOx, .CS_pin   = GPIO_PIN_x,
        .MOSI_port = GPIOx, .MOSI_pin = GPIO_PIN_x,
        .MISO_port = NULL,  .MISO_pin = 0,
        .CPOL = 0, .CPHA = 0
    };
    myspi_io_init(&hspi);
    myspi_write_reg(&hspi,0x12,0x34);

*******************************************************************************/
#ifndef __MYSPI_H
#define __MYSPI_H
#ifdef __cplusplus
extern "c"
{
#endif


#include "stdint.h"
#include "stm32f1xx.h"

typedef struct{
    GPIO_TypeDef*   SCK_port;
    uint32_t		SCK_pin;
    GPIO_TypeDef*	CS_port;
    uint32_t		CS_pin;
    GPIO_TypeDef*	MOSI_port;
    uint32_t		MOSI_pin;
    GPIO_TypeDef*	MISO_port;
    uint32_t		MISO_pin;
    uint8_t			CPOL;
    uint8_t			CPHA;
} myspi;

/******************* Basic Functions **************************/

void        myspi_delay             ( void );

void        myspi_io_init           ( myspi * hspi );

void        myspi_transceive_start  ( myspi * hspi );
void        myspi_transmit_byte     ( myspi * hspi, uint8_t dat );
uint8_t     myspi_receive_byte      ( myspi * hspi );
uint8_t     myspi_transceive_byte   ( myspi * hspi, uint8_t dat );
void        myspi_transceive_end    ( myspi * hspi );

/******************* Main Functions **************************/

void        myspi_write_byte        ( myspi * hspi, uint8_t addr, uint8_t dat);
void        myspi_write_bytes       ( myspi * hspi, uint8_t addr, const uint8_t * dat, uint32_t len );
uint8_t     myspi_read_byte         ( myspi * hspi, uint8_t addr );
void        myspi_read_bytes        ( myspi * hspi, uint8_t addr, uint8_t * dat, uint32_t len );

/******************* Extend Functions **************************/

void        myspi_write             ( myspi * hspi, uint8_t * dat, uint32_t len );
void        myspi_read              ( myspi * hspi, uint8_t * dat, uint32_t len );
void        myspi_write_read        ( myspi * hspi, uint8_t * dat_in,uint8_t * dat_out, uint32_t len);


#ifdef __cplusplus
}
#endif
#endif
