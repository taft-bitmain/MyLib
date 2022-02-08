/*******************************************************************************
 * @file     ssd1306.h
 * @brief    drive the oled with SSD1306
 * @version  V1.3
 * @date     2021.7.29
 * @author   RainingRabbits 1466586342@qq.com
 * @note     see https://blog.csdn.net/qq_36461474/article/details/112591234
 * @code     UTF-8
 ******************************************************************************/

#include "ssd1306.h"

/***************** Basic Interface *****************/

void OLED_SendInit(void)
{
#if ( OLED_INTERFACE == 0 )
	;
#else
	OLED_WritePinRST(0);
    MySPI_IO_Init(&myspi1);
	HAL_Delay(1);
	OLED_WritePinRST(1);
#endif
}

inline void OLED_SendCmd(uint8_t *data,unsigned int len)
{
    
#if ( OLED_INTERFACE == 0 )
	MyI2C_WriteMem(&myi2c1,OLED_I2C_CMD,data,len);
#else
	OLED_WritePinDC(0);
	MySPI_Write(&myspi1,data,len);
#endif
}
inline void OLED_SendData(uint8_t *data,unsigned int len)
{
#if ( OLED_INTERFACE == 0 )
	MyI2C_WriteMem(&myi2c1,OLED_I2C_DATA,data,len);
#else
	OLED_WritePinDC(1);
	MySPI_Write(&myspi1,data,len);
#endif
}

/************* Basic Interface end *****************/


void OLED_DisPlay_Power(uint8_t flag)
{
	uint8_t cmd[3] = {
		OLED_GEPUMP,
		flag==0?OLED_GEPUMP_0:OLED_GEPUMP_1,
		flag==0?OLED_DISPLAY_0:OLED_DISPLAY_1
	};
	OLED_SendCmd(cmd,3);
}

void OLED_DisPlay_EntireOn(uint8_t flag)
{
	uint8_t cmd[1] = {	flag==0? OLED_ENTIREON_0: OLED_ENTIREON_1};
	OLED_SendCmd(cmd,1);
}

void OLED_DisPlay_Inverse(uint8_t flag)
{
	uint8_t cmd[1] = {	flag==0? OLED_INVERSE_0:OLED_INVERSE_1 };
	OLED_SendCmd(cmd,1);
}	

void OLED_DisPlay_Contrast(uint8_t level)
{
	uint8_t cmd[2] = {OLED_CONTRAST,OLED_CONTRAST_MASK&level};
	OLED_SendCmd(cmd,2);
}

void OLED_DisPlay_CulRemap(uint8_t flag)
{
	uint8_t cmd[1] = {	flag==0?OLED_SEGREMAP_0:OLED_SEGREMAP_1};
	OLED_SendCmd(cmd,1);
}	

void OLED_DisPlay_RowRemap(uint8_t flag)
{
	uint8_t cmd[1] = {	flag==0? OLED_OUTPUTDIR_0:OLED_OUTPUTDIR_1 };
	OLED_SendCmd(cmd,1);
}

//FPS 0,1,2,3,4,5,6,7 -> 2,3,4,5,25,64,128,256
const uint8_t SPEED_FPS[8]={7,4,5,0,6,1,2,3};

void OLED_Display_HScroll(uint8_t dir,uint8_t start_page,uint8_t end_page,uint8_t speed)
{
	if(end_page<start_page)
		end_page = start_page;
	uint8_t cmd[9] = {
		OLED_SCROLL_DISABLE,
		dir==0? OLED_HSCROLL_0:OLED_HSCROLL_1, 
		OLED_DUMMY_BYTE_00,
		OLED_HSCROLL_MASK1&start_page,
		SPEED_FPS[OLED_HSCROLL_MASK2&speed],
		OLED_HSCROLL_MASK3&end_page,
		OLED_DUMMY_BYTE_00,
		OLED_DUMMY_BYTE_FF,
		OLED_SCROLL_ENABLE
	};
	OLED_SendCmd(cmd,9);
}

void OLED_Display_VHScroll(uint8_t dir,uint8_t start_page,uint8_t end_page,uint8_t speed,uint8_t offset)
{
	if(end_page<start_page)
		end_page = start_page;
	uint8_t cmd[8] = {
		OLED_SCROLL_DISABLE,
		dir==0? OLED_VHSCROLL_0:OLED_VHSCROLL_1, 
		OLED_DUMMY_BYTE_00,
		OLED_VHSCROLL_MASK1&start_page,
		SPEED_FPS[OLED_VHSCROLL_MASK2&speed],
		OLED_VHSCROLL_MASK3&end_page,
		OLED_VHSCROLL_MASK4&offset,
		OLED_SCROLL_ENABLE
	};
	OLED_SendCmd(cmd,8);
}

