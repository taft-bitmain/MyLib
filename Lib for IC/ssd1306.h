/********************************************************************************
 * @file     ssd1306.h
 * @brief    drive the oled with SSD1306
 * @version  V1.5
 * @date     2022.4.21
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 *******************************************************************************/
/*******************************************************************************
TIPS:
   
   1. Diffierent oled size have different COM_PIN_CONFIGRATION, look up the datasheet.
   2. Dont not use 3-spi, plz... It will send nine bits during a write cycle,
      and its MSB stand for DATA (1) or CMD (0).
      
EXAMPLE CODE:

    // reference to mygui.c/.h

*******************************************************************************/

#ifndef __SSD1306_H
#define __SSD1306_H

#include "stdint.h"

#define     SSD1306_X_MAX      128
#define     SSD1306_Y_MAX      64

//#define     SSD1306_IF_I2C_SW
#define     SSD1306_IF_I2C_HW
//#define     SSD1306_IF_SPI_SW
//#define     SSD1306_IF_SPI_HW

/****************** Basic Interface *************************/

#define SSD1306_I2C_ADDR	0x3C
#define SSD1306_I2C_CMD     0x00
#define SSD1306_I2C_DATA	0x40

// Software I2C
#ifdef SSD1306_IF_I2C_SW

#include "myi2c.h"

static myi2c ssd1306_i2c =
{
    .SCL_port = GPIOx,.SCL_pin = GPIO_PIN_x,
    .SDA_port = GPIOx,.SDA_pin = GPIO_PIN_x,
    .speed = 1,.slaver_addr = SSD1306_I2C_ADDR
};

static inline void SSD1306_LL_Init(void)
{
    myi2c_io_init(&ssd1306_i2c);
}

static inline void SSD1306_LL_SendCmd(uint8_t *data,uint16_t len)
{
	myi2c_write_bytes(&ssd1306_i2c, SSD1306_I2C_CMD, 1, data, len);
}
static inline void SSD1306_LL_SendData(uint8_t *data,uint16_t len)
{
	myi2c_write_bytes(&ssd1306_i2c, SSD1306_I2C_DATA, 1, data, len);
}

#endif

// Hardware I2C
#ifdef SSD1306_IF_I2C_HW

#define SSD1306_I2C_HANDLE        hi2c1

#include "i2c.h"

static inline void SSD1306_LL_Init(void)
{
    ;
}

static inline void SSD1306_LL_SendCmd(uint8_t *data,uint16_t len)
{
	HAL_I2C_Mem_Write( &SSD1306_I2C_HANDLE , SSD1306_I2C_ADDR<<1, SSD1306_I2C_CMD, I2C_MEMADD_SIZE_8BIT , data, len, 500 );
}
static inline void SSD1306_LL_SendData(uint8_t *data,uint16_t len)
{
	HAL_I2C_Mem_Write( &SSD1306_I2C_HANDLE , SSD1306_I2C_ADDR<<1, SSD1306_I2C_DATA, I2C_MEMADD_SIZE_8BIT , data, len, 500 );
}

#endif

// Software SPI
#ifdef SSD1306_IF_SPI_SW

#include "myspi.h"

#define SSD1306_LL_PinRST(x)		( x ? (GPIOx->BSRR = GPIO_PIN_x) : (GPIOx->BRR = GPIO_PIN_x) )
#define SSD1306_LL_PinDC(x)		    ( x ? (GPIOx->BSRR = GPIO_PIN_x) : (GPIOx->BRR = GPIO_PIN_x) )

static myspi ssd1306_spi =
{
    .SCK_port  = GPIOx, .SCK_pin  = GPIO_PIN_x,
    .CS_port   = GPIOx, .CS_pin   = GPIO_PIN_x,
    .MOSI_port = GPIOx, .MOSI_pin = GPIO_PIN_x,
    .MISO_port = NULL,  .MISO_pin = 0,
    .CPOL = 0, .CPHA = 0
};

