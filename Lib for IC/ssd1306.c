/*******************************************************************************
 * @file     ssd1306.h
 * @brief    drive the oled with SSD1306
 * @version  V1.5
 * @date     2022.4.21
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 ******************************************************************************/

#include "ssd1306.h"



/******************* Main Functions **************************/

void SSD1306_DisPlay_Power(uint8_t flag)
{
	uint8_t cmd[3] = {
		SSD1306_GEPUMP,
		flag==0?SSD1306_GEPUMP_0:SSD1306_GEPUMP_1,
		flag==0?SSD1306_DISPLAY_0:SSD1306_DISPLAY_1
	};
	SSD1306_LL_SendCmd(cmd,3);
}

void SSD1306_DisPlay_EntireOn(uint8_t flag)
{
	uint8_t cmd[1] = {	flag==0? SSD1306_ENTIREON_0: SSD1306_ENTIREON_1};
	SSD1306_LL_SendCmd(cmd,1);
}

void SSD1306_DisPlay_Inverse(uint8_t flag)
{
	uint8_t cmd[1] = {	flag==0? SSD1306_INVERSE_0:SSD1306_INVERSE_1 };
	SSD1306_LL_SendCmd(cmd,1);
}	

void SSD1306_DisPlay_Contrast(uint8_t level)
{
	uint8_t cmd[2] = {SSD1306_CONTRAST,SSD1306_CONTRAST_MASK&level};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_DisPlay_CulRemap(uint8_t flag)
{
	uint8_t cmd[1] = {	flag==0?SSD1306_SEGREMAP_0:SSD1306_SEGREMAP_1};
	SSD1306_LL_SendCmd(cmd,1);
}	

void SSD1306_DisPlay_RowRemap(uint8_t flag)
{
	uint8_t cmd[1] = {	flag==0? SSD1306_OUTPUTDIR_0:SSD1306_OUTPUTDIR_1 };
	SSD1306_LL_SendCmd(cmd,1);
}

//FPS 0,1,2,3,4,5,6,7 -> 2,3,4,5,25,64,128,256
const uint8_t SPEED_FPS[8]={7,4,5,0,6,1,2,3};

void SSD1306_Display_HScroll(uint8_t dir,uint8_t start_page,uint8_t end_page,uint8_t speed)
{
	if(end_page<start_page)
		end_page = start_page;
	uint8_t cmd[9] = {
		SSD1306_SCROLL_DISABLE,
		dir==0? SSD1306_HSCROLL_0:SSD1306_HSCROLL_1, 
		SSD1306_DUMMY_BYTE_00,
		SSD1306_HSCROLL_MASK1&start_page,
		SPEED_FPS[SSD1306_HSCROLL_MASK2&speed],
		SSD1306_HSCROLL_MASK3&end_page,
		SSD1306_DUMMY_BYTE_00,
		SSD1306_DUMMY_BYTE_FF,
		SSD1306_SCROLL_ENABLE
	};
	SSD1306_LL_SendCmd(cmd,9);
}

void SSD1306_Display_VHScroll(uint8_t dir,uint8_t start_page,uint8_t end_page,uint8_t speed,uint8_t offset)
{
	if(end_page<start_page)
		end_page = start_page;
	uint8_t cmd[8] = {
		SSD1306_SCROLL_DISABLE,
		dir==0? SSD1306_VHSCROLL_0:SSD1306_VHSCROLL_1, 
		SSD1306_DUMMY_BYTE_00,
		SSD1306_VHSCROLL_MASK1&start_page,
		SPEED_FPS[SSD1306_VHSCROLL_MASK2&speed],
		SSD1306_VHSCROLL_MASK3&end_page,
		SSD1306_VHSCROLL_MASK4&offset,
		SSD1306_SCROLL_ENABLE
	};
	SSD1306_LL_SendCmd(cmd,8);
}

void SSD1306_Display_VScrollArea(uint8_t row_start,uint8_t row_length)
{
	uint8_t cmd[3] = {
		SSD1306_VSCROLLAREA,
		SSD1306_VSCROLLAREA_MASK1&row_start,
		SSD1306_VSCROLLAREA_MASK2&row_length,
	};
	SSD1306_LL_SendCmd(cmd,3);
}

void SSD1306_Display_ScrollStop(void)
{
	uint8_t cmd[1] = {SSD1306_SCROLL_DISABLE};
	SSD1306_LL_SendCmd(cmd,1);
}

void SSD1306_Address_Mode(uint8_t mode)
{
	uint8_t cmd[2] = {SSD1306_SETMEMADDRMODE, mode};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_Address_Culumn_M2(uint8_t addr)
{
	uint8_t cmd[2] = {
		SSD1306_LCUL4PAGEADDR|(0x0F&(addr)),
		SSD1306_HCUL4PAGEADDR|(0x0F&(addr>>4))
	};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_Address_Page_M2(uint8_t addr)
{
	uint8_t cmd[1] = {SSD1306_SETPAGESTART|(0x07&addr)};
	SSD1306_LL_SendCmd(cmd,1);
}

void SSD1306_Address_Culumn_M0M1(uint8_t start,uint8_t end)
{
	uint8_t cmd[3] = {
		SSD1306_SETCULADDR,
		SSD1306_SETCULADDR_MASK1&start,
		SSD1306_SETCULADDR_MASK2&end
	};
	SSD1306_LL_SendCmd(cmd,3);
}

void SSD1306_Address_Page_M0M1(uint8_t start,uint8_t end)
{
	uint8_t cmd[3] = {
		SSD1306_SETPAGEADDR,
		SSD1306_SETPAGEADDR_MASK1&start,
		SSD1306_SETPAGEADDR_MASK2&end
	};
	SSD1306_LL_SendCmd(cmd,3);
}


void SSD1306_Set_Clock(uint8_t count)
{
	uint8_t cmd[2] = {SSD1306_SETCLOCK,count};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_Set_Precharge(uint8_t count)
{
	uint8_t cmd[2] = {SSD1306_SETPREuint8_tGE,count};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_Set_Vcomh(uint8_t level)
{
	uint8_t cmd[2] = {SSD1306_SETVCOMH,SSD1306_SETVCOMH_MASk&level};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_Set_MuxRatio(uint8_t count)
{
	if(count<15) count = 15;
	if(count>63) count = 63;
	uint8_t cmd[2] = {SSD1306_MULRATIO,count};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_Set_Offset(uint8_t count)
{
	uint8_t cmd[2] = {SSD1306_DISPOFFSET,SSD1306_DISPOFFSET_MASK&count};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_Set_COMPinCfg(uint8_t mode)
{
	uint8_t cmd[2] = {SSD1306_COMPINCFG,mode};
	SSD1306_LL_SendCmd(cmd,2);
}

void SSD1306_Init(void)
{
    SSD1306_LL_Init();
	SSD1306_DisPlay_Power(0);
	SSD1306_DisPlay_EntireOn(0);
	SSD1306_DisPlay_Inverse(0);
	SSD1306_DisPlay_Contrast(0xFF);
	SSD1306_DisPlay_CulRemap(1);
	SSD1306_DisPlay_RowRemap(1);
    
	SSD1306_Address_Mode(SSD1306_ADDRMODE_2);
	SSD1306_Address_Page_M2(0);
	SSD1306_Address_Culumn_M2(0);
//  or
//	SSD1306_Address_Mode(SSD1306_ADDRMODE_0);
//	SSD1306_Address_Culumn_M0M1(0,127);
//	SSD1306_Address_Page_M0M1(0,7);


    
#if SSD1306_Y_MAX == 32
    //SSD1306_Set_COMPinCfg(SSD1306_COMPINCFG_0); // 128*32
#elif SSD1306_Y_MAX == 64
	SSD1306_Set_COMPinCfg(SSD1306_COMPINCFG_1); // 128*64
#else
#error SSD1306 screen size define invalid(only can be 32 or 64)!
#endif

	SSD1306_Set_Clock(0xF0);
	SSD1306_Set_Precharge(0x22);
	SSD1306_Set_Vcomh(0x20);
	SSD1306_Set_MuxRatio(SSD1306_Y_MAX - 1);
	SSD1306_Set_Offset(0x00);

	SSD1306_DisPlay_Power(1);
}

void  SSD1306_Refresh(uint8_t *data)
{
    SSD1306_Address_Culumn_M0M1(0,127);
    SSD1306_Address_Page_M0M1(0,7);
    SSD1306_LL_SendData(data,128*8);
}

void  SSD1306_RefreshByIndex(uint8_t *index,uint8_t *data)
{
	int i;
	for(i=0;i<SSD1306_Y_MAX/8;i++)
	{
		if(index[i])
		{
			index[i] = 0;
			SSD1306_Address_Page_M2(i);
			SSD1306_Address_Culumn_M2(0);
			SSD1306_LL_SendData(data+SSD1306_X_MAX*i,128);
		}
	}
}
