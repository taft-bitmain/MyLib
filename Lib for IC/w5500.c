/*******************************************************************************
 * @file     w5500.c
 * @brief
 * @version  V1.0
 * @date     2021.7.9
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "w5500.h"
#include "base.h"

/*********** BufferSize Set ***************/
// Must be 0, 1, 2, 4, 8 or 16
// Sum must be <= 16(KB)
uint8_t TxBufferSize[8] = {8,2,1,1,1,1,1,1};
uint8_t RxBufferSize[8] = {8,2,1,1,1,1,1,1};

/***************************** SPI接口 **************************************/
#include "spi.h"

void W5500_Delay(uint16_t ms)
{
    HAL_Delay(ms);
}

void W5500_SPI_Init(void)
{
    W5500_CS(1);
    W5500_RST(1);
}
void W5500_SPI_Wirte(uint32_t header,uint8_t* data_in,uint16_t size)
{
    uint8_t tmp[3];
    W5500_CS(0);
    tmp[0] = (header >> 16) & 0xFF;
    tmp[1] = (header >> 8) & 0xFF;
    tmp[2] = (header & 0xFF) | 0x04;
    HAL_SPI_Transmit(&hspi1,tmp,3,1000);
    HAL_SPI_Transmit(&hspi1,data_in,size,1000);
    W5500_CS(1);
}
void W5500_SPI_WirteByte(uint32_t header,uint8_t data_in)
{
    uint8_t tmp[3];
    W5500_CS(0);
    tmp[0] = (header >> 16) & 0xFF;
    tmp[1] = (header >> 8) & 0xFF;
    tmp[2] = (header & 0xFF) | 0x04;
    HAL_SPI_Transmit(&hspi1,tmp,3,1000);
    HAL_SPI_Transmit(&hspi1,&data_in,1,1000);
    W5500_CS(1);
}

void W5500_SPI_Read(uint32_t header,uint8_t* data_out,uint16_t size)
{
    uint8_t tmp[3];
    W5500_CS(0);
    tmp[0] = (header >> 16) & 0xFF;
    tmp[1] = (header >> 8) & 0xFF;
    tmp[2] = (header & 0xFB);
    HAL_SPI_Transmit(&hspi1,tmp,3,1000);
    HAL_SPI_Receive(&hspi1,data_out,size,1000);
    W5500_CS(1);
}

uint8_t W5500_SPI_ReadByte(uint32_t header)
{
    uint8_t tmp[3];
    W5500_CS(0);
    tmp[0] = (header >> 16) & 0xFF;
    tmp[1] = (header >> 8) & 0xFF;
    tmp[2] = (header & 0xFB);
    HAL_SPI_Transmit(&hspi1,tmp,3,1000);
    HAL_SPI_Receive(&hspi1,tmp,1,1000);
    W5500_CS(1);
    return tmp[0];
}

void W5500_Reset(void)
{
    W5500_RST(0);
    W5500_Delay(1);
    W5500_RST(1);
    W5500_Delay(2);
}

/**************************** W5500配置 ************************************/

void W5500_SetBufferSize(uint8_t s,uint8_t tx_kb,uint8_t rx_kb)
{
    W5500_SPI_WirteByte(Sn_TXMEM_SIZE(s),tx_kb);
    W5500_SPI_WirteByte(Sn_RXMEM_SIZE(s),rx_kb);
}

void W5500_GetBufferSize(uint8_t s,uint8_t* tx_kb,uint8_t* rx_kb)
{
    *tx_kb = W5500_SPI_ReadByte(Sn_RXMEM_SIZE(s));
    *rx_kb = W5500_SPI_ReadByte(Sn_RXMEM_SIZE(s));
}


