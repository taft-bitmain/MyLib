/*******************************************************************************
 * @file     rc522.h
 * @brief    Simple funtions to drive MFRC522
 * @version  V1.0
 * @date     2022.1.10
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
    This files maybe exsit some bugs, use with caution.
    
EXAMPLE CODE:

    uint8_t defaultkeyA[6]= {0xFF,0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t TagType[4];
    uint8_t uid[4];
    uint8_t block0[16];

    uint8_t  i;
    if( RC522_Request( PICC_REQALL, TagType ) == STATUS_OK )
    {
        if( RC522_Anticoll( uid ) == STATUS_OK )
        {
            if( RC522_Select( uid ) == STATUS_OK )
            {
                if( RC522_AuthState( PICC_AUTHENT1A, 0x00, DefaultKeyA, uid ) == STATUS_OK )
                {
                    if( RC522_Read ( 0x00, block0) == STATUS_OK)
                    {
                        Myprintf("\r\nuid is:\r\n");
                        for (i = 0; i < 4 ; i++)
                        {
                            Myprintf("%X ",uid[i]);
                        }
                        Myprintf("\r\nblock0 is:\r\n");
                        for (i = 0; i < 16 ; i++)
                        {
                            Myprintf("%X ",block0[i]);
                        }
                    }
                }
            }
        }
    }

*******************************************************************************/
#ifndef __RC522_H
#define __RC522_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"


/*********************** RC522 Interface **********************/
void        RC522_Delay         ( uint32_t count );

#include "base.h"
void        RC522_PinRst        ( uint8_t state );

#include "myspi.h"
void        RC522_InterfaceInit ( void );
uint8_t     RC522_RegRead       ( uint8_t reg );
void        RC522_RegWrite      ( uint8_t reg, uint8_t value );
void        RC522_SetBitMask    ( uint8_t reg, uint8_t mask );
void        RC522_ClearBitMask  ( uint8_t reg, uint8_t mask );

/********************** RC522 Definetions  ********************/

// MFRC522 Registers Overview
// PAGE 0 : Command and Status
//          RFU00                 0x00
#define     CommandReg            0x01
    // MFRC522 Commands
    #define     CMD_IDLE              0x00          // cancel current command
    #define     CMD_MEM               0x01
    #define     CMD_GRID              0x02
    #define     CMD_CALCCRC           0x03
    #define     CMD_TRANSMIT          0x04
    #define     CMD_NCMD              0x07
    #define     CMD_RECEIVE           0x08
    #define     CMD_TRANSCEIVE        0x0C
    #define     CMD_MFAUTHENT         0x0E
    #define     CMD_SOFTRESET         0x0F
#define     ComIEnReg             0x02
#define     DivlEnReg             0x03
#define     ComIrqReg             0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
//          RFU0F                 0x0F
// PAGE 1 : Command
//          RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxASKReg              0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
//          RFU1A                 0x1A
//          RFU1B                 0x1B
#define     MfTxReg               0x1C
#define     MfRxReg               0x1D
//          RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2 : CFG
//          RFU20                 0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
//          RFU23                 0x23
#define     ModWidthReg           0x24
//          RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3 : TestRegiste
//          RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
//          RFU3C                 0x3C
//          RFU3D                 0x3D
//          RFU3E                 0x3E
//          RFU3F		  		  0x3F


//Define error code when communicate with MFRC522
#define 	STATUS_OK             0x00
#define 	STATUS_ERR            0x01
#define 	STATUS_TIMEOUT        0x02
#define 	STATUS_CRC_ERR        0x03

// Commands sent to the PICC
#define     PICC_REQIDL           0x26               // Request command
#define     PICC_REQALL           0x52               // 
#define     PICC_ANTICOLL1        0x93               // 
#define     PICC_ANTICOLL2        0x95               // 
#define     PICC_AUTHENT1A        0x60               // 
#define     PICC_AUTHENT1B        0x61               // 
#define     PICC_READ             0x30               // 
#define     PICC_WRITE            0xA0               // 
#define     PICC_DECREMENT        0xC0               // 
#define     PICC_INCREMENT        0xC1               // 
#define     PICC_RESTORE          0xC2               // 
#define     PICC_TRANSFER         0xB0               // 
#define     PICC_HALT             0x50               // 

#define     MAXRLEN               18

/********************** RC522 Main Functions  ********************/
void        RC522_Reset                 ( void );
void        RC522_AntennaOn             ( void );
void        RC522_AntennaOff            ( void );
void        RC522_SoftPowerDown         ( void );
void        RC522_SoftPowerUp           ( void );
void        RC522_ConfigISOType         ( uint8_t type );
void        RC522_Init                  ( void );

void        RC522_CalulateCRC           ( uint8_t * pdatain, uint8_t bytein, uint8_t * pdataout );
uint8_t     RC522_Communication         ( uint8_t cmd, uint8_t * pdatain, uint8_t bytein, uint8_t * pdataout, uint32_t * bitout );
uint8_t     RC522_Halt                  ( void );
uint8_t     RC522_Request               ( uint8_t reqcode, uint8_t * ptagtype );
uint8_t     RC522_Anticoll              ( uint8_t * puid );
uint8_t     RC522_Select                ( uint8_t * puid );
uint8_t     RC522_AuthState             ( uint8_t authmode, uint8_t blockaddr, uint8_t * pkey, uint8_t * puid );
uint8_t     RC522_Write                 ( uint8_t blockaddr, uint8_t * pdata );
uint8_t     RC522_Read                  ( uint8_t blockaddr, uint8_t * pdata );


#ifdef __cplusplus
}
#endif
#endif
