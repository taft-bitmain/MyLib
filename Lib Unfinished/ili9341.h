/*******************************************************************************
 * @file     ili9341.h
 * @brief    drive the TFT with ILI9341
 * @version  V1.0
 * @date     2022.2.25
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE:
	
*******************************************************************************/
#ifndef __ILI9341_H
#define __ILI9341_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"

#define TFTLCD_X_MAX 240
#define TFTLCD_Y_MAX 320


/****************** Basic Interface *************************/

// interface : 4-wire 8-bit data serial interface Ⅱ
 
#include "myspi.h"

static MySPI ili9341_spi = {
	.SCK_Port  = GPIOB, .SCK_Bit  = GPIO_PIN_5,
    .CS_Port   = GPIOB, .CS_Bit   = GPIO_PIN_9,
    .MOSI_Port = GPIOB, .MOSI_Bit = GPIO_PIN_6,
    .MISO_Port = GPIOB, .MISO_Bit = GPIO_PIN_3,
    .CPOL = 0, .CPHA = 0
};

#define ILI9341_SetPinDC(x)		    ( x ? (GPIOB->BSRR = GPIO_PIN_7) : (GPIOB->BRR = GPIO_PIN_7) )
#define ILI9341_SetPinRST(x)		( x ? (GPIOB->BSRR = GPIO_PIN_8) : (GPIOB->BRR = GPIO_PIN_8) )
#define ILI9341_SetPinLED(x)	    ( x ? (GPIOB->BSRR = GPIO_PIN_4) : (GPIOB->BRR = GPIO_PIN_4) )

void ILI9341_LL_DelayMs             ( uint32_t ms );

void ILI9341_LL_Init                ( void );
void ILI9341_LL_Write               ( uint8_t cmd, uint8_t * data, uint16_t len );
void ILI9341_LL_Read		        ( uint8_t cmd, uint8_t * data, uint16_t len );

/******************* Main Functions **************************/

void    ILI9341_Init            ( void );
void    ILI9341_SetArea         ( uint16_t Xstart, uint16_t Xend, uint16_t Ystart, uint16_t Yend );
void    ILI9341_Clear           ( uint16_t RGB );
void    ILI9341_RefreshAll      ( uint16_t * RGBs );

/******************* Color Defination **************************/

#define     RED          0XF800
#define     GREEN        0X07E0
#define     BLUE         0X001F
#define     WHITE        0XFFFF
#define     BLACK        0X0000

/****************** Command Set ****************************/

// Regulative Command Set
#define     ILI9341_CMD_Nop                     0x00
#define     ILI9341_CMD_SoftReset               0x01
#define     ILI9341_CMD_RdDispID                0x04
#define     ILI9341_CMD_RdDispSta               0x09
#define     ILI9341_CMD_RdDispPowerMmode        0x0A
#define     ILI9341_CMD_RdDispMADCTL            0x0B
#define     ILI9341_CMD_RdDispPixFormat         0x0C
#define     ILI9341_CMD_RdDispImgFormat         0x0D
#define     ILI9341_CMD_RdDispSigMode           0x0E
#define     ILI9341_CMD_RdDispSelfDiagResult    0x0F
#define     ILI9341_CMD_Sleep                   0x10
#define     ILI9341_CMD_SleepOut                0x11
#define     ILI9341_CMD_PartialON               0x12
#define     ILI9341_CMD_DispModeON              0x13
#define     ILI9341_CMD_DispInvOFF              0x20
#define     ILI9341_CMD_DispInvON               0x21
#define     ILI9341_CMD_GammaSet                0x26
#define     ILI9341_CMD_DisplayOFF              0x28
#define     ILI9341_CMD_DisplayON               0x29
#define     ILI9341_CMD_ColumnAddrSet           0x2A
#define     ILI9341_CMD_PageAddrSet             0x2B
#define     ILI9341_CMD_MemWrite                0x2C
#define     ILI9341_CMD_ColorSet                0x2D
#define     ILI9341_CMD_MemRead                 0x2E
#define     ILI9341_CMD_PartialArea             0x30
#define     ILI9341_CMD_VScrollDef              0x33
#define     ILI9341_CMD_TearingEffectLineOFF    0x34
#define     ILI9341_CMD_TearingEffectLineON     0x35
#define     ILI9341_CMD_MemAccessCtrl           0x36
#define     ILI9341_CMD_VScrollSrartAddr        0x37
#define     ILI9341_CMD_IdleModeON              0x38
#define     ILI9341_CMD_IdleModeOFF             0x39
#define     ILI9341_CMD_PixFormatSet            0x3A
#define     ILI9341_CMD_WrMemContinue           0x3C
#define     ILI9341_CMD_RdMemContinue           0x3E
#define     ILI9341_CMD_SetTearScanline         0x44
#define     ILI9341_CMD_GetScanline             0x45
#define     ILI9341_CMD_WrDispBrightness        0x51
#define     ILI9341_CMD_RdDispBrightness        0x52
#define     ILI9341_CMD_WrCTRLDisp              0x53
#define     ILI9341_CMD_RdCTRLDisp              0x54
#define     ILI9341_CMD_WrContAdaBriCtrl        0x55
#define     ILI9341_CMD_RdContAdaBriCtrl        0x56
#define     ILI9341_CMD_WrCABCMinBri            0x5E
#define     ILI9341_CMD_RdCABCMinBri            0x5F
#define     ILI9341_CMD_RdID1                   0xDA
#define     ILI9341_CMD_RdID2                   0xDB
#define     ILI9341_CMD_RdID3                   0xDC

