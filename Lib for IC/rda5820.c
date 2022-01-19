/*******************************************************************************
 * @file     rda5820.c
 * @brief
 * @version  V1.0
 * @date     2021.7.18
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "rda5820.h"
#include "base.h"

MYIIC hrda5820 = {  .GPIO_SCL = GPIOB,.Pin_SCL = GPIO_PIN_1,
                    .GPIO_SDA = GPIOB,.Pin_SDA = GPIO_PIN_0,
                    .Speed = 2,.DevAddr = 0x22};

#define IIC_Init()                   MYIIC_Init(&hrda5820)
#define RDA5820_WriteReg(addr,dat)   MYIIC_WriteReg2(&hrda5820,addr,dat)
#define RDA5820_ReadReg(addr)        MYIIC_ReadReg2(&hrda5820,addr)
#define delay_ms(ms)                 HAL_Delay(ms)                    
                    

//初始化
//0,初始化成功;
//其他,初始化失败.
uint8_t RDA5820_Init(void)
{
    uint16_t id;
    IIC_Init();
     //初始化IIC_口 
    id = RDA5820_ReadReg(RDA5820_R00); //读取ID =0X5805
    if(id==0X5805) //读取ID正确
    { 
        RDA5820_WriteReg(RDA5820_R02,0x0002); //芯片软复位
        delay_ms(400);//等待复位结束
        RDA5820_WriteReg(RDA5820_R02,0x0001); //芯片上电
        delay_ms(600);
        RDA5820_WriteReg(RDA5820_R02,0xE201); //芯片上电,不复位 正常天线 32.768时钟 循环搜索 不开始搜索 想上搜索 低音增强 立体声 非静音 非高阻抗
        RDA5820_WriteReg(RDA5820_R03,0x0000); //100k apace 87-108baud 不开启调谐
        RDA5820_WriteReg(RDA5820_R04,0x0000); //关闭中断 0.75us去加重 不使能iis io口全部浮空
        RDA5820_WriteReg(RDA5820_R05,0X8578); //搜索强度8,LNAN,1.8mA,VOL最大 无输入低噪声
        // RDA5820_WriteReg(RDA5820_R0A,0X7800); //0A不用设置
        // RDA5820_WriteReg(RDA5820_R0B,0X7800); //0b不用设置
        RDA5820_WriteReg(RDA5820_R40,0X0000); //半自动搜台 RX工作模式
        RDA5820_WriteReg(RDA5820_R41,0X0000); //RDS应答为0 不复位fifo fifo深度为0
        // RDA5820_WriteReg(RDA5820_R42,0X0000); //42不用设置
        RDA5820_WriteReg(RDA5820_R4A,0X0010); //fifo满中断
        RDA5820_FreqSet(8700);//设置初始化频率81.00M
    }
    else
        return 1;//初始化失败
    return 0;
}


//设置RDA5820为RX模式
void RDA5820_RxMode(void)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X40); //读取0X40的内容
    temp&=0xfff0; //RX 模式   
    RDA5820_WriteReg(0X40,temp) ; //FM RX模式 
}

//设置RDA5820为TX模式
void RDA5820_TxMode(void)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X40); //读取0X40的内容
    temp&=0xfff0;
    temp|=0x0001;    //TX 模式
    RDA5820_WriteReg(0X40,temp) ; //FM TM 模式 
}

//得到信号强度
//返回值范围:0~127
uint8_t RDA5820_RssiGet(void)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X0B); //读取0X0B的内容
    return temp>>9;                 //返回信号强度
}

//设置音量ok
//vol:0~15;
void RDA5820_VolSet(uint8_t vol)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X05); //读取0X05的内容
    temp&=0XFFF0;
    temp|=vol&0X0F;    
    RDA5820_WriteReg(0X05,temp) ; //设置音量 
}

//静音设置
//mute:0,不静音;1,静音
void RDA5820_MuteSet(uint8_t mute)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X02); //读取0X02的内容
    if(mute)temp|=1<<14;
    else temp&=~(1<<14);        
    RDA5820_WriteReg(0X02,temp) ; //设置MUTE 
}

//设置灵敏度
//rssi:0~127;
void RDA5820_RssiSet(uint8_t rssi)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X05); //读取0X05的内容
    temp&=0X80FF;
    temp|=(uint16_t)rssi<<8;    
    RDA5820_WriteReg(0X05,temp) ; //设置RSSI 
}

//设置TX发送功率
//gain:0~63
void RDA5820_TxPagSet(uint8_t gain)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X42); //读取0X42的内容
    temp&=0XFFC0;
    temp|=gain;   //GAIN
    RDA5820_WriteReg(0X42,temp) ; //设置PA的功率
}

//设置TX 输入信号增益
//gain:0~7
void RDA5820_TxPgaSet(uint8_t gain)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X42); //读取0X42的内容
    temp&=0XF8FF;
    temp|=gain<<8;     //GAIN
    RDA5820_WriteReg(0X42,temp) ; //设置PGA
}

//设置RDA5820的工作频段
//band:0,87~108Mhz;1,76~91Mhz;2,76~108Mhz;3,用户自定义(53H~54H)
void RDA5820_BandSet(uint8_t band)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X03); //读取0X03的内容
    temp&=0XFFF3;
    temp|=band<<2;     
    RDA5820_WriteReg(0X03,temp) ; //设置BAND
}

//设置RDA5820的步进频率
//band:0,100Khz;1,200Khz;3,50Khz;3,保留
void RDA5820_SpaceSet(uint8_t spc)
{
    uint16_t temp;
    temp=RDA5820_ReadReg(0X03); //读取0X03的内容
    temp&=0XFFFC;
    temp|=spc;     
    RDA5820_WriteReg(0X03,temp) ; //设置BAND
}

//设置RDA5820的频率
//freq:频率值(单位为10Khz),比如10805,表示108.05Mhz
void RDA5820_FreqSet(uint16_t freq)
{
    uint16_t temp;
    uint8_t spc=0,band=0;
    uint16_t fbtm,chan;
    temp=RDA5820_ReadReg(0X03); //读取0X03的内容
    temp&=0X001F;
    band=(temp>>2)&0x03; //得到频带
    spc=temp&0x03; //得到分辨率
    if(spc==0)spc=10;
    else if(spc==1)spc=20;
    else spc=5;
    if(band==0)fbtm=8700;
    else if(band==1||band==2)fbtm=7600;
    else 
    {
        fbtm=RDA5820_ReadReg(0X53);//得到bottom频率
        fbtm*=10;
    }
    if(freq<fbtm)return;
    chan=(freq-fbtm)/spc; //得到CHAN应该写入的值
    chan&=0X3FF; //取低10位  
    temp|=chan<<6;
    temp|=1<<4; //TONE ENABLE     
    RDA5820_WriteReg(0X03,temp) ; //设置频率
    delay_ms(20); //等待20ms
    while((RDA5820_ReadReg(0X0B)&(1<<7))==0);//等待FM_READY
    
}
//得到当前频率
//返回值:频率值(单位10Khz)
uint16_t RDA5820_FreqGet(void)
{
    uint16_t temp;
    uint8_t spc=0,band=0;
    uint16_t fbtm,chan;
    temp=RDA5820_ReadReg(0X03); //读取0X03的内容
    chan=temp>>6;   
    band=(temp>>2)&0x03; //得到频带
    spc=temp&0x03; //得到分辨率
    if(spc==0)spc=10;
    else if(spc==1)spc=20;
    else spc=5;
    if(band==0)fbtm=8700;
    else if(band==1||band==2)fbtm=7600;
    else 
    {
        fbtm=RDA5820_ReadReg(0X53);//得到bottom频率
        fbtm*=10;
    }
    temp=fbtm+chan*spc;  
    return temp;//返回频率值
}

uint16_t RDA5820_Seek(void)
{
	uint16_t temp;
	uint16_t count = 0;
    temp=RDA5820_ReadReg(RDA5820_R02); //读取0X03的内容
    temp|=0X0100;     
    RDA5820_WriteReg(RDA5820_R02,temp) ; //开始搜台
	do
	{
		delay_ms(20);
		temp = RDA5820_ReadReg(RDA5820_R02);//读取搜台结果
		temp&= 0x0100;
		count++;
		if(count > 5000)
			return 0xffff;

	}while(temp == 0x0100);
	
	return count;
}
