/*******************************************************************************
 * @file     myi2c.c
 * @brief    simulation i2c interface
 * @version  V1.4
 * @date     2022.2.27
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "myi2c.h"

/******************* IO Operation **************************/

#define     SET_SCL(hi2c)       ( hi2c->SCL_port->BSRR = hi2c->SCL_pin )
#define     RESET_SCL(hi2c)     ( hi2c->SCL_port->BRR  = hi2c->SCL_pin )
#define     SET_SDA(hi2c)       ( hi2c->SDA_port->BSRR = hi2c->SDA_pin )
#define     RESET_SDA(hi2c)     ( hi2c->SDA_port->BRR  = hi2c->SDA_pin )
#define     GET_SDA(hi2c)       ( hi2c->SDA_port->IDR  & hi2c->SDA_pin )

#define     DELAY()             myi2c_delay(hi2c->speed)

/******************* Basic Functions **************************/

void myi2c_delay ( uint32_t x )
{
	uint32_t t = x * 8;
	while(t--);
}

void myi2c_io_init ( myi2c * hi2c )
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    //__HAL_RCC_GPIOx_CLK_ENABLE();
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(hi2c->SCL_port, hi2c->SCL_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(hi2c->SDA_port, hi2c->SDA_pin, GPIO_PIN_SET);

    /*Configure GPIO pins : SCL */
    GPIO_InitStruct.Pin = hi2c->SCL_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(hi2c->SCL_port, &GPIO_InitStruct);
    
    /*Configure GPIO pins : SCL */
    GPIO_InitStruct.Pin = hi2c->SDA_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(hi2c->SDA_port, &GPIO_InitStruct);
}

void myi2c_start(myi2c *hi2c)
{
    // set SDA output
	SET_SDA(hi2c);
	RESET_SCL(hi2c);
	DELAY();
	SET_SCL(hi2c);
	DELAY();
	RESET_SDA(hi2c);
	DELAY();
	RESET_SCL(hi2c);
	DELAY();
}
uint8_t myi2c_transmit(myi2c *hi2c,uint8_t dat)
{
	uint8_t index,flag = 0;
    // set SDA output
	for(index = 0;index < 8;index++)
    {
		if(dat & 0x80)
            SET_SDA(hi2c);
        else
            RESET_SDA(hi2c);
        dat <<= 1;
		DELAY();
		SET_SCL(hi2c);
		DELAY();DELAY();
		RESET_SCL(hi2c);
		DELAY();
    }
    // set SDA input
	SET_SDA(hi2c);
    
	DELAY();
	SET_SCL(hi2c);
	DELAY();DELAY();
	if( !GET_SDA(hi2c) )
		flag = 1;
	RESET_SCL(hi2c);
	DELAY();
	return flag;
}
uint8_t myi2c_receive(myi2c *hi2c,uint8_t ack)
{
	uint8_t index,dat = 0x00;
    // set SDA input
	SET_SDA(hi2c);
    
	for(index = 0;index < 8;index++)
    {
		DELAY();
		SET_SCL(hi2c);
		DELAY();DELAY();
        dat <<= 1;
		if( GET_SDA(hi2c) )
			dat |= 0x01;
		RESET_SCL(hi2c);
		DELAY();
    }
    // set SDA output
    if(ack)
        RESET_SDA(hi2c);
    else
        SET_SDA(hi2c);
	DELAY();
	SET_SCL(hi2c);
	DELAY();DELAY();
	RESET_SCL(hi2c);
	DELAY();
    SET_SDA(hi2c);
	return dat;
}
void myi2c_end(myi2c *hi2c)
{
    // set SDA output
	RESET_SDA(hi2c);
	DELAY();
	SET_SCL(hi2c);
	DELAY();
	SET_SDA(hi2c);
	DELAY();
	RESET_SCL(hi2c);
	DELAY();
}

/******************* Main Functions **************************/

uint8_t myi2c_write ( myi2c * hi2c, const uint8_t * dat, uint32_t len )
{
    uint8_t flag = 1;
    uint32_t i;
    
	myi2c_start(hi2c);
    
	flag &= myi2c_transmit(hi2c,( hi2c->slaver_addr << 1) & 0xFE);
	
    for( i = 0; i < len; i++ )
        flag &= myi2c_transmit(hi2c,dat[i]);
    
	myi2c_end(hi2c);
    
	return flag;
}