static inline void SSD1306_LL_Init(void)
{
	SSD1306_LL_PinRST(0);
    myspi_io_init(&ssd1306_spi);
	SSD1306_LL_PinRST(1);
}

static inline void SSD1306_LL_SendCmd(uint8_t *data,unsigned int len)
{
	SSD1306_LL_PinDC(0);
	myspi_write(&ssd1306_spi,data,len);
}

static inline void SSD1306_LL_SendData(uint8_t *data,unsigned int len)
{
	SSD1306_LL_PinDC(1);
	myspi_write(&ssd1306_spi,data,len);
}

#endif

// Hardware SPI

#ifdef SSD1306_IF_SPI_HW

#define SSD1306_SPI_HANDLE        hspi2
#include "spi.h"

#define SSD1306_LL_PinRST(x)		( x ? (GPIOC->BSRR = GPIO_PIN_7) : (GPIOC->BRR = GPIO_PIN_7) )
#define SSD1306_LL_PinDC(x)		    ( x ? (GPIOC->BSRR = GPIO_PIN_6) : (GPIOC->BRR = GPIO_PIN_6) )
#define SSD1306_LL_PinCS(x)		    ( x ? (GPIOB->BSRR = GPIO_PIN_12) : (GPIOB->BRR = GPIO_PIN_12) )


static inline void SSD1306_LL_Init(void)
{
    SSD1306_LL_PinRST(1);
	SSD1306_LL_PinCS(1);
}

static inline void SSD1306_LL_SendCmd(uint8_t *data,uint16_t len)
{
	SSD1306_LL_PinDC(0);
    SSD1306_LL_PinCS(0);
    HAL_SPI_Transmit(&SSD1306_SPI_HANDLE,data,len,500);
    SSD1306_LL_PinCS(1);
}

static inline void SSD1306_LL_SendData(uint8_t *data,uint16_t len)
{
	SSD1306_LL_PinDC(1);
    SSD1306_LL_PinCS(0);
	HAL_SPI_Transmit(&SSD1306_SPI_HANDLE,data,len,500);
    SSD1306_LL_PinCS(1);
}

#endif


/******************* Main Functions **************************/

void    SSD1306_DisPlay_Power       (uint8_t flag);
void    SSD1306_DisPlay_EntireOn    (uint8_t flag);
void    SSD1306_DisPlay_Inverse     (uint8_t flag);
void    SSD1306_DisPlay_Contrast    (uint8_t level);
void    SSD1306_DisPlay_CulRemap    (uint8_t flag);
void    SSD1306_DisPlay_RowRemap    (uint8_t flag);
void    SSD1306_Display_HScroll     (uint8_t dir,uint8_t start_page,uint8_t end_page,uint8_t spped);
void    SSD1306_Display_VHScroll    (uint8_t dir,uint8_t start_page,uint8_t end_page,uint8_t speed,uint8_t offset);
void    SSD1306_Display_VScrollArea	(uint8_t row_start,uint8_t row_length);
void    SSD1306_Display_ScrollStop  (void);

void    SSD1306_Address_Mode        (uint8_t mode);
void    SSD1306_Address_Culumn_M2   (uint8_t addr);
void    SSD1306_Address_Page_M2     (uint8_t addr);
void    SSD1306_Address_Culumn_M0M1 (uint8_t start,uint8_t end);
void    SSD1306_Address_Page_M0M1   (uint8_t start,uint8_t end);

void    SSD1306_Set_Clock           (uint8_t count);
void    SSD1306_Set_Precharge       (uint8_t count);
void    SSD1306_Set_Vcomh           (uint8_t level);
void    SSD1306_Set_MuxRatio        (uint8_t count);
void    SSD1306_Set_Offset          (uint8_t count);
void    SSD1306_Set_COMPinCfg       (uint8_t mode);