// Extended Command Set
#define     ILI9341_CMD_RGBIfSigCtrl            0xB0
#define     ILI9341_CMD_FrameCtrlNormal         0xB1
#define     ILI9341_CMD_FrameCtrlIdle           0xB2
#define     ILI9341_CMD_FrameCtrlPartial        0xB3
#define     ILI9341_CMD_DispInvCtrl             0xB4
#define     ILI9341_CMD_BlankingPorchCtrl       0xB5
#define     ILI9341_CMD_DispFuncCtrl            0xB6
#define     ILI9341_CMD_EntryModeSet            0xB7
#define     ILI9341_CMD_BlacklightCtrl1         0xB8
#define     ILI9341_CMD_BlacklightCtrl2         0xB9
#define     ILI9341_CMD_BlacklightCtrl3         0xBA
#define     ILI9341_CMD_BlacklightCtrl4         0xBB
#define     ILI9341_CMD_BlacklightCtrl5         0xBC
#define     ILI9341_CMD_BlacklightCtrl6         0xBD
#define     ILI9341_CMD_BlacklightCtrl7         0xBE
#define     ILI9341_CMD_BlacklightCtrl8         0xBF
#define     ILI9341_CMD_PowerCtrl1              0xC0
#define     ILI9341_CMD_PowerCtrl2              0xC1
#define     ILI9341_CMD_VCOMCtrl1               0xC5
#define     ILI9341_CMD_VCOMCtrl2               0xC7
#define     ILI9341_CMD_NVMemWrite              0xD0
#define     ILI9341_CMD_NVMemProtectionKey      0xD1
#define     ILI9341_CMD_NVMemStaRead            0xD2
#define     ILI9341_CMD_RdID4                   0xD3
#define     ILI9341_CMD_PosiGammaCorrection     0xE0
#define     ILI9341_CMD_NegGammaCorrection      0xE1
#define     ILI9341_CMD_DigiGammaCtrl1          0xE2
#define     ILI9341_CMD_DigiGammaCtrl2          0xE3
#define     ILI9341_CMD_InterfaceCtrl           0xE6

// Extend register command
#define     ILI9341_CMD_PowerCtrlA              0xCB
#define     ILI9341_CMD_PowerCtrlB              0xCF
#define     ILI9341_CMD_DriverTimingCtrlA       0xE8
#define     ILI9341_CMD_DriverTimingCtrlB       0xEA
#define     ILI9341_CMD_PowerOnSeqCtrl          0xED
#define     ILI9341_CMD_Enable3G                0xF2
#define     ILI9341_CMD_PumpRatioCtrl           0xF7

#ifdef __cplusplus
}
#endif
#endif
