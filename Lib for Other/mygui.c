/*******************************************************************************
 * @file     mygui.c
 * @brief    load fonts or draw shapes to screen 
 * @version  V1.3
 * @date     2021.2.8
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 ******************************************************************************/
#include "mygui.h"

uint8_t MYGUI_RAM[MYGUI_XBYTE_MAX*MYGUI_YBYTE_MAX];
static uint8_t MYGUI_DIRECTION;

/* for some devices, use Fresh_Index to
    send the only changed data */
uint8_t Fresh_Index[MYGUI_YBYTE_MAX];

void MYGUI_Init(void)
{
	//TODO
	//screen init
	OLED_Init();
	//TODO end
	
	MYGUI_Clear(0x00,0);
}


void MYGUI_Refresh(void)
{
	//TODO
	//screen display refresh ,transmit MYGUI_RAM to screen.
	OLED_RefreshByIndex(Fresh_Index,MYGUI_RAM);
	//TODO end
}

void MYGUI_DrawPoint(int16_t x,int16_t y,uint8_t color)
{
	int16_t tmp;
	if( x<0 || y<0 )
		return;
	switch(MYGUI_DIRECTION)
	{
		case 0:
			if(x>=MYGUI_X_MAX||y>=MYGUI_Y_MAX)
				return;
			break;
		case 1:
			if(x>=MYGUI_Y_MAX||y>=MYGUI_X_MAX)
				return;
			tmp = x;
			x = MYGUI_X_MAX-1-y;
			y = tmp;
			break;
		case 2:
			if(x>=MYGUI_X_MAX||y>=MYGUI_Y_MAX)
				return;
			x = MYGUI_X_MAX-1-x;
			y = MYGUI_Y_MAX-1-y;
			break;
		case 3:
			if(x>=MYGUI_Y_MAX||y>=MYGUI_X_MAX)
				return;
			tmp = x;
			x = y;
			y = MYGUI_Y_MAX-1-tmp;
			break;
		default:return;
	}
	//TODO
	//Position mapping to ture RAM that is send to screen controller.
	//For diffrent screens, you may need to re-write below.
	uint16_t ybyte = y / 8;
	uint8_t data = 0x01 << (y % 8);
	if(color)
	{
		if( !(MYGUI_RAM[ybyte*MYGUI_XBYTE_MAX+x]&data) )
		{
			MYGUI_RAM[ybyte*MYGUI_XBYTE_MAX+x] |= data;
			Fresh_Index[ybyte]=1;
		}
	}
	else
	{
		if( (MYGUI_RAM[ybyte*MYGUI_XBYTE_MAX+x]&data) )
		{
			MYGUI_RAM[ybyte*MYGUI_XBYTE_MAX+x] &= (~data);
			Fresh_Index[ybyte]=1;
		}
		
	}
	//TODO end
}

void MYGUI_Clear(uint8_t data,uint8_t dir)
{
	uint16_t i,j;
	for(i=0;i<MYGUI_XBYTE_MAX;i++)
		for(j=0;j<MYGUI_YBYTE_MAX;j++)
			MYGUI_RAM[j*MYGUI_XBYTE_MAX+i] = data;
	for(i = 0; i < MYGUI_YBYTE_MAX;i++)
		Fresh_Index[i] = 1;
	MYGUI_Refresh();
	MYGUI_DIRECTION = dir%4;
}

void MYGUI_DrawLine(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint8_t color)
{
	int16_t dx=x2-x1,dy=y2-y1,i;
	if(x1==x2&&y1==y2)
	{
		MYGUI_DrawPoint(x1,y1,color);
		return;
	}
	if(x1>x2)
		dx = x1-x2;
	else
		dx = x2-x1;
	if(y1>y2)
		dy = y1-y2;
	else
		dy = y2-y1;
	if(dx>dy)
	{
		dx = x2-x1;
		dy = y2-y1;
		for(i=x1;i!=x2;i=i+(dx>0?1:-1))
			MYGUI_DrawPoint(i,(int16_t)(dy*1.0/dx*(i-x1)+y1+0.5),color);
		MYGUI_DrawPoint(i,(int16_t)(dy*1.0/dx*(i-x1)+y1+0.5),color);
	}
	else
	{
		dx = x2-x1;
		dy = y2-y1;
		for(i=y1;i!=y2;i=i+(dy>0?1:-1))
			MYGUI_DrawPoint((int16_t)(dx*1.0/dy*(i-y1)+x1+0.5),i,color);
		MYGUI_DrawPoint((int16_t)(dx*1.0/dy*(i-y1)+x1+0.5),i,color);
	}
}

