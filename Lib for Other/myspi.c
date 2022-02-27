/*******************************************************************************
 * @file     myspi.c
 * @brief    simulation spi interface
 * @version  V1.1
 * @date     2021.8.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "myspi.h"

#define     SET_SCK(hspi)       ( hspi->SCK_port->BSRR  = hspi->SCK_pin  )
#define     RESET_SCK(hspi)     ( hspi->SCK_port->BRR   = hspi->SCK_pin  )
#define     SET_CS(hspi)        ( hspi->CS_port->BSRR   = hspi->CS_pin   )
#define     RESET_CS(hspi)      ( hspi->CS_port->BRR    = hspi->CS_pin   )
#define     SET_MOSI(hspi)      ( hspi->MOSI_port->BSRR = hspi->MOSI_pin )
#define     RESET_MOSI(hspi)    ( hspi->MOSI_port->BRR  = hspi->MOSI_pin )
#define     GET_MISO(hspi)      ( hspi->MISO_port->IDR  & hspi->MISO_pin )


static inline void delay( void )
{
	__NOP();__NOP();
}


inline void MySPI_IO_Init( myspi * hspi )
{
    // TODO : configurate pin(CS/SCK/MOSI/MISO)
    
    SET_CS( hspi );
    (hspi->CPOL) ?  SET_SCK(hspi) :RESET_SCK(hspi);
}

inline void myspi_transceive_start( myspi * hspi )
{
    (hspi->CPOL) ?  SET_SCK(hspi) : RESET_SCK(hspi);
    RESET_CS( hspi );
}

void myspi_transmit_byte( myspi * hspi, uint8_t dat )
{
    uint8_t i;
	for(i = 0; i < 8; i++)
	{
		if( !(hspi->CPHA) )
        {
            if( dat & 0x80 )
                SET_MOSI(hspi);
            else
                RESET_MOSI(hspi);
        }
        
		(hspi->CPOL) ?  RESET_SCK(hspi) :SET_SCK(hspi);
        
		delay();
        
		if( hspi->CPHA )
        {
            if(dat & 0x80)
                SET_MOSI(hspi);
            else
                RESET_MOSI(hspi);
        }
        
		(hspi->CPOL) ?  SET_SCK(hspi) :RESET_SCK(hspi);
        
        dat <<= 1;
        
		delay();
	}
}

uint8_t myspi_receive_byte(myspi *hspi)
{
    uint8_t i,dat_rcv;
	for(i = 0; i < 8; i++)
	{
        dat_rcv <<= 1;
        
        (hspi->CPOL) ?  RESET_SCK(hspi) :SET_SCK(hspi);
        
        delay();
        
        if(!(hspi->CPHA))
            dat_rcv |= (GET_MISO(hspi) ? 1 : 0);
        
        (hspi->CPOL) ?  SET_SCK(hspi) :RESET_SCK(hspi);
        
        delay();
        
        if(hspi->CPHA)
            dat_rcv |= (GET_MISO(hspi) ? 1 : 0);
	}
    return dat_rcv;
}

uint8_t myspi_transceive_byte(myspi *hspi,uint8_t dat)
{
    uint8_t i,dat_rcv;
	for(i = 0; i < 8; i++)
	{
		dat_rcv <<= 1;
        
        if( !(hspi->CPHA) )
        {
            if(dat & 0x80)
                SET_MOSI(hspi);
            else
                RESET_MOSI(hspi);
        }
        
		(hspi->CPOL) ?  RESET_SCK(hspi) :SET_SCK(hspi);
        
		delay();
                
        if(!(hspi->CPHA))
            dat_rcv |= (GET_MISO(hspi) ? 1 : 0);
        
		if( hspi->CPHA )
        {
            if(dat & 0x80 )
                SET_MOSI(hspi);
            else
                RESET_MOSI(hspi);
        }
        
		(hspi->CPOL) ?  SET_SCK(hspi) :RESET_SCK(hspi);
        
		delay();
                
        if(hspi->CPHA)
            dat_rcv |= (GET_MISO(hspi) ? 1 : 0);
        
        dat <<= 1;
	}
    return dat_rcv;
}

inline void myspi_transceive_end(myspi *hspi)
{
    SET_CS(hspi);
}

void myspi_write_reg(myspi *hspi,uint8_t RegAddr,uint8_t dat)
{
	myspi_transceive_start(hspi);
	delay();
    
	myspi_transmit_byte(hspi,RegAddr);
    
    myspi_transmit_byte(hspi,dat);
    
	myspi_transceive_end(hspi);
	delay();
}
uint8_t myspi_read_reg(myspi *hspi,uint8_t RegAddr)
{
	uint8_t dat;
    
	myspi_transceive_start(hspi);
	delay();
    
	myspi_transmit_byte(hspi,RegAddr);
	dat = myspi_receive_byte(hspi);
    
	myspi_transceive_end(hspi);
	delay();
    
	return dat;
}
void myspi_write_mem(myspi *hspi,uint8_t RegAddr,uint8_t *dat,uint16_t len)
{
	myspi_transceive_start(hspi);
	delay();
    
	myspi_transmit_byte(hspi,RegAddr);
	while(len--)
	{
		myspi_transmit_byte(hspi,*dat);
		dat++;
	}
    
	myspi_transceive_end(hspi);
	delay();
}
void myspi_read_mem(myspi *hspi,uint8_t RegAddr,uint8_t *dat,uint16_t len)
{
	myspi_transceive_start(hspi);
	delay();
    
	myspi_transmit_byte(hspi,RegAddr);
	
	while(len--)
	{
		*dat = myspi_receive_byte(hspi);
		dat++;
	}
    
	myspi_transceive_end(hspi);
	delay();
}
void myspi_write(myspi *hspi,uint8_t *dat,uint16_t len)
{
	myspi_transceive_start(hspi);
	delay();
    
	while(len--)
	{
		myspi_transmit_byte(hspi,*dat);
		dat++;
	}
    
	myspi_transceive_end(hspi);
	delay();
}
void myspi_read(myspi *hspi,uint8_t *dat,uint16_t len)
{
	myspi_transceive_start(hspi);
	delay();
    
	while(len--)
	{
		*dat = myspi_receive_byte(hspi);
		dat++;
	}
    
	myspi_transceive_end(hspi);
	delay();
}
void myspi_write_read(myspi *hspi,uint8_t *dat_in,uint8_t *dat_out,uint16_t len)
{
	myspi_transceive_start(hspi);
	delay();
    
	while(len--)
	{
		*dat_out = myspi_transceive_byte(hspi,*dat_in);
		dat_in++;dat_out++;
	}
    
	myspi_transceive_end(hspi);
	delay();
}
