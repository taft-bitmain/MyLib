/*******************************************************************************
 * @file     hmi.c
 * @brief    basic interface to operate uart HMI
 * @version  V1.2
 * @date     2021.7.24
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "hmi.h"

uint8_t	HMI_DataTx[128];
uint8_t	HMI_DataRx[256];
uint8_t	HMI_DataRcvCount;

//重写串口IDLE接收函数
void USART2_IDLE_Callback(uint8_t *data,uint16_t len)
{
	HMI_CommandRecv(data,len);
}

void HMI_CommandSendf(uint8_t *data, uint16_t len)
{
	data[len++] = 0xff;
	data[len++] = 0xff;
	data[len++] = 0xff;
	HAL_UART_Transmit(&HMI_UART,(uint8_t*)data,len,100);
}

void HMI_CommandSend(uint8_t *data, uint16_t len)
{
	HAL_UART_Transmit_DMA(&HMI_UART,(uint8_t*)data,len);
}

uint16_t HMI_CommandRecv(uint8_t *data,uint16_t len)
{
	memcpy(HMI_DataRx,data,len);
	HMI_DataRcvCount += len;
	return len;
}


/********************************* 基本HMI指令 *********************************/
void HMI_page(uint16_t pageid)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"page %d",pageid);
	
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_ref(uint16_t objid)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"ref %d",objid);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_click(uint16_t objid,uint16_t event)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"click %d,%d",objid,event);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_get(uint16_t objid,uint16_t event)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"get %d,%d",objid,event);
	HMI_CommandSendf(HMI_DataTx,len);
	//todo
}
void HMI_vis(uint16_t objid,uint16_t state)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"vis %d,%d",objid,state);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_tsw(uint16_t objid,uint16_t state)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"tsw %d,%d",objid,state);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_add(uint16_t objid,uint16_t ch,uint16_t val)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"add %d,%d,%d",objid,ch,val);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_cle(uint16_t objid,uint16_t ch)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"cle %d,%d",objid,ch);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_addt(uint16_t objid,uint16_t ch,uint8_t *data,uint16_t qyt)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"addt %d,%d,%d",objid,ch,qyt);
	HMI_DataRcvCount = 0;
	HMI_CommandSendf(HMI_DataTx,len);
	uint32_t tickstart = HAL_GetTick();
  while ((HAL_GetTick() - tickstart) < 500)
		if((HMI_DataRcvCount >= 4) && (HMI_DataRx[0] == 0xFE))
			goto ready;
	return ;
ready:
	HMI_CommandSend(data,qyt);
	tickstart = HAL_GetTick();
	while ((HAL_GetTick() - tickstart) < 500)
		if((HMI_DataRcvCount >= 4) && (HMI_DataRx[0] == 0xFD))
			return;
	return ;
}
void HMI_rest(void)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"rest");
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_SetTextStr(char  *obj,char *str)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"%s.txt=\"%s\"",obj,str);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_SetTextNum(char  *obj,int32_t num)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"%s.txt=\"%d\"",obj,num);
	HMI_CommandSendf(HMI_DataTx,len);
}

void HMI_SetText(char *obj,const char *str,...)
{
    va_list arg_ptr;
    uint16_t len,len2;
    va_start(arg_ptr, str);
    len  = sprintf ((char*)HMI_DataTx,"%s.txt=\"",obj);
    len2 = vsprintf((char*)HMI_DataTx+len, str, arg_ptr);
    HMI_DataTx[len+len2] = '\"';
    va_end(arg_ptr);
    HMI_CommandSendf(HMI_DataTx,len+len2+1);
}

uint8_t HMI_GetEvent(uint32_t *event)
{
	uint16_t index;
	if(HMI_DataRcvCount)
	{
		for(index = 0;index<HMI_DataRcvCount;index++)
		{
			if(HMI_DataRx[index] == 0x65)
			{
				*event = (HMI_DataRx[index]<<24) |\
                 (HMI_DataRx[index+1]<<16) |\
								 (HMI_DataRx[index+2]<<8) |\
								  HMI_DataRx[index+3];
				HMI_DataRcvCount = 0;
				return 1;
			}
            if(HMI_DataRx[index] == 0x71)
            {
                *event = (HMI_DataRx[index]<<24) |\
                 (HMI_DataRx[index+1]<<16) |\
                                 (HMI_DataRx[index+2]<<8) |\
                                  HMI_DataRx[index+3];
                HMI_DataRcvCount = 0;
                return 1;
            }
//      if(HMI_DataRx[index] == 0x70)
//			{
//				*event = (HMI_DataRx[index]<<24) |\
//                 (HMI_DataRx[index+1]<<16) |\
//								 (HMI_DataRx[index+2]<<8) |\
//								  HMI_DataRx[index+3];
//				HMI_DataRcvCount = 0;
//				return 1;
//			}
		}
		HMI_DataRcvCount = 0;
	}
	return 0;
}

uint8_t HMI_GetText(char *obj,char *dat)
{
  uint16_t len = sprintf((char*)HMI_DataTx,"get %s.txt",obj);
  HMI_CommandSendf(HMI_DataTx,len);
  len = 100;
  while(len--)
  {
    if(HMI_DataRcvCount)
    {
      uint16_t i;
      if(HMI_DataRx[0] != 0x70)
        goto exit;
      for( i = 1; i < HMI_DataRcvCount; i++ )
      {
        if(HMI_DataRx[i] != 0xFF)
        *dat++ = HMI_DataRx[i];
      }
      *dat = 0;
      HMI_DataRcvCount = 0;
      return 1;
    }
    HAL_Delay(1);
  }
exit:
  HMI_DataRcvCount = 0;
  return 0;
}

/********************************* GUI绘图指令 *********************************/
void HMI_GUI_cls(uint16_t color)
{
	uint16_t len = sprintf((char*)HMI_DataTx,"cls %d",color);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_pic(uint16_t x,uint16_t y,uint16_t picid)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"pic %d,%d,%d",x,y,picid);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_picq(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t picid)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"picq %d,%d,%d,%d,%d",x,y,w,h,picid);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_xpic(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t x0,uint16_t y0,uint16_t picid)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"xpic %d,%d,%d,%d,%d,%d,%d",x,y,w,h,x0,y0,picid);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_xstr(uint16_t x,uint16_t y,uint16_t w,uint16_t h,\
									uint16_t fontid,uint16_t pointcolor,uint16_t backcolor,\
									uint16_t xcenter,uint16_t ycenter,uint16_t sta,\
									char *str)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"xstr %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\"%s\"",\
	  x,y,w,h,fontid,pointcolor,backcolor,xcenter,ycenter,sta,str);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_fill(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"fill %d,%d,%d,%d,%d",x,y,w,h,color);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_line(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"line %d,%d,%d,%d,%d",x1,y1,x2,y2,color);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_draw(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"draw %d,%d,%d,%d,%d",x1,y1,x2,y2,color);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_cir(uint16_t x,uint16_t y,uint16_t r,uint16_t color)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"cir %d,%d,%d,%d",x,y,r,color);
	HMI_CommandSendf(HMI_DataTx,len);
}
void HMI_GUI_cirs(uint16_t x,uint16_t y,uint16_t r,uint16_t color)
{
	uint16_t len = sprintf((char*)HMI_DataTx,\
		"cirs %d,%d,%d,%d",x,y,r,color);
	HMI_CommandSendf(HMI_DataTx,len);
}