void    SSD1306_Init                (void);
void    SSD1306_Refresh             (uint8_t *data);
void    SSD1306_RefreshByIndex      (uint8_t *index,uint8_t *data);

/*******************Command Table**********************************/

//Fundamental Command Table
#define SSD1306_CONTRAST		    0x81
#define SSD1306_CONTRAST_MASK	    0xFF

#define SSD1306_ENTIREON_0		    0xA4
#define SSD1306_ENTIREON_1		    0xA5
#define SSD1306_INVERSE_0		    0xA6
#define SSD1306_INVERSE_1		    0xA7
#define SSD1306_DISPLAY_0		    0xAE
#define SSD1306_DISPLAY_1		    0xAF

//Scrolling Command Table
#define SSD1306_HSCROLL_0		    0x26
#define SSD1306_HSCROLL_1		    0x27
#define SSD1306_HSCROLL_MASK1	    0x07
#define SSD1306_HSCROLL_MASK2	    0x07
#define SSD1306_HSCROLL_MASK3	    0x07

#define SSD1306_VHSCROLL_0		    0x29
#define SSD1306_VHSCROLL_1		    0x2A
#define SSD1306_VHSCROLL_MASK1	    0x07
#define SSD1306_VHSCROLL_MASK2	    0x07
#define SSD1306_VHSCROLL_MASK3	    0x07
#define SSD1306_VHSCROLL_MASK4	    0x3F

#define SSD1306_SCROLL_DISABLE	    0x2E
#define SSD1306_SCROLL_ENABLE	    0x2F

#define SSD1306_VSCROLLAREA		0xA3
#define SSD1306_VSCROLLAREA_MASK1	0x3F
#define SSD1306_VSCROLLAREA_MASK2	0x7F

//Addressing Settting Command Table
#define SSD1306_LCUL4PAGEADDR      0x00
#define SSD1306_HCUL4PAGEADDR      0x10
#define SSD1306_SETMEMADDRMODE     0x20
#define SSD1306_ADDRMODE_0         0x00
#define SSD1306_ADDRMODE_1         0x01
#define SSD1306_ADDRMODE_2         0x02

#define SSD1306_SETCULADDR			0x21
#define SSD1306_SETCULADDR_MASK1	0x7F
#define SSD1306_SETCULADDR_MASK2	0x7F

#define SSD1306_SETPAGEADDR		0x22
#define SSD1306_SETPAGEADDR_MASK1	0x07
#define SSD1306_SETPAGEADDR_MASK2	0x07

#define SSD1306_SETPAGESTART		0xB0

//Hardware Configuration Command Table
#define SSD1306_SETDISPSLINE_63	0x40
#define SSD1306_SEGREMAP_0			0xA0
#define SSD1306_SEGREMAP_1			0xA1
#define SSD1306_MULRATIO			0xA8

#define SSD1306_OUTPUTDIR_0		0xC0
#define SSD1306_OUTPUTDIR_1		0xC8
#define SSD1306_DISPOFFSET			0xD3
#define SSD1306_DISPOFFSET_MASK	0x3F

#define SSD1306_COMPINCFG			0xDA
#define SSD1306_COMPINCFG_0		0x02
#define SSD1306_COMPINCFG_1		0x12
#define SSD1306_COMPINCFG_2		0x22
#define SSD1306_COMPINCFG_3		0x32

//Timing & Driving Scheme Setting Table
#define SSD1306_SETCLOCK			0xD5
#define SSD1306_SETPREuint8_tGE    0xD9
#define SSD1306_SETVCOMH			0xDB
#define SSD1306_SETVCOMH_MASk		0x70

#define SSD1306_NOP				0xE3

//MISC
#define SSD1306_GEPUMP             0x8D
#define SSD1306_GEPUMP_0           0x10
#define SSD1306_GEPUMP_1           0x14

#define SSD1306_DUMMY_BYTE_00	    0x00
#define SSD1306_DUMMY_BYTE_FF	    0xFF


#endif
