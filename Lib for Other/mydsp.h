/*******************************************************************************
 * @file     mydsp.h
 * @brief    simple operations to signal
 * @version  V1.2
 * @date     2021.5.29
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
EXAMPLE CODE:
*******************************************************************************/
#ifndef __MYDSP_H
#define __MYDSP_H

#include "stdint.h"
//滑动窗口滤波
uint16_t	MyDSP_Filter			(uint16_t *datain,uint16_t *dataout, uint16_t len,uint16_t n);
//获取信号周期
uint16_t	MyDSP_GetCycleLen	(uint16_t *data, uint16_t len);

int16_t   MyDSP_GetPhase    (uint16_t *dat1,uint16_t *dat2, uint16_t len,uint16_t range);

uint16_t  MyDSP_Get         (uint16_t *data, uint16_t len,uint16_t *max,uint16_t *min,uint16_t *dc,uint16_t *rectcoeff);
//获取信号峰值
uint16_t	MyDSP_GetVpp			(uint16_t *data, uint16_t len,uint16_t *max,uint16_t *min);
//获取信号直流分量
uint16_t	MyDSP_GetDCLevel  (uint16_t *data, uint16_t len);
//获取方波占空比
uint16_t MyDSP_GetDuty      (uint16_t *data, uint16_t len,uint16_t dc);
//获取方波占空比
uint16_t MyDSP_Correct      (uint16_t *data, uint16_t len,int16_t a,float b,int16_t c);
//获取正弦信号峰峰值
uint16_t	MyDSP_GetSinVpp			(uint16_t *data,uint16_t len,uint16_t *dc_level);
//串口打印信号
void		MyDSP_Print				(const char *dataname,uint16_t *data, uint16_t len);
//信号匹配算法
uint16_t	MyDSP_Match				(const uint16_t *matchTab,uint16_t len,uint16_t wid,const uint16_t *dataIn,const float *dataWei);
//内存格式转化
void		MyDSP_B16toB8			(uint16_t *data,uint16_t len);
//信号插值绘图
void		MyDSP_LinePlot			(const uint16_t *datain,const uint16_t *linein,const uint16_t countin,uint16_t *dataout,const  uint16_t countout);
//信号插值绘图（横坐标为对数坐标）
void 		MyDSP_LogPlot			(const uint16_t *datain,const uint32_t *login,const uint16_t countin,uint16_t *dataout,const uint16_t countout);
#endif
