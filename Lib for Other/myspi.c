/*******************************************************************************
 * @file     myspi.c
 * @brief    simulation spi interface
 * @version  V1.1
 * @date     2021.8.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "myspi.h"

#define     SET_SCK(hspi)       ( hspi->SCK_Port->BSRR  = hspi->SCK_Bit  )
#define     RESET_SCK(hspi)     ( hspi->SCK_Port->BRR   = hspi->SCK_Bit  )
#define     SET_CS(hspi)        ( hspi->CS_Port->BSRR   = hspi->CS_Bit   )
#define     RESET_CS(hspi)      ( hspi->CS_Port->BRR    = hspi->CS_Bit   )
#define     SET_MOSI(hspi)      ( hspi->MOSI_Port->BSRR = hspi->MOSI_Bit )
#define     RESET_MOSI(hspi)    ( hspi->MOSI_Port->BRR  = hspi->MOSI_Bit )
#define     GET_MISO(hspi)      ( hspi->MISO_Port->IDR  & hspi->MISO_Bit )
#define     DELAY()             delay(hspi->Speed)


static void delay(uint16_t x)
{
	uint16_t t = x * 8;
	while(t--);
}

void MySPI_IO_Init(MySPI *hspi)
{
    SET_CS(hspi);
    (hspi->CPOL) ?  SET_SCK(hspi) :RESET_SCK(hspi);
}

void MySPI_TransmitByte(MySPI *hspi,uint8_t dat)
{
    uint8_t i;
	for(i = 0; i < 8; i++)
	{
		if( !(hspi->CPHA) )
        {
            if(dat & (0x80 >> i))
                SET_MOSI(hspi);
            else
                RESET_MOSI(hspi);
        }
		(hspi->CPOL) ?  RESET_SCK(hspi) :SET_SCK(hspi);
		DELAY();
		if( hspi->CPHA )
        {
            if(dat & (0x80 >> i))
                SET_MOSI(hspi);
            else
                RESET_MOSI(hspi);
        }
		(hspi->CPOL) ?  SET_SCK(hspi) :RESET_SCK(hspi);
		DELAY();
	}
}

uint8_t MySPI_ReceiveByte(MySPI *hspi)
{
    uint8_t i,dat_rcv;
	for(i = 0; i < 8; i++)
	{
        (hspi->CPOL) ?  RESET_SCK(hspi) :SET_SCK(hspi);
        DELAY();
        dat_rcv <<= 1;
        if(!(hspi->CPHA))
            dat_rcv |= (GET_MISO(hspi) ? 1 : 0);
        (hspi->CPOL) ?  SET_SCK(hspi) :RESET_SCK(hspi);
        DELAY();
        if(hspi->CPHA)
            dat_rcv |= (GET_MISO(hspi) ? 1 : 0);
	}
    return dat_rcv;
}

uint8_t MySPI_TransmitReceiveByte(MySPI *hspi,uint8_t dat)
{
    uint8_t i,dat_rcv;
	for(i = 0; i < 8; i++)
	{
		if( !(hspi->CPHA) )
        {
            if(dat & (0x80 >> i))
                SET_MOSI(hspi);
            else
                RESET_MOSI(hspi);
        }
		(hspi->CPOL) ?  RESET_SCK(hspi) :SET_SCK(hspi);
		DELAY();
        dat <<= 1;
        if(!(hspi->CPHA))
            dat |= (GET_MISO(hspi) ? 1 : 0);
		if( hspi->CPHA )
        {
            if(dat & (0x80 >> i))
                SET_MOSI(hspi);
            else
                RESET_MOSI(hspi);
        }
		(hspi->CPOL) ?  SET_SCK(hspi) :RESET_SCK(hspi);
		DELAY();
        if(hspi->CPHA)
            dat |= (GET_MISO(hspi) ? 1 : 0);
	}
    return dat_rcv;
}

void MySPI_WriteReg(MySPI *hspi,uint8_t RegAddr,uint8_t dat)
{
	RESET_CS(hspi);
	DELAY();
	MySPI_TransmitByte(hspi,RegAddr);
    MySPI_TransmitByte(hspi,dat);
	SET_CS(hspi);
	DELAY();
}
uint8_t MySPI_ReadReg(MySPI *hspi,uint8_t RegAddr)
{
	uint8_t dat;
	RESET_CS(hspi);
	DELAY();
	MySPI_TransmitByte(hspi,RegAddr);
	dat = MySPI_ReceiveByte(hspi);
	SET_CS(hspi);
	DELAY();
	return dat;
}
void MySPI_WriteMem(MySPI *hspi,uint8_t RegAddr,uint8_t *dat,uint16_t len)
{
	RESET_CS(hspi);
	DELAY();
	MySPI_TransmitByte(hspi,RegAddr);
	while(len--)
	{
		MySPI_TransmitByte(hspi,*dat);
		dat++;
	}
	SET_CS(hspi);
	DELAY();
}
void MySPI_ReadMem(MySPI *hspi,uint8_t RegAddr,uint8_t *dat,uint16_t len)
{
	RESET_CS(hspi);
	DELAY();
	MySPI_TransmitByte(hspi,RegAddr);
	
	while(len--)
	{
		*dat = MySPI_ReceiveByte(hspi);
		dat++;
	}
	SET_CS(hspi);
	DELAY();
}
void MySPI_Write(MySPI *hspi,uint8_t *dat,uint16_t len)
{
	RESET_CS(hspi);
	DELAY();
	while(len--)
	{
		MySPI_TransmitByte(hspi,*dat);
		dat++;
	}
	SET_CS(hspi);
	DELAY();
}
void MySPI_Read(MySPI *hspi,uint8_t *dat,uint16_t len)
{
	RESET_CS(hspi);
	DELAY();
	while(len--)
	{
		*dat = MySPI_ReceiveByte(hspi);
		dat++;
	}
	SET_CS(hspi);
	DELAY();
}
void MySPI_ReadWrite(MySPI *hspi,uint8_t *dat_in,uint8_t *dat_out,uint16_t len)
{
	RESET_CS(hspi);
	DELAY();
	while(len--)
	{
		*dat_out = MySPI_TransmitReceiveByte(hspi,*dat_in);
		dat_in++;dat_out++;
	}
	SET_CS(hspi);
	DELAY();
}
