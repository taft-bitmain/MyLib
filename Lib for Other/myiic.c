/*******************************************************************************
 * @file     myiic.c
 * @brief    simulation iic interface
 * @version  V1.3
 * @date     2021.8.6
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "myiic.h"

#define     SET_SCL(hiic)       ( hiic->SCL_Port->BSRR = hiic->SCL_Bit )
#define     RESET_SCL(hiic)     ( hiic->SCL_Port->BRR  = hiic->SCL_Bit )
#define     SET_SDA(hiic)       ( hiic->SDA_Port->BSRR = hiic->SDA_Bit )
#define     RESET_SDA(hiic)     ( hiic->SDA_Port->BRR  = hiic->SDA_Bit )
#define     GET_SDA(hiic)       ( hiic->SDA_Port->IDR  & hiic->SDA_Bit )
#define     DELAY()             delay(hiic->Speed)

static void delay(uint16_t x)
{
	uint16_t t = x * 8;
	while(t--);
}

void MyIIC_Start(MyIIC *hiic)
{
    // set SDA output
	SET_SDA(hiic);
	RESET_SCL(hiic);
	DELAY();
	SET_SCL(hiic);
	DELAY();
	RESET_SDA(hiic);
	DELAY();
	RESET_SCL(hiic);
	DELAY();
}
uint8_t MyIIC_Transmit(MyIIC *hiic,uint8_t dat)
{
	uint8_t index,flag = 0;
    // set SDA output
	for(index = 0;index < 8;index++)
    {
		if(dat & 0x80)
            SET_SDA(hiic);
        else
            RESET_SDA(hiic);
        dat <<= 1;
		DELAY();
		SET_SCL(hiic);
		DELAY();DELAY();
		RESET_SCL(hiic);
		DELAY();
    }
    // set SDA input
	SET_SDA(hiic);
    
	DELAY();
	SET_SCL(hiic);
	DELAY();DELAY();
	if( !GET_SDA(hiic) )
		flag = 1;
	RESET_SCL(hiic);
	DELAY();
	return flag;
}
uint8_t MyIIC_Receive(MyIIC *hiic,uint8_t ack)
{
	uint8_t index,dat = 0x00;
    // set SDA input
	SET_SDA(hiic);
    
	for(index = 0;index < 8;index++)
    {
		DELAY();
		SET_SCL(hiic);
		DELAY();DELAY();
        dat <<= 1;
		if( GET_SDA(hiic) )
			dat |= 0x01;
		RESET_SCL(hiic);
		DELAY();
    }
    // set SDA output
    if(ack)
        RESET_SDA(hiic);
    else
        SET_SDA(hiic);
	DELAY();
	SET_SCL(hiic);
	DELAY();DELAY();
	RESET_SCL(hiic);
	DELAY();
    SET_SDA(hiic);
	return dat;
}
void MyIIC_End(MyIIC *hiic)
{
    // set SDA output
	RESET_SDA(hiic);
	DELAY();
	SET_SCL(hiic);
	DELAY();
	SET_SDA(hiic);
	DELAY();
	RESET_SCL(hiic);
	DELAY();
}




uint8_t MyIIC_WriteReg(MyIIC *hiic,uint8_t RegAddr,uint8_t dat)
{
	uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,RegAddr);
	flag &= MyIIC_Transmit(hiic,dat);
	MyIIC_End(hiic);
	return flag;
}

uint8_t MyIIC_ReadReg(MyIIC *hiic,uint8_t RegAddr)
{
	uint8_t  flag = 1,dat;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,RegAddr);
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	dat = MyIIC_Receive(hiic,0);
	MyIIC_End(hiic);
	return dat;
}

uint8_t	MyIIC_WriteReg2 (MyIIC *hiic,uint8_t RegAddr,uint16_t dat)
{
    uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,RegAddr);
	flag &= MyIIC_Transmit(hiic,(uint8_t)(dat>>8));
    flag &= MyIIC_Transmit(hiic,(uint8_t)(dat));
	MyIIC_End(hiic);
	return flag;
}

uint16_t MyIIC_ReadReg2 (MyIIC *hiic,uint8_t RegAddr)
{
    uint8_t flag = 1;
    uint16_t dat;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,RegAddr);
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	dat = MyIIC_Receive(hiic,1);
    dat <<= 8;
    dat |= MyIIC_Receive(hiic,0);
	MyIIC_End(hiic);
	return dat;
}


uint8_t MyIIC_WriteMem(MyIIC *hiic,uint8_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,MemAddr);
	while(len--)
		flag &= MyIIC_Transmit(hiic,*dat++);
	MyIIC_End(hiic);
	return flag;
}

uint8_t MyIIC_ReadMem(MyIIC *hiic,uint8_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,MemAddr);
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	while(--len>0)
		*dat++ = MyIIC_Receive(hiic,1);
	*dat = MyIIC_Receive(hiic,0);
	MyIIC_End(hiic);
	return flag;
}

uint8_t MyIIC_WriteMem2(MyIIC *hiic,uint16_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,(uint8_t)(MemAddr>>8));
	flag &= MyIIC_Transmit(hiic,(uint8_t) MemAddr);
	while(len--)
		flag &= MyIIC_Transmit(hiic,*dat++);
	MyIIC_End(hiic);
	return flag;
}

uint8_t MyIIC_ReadMem2(MyIIC *hiic,uint16_t MemAddr,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,(uint8_t)(MemAddr>>8));
	flag &= MyIIC_Transmit(hiic,(uint8_t) MemAddr);
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	while(--len>0)
		*dat++ = MyIIC_Receive(hiic,1);
	*dat = MyIIC_Receive(hiic,0);
	MyIIC_End(hiic);
	return flag;
}


uint8_t	MyIIC_WriteByte(MyIIC *hiic,uint8_t dat)
{
    uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	flag &= MyIIC_Transmit(hiic,dat);
	MyIIC_End(hiic);
	return flag;
}

uint8_t	MyIIC_ReadByte(MyIIC *hiic)
{
    uint8_t dat;
	MyIIC_Start(hiic);
	MyIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	dat = MyIIC_Receive(hiic,0);
	MyIIC_End(hiic);
	return dat;
}

uint8_t MyIIC_WriteBytes(MyIIC *hiic,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)&0xFE);
	while(len--)
		flag &= MyIIC_Transmit(hiic,*dat++);
	MyIIC_End(hiic);
	return flag;
}

uint8_t MyIIC_ReadBytes(MyIIC *hiic,uint8_t *dat,uint16_t len)
{
	uint8_t flag = 1;
	MyIIC_Start(hiic);
	flag &= MyIIC_Transmit(hiic,(hiic->DevAddr)|0x01);
	while(--len > 0)
		*dat++ = MyIIC_Receive(hiic,1);
	*dat = MyIIC_Receive(hiic,0);
	MyIIC_End(hiic);
	return flag;
}


uint8_t MyIIC_Detect(MyIIC *hiic)
{
	uint8_t DevAddr,isFind;
    isFind = 0;
    DevAddr = 0x00;
	for(uint8_t i = 0;i < 128; i++,DevAddr += 0x02)
	{	
		MyIIC_Start(hiic);
		isFind = MyIIC_Transmit(hiic,DevAddr);
		MyIIC_End(hiic);
        if(isFind)
        {
            hiic->DevAddr = DevAddr;
            return isFind;
        }
	}
	return 0;
}
