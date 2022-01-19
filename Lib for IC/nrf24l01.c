/*******************************************************************************
 * @file     nrf24l01.c
 * @brief
 * @version  V1.0
 * @date     2021.7.19
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "nrf24l01.h"


/****************** inteface ******************/
#define NRF24L01_SPI_Init()                 MYSPI_Init(&myspi1)
#define NRF24L01_SPI_WriteReg(reg,dat)      MYSPI_WriteReg(&myspi1,reg,dat)
#define NRF24L01_SPI_ReadReg(reg)           MYSPI_ReadReg(&myspi1,reg)
#define NRF24L01_SPI_WriteMem(reg,pBuf,len) MYSPI_WriteMem(&myspi1,reg,pBuf,len)
#define NRF24L01_SPI_ReadMem(reg,pBuf,len)  MYSPI_ReadMem(&myspi1,reg,pBuf,len)

#define NRF24L01_PIN_CE                     PGout(6)
#define NRF24L01_PIN_IRQ                    PGin(8)

MYSPI myspi1 = {    .GPIO_SCK  = GPIOB, .Pin_SCK  = GPIO_PIN_13,
					.GPIO_MOSI = GPIOB, .Pin_MOSI = GPIO_PIN_15,
					.GPIO_MISO = GPIOB, .Pin_MISO = GPIO_PIN_14,
					.GPIO_CS   = GPIOG, .Pin_CS   = GPIO_PIN_7,
					.Speed = 2, .CPOL = 0, .CPHA = 0};

/****************** some configrations ******************/
                    
#define TX_ADR_WIDTH    5   //5字节的地址宽度
#define RX_ADR_WIDTH    5   //5字节的地址宽度
#define TX_PLOAD_WIDTH  32  //20字节的用户数据宽度
#define RX_PLOAD_WIDTH  32  //20字节的用户数据宽度

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01};


/****************** function ******************/
void NRF24L01_Init(void)
{
	NRF24L01_SPI_Init();
    NRF24L01_PIN_CE = 1;
}

uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	uint8_t i;  	 
	NRF24L01_SPI_WriteMem(REG_WRITE+TX_ADDR,buf,5);
	NRF24L01_SPI_ReadMem(TX_ADDR,buf,5);
	for(i=0;i<5;i++)
    {
        if(buf[i]!=0XA5)
            return 1;
    }
    /**** NRF24L01 is on wire ***/
	return 0;
}
			    
	   
void RX_Mode(void)
{
	NRF24L01_PIN_CE=0;	  
  	NRF24L01_SPI_WriteMem(REG_WRITE +RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	  
  	NRF24L01_SPI_WriteReg(REG_WRITE + EN_AA,0x01);    //使能通道0的自动应答
  	NRF24L01_SPI_WriteReg(REG_WRITE + EN_RXADDR,0x01);//使能通道0的接收地址
  	NRF24L01_SPI_WriteReg(REG_WRITE + RF_CH,40);	     //设置RF通信频率
  	NRF24L01_SPI_WriteReg(REG_WRITE + RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度
  	NRF24L01_SPI_WriteReg(REG_WRITE + RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启
  	NRF24L01_SPI_WriteReg(REG_WRITE + CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式
  	NRF24L01_PIN_CE = 1; //CE为高,进入接收模式 
}

void TX_Mode(void)
{														 
	NRF24L01_PIN_CE = 0;
  	NRF24L01_SPI_WriteMem(REG_WRITE + TX_ADDR,(uint8_t*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址
  	NRF24L01_SPI_WriteMem(REG_WRITE + RX_ADDR_P0,(uint8_t*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK

  	NRF24L01_SPI_WriteReg(REG_WRITE + EN_AA,0x01);     //使能通道0的自动应答
  	NRF24L01_SPI_WriteReg(REG_WRITE + EN_RXADDR,0x01); //使能通道0的接收地址
  	NRF24L01_SPI_WriteReg(REG_WRITE + SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
  	NRF24L01_SPI_WriteReg(REG_WRITE + RF_CH,40);       //设置RF通道为40
  	NRF24L01_SPI_WriteReg(REG_WRITE + RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
  	NRF24L01_SPI_WriteReg(REG_WRITE + CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
	NRF24L01_PIN_CE = 1;
}

uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t sta; 
  	NRF24L01_SPI_WriteMem(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF 32个字节
 	NRF24L01_PIN_CE = 1;//启动发送	   
	while(NRF24L01_PIN_IRQ != 0);
	sta = NRF24L01_SPI_ReadReg(STATUS); 
	NRF24L01_SPI_WriteReg(REG_WRITE + STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta & MAX_TX)//达到最大重发次数
	{
		NRF24L01_SPI_WriteReg(FLUSH_TX,0xff);//清除TX FIFO寄存器
		return 0;
	}
	if(sta&TX_OK)//发送完成
	{
		return TX_OK;
	}
	return 0;//其他原因发送失败
}

uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t sta;
	sta = NRF24L01_SPI_ReadReg(STATUS);  //读取状态寄存器的值
	NRF24L01_SPI_WriteReg(REG_WRITE+STATUS,sta); //清除TX_DS或MAX_RT中断标志
	if(sta&RX_OK)//接收到数据
	{
		NRF24L01_SPI_ReadMem(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
		NRF24L01_SPI_WriteReg(FLUSH_RX,0xff);//清除RX FIFO寄存器
		return RX_OK;
	}	   
	return 0;//没收到任何数据
}