void OLED_Display_VScrollArea(uint8_t row_start,uint8_t row_length)
{
	uint8_t cmd[3] = {
		OLED_VSCROLLAREA,
		OLED_VSCROLLAREA_MASK1&row_start,
		OLED_VSCROLLAREA_MASK2&row_length,
	};
	OLED_SendCmd(cmd,3);
}

void OLED_Display_ScrollStop(void)
{
	uint8_t cmd[1] = {OLED_SCROLL_DISABLE};
	OLED_SendCmd(cmd,1);
}

void OLED_Address_Mode(uint8_t mode)
{
	uint8_t cmd[2] = {OLED_SETMEMADDRMODE, mode};
	OLED_SendCmd(cmd,2);
}

void OLED_Address_Culumn_M2(uint8_t addr)
{
	uint8_t cmd[2] = {
		OLED_LCUL4PAGEADDR|(0x0F&(addr)),
		OLED_HCUL4PAGEADDR|(0x0F&(addr>>4))
	};
	OLED_SendCmd(cmd,2);
}

void OLED_Address_Page_M2(uint8_t addr)
{
	uint8_t cmd[1] = {OLED_SETPAGESTART|(0x07&addr)};
	OLED_SendCmd(cmd,1);
}

void OLED_Address_Culumn_M0M1(uint8_t start,uint8_t end)
{
	uint8_t cmd[3] = {
		OLED_SETCULADDR,
		OLED_SETCULADDR_MASK1&start,
		OLED_SETCULADDR_MASK2&end
	};
	OLED_SendCmd(cmd,3);
}

void OLED_Address_Page_M0M1(uint8_t start,uint8_t end)
{
	uint8_t cmd[3] = {
		OLED_SETPAGEADDR,
		OLED_SETPAGEADDR_MASK1&start,
		OLED_SETPAGEADDR_MASK2&end
	};
	OLED_SendCmd(cmd,3);
}


void OLED_Set_Clock(uint8_t count)
{
	uint8_t cmd[2] = {OLED_SETCLOCK,count};
	OLED_SendCmd(cmd,2);
}

void OLED_Set_Precharge(uint8_t count)
{
	uint8_t cmd[2] = {OLED_SETPREuint8_tGE,count};
	OLED_SendCmd(cmd,2);
}

void OLED_Set_Vcomh(uint8_t level)
{
	uint8_t cmd[2] = {OLED_SETVCOMH,OLED_SETVCOMH_MASk&level};
	OLED_SendCmd(cmd,2);
}

void OLED_Set_MuxRatio(uint8_t count)
{
	if(count<15) count = 15;
	if(count>63) count = 63;
	uint8_t cmd[2] = {OLED_MULRATIO,count};
	OLED_SendCmd(cmd,2);
}

void OLED_Set_Offset(uint8_t count)
{
	uint8_t cmd[2] = {OLED_DISPOFFSET,OLED_DISPOFFSET_MASK&count};
	OLED_SendCmd(cmd,2);
}

void OLED_Set_COMPinCfg(uint8_t mode)
{
	uint8_t cmd[2] = {OLED_COMPINCFG,mode};
	OLED_SendCmd(cmd,2);
}

void OLED_Init(void)
{
    OLED_SendInit();
	OLED_DisPlay_Power(0);
	OLED_DisPlay_EntireOn(0);
	OLED_DisPlay_Inverse(0);
	OLED_DisPlay_Contrast(0xFF);
	OLED_DisPlay_CulRemap(1);
	OLED_DisPlay_RowRemap(1);
    
	OLED_Address_Mode(OLED_ADDRMODE_2);
	OLED_Address_Page_M2(0);
	OLED_Address_Culumn_M2(0);
//  or
//	OLED_Address_Mode(OLED_ADDRMODE_0);
//	OLED_Address_Culumn_M0M1(0,127);
//	OLED_Address_Page_M0M1(0,7);

	OLED_Set_Clock(0xF0);
	OLED_Set_Precharge(0x22);
	OLED_Set_Vcomh(0x20);
	OLED_Set_MuxRatio(OLED_Y_MAX - 1);
	OLED_Set_Offset(0x00);
    
    /*********** todo *****************/
	OLED_Set_COMPinCfg(OLED_COMPINCFG_0);
    
	OLED_DisPlay_Power(1);
}

void  OLED_Refresh(uint8_t *data)
{
    OLED_Address_Culumn_M0M1(0,127);
    OLED_Address_Page_M0M1(0,7);
    OLED_SendData(data,128*8);
}

void  OLED_RefreshByIndex(uint8_t *index,uint8_t *data)
{
	int i;
	for(i=0;i<OLED_Y_MAX/8;i++)
	{
		if(index[i])
		{
			index[i] = 0;
			OLED_Address_Page_M2(i);
			OLED_Address_Culumn_M2(0);
			OLED_SendData(data+OLED_X_MAX*i,128);
		}
	}
}
