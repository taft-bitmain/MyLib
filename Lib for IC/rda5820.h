/*******************************************************************************
 * @file     rda5820.h
 * @brief    
 * @version  V1.0
 * @date     2021.7.18
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __RDA5820_H
#define __RDA5820_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "myiic.h"

#define RDA5820_R00 0X00  //读出16个位的ID =0X5820
//16位的辨识ID 0x5820

#define RDA5820_R02 0X02  //DHIZ[15],DMUTE[14],MONO[13],BASS[12],SEEKUP[9],SEEK[8],SKMODE[7],CLK_MODE[6:4],ANTENNA[2],SOFTRESET[1],ENABLE[0]
//bit0 ENABLE 芯片上电1 芯片不上电0
//bit1 SOFTRESET 芯片软复位1 不复位0
//bit2 ANTENNA 1使用内部小天线 0使用正常的外部天线(内部小天线速度慢很多)
//bit6-bit4 时钟频率选择,根据实际频率选择 000 32.768khz
//bit7 SKMODE 0循环搜索 1到达上线或者下限时停止搜索
//bit8 SEEK 0搜索已经停止 1搜索开始
//bit9 搜索方向 0向下 1向上
//bit12 BASS 0不使能低音增强 1使能低音增强
//bit13 MONO 声道选择 0立体声 1非立体声
//bit14 DMUTE 静音选择 0静音 1非静音
//bit15 DHIZ 音频输出高阻抗选择 0选择高阻抗 1不选择高阻抗

#define RDA5820_R03 0X03  //CHAN[15:6],TUNE[4],BAND[3:2],SPACE[1:0]
//bit15-6 当前频道,通过baud可以计算当前频率
//bit4 调谐 0关闭调谐 1启动调谐
//bit3-2 baud 频段选择 四种选择
//bit1-0 步进选择 00为100K

#define RDA5820_R04 0X04  //STCIEN[14](搜索完成中断),DE[11],I2S_ENABLE[6],GPIO3~1[5:0]
//bit14 1开启调谐调频中断 0关闭中断 中断引脚显示在GPIO2上
//bit11 去加重时间选择 0 75us 1 50us
//bit6 i2s使能 0不使能 1使能
//bit5-bit0 三个GPIO的功能选择 建议都是00

#define RDA5820_R05 0X05  //INT_MODE[15],SEEKTH[14:8](设定信号强度),LNA_PORT_SEL[7:6],LNA_ICSEL_BIT[5:4],VOLUME[3:0];
//bit15 中断模式 0短时间中断 1直到清除0c才取消中断
//bit14-8 搜索阈值 高于这个值认为是有效台
//bit7-6 输入低噪声放大模式 00无输入
//bit5-4 输入低噪声电流 00 1.8ma
//bit3-0 音量 0000最低1111最高

#define RDA5820_R0A 0X0A  //STC[14](搜索完成标志),SF[13](搜索失败标志),ST[10](立体声标志),READCHAN[9:0](频率).
//bit14 搜台完成标志 0未完成 1完成
//bit13 搜台失败标志 0成功 1失败
//bit10 立体声指示 0单声道 1立体声 
//bit9-bit0 当前频率指示,可以计算出当前频率

#define RDA5820_R0B 0X0B  //RSSI[15:9](信号强度),FM_TRUE[8](站点标志),FM_READY[7](fm就绪标志)
//bit15-9 当前信号强度
//bit8 1当前频率是一个有效台 0不是
//bit7 1 已经准备好软件搜台 0没准备好

#define RDA5820_R40 0X40  //AUTO_SEEK[15](全/半自动搜台),CHAN_SCAN[14](搜空/忙台),CHIP_FUN[3:0](工作模式)
//bit15 1全自动搜台 0手动搜台
//bit14 为FM发射搜索一个空闲频率
//bit3-0  选择工作模式 0000 rx模式

#define RDA5820_R41 0X41  //MEM_CLR[15](内存清除),RPT_TIME[14:9](RDS应答次数),MEM_DEPTH[8:0](FIFO大小,最大256)
//bit15  1复位fifo 0不复位
//bit14-9 传输数据是RDS应答次数
//bit8-0 fifo的深度 0-256

#define RDA5820_R42 0X42  //TX_SIG_GAIN[10:8],TX_PA_GAIN[5:0](发射功率).
//bit10-8 发射增益 000最小 111最大
//bit5-0  发射功率

#define RDA5820_R4A 0X4A  //MEM_INTMOD[4:0]
//bit4-bit0 内存数据的中断模式 空中断 重复空中断 满中断 

#define RDA5820_R4B 0X4B  //CHAN_NUM[7:0](仅在全自动搜台有效,代表电台数量).
//bit7-bit0 电台的数量 

#define RDA5820_R4C 0X4C  //TX_AUDIO_DEV[15:0]
//发送音频设置

#define RDA5820_R4D 0X4D  //TX_PILOT_DEV[15:0]
//发送频点设置

#define RDA5820_R4E 0X4E  //TX_RDS_DEV[15:0]
//发送RDS设置

#define RDA5820_R53 0X53  //CHAN_BOTTON[10:0](通道最低频率,100kHZ为单位)
//通道最低频率

#define RDA5820_R54 0X54  //CHAN_TOP[10:0](通道最高频率,100kHZ为单位)
//通道最高频率

#define RDA5820_R64 0X64  //RDS_RXMOD[4:3](FIFO模式[10]或者寄存器模式[11]) 
//rds接收模式 有fifo模式和寄存器模式

#define RDA5820_READ    0X23  //读RDA5820
#define RDA5820_WRITE 0X22  //写RDA5820

uint8_t     RDA5820_Init     (void);                          //初始化
void        RDA5820_WriteReg (uint8_t addr,uint16_t val);          //写RDA5820寄存器
uint16_t    RDA5820_ReadReg  (uint8_t addr); //读RDA5820寄存器
void        RDA5820_RxMode   (void); //设置RDA5820为RX模式
void        RDA5820_TxMode   (void); //设置RDA5820为TX模式
uint8_t     RDA5820_RssiGet  (void);
void        RDA5820_MuteSet  (uint8_t mute);
void        RDA5820_RssiSet  (uint8_t rssi);
void        RDA5820_VolSet   (uint8_t vol);
void        RDA5820_TxPagSet (uint8_t gain);
void        RDA5820_TxPgaSet (uint8_t gain);
void        RDA5820_BandSet  (uint8_t band);
void        RDA5820_SpaceSet (uint8_t spc);
void        RDA5820_FreqSet  (uint16_t freq);
uint16_t    RDA5820_FreqGet  (void);
uint16_t    RDA5820_Seek     (void);


#ifdef __cplusplus
}
#endif
#endif
