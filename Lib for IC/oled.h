/********************************************************************************
 * @file     oled.h
 * @brief    drive the oled with SSD1306
 * @version  V1.3
 * @date     2021.7.29
 * @author   RainingRabbits 1466586342@qq.com
 * @note     see https://blog.csdn.net/qq_36461474/article/details/112591234
 * @code     UTF-8
 *******************************************************************************/
 /*******************************************************************************
TIPS:
    
EXAMPLE CODE:
	OLED_Init();

*******************************************************************************/

#ifndef __OLED_H
#define __OLED_H

#define OLED_X_MAX 128
#define OLED_Y_MAX 64

/*
    0 for i2c, and 1 for 4-spi.
    Dont not use 3-spi, plz...
    It will send nine bits during a write cycle,
    and its MSB stand for DATA (1) or CMD (0).
*/
#define OLED_INTERFACE  1

/****************** Basic Interface *************************/

#if ( OLED_INTERFACE == 0 )

#include "myiic.h"

#define OLED_IIC_ADDR	0x78
#define OLED_IIC_CMD	0x00
#define OLED_IIC_DATA	0x40

static MyIIC myiic1 = {
	.SCL_Port = GPIOG,.SCL_Bit = GPIO_PIN_2,
	.SDA_Port = GPIOG,.SDA_Bit = GPIO_PIN_4,
	.Speed = 1 ,.DevAddr = OLED_IIC_ADDR
};

#else

#include "myspi.h"

#define OLED_WritePinRST(x)		( x ? (GPIOE->BSRR = GPIO_PIN_8) : (GPIOE->BRR = GPIO_PIN_8) )
#define OLED_WritePinDC(x)		( x ? (GPIOE->BSRR = GPIO_PIN_9) : (GPIOE->BRR = GPIO_PIN_9) )

static MySPI myspi1 = {
    .SCK_Port  = GPIOE, .SCK_Bit  = GPIO_PIN_11,
    .CS_Port   = GPIOE, .CS_Bit   = GPIO_PIN_10,
    .MOSI_Port = GPIOE, .MOSI_Bit = GPIO_PIN_12,
    .MISO_Port = NULL,  .MISO_Bit = 0,
    .Speed = 2, .CPOL = 0, .CPHA = 0
};

#endif

void OLED_SendInit		        (void);
void OLED_SendCmd		        (uint8_t *data,unsigned int len);
void OLED_SendData		        (uint8_t *data,unsigned int len);

/******************* do not modify *****************************/
void OLED_DisPlay_Power			(uint8_t flag);
void OLED_DisPlay_EntireOn		(uint8_t flag);
void OLED_DisPlay_Inverse		(uint8_t flag);
void OLED_DisPlay_Contrast		(uint8_t level);
void OLED_DisPlay_CulRemap		(uint8_t flag);
void OLED_DisPlay_RowRemap		(uint8_t flag);
void OLED_Display_HScroll		(uint8_t dir,uint8_t start_page,uint8_t end_page,uint8_t spped);
void OLED_Display_VHScroll		(uint8_t dir,uint8_t start_page,uint8_t end_page,uint8_t speed,uint8_t offset);
void OLED_Display_VScrollArea	(uint8_t row_start,uint8_t row_length);
void OLED_Display_ScrollStop	(void);

void OLED_Address_Mode			(uint8_t mode);
void OLED_Address_Culumn_M2		(uint8_t addr);
void OLED_Address_Page_M2		(uint8_t addr);
void OLED_Address_Culumn_M0M1	(uint8_t start,uint8_t end);
void OLED_Address_Page_M0M1		(uint8_t start,uint8_t end);

void OLED_Set_Clock				(uint8_t count);
void OLED_Set_Precharge         (uint8_t count);
void OLED_Set_Vcomh				(uint8_t level);
void OLED_Set_MuxRatio			(uint8_t count);
void OLED_Set_Offset			(uint8_t count);
void OLED_Set_COMPinCfg			(uint8_t mode);

void OLED_Init		            (void);
void OLED_Refresh	            (uint8_t *data);
void OLED_RefreshByIndex	    (uint8_t *index,uint8_t *data);

/*******************Command Table**********************************/
//Fundamental Command Table
#define OLED_CONTRAST		    0x81
#define OLED_CONTRAST_MASK	    0xFF

#define OLED_ENTIREON_0		    0xA4
#define OLED_ENTIREON_1		    0xA5
#define OLED_INVERSE_0		    0xA6
#define OLED_INVERSE_1		    0xA7
#define OLED_DISPLAY_0		    0xAE
#define OLED_DISPLAY_1		    0xAF

//Scrolling Command Table
#define OLED_HSCROLL_0		    0x26
#define OLED_HSCROLL_1		    0x27
#define OLED_HSCROLL_MASK1	    0x07
#define OLED_HSCROLL_MASK2	    0x07
#define OLED_HSCROLL_MASK3	    0x07

#define OLED_VHSCROLL_0		    0x29
#define OLED_VHSCROLL_1		    0x2A
#define OLED_VHSCROLL_MASK1	    0x07
#define OLED_VHSCROLL_MASK2	    0x07
#define OLED_VHSCROLL_MASK3	    0x07
#define OLED_VHSCROLL_MASK4	    0x3F

#define OLED_SCROLL_DISABLE	    0x2E
#define OLED_SCROLL_ENABLE	    0x2F

#define OLED_VSCROLLAREA		0xA3
#define OLED_VSCROLLAREA_MASK1	0x3F
#define OLED_VSCROLLAREA_MASK2	0x7F

//Addressing Settting Command Table
#define OLED_LCUL4PAGEADDR      0x00
#define OLED_HCUL4PAGEADDR      0x10
#define OLED_SETMEMADDRMODE     0x20
#define OLED_ADDRMODE_0         0x00
#define OLED_ADDRMODE_1         0x01
#define OLED_ADDRMODE_2         0x02

#define OLED_SETCULADDR			0x21
#define OLED_SETCULADDR_MASK1	0x7F
#define OLED_SETCULADDR_MASK2	0x7F

#define OLED_SETPAGEADDR		0x22
#define OLED_SETPAGEADDR_MASK1	0x07
#define OLED_SETPAGEADDR_MASK2	0x07

#define OLED_SETPAGESTART		0xB0

//Hardware Configuration Command Table
#define OLED_SETDISPSLINE_63	0x40
#define OLED_SEGREMAP_0			0xA0
#define OLED_SEGREMAP_1			0xA1
#define OLED_MULRATIO			0xA8

#define OLED_OUTPUTDIR_0		0xC0
#define OLED_OUTPUTDIR_1		0xC8
#define OLED_DISPOFFSET			0xD3
#define OLED_DISPOFFSET_MASK	0x3F

#define OLED_COMPINCFG			0xDA
#define OLED_COMPINCFG_0		0x02
#define OLED_COMPINCFG_1		0x12
#define OLED_COMPINCFG_2		0x22
#define OLED_COMPINCFG_3		0x32

//Timing & Driving Scheme Setting Table
#define OLED_SETCLOCK			0xD5
#define OLED_SETPREuint8_tGE    0xD9
#define OLED_SETVCOMH			0xDB
#define OLED_SETVCOMH_MASk		0x70

#define OLED_NOP				0xE3

//MISC
#define OLED_uint8_tGEPUMP      0x8D
#define OLED_uint8_tGEPUMP_0    0x10
#define OLED_uint8_tGEPUMP_1    0x14

#define OLED_DUMMY_BYTE_00	    0x00
#define OLED_DUMMY_BYTE_FF	    0xFF


#endif