void MYGUI_DrawRectangle(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint8_t fill,uint8_t color)
{
	int16_t i,j;
	if(x1>x2)
	{
		i=x1;x1=x2;x2=i;
	}
	if(y1>y2)
	{
		i=y1;y1=y2;y2=i;
	}
	if(fill)
	{
		for(i=x1; i<=x2; i++)
        	for(j=y1; j<=y2; j++)
            	MYGUI_DrawPoint(i,j,color);
	}
	else
	{
		for(i=x1; i<=x2; i++)
		{
			MYGUI_DrawPoint(i,y1,color);
			MYGUI_DrawPoint(i,y2,color);
		}
		for(i=y1; i<=y2; i++)
		{
			MYGUI_DrawPoint(x1,i,color);
			MYGUI_DrawPoint(x2,i,color);
		}
	}
}

void MYGUI_DrawCircle(int16_t x,int16_t y,uint16_t r,uint8_t fill,uint8_t color)
{
	int16_t i,j,jlim;
	for(i=0;i<=r*0.71;i++)
	{
		int16_t j2 = r*r-(i)*(i);
		for(j=0;j*j<=j2;j++);
		jlim = --j;
		if(j2-j*j>(j+1)*(j+1)-j2)
			jlim = j+1;
		if(fill)
		{
			for(j=0;j<=jlim;j++)
			{
				MYGUI_DrawPoint(x+i,y+j,color);
				MYGUI_DrawPoint(x+i,y-j,color);
				MYGUI_DrawPoint(x-i,y+j,color);
				MYGUI_DrawPoint(x-i,y-j,color);
				MYGUI_DrawPoint(x+j,y+i,color);
				MYGUI_DrawPoint(x+j,y-i,color);
				MYGUI_DrawPoint(x-j,y+i,color);
				MYGUI_DrawPoint(x-j,y-i,color);
			}
		}
		else
		{
			MYGUI_DrawPoint(x+i,y+jlim,color);
			MYGUI_DrawPoint(x+i,y-jlim,color);
			MYGUI_DrawPoint(x-i,y+jlim,color);
			MYGUI_DrawPoint(x-i,y-jlim,color);
			MYGUI_DrawPoint(x+jlim,y+i,color);
			MYGUI_DrawPoint(x+jlim,y-i,color);
			MYGUI_DrawPoint(x-jlim,y+i,color);
			MYGUI_DrawPoint(x-jlim,y-i,color);
		}
	}
}

void MYGUI_Load(int16_t x,int16_t y,uint16_t xlen,uint16_t ylen,const uint8_t *data,uint8_t color)
{
	int16_t i,j;
	for(j=0;j<ylen;j++)
	{
		for(i=0;i<xlen;i++)
		{
			if( data[(j/8)*xlen+i] & (0x01<<(j%8)) )
				MYGUI_DrawPoint(x+i,y+j,color);
			else
				MYGUI_DrawPoint(x+i,y+j,!color);
		}
	}
}

#if (MYGUI_USE_FONTS == 1)

void MYGUI_Print(int16_t x,int16_t y,const char *data,uint16_t datalen,FONT_SIZE size,uint8_t color)
{
	int16_t tmp;
	if(size==FONT_0806)
	{
		while(datalen--)
		{
			tmp = *data;
			if(' '<=tmp && tmp <= '~')
			{
				tmp -= ' ';
				MYGUI_Load(x,y,6,8,(uint8_t *)ASCII_0806[tmp],color);
				x+=6;
			}
			data++;
		}
	}
	else if(size==FONT_1206)
	{
		while(datalen--)
		{
			tmp = *data;
			if(' '<=tmp && tmp <= '~')
			{
				tmp -= ' ';
				MYGUI_Load(x,y,6,12,(uint8_t *)ASCII_1206[tmp],color);
				x+=6;
			}
			data++;
		}
	}
	else if(size==FONT_1608)
	{
		while(datalen--)
		{
			tmp = *data;
			if(' '<=tmp && tmp <= '~')
			{
				tmp -= ' ';
				MYGUI_Load(x,y,8,16,(uint8_t *)ASCII_1608[tmp],color);
				x+=8;
			}
			data++;
			
		}
	}
	else if(size==FONT_2412)
	{
		while(datalen--)
		{
			tmp = *data;
			if(' '<=tmp && tmp <= '~')
			{
				tmp -= ' ';
				MYGUI_Load(x,y,12,24,(uint8_t *)ASCII_2412[tmp],color);
				x+=12;
			}
			data++;
			
		}
	}
}

void MYGUI_PrintNum(int16_t x,int16_t y,int32_t num,FONT_SIZE size,uint8_t color)
{
	uint8_t data[11] = {0};
	uint8_t i = 0;;
	if(num < 0)
	{
		data[i++] = '-';
		num = -num;
	}
	for(i=10;i>0;i--)
    {
    	data[i] = num%10 + '0';
		num /=10;
		if(num == 0)
			break;
    }
	MYGUI_Print(x,y,(char*)data,11,size,color);
}

#endif
