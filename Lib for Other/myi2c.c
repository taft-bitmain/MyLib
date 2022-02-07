/*******************************************************************************
 * @file     myi2c.c
 * @brief    simulation i2c interface
 * @version  V1.3
 * @date     2021.8.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "myi2c.h"

#define     SET_SCL(hi2c)       ( hi2c->SCL_Port->BSRR = hi2c->SCL_Bit )
#define     RESET_SCL(hi2c)     ( hi2c->SCL_Port->BRR  = hi2c->SCL_Bit )
#define     SET_SDA(hi2c)       ( hi2c->SDA_Port->BSRR = hi2c->SDA_Bit )
#define     RESET_SDA(hi2c)     ( hi2c->SDA_Port->BRR  = hi2c->SDA_Bit )
#define     GET_SDA(hi2c)       ( hi2c->SDA_Port->IDR  & hi2c->SDA_Bit )
#define     DELAY()             delay(hi2c->Speed)

static void delay(uint16_t x)
{
	uint16_t t = x * 8;
	while(t--);
}

void MyI2C_Start(MyI2C *hi2c)
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
uint8_t MyI2C_Transmit(MyI2C *hi2c,uint8_t dat)
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
uint8_t MyI2C_Receive(MyI2C *hi2c,uint8_t ack)
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
void MyI2C_End(MyI2C *hi2c)
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




uint8_t MyI2C_WriteReg(MyI2C *hi2c,uint8_t RegAddr,uint8_t dat)
{
	uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,RegAddr);
	flag &= MyI2C_Transmit(hi2c,dat);
	MyI2C_End(hi2c);
	return flag;
}

uint8_t MyI2C_ReadReg(MyI2C *hi2c,uint8_t RegAddr)
{
	uint8_t  flag = 1,dat;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,RegAddr);
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)|0x01);
	dat = MyI2C_Receive(hi2c,0);
	MyI2C_End(hi2c);
	return dat;
}

uint8_t	MyI2C_WriteReg2 (MyI2C *hi2c,uint8_t RegAddr,uint16_t dat)
{
    uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,RegAddr);
	flag &= MyI2C_Transmit(hi2c,(uint8_t)(dat>>8));
    flag &= MyI2C_Transmit(hi2c,(uint8_t)(dat));
	MyI2C_End(hi2c);
	return flag;
}

uint16_t MyI2C_ReadReg2 (MyI2C *hi2c,uint8_t RegAddr)
{
    uint8_t flag = 1;
    uint16_t dat;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,RegAddr);
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)|0x01);
	dat = MyI2C_Receive(hi2c,1);
    dat <<= 8;
    dat |= MyI2C_Receive(hi2c,0);
	MyI2C_End(hi2c);
	return dat;
}


uint8_t MyI2C_WriteMem(MyI2C *hi2c,uint8_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,MemAddr);
	while(len--)
		flag &= MyI2C_Transmit(hi2c,*dat++);
	MyI2C_End(hi2c);
	return flag;
}

uint8_t MyI2C_ReadMem(MyI2C *hi2c,uint8_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,MemAddr);
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)|0x01);
	while(--len>0)
		*dat++ = MyI2C_Receive(hi2c,1);
	*dat = MyI2C_Receive(hi2c,0);
	MyI2C_End(hi2c);
	return flag;
}

uint8_t MyI2C_WriteMem2(MyI2C *hi2c,uint16_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,(uint8_t)(MemAddr>>8));
	flag &= MyI2C_Transmit(hi2c,(uint8_t) MemAddr);
	while(len--)
		flag &= MyI2C_Transmit(hi2c,*dat++);
	MyI2C_End(hi2c);
	return flag;
}

uint8_t MyI2C_ReadMem2(MyI2C *hi2c,uint16_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,(uint8_t)(MemAddr>>8));
	flag &= MyI2C_Transmit(hi2c,(uint8_t) MemAddr);
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)|0x01);
	while(--len>0)
		*dat++ = MyI2C_Receive(hi2c,1);
	*dat = MyI2C_Receive(hi2c,0);
	MyI2C_End(hi2c);
	return flag;
}


uint8_t	MyI2C_WriteByte(MyI2C *hi2c,uint8_t dat)
{
    uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	flag &= MyI2C_Transmit(hi2c,dat);
	MyI2C_End(hi2c);
	return flag;
}

uint8_t	MyI2C_ReadByte(MyI2C *hi2c)
{
    uint8_t dat;
	MyI2C_Start(hi2c);
	MyI2C_Transmit(hi2c,(hi2c->DevAddr)|0x01);
	dat = MyI2C_Receive(hi2c,0);
	MyI2C_End(hi2c);
	return dat;
}

uint8_t MyI2C_WriteBytes(MyI2C *hi2c,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)&0xFE);
	while(len--)
		flag &= MyI2C_Transmit(hi2c,*dat++);
	MyI2C_End(hi2c);
	return flag;
}

uint8_t MyI2C_ReadBytes(MyI2C *hi2c,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyI2C_Start(hi2c);
	flag &= MyI2C_Transmit(hi2c,(hi2c->DevAddr)|0x01);
	while(--len > 0)
		*dat++ = MyI2C_Receive(hi2c,1);
	*dat = MyI2C_Receive(hi2c,0);
	MyI2C_End(hi2c);
	return flag;
}


uint8_t MyI2C_Detect(MyI2C *hi2c)
{
	uint8_t DevAddr,isFind;
    isFind = 0;
    DevAddr = 0x00;
	for(uint8_t i = 0;i < 128; i++,DevAddr += 0x02)
	{	
		MyI2C_Start(hi2c);
		isFind = MyI2C_Transmit(hi2c,DevAddr);
		MyI2C_End(hi2c);
        if(isFind)
        {
            hi2c->DevAddr = DevAddr;
            return isFind;
        }
	}
	return 0;
}
