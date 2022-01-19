/*******************************************************************************
 * @file     ad9910.h
 * @brief    
 * @version  V1.0
 * @date     2021.7.5
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:

EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __AD9910_H
#define __AD9910_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "base.h"

/************************* AD9910寄存器地址定义 ******************************/
#define AD9910_REG_CFR1     0x00    // 控制功能寄存器1
#define AD9910_REG_CFR2     0x01    // 控制功能寄存器2
#define AD9910_REG_CFR3     0x02    // 控制功能寄存器3
#define AD9910_REG_ADAC     0x03    // 辅助DAC控制
#define AD9910_REG_UDR      0x04    // I/O更新速率

#define AD9910_REG_FTW      0x07    // 频率调谐字
#define AD9910_REG_POW      0x08    // 相位偏移字
#define AD9910_REG_ASF      0x09    // 振幅比例因子
#define AD9910_REG_MCS      0x0A    // 多芯片同步
#define AD9910_REG_DRL      0x0B    // 数字斜坡限值
#define AD9910_REG_DRS      0x0C    // 数字斜坡步长
#define AD9910_REG_DRR      0x0D    // 数字斜坡速率
#define AD9910_REG_PROF0    0x0E    // 单频调制配置0
#define AD9910_REG_RAMP0    0x0E    // RAM配置0

/************************* IO接口定义 ******************************/
#define AD9910_IO_CSB		PAout(0)
#define AD9910_IO_SCLK		PAout(2)
#define AD9910_IO_SDIO		PAout(4)
#define AD9910_IO_UPD		PAout(6)
#define AD9910_IO_RST   	PCout(2)

/************************* 函数功能声明 ******************************/
void    AD9910_Delay       (uint32_t length);
void    AD9910_IOInit      (void);
void    AD9910_IOUpdate    (void);
void    AD9910_Reset       (void);
void    AD9910_WriteData   (uint8_t Address, const uint8_t* Data, uint8_t DataLen);
void    AD9910_Init        (void);
void    AD9910_SetFreq     (uint32_t Freq);
void    AD9910_SetAmpli    (uint16_t Ampli);
void    AD9910_SetPhase    (uint16_t Phase);


#ifdef __cplusplus
}
#endif
#endif
