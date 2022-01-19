/*******************************************************************************
 * @file     ad9959.h
 * @brief    
 * @version  V1.0
 * @date     2021.7.4
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1. 所有IO推挽输出
EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __AD9959_H
#define __AD9959_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "base.h"

/************************* AD9959寄存器地址定义 ******************************/
#define AD9959_REG_CSR      0x00   // 通道选择寄存器
#define AD9959_REG_FR1      0x01   // 功能寄存器1
#define AD9959_REG_FR2      0x02   // 功能寄存器2
#define AD9959_REG_CFR      0x03   // 通道功能寄存器
#define AD9959_REG_CFTW0    0x04   // 通道频率转换字寄存器
#define AD9959_REG_CPOW0    0x05   // 通道相位转换字寄存器
#define AD9959_REG_ACR      0x06   // 幅度控制寄存器
#define AD9959_REG_LSRR     0x07   // 通道线性扫描斜率寄存器
#define AD9959_REG_RDW      0x08   // 通道线性向上扫描寄存器
#define AD9959_REG_FDW      0x09   // 通道线性向下扫描寄存器


/************************* IO接口定义 ******************************/
//#define AD9959_IO_P0		PAout(0)
//#define AD9959_IO_P1		PAout(2)
//#define AD9959_IO_P2		PAout(4)
//#define AD9959_IO_P3		PAout(6)
#define AD9959_IO_SD0		PAout(2)
//#define AD9959_IO_SD1		PBout(0)
//#define AD9959_IO_SD2		PBout(2)
#define AD9959_IO_SD3		PAout(4)
#define AD9959_IO_SCK		PAout(6)
#define AD9959_IO_CS		PCout(4)
#define AD9959_IO_UP		PBout(0)
#define AD9959_IO_RST   	PBout(2)
//#define AD9959_IO_PDC   	PAout(2)

/************************* 函数功能声明 ******************************/
void    AD9959_Delay       (uint32_t length);
void    AD9959_IOInit      (void);
void    AD9959_IOUpdate    (void);
void    AD9959_Reset       (void);
void    AD9959_WriteData   (uint8_t Address, const uint8_t* Data, uint8_t DataLen);
void    AD9959_Init        (void);
void    AD9959_SetFreq     (uint8_t Channel,uint32_t Freq);
void    AD9959_SetAmpli    (uint8_t Channel,uint16_t Ampli);
void    AD9959_SetPhase    (uint8_t Channel,uint16_t Phase);


#ifdef __cplusplus
}
#endif
#endif