void W5500_Write_TxBuffer(uint8_t s, uint8_t *data, uint16_t len)		// MCU把数据发送给W5500的过程，W5500将数据写入buf，并更新数据的写指针的地址
{
    uint16_t ptr =0;
    uint8_t tmp[2];
    uint32_t header =0;
    if(len == 0)
    {
        return;
    }

    /************* Read Pointer **************/
    W5500_SPI_Read( Sn_TX_WR0(s),tmp,2);
    ptr = (tmp[0] << 8) + tmp[1];

    /************* Wrtite Data ***************/
    header = Sn_TxBuffer(s) | (ptr << 8);
    W5500_SPI_Wirte(header, data, len);
    
    /************* Update Pointer ************/
    ptr += len;
    tmp[0] = (ptr & 0xFF00) >> 8;
    tmp[1] = (ptr & 0x00FF);
    W5500_SPI_Wirte(Sn_TX_WR0(s),tmp,2);
}
void W5500_Read_RxBuffer(uint8_t s, uint8_t *data, uint16_t len)
{
    uint16_t ptr =0;
    uint8_t tmp[2];
    uint32_t header =0;

    if(len == 0)
    {
        return;
    }

    /************* Read Pointer **************/
    W5500_SPI_Read( Sn_RX_RD0(s),tmp,2);
    ptr = (tmp[0] << 8) + tmp[1];
    
    /************* Read Data ***************/
    header = Sn_RxBuffer(s) | (ptr << 8);
    W5500_SPI_Read(header, data, len);
    
    /************* Update Pointer ************/
    ptr += len;
    tmp[0] = (ptr & 0xFF00) >> 8;
    tmp[1] = (ptr & 0x00FF);
    W5500_SPI_Wirte(Sn_RX_RD0(s),tmp,2);
}

void W5500_SetNetwork(void)
{
    /*********** IP and MAC Config ***********/
    uint8_t tmp[] = {
        192,168,1,1,                        //gateway
        255,255,255,0,                      //subnet mask
        0x00,0x08,0xdc,0x11,0x11,0x11,      //MAC
        192,168,1,150                       //ip
    };
    W5500_SPI_Wirte(GAR0,tmp,18);
    
    /*********** Retry timer(100us) ***********/
    W5500_SPI_WirteByte(RTR0,2000/256);
    W5500_SPI_WirteByte(RTR1,2000%256);
    
    /*********** Retry Count ******************/
    W5500_SPI_WirteByte(RCR,3);
    
    /*********** PHY Config *******************/
//    W5500_SPI_WirteByte(PHYCFGR,0xF8);
//    W5500_SPI_WirteByte(PHYCFGR,0x78);
}

