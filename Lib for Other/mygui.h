/*******************************************************************************
 * @file     mygui.h
 * @brief    load fonts or draw shapes to screen 
 * @version  V1.4
 * @date     2021.7.29
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 ******************************************************************************/
/*******************Hareware Define**********************************
TIPS:
    1. The Ram to display screen is show as below. For example, the ram is 
    unsigend uint8_t MYGUI_RAM[ MYGUI_XBYTE_MAX*MYGUI_YBYTE_MAX ]={a,b,c,d...};
The screen is	
                 x0 x1 x2 x3 x4 .. xmax
               
            y0   a0 b0 c0 d0 e0 .. h0
            y1   a1 .. .. .. .. .. ..
            y2   a2 .. .. .. .. .. ..
            y3   a3 .. .. .. .. .. ..
            y4   a4 .. .. .. .. .. ..
            y5   a5 .. .. .. .. .. ..
            y6   a6 .. .. .. .. .. ..
            y7   a7 .. .. .. .. .. ..
            y8   i0 g0 k0 l0 m0 .. p0
            y9   .. .. .. .. .. .. ..
            ..   .. .. .. .. .. .. ..
          ymax   .. .. .. .. .. .. ..
This is important for you to create fonts or bmp.
    2. For different screens, you only need to re-write the function MYGUI_DrawPoint.
    3. MYGUI_Clear will fill screen with parameter data, parameter dir appoint to
the rotation degree.
           dir  -> rotation degree
            0   ->      0
            1   ->      90
            2   ->      180
            3   ->      270
    4. You need to call MYGUI_Refresh() after any write MYGUI_RAM operations, including Draw, Load and Print. 
    5. All you need to attention is noted by 
        //TODO
        //TODO end
        include file mygui.c.

EXAMPLE CODE:
    MYGUI_Init();
    HAL_Delay(1000);
    MYGUI_Clear(0x00,0);
    HAL_Delay(1000);
    MYGUI_Clear(0xFF,0);
    
    HAL_Delay(1000);
    MYGUI_Print(0,0,"hello",5,FONT_1608,1);
    MYGUI_Refresh();
    
    HAL_Delay(1000);
    MYGUI_PrintNum(0,31,12345,FONT_1608,1);
    MYGUI_Refresh();
*******************************************************************************/


#ifndef __MYGUI_H
#define __MYGUI_H
#ifdef __cplusplus
extern "c" {
#endif

#define MYGUI_USE_FONTS     1

#include "stdint.h"

/******************** TO DO ********************/

#include "oled.h"
#define MYGUI_X_MAX			OLED_X_MAX
#define MYGUI_Y_MAX			OLED_Y_MAX
#define MYGUI_XBYTE_MAX		OLED_X_MAX
#define MYGUI_YBYTE_MAX		OLED_Y_MAX/8

void MYGUI_Init			    (void);
void MYGUI_Refresh		    (void);
void MYGUI_DrawPoint  	    (int16_t x,int16_t y,uint8_t color);

/****************** TO DO end ******************/

void MYGUI_Clear		    (uint8_t data,uint8_t dir);
void MYGUI_DrawLine		    (int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint8_t color);
void MYGUI_DrawRectangle    (int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint8_t fill,uint8_t color);
void MYGUI_DrawCircle 	    (int16_t x,int16_t y,uint16_t r,uint8_t fill,uint8_t color);
void MYGUI_Load		    	(int16_t x,int16_t y,uint16_t xlen,uint16_t ylen,const uint8_t *data,uint8_t color);


#if (MYGUI_USE_FONTS == 1)
#include "fonts.h"

void MYGUI_Print		    (int16_t x,int16_t y,const char *data,uint16_t datalen,FONT_SIZE size,uint8_t color);
void MYGUI_PrintNum	   	    (int16_t x,int16_t y,int32_t num,FONT_SIZE size,uint8_t color);

#endif

#ifdef __cplusplus
}
#endif
#endif
