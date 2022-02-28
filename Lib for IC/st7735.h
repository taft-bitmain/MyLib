/*******************************************************************************
 * @file     st7735.h
 * @brief
 * @version  V1.0
 * @date     2022.2.28
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE:

	ST7735_Init();
    ST7735_Clear(COLOR_WHITE);
    ST7735_LL_Delay(500);
    ST7735_Clear(COLOR_BLACK);
    ST7735_LL_Delay(500);
    ST7735_Clear(COLOR_RED  );
    ST7735_LL_Delay(500);
    ST7735_Clear(COLOR_GREEN);
    ST7735_LL_Delay(500);
    ST7735_Clear(COLOR_BLUE );
    ST7735_LL_Delay(500);
    
*******************************************************************************/
#ifndef __ST7735_H
#define __ST7735_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"


#define     ST7735_COL_MAX       80
#define     ST7735_ROW_MAX       160

// for 0.96' 80*160
#define     ST7735_COL_OFFSET    26
#define     ST7735_ROW_OFFSET    1

/****************** Basic Interface *************************/

#include "myspi.h"

#define ST7735_LL_PinRST(x)         ( x ? (GPIOB->BSRR = GPIO_PIN_4) : (GPIOB->BRR = GPIO_PIN_4) )
#define ST7735_LL_PinDC(x)          ( x ? (GPIOB->BSRR = GPIO_PIN_7) : (GPIOB->BRR = GPIO_PIN_7) )
#define ST7735_LL_PinBLK(x)         ( x ? (GPIOB->BSRR = GPIO_PIN_3) : (GPIOB->BRR = GPIO_PIN_3) )

static myspi st7735_spi =
{
    .SCK_port  = GPIOB, .SCK_pin  = GPIO_PIN_8,
    .CS_port   = GPIOB, .CS_pin   = GPIO_PIN_5,
    .MOSI_port = GPIOB, .MOSI_pin = GPIO_PIN_6,
    .MISO_port = NULL,  .MISO_pin = 0,
    .CPOL = 0, .CPHA = 0
};


void    ST7735_LL_Init              ( void );
void    ST7735_LL_Delay             ( uint16_t ms );
void    ST7735_LL_SendCmd           ( uint8_t cmd );
void    ST7735_LL_SendData          ( uint8_t data );
void    ST7735_LL_SendPixel         ( uint16_t rgb );

/******************* Main Functions **************************/

void    ST7735_Init                 ( void );
void    ST7735_SetAddress           ( uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end );
void    ST7735_Clear                ( uint16_t color );
void    ST7735_RefreshPart          ( uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint16_t * colors );

/******************* Command Set *****************************/

// System Function Command List
#define     ST7735_CMD_NOP                  0x00
#define     ST7735_CMD_SWRESET              0x01
#define     ST7735_CMD_RDDID                0x04
#define     ST7735_CMD_RDDST                0x09
#define     ST7735_CMD_RDDPM                0x0A
#define     ST7735_CMD_RDDMADCTL            0x0B
#define     ST7735_CMD_RDDCOLMOD            0x0C
#define     ST7735_CMD_RDDIM                0x0D
#define     ST7735_CMD_RDDSM                0x0E
#define     ST7735_CMD_RDDSDR               0x0F
#define     ST7735_CMD_SLPIN                0x10
#define     ST7735_CMD_SLPOUT               0x11
#define     ST7735_CMD_PTLON                0x12
#define     ST7735_CMD_NORON                0x13
#define     ST7735_CMD_INVOFF               0x20
#define     ST7735_CMD_INVON                0x21
#define     ST7735_CMD_GAMSET               0x26
#define     ST7735_CMD_DISPOFF              0x28
#define     ST7735_CMD_DISPON               0x29
#define     ST7735_CMD_CASET                0x2A
#define     ST7735_CMD_RASET                0x2B
#define     ST7735_CMD_RAMWR                0x2C
#define     ST7735_CMD_RGBSET               0x2D
#define     ST7735_CMD_RAMRD                0x2E

#define     ST7735_CMD_PTRAR                0x30

#define     ST7735_CMD_SCRLAR               0x33
#define     ST7735_CMD_TEOFF                0x34
#define     ST7735_CMD_TEON                 0x35
#define     ST7735_CMD_MADCTL               0x36
#define     ST7735_CMD_VSCSAD               0x37
#define     ST7735_CMD_IDMOFF               0x38
#define     ST7735_CMD_IDMON                0x39
#define     ST7735_CMD_COLMOD               0x3A

#define     ST7735_CMD_RDID1                0xDA
#define     ST7735_CMD_RDID2                0xDB
#define     ST7735_CMD_RDID3                0xDC

// Panel Function Command List
#define     ST7735_CMD_FRMCTR1              0xB1
#define     ST7735_CMD_FRMCTR2              0xB2
#define     ST7735_CMD_FRMCTR3              0xB3
#define     ST7735_CMD_INVCTR               0xB4

#define     ST7735_CMD_PWCTR1               0xC0
#define     ST7735_CMD_PWCTR2               0xC1
#define     ST7735_CMD_PWCTR3               0xC2
#define     ST7735_CMD_PWCTR4               0xC3
#define     ST7735_CMD_PWCTR5               0xC4
#define     ST7735_CMD_VMCTR1               0xC5

#define     ST7735_CMD_VMOFCTR1             0xC7

#define     ST7735_CMD_WRID2                0xD1
#define     ST7735_CMD_WRID3                0xD2

#define     ST7735_CMD_NVCTR1               0xD9

#define     ST7735_CMD_NVCTR2               0xDE
#define     ST7735_CMD_NVCTR3               0xDF

#define     ST7735_CMD_GAMCTRP1             0xE0
#define     ST7735_CMD_GAMCTRN1             0xE1

#define     ST7735_CMD_GCV                  0xFC


/******************* Color Defination *****************************/

#define     COLOR_WHITE     0xffff
#define     COLOR_BLACK     0x0000
#define     COLOR_RED       0xF800
#define     COLOR_GREEN     0X07E0
#define     COLOR_BLUE      0x001F



#ifdef __cplusplus
}
#endif
#endif