uint8_t myi2c_write_byte ( myi2c * hi2c, uint16_t addr, uint8_t addr_len, uint8_t dat )
{
    uint8_t flag = 1;
    
	myi2c_start(hi2c);
    
	flag &= myi2c_transmit(hi2c,( hi2c->slaver_addr << 1) & 0xFE);
    
    if( addr == 2 )
        flag &= myi2c_transmit(hi2c,(uint8_t)(addr>>8));
    flag &= myi2c_transmit(hi2c,(uint8_t)(addr));
	
    flag &= myi2c_transmit(hi2c,dat);
    
	myi2c_end(hi2c);
    
	return flag;
}

uint8_t myi2c_write_bytes ( myi2c * hi2c, uint16_t addr, uint8_t addr_len, const uint8_t * dat, uint32_t len )
{
    uint8_t flag = 1;
    uint32_t i;
    
	myi2c_start(hi2c);
    
	flag &= myi2c_transmit(hi2c,( hi2c->slaver_addr << 1) & 0xFE);
    
    if( addr_len == 2 )
        flag &= myi2c_transmit(hi2c,(uint8_t)(addr>>8));
    flag &= myi2c_transmit(hi2c,(uint8_t)(addr));
	
    for( i = 0; i < len; i++ )
        flag &= myi2c_transmit(hi2c,dat[i]);
    
	myi2c_end(hi2c);
    
	return flag;
}

uint8_t myi2c_read ( myi2c * hi2c, uint8_t * dat, uint32_t len )
{
    uint8_t flag = 1;
    uint32_t i;
    
	myi2c_start(hi2c);
    
    flag &= myi2c_transmit(hi2c,( hi2c->slaver_addr << 1) | 0x01);
	
    for( i = 0; i < len - 1; i++ )
        dat[i] = myi2c_receive( hi2c, 1 );
    dat[i] = myi2c_receive( hi2c, 0 );
    
	myi2c_end(hi2c);
    
	return flag;
}


uint8_t myi2c_read_byte ( myi2c * hi2c, uint16_t addr, uint8_t addr_len, uint8_t * dat )
{
    uint8_t flag = 1;
    
	myi2c_start(hi2c);
    
	flag &= myi2c_transmit(hi2c,( hi2c->slaver_addr << 1) & 0xFE);
    
    if( addr_len == 2 )
        flag &= myi2c_transmit(hi2c,(uint8_t)(addr>>8));
    flag &= myi2c_transmit(hi2c,(uint8_t)(addr));
    
    myi2c_start(hi2c);
    
    flag &= myi2c_transmit(hi2c,( hi2c->slaver_addr << 1) | 0x01);
	
    *dat = myi2c_receive( hi2c, 0 );
    
	myi2c_end(hi2c);
    
	return flag;
}

uint8_t myi2c_read_bytes ( myi2c * hi2c, uint16_t addr, uint8_t addr_len, uint8_t * dat, uint32_t len )
{
    uint8_t flag = 1;
    uint32_t i;
    
	myi2c_start(hi2c);
    
	flag &= myi2c_transmit(hi2c,( hi2c->slaver_addr << 1) & 0xFE);
    
    if( addr_len == 2 )
        flag &= myi2c_transmit(hi2c,(uint8_t)(addr>>8));
    flag &= myi2c_transmit(hi2c,(uint8_t)(addr));
    
    myi2c_start(hi2c);
    
    flag &= myi2c_transmit(hi2c,( hi2c->slaver_addr << 1) | 0x01);
	
    for( i = 0; i < len - 1; i++ )
        dat[i] = myi2c_receive( hi2c, 1 );
    dat[i] = myi2c_receive( hi2c, 0 );
    
	myi2c_end(hi2c);
    
	return flag;
}

/******************* Extend Functions **************************/

uint8_t myi2c_detect ( myi2c * hi2c )
{
	uint8_t i,find = 0;
	for( i = 0; i <= 0x7F; i++ )
	{	
		myi2c_start(hi2c);
		find = myi2c_transmit(hi2c,i<<1);
		myi2c_end(hi2c);
        if(find)
        {
            hi2c->slaver_addr = i;
            return 1;
        }
	}
	return 0;
}
