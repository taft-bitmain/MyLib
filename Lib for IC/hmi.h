/*******************************************************************************
 * @file     hmi.h
 * @brief    basic interface to operate uart HMI
 * @version  V1.2
 * @date     2021.7.24
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1. 注意硬件接口部分，其中的内容注意修改
EXAMPLE CODE:
    uint32_t event;
    while(1)
    {
        if(HMI_GetEvent(&event))
        {
            if(event == 0x65000100)
            {
                HMI_SetTextStr("t0","ABCDE");
            }
            else if(event == 0x65000200)
            {
                HMI_SetTextNum("t1",12345);
            }
        }
    }
*******************************************************************************/
#ifndef __HMI_H
#define __HMI_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "base.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


/********************************* 硬件接口 *********************************/

#define HMI_UART		    huart2

uint16_t    HMI_CommandRecv(uint8_t *data,uint16_t len);
void        HMI_CommandSendf(uint8_t *data, uint16_t len);
void        HMI_CommandSend(uint8_t *data, uint16_t len);
/******************************** 常变量定义 *********************************/


#define HMI_RED				63488
#define HMI_GREEN			2016
#define HMI_BLUE			31
#define HMI_GRAY			33840
#define HMI_BLACK			0
#define HMI_WHILE			65535
#define HMI_BROWN			48192
#define HMI_YELLOW          65504

#define HMI_OBJ_ALL		    255


extern uint8_t	HMI_DataTx[128];
extern uint8_t	HMI_DataRx[256];
extern uint8_t	HMI_DataRcvCount;

/****************************** 基本HMI指令 ******************************/
void HMI_page(uint16_t pageid);
void HMI_ref(uint16_t objid);
void HMI_click(uint16_t objid,uint16_t event);//event 0:弹起 1:按下
void HMI_get(uint16_t objid,uint16_t event);
void HMI_vis(uint16_t objid,uint16_t state);
void HMI_tsw(uint16_t objid,uint16_t state);
void HMI_add(uint16_t objid,uint16_t ch,uint16_t val);
void HMI_cle(uint16_t objid,uint16_t ch);
void HMI_addt(uint16_t objid,uint16_t ch,uint8_t *data,uint16_t qyt);//尽量使用这个函数
void HMI_rest(void);

void HMI_SetTextStr(char  *obj,char *str);
void HMI_SetTextNum(char  *obj,int32_t num);
void HMI_SetText(char *obj,const char *str,...);

uint8_t HMI_GetEvent(uint32_t *event);
uint8_t HMI_GetText(char *obj,char *dat);

/***************************** GUI绘图指令 *******************************/
void HMI_GUI_cls(uint16_t color);
void HMI_GUI_pic(uint16_t x,uint16_t y,uint16_t picid);
void HMI_GUI_picq(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t picid);
void HMI_GUI_xpic(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t x0,uint16_t y0,uint16_t picid);
void HMI_GUI_xstr(uint16_t x,uint16_t y,uint16_t w,uint16_t h,\
									uint16_t fontid,uint16_t pointcolor,uint16_t backcolor,\
									uint16_t xcenter,uint16_t ycenter,uint16_t sta,\
									char *str);
#define HMI_GUI_text(x,y,str) \
		HMI_GUI_xstr(x,y,100,16,0,HMI_BLACK,HMI_WHILE,0,0,3,str)
void HMI_GUI_fill(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color);
void HMI_GUI_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void HMI_GUI_draw(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void HMI_GUI_cir(uint16_t x,uint16_t y,uint16_t r,uint16_t color);
void HMI_GUI_cirs(uint16_t x,uint16_t y,uint16_t r,uint16_t color);

#ifdef __cplusplus
}
#endif
#endif