void W5500_DumpComReg(void)
{
    uint8_t tmp[47], i;
    W5500_SPI_Read(MR,tmp,47);
    
    i = 0;
    Myprintf("Dump Common Register:\r\n");
    
    Myprintf("Mode:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Gateway:");
    Myprintf("%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d\r\n",tmp[i++]);
    
    Myprintf("Subnet Mask:");
    Myprintf("%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d\r\n",tmp[i++]);
    
    Myprintf("Source MAC:");
    Myprintf("%02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X\r\n",tmp[i++]);
    
    Myprintf("Source IP:");
    Myprintf("%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d\r\n",tmp[i++]);
    
    Myprintf("Interrupt Low Level Timer:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("Interrupt:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Interrupt Mask:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Socket Interrupt:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Socket Interrupt Mask:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Retry Time:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("Retry Count:");
    Myprintf("%d\r\n",tmp[i++]);
    
    Myprintf("PPP LCP Request Timer:");
    Myprintf("%d\r\n",tmp[i++]);
    
    Myprintf("PPP LCP Magic number:");
    Myprintf("%d\r\n",tmp[i++]);
    
    Myprintf("PPP Destination MAC:");
    Myprintf("%02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X\r\n",tmp[i++]);
    
    Myprintf("PPP Session ID:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("PPP MAX Segment Size:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("Unreachable IP:");
    Myprintf("%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d\r\n",tmp[i++]);
    
    Myprintf("Unreachable Port:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
     
    Myprintf("PHY Config:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("\r\n");
    
}
void W5500_DumpSocket(uint8_t s)
{
    uint8_t tmp[48],i;
    W5500_SPI_Read(Sn_MR(s),tmp,48);
    
    i = 0;
    Myprintf("Socket Register %d is:\r\n",s);
    
    Myprintf("Mode:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Command:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Interrupt:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Status:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Source Port:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("Destination MAC:");
    Myprintf("%02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X",tmp[i++]);
    Myprintf(" %02X\r\n",tmp[i++]);
    
    Myprintf("Destination IP:");
    Myprintf("%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d",tmp[i++]);
    Myprintf(",%d\r\n",tmp[i++]);
    
    Myprintf("Destination Port:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("MAX Segment Size:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    i++;
    
    Myprintf("IP TOS:");
    Myprintf("%d\r\n",tmp[i++]);
    
    Myprintf("IP TTL:");
    Myprintf("%d\r\n",tmp[i++]);
    
    i += 7;
    
    Myprintf("Rx Buffer Size:");
    Myprintf("%d\r\n",tmp[i++]);
    
    Myprintf("Tx Buffer Size:");
    Myprintf("%d\r\n",tmp[i++]);
    
    Myprintf("Tx Free Size:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("Tx Read Pointer:");
    Myprintf("%02X",tmp[i++]);
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Tx Write Pointer:");
    Myprintf("%02X",tmp[i++]);
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Rx Received Size:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("Rx Read Pointer:");
    Myprintf("%02X",tmp[i++]);
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Rx Write Pointer:");
    Myprintf("%02X",tmp[i++]);
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Interrupt Mask:");
    Myprintf("%02X\r\n",tmp[i++]);
    
    Myprintf("Fragment Offset:");
    Myprintf("%d\r\n",tmp[i] * 256 + tmp[i+1]);
    i++;i++;
    
    Myprintf("Keep alive timer:");
    Myprintf("%d\r\n",tmp[i++]);
    
    Myprintf("\r\n");
}



void W5500_Init(void)
{
    uint8_t i;
    
    W5500_SPI_Init();
    W5500_Reset();
    W5500_SetNetwork();
    
    for( i = 0; i < 8; i++ )
    {
    	W5500_SetBufferSize(i,TxBufferSize[i],RxBufferSize[i]);
    }
}


uint8_t W5500_Open(uint8_t s, uint16_t port, uint8_t protocol, uint8_t flag)
{
    if (  ((protocol&0x0F) == Sn_MR_TCP)    ||
          ((protocol&0x0F) == Sn_MR_UDP)    ||
          ((protocol&0x0F) == Sn_MR_IPRAW)  ||
          ((protocol&0x0F) == Sn_MR_MACRAW) ||
          ((protocol&0x0F) == Sn_MR_PPPOE)     )
    {
        W5500_Close(s);
        W5500_SPI_WirteByte(Sn_MR(s) ,protocol | flag);
        
        /********** default port is 111*(s+1) ***************/
        if(port == 0)
            port = 111 * (s + 1);
        
        W5500_SPI_WirteByte( Sn_PORT0(s) ,(uint8_t)((port & 0xff00) >> 8));
        W5500_SPI_WirteByte( Sn_PORT1(s) ,(uint8_t)(port & 0x00ff));
        
        /********** OPEN Socket *****************************/
        W5500_SPI_WirteByte( Sn_CR(s) ,Sn_CR_OPEN);
        
        /********** Wait ************************************/
        while( W5500_SPI_ReadByte( Sn_SR(s) !=  SOCK_INIT) );
        
        return 1;
   }
   return 0;
}
void W5500_Close(uint8_t s)
{
    W5500_SPI_WirteByte( Sn_CR(s) ,Sn_CR_CLOSE); 
}
uint8_t W5500_Connect(uint8_t s, uint8_t * addr, uint16_t port)
{
    if (    ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
            ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
            (port == 0x00)   )
    {
      return 0;
    }
    
    if( W5500_SPI_ReadByte(Sn_SR(s)) != SOCK_INIT)
    {
        return 0;
    }
    
    /************* Write Socket ip and port ****************/
    W5500_SPI_Wirte( Sn_DIPR0(s), addr,4);
    
    W5500_SPI_WirteByte( Sn_DPORT0(s), (uint8_t)((port & 0xff00) >> 8));
    W5500_SPI_WirteByte( Sn_DPORT1(s), (uint8_t)(port & 0x00ff));
    
    /******************* Socket Connect ********************/
    W5500_SPI_WirteByte( Sn_CR(s) ,Sn_CR_CONNECT);
    
    Myprintf("Connect...\r\n");
    
    while ( W5500_SPI_ReadByte(Sn_SR(s)) != SOCK_ESTABLISHED )
    {
        if(W5500_SPI_ReadByte(Sn_SR(s)) == SOCK_CLOSED)
        {
            return 0;
        }
    }
    return 1;
}
void W5500_Disconnect(uint8_t s)
{
    W5500_SPI_WirteByte( Sn_CR(s) ,Sn_CR_DISCON);
    while ( W5500_SPI_ReadByte(Sn_SR(s)) != SOCK_CLOSED );
}
uint8_t W5500_Listen(uint8_t s)
{
    uint8_t status;
    status = W5500_SPI_ReadByte( Sn_SR(s) ); 
    if (status == SOCK_INIT || status == SOCK_CLOSED)
    {
        W5500_SPI_WirteByte( Sn_CR(s) ,Sn_CR_LISTEN);
        return 1;
    }
    return 0;
}
uint16_t W5500_Send(uint8_t s, const uint8_t * buf, uint16_t len)
{
    uint8_t tmp[2],status;
    uint16_t FreeSize = 0;
    
    /************** Check State ***************/
    status = W5500_SPI_ReadByte(Sn_SR(s));
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
    {
        return 0;
    }
    
    /************** Check FreeSize ***************/
    W5500_SPI_Read(Sn_TX_FSR0(0),tmp ,2);
    FreeSize = (tmp[0] << 8) + tmp[1];
    len = (len > FreeSize) ? FreeSize:len;
    
    /*************** Write Buffer *************/
    W5500_Write_TxBuffer(s, (uint8_t *)buf, len);
    
    /*************** Send ant Wait *************/
    W5500_SPI_WirteByte( Sn_CR(s) ,Sn_CR_SEND);
    
//    while( (W5500_SPI_ReadByte( Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
//    {
//        if (W5500_SPI_ReadByte( Sn_IR(s) ) & Sn_IR_TIMEOUT)
//        {
//            /* clear interrupt */
//            W5500_SPI_WirteByte( Sn_IR(s) , (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */
//            return 0;
//        }
//    }
//    W5500_SPI_WirteByte( Sn_IR(s) , Sn_IR_SEND_OK);
    
    return len;
}
uint16_t W5500_Recv(uint8_t s, uint8_t * buf, uint16_t len)
{
    uint8_t tmp[2];
    uint16_t ReceiceSize = 0;
    if ( len > 0 )
    {
        /************ Check Receice Size *********/
        W5500_SPI_Read(Sn_RX_RSR0(0),tmp ,2);
        ReceiceSize = (tmp[0] << 8) + tmp[1];
        len = (len > ReceiceSize) ? ReceiceSize:len;
        
        /*************** Read Buffer *************/
        W5500_Read_RxBuffer(s, buf, len);
        
        W5500_SPI_WirteByte( Sn_CR(s) ,Sn_CR_RECV);
    }
    return len;
}

void W5500_Send_ka(uint8_t s)
{
    W5500_SPI_WirteByte(Sn_CR(s),Sn_CR_SEND_KEEP);
}

