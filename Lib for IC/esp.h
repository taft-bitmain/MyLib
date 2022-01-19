/*******************************************************************************
 * @file     esp.h
 * @brief    
 * @version  V1.1
 * @date     2021.6.2
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	1. 代码未完善，目前仅支持透传模式，使用者耗子尾汁
EXAMPLE CODE:

ESP_LINK mylink;

  esp8266.Mode = STA;
	strcpy(esp8266.SSID, "xxxxxxxx");
	strcpy(esp8266.PSWD, "xxxxxxxx");
	esp8266.AUTOCONN = Enable;
	esp8266.CIPMux = Disable;
	esp8266.CIPMode = Enable;
	esp8266.Transparent = Disable;
  
  ESP_Set(&esp8266);
 
  mylink.LinkID = Single_ID;
  mylink.NetPro = TCP;
  strcpy(mylink.IP,"192.168.230.61");
  mylink.Port = 8889;
  
  ESP_Link(&esp8266,&mylink);
  
  ESP_Send(&esp8266,&mylink,"hello from esp8266",18);
  
  while()
  {
    if(mylink.RecData.Count)
    {
      CDC_Transmit_FS((uint8_t*)mylink.RecData.Tail, mylink.RecData.Count);
      ESP_ClearData( &(mylink.RecData) );
    }
  }
	
*******************************************************************************/
#ifndef __ESP_H
#define __ESP_H

#include "string.h"

#define ESP_USE_RSTPIN	    0

#define ESP_RXLEN           1024


typedef enum{
  Disable = 0,
  Enable = 1
} ESP_BOOL;

typedef enum{
  STA = 1,
  AP = 2,
  AP_STA  = 3
} ESP_Mode;

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4
} ESP_APMode;

typedef enum{
	 TCP,
	 UDP,
	 SSL
} ESP_NetPro;

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID = 5
} ESP_LINKID;

typedef struct
{
  volatile unsigned int Count;
  char *Head;
  char *Tail;
  char  Data[ESP_RXLEN];
}ESP_DATA;

typedef struct{
	ESP_LINKID LinkID;
	ESP_NetPro NetPro;
	ESP_DATA   RecData;
	char       IP[80];
	unsigned int Port;
	unsigned int LocalPort;
	unsigned int Keep;
}ESP_LINK;

typedef struct{
	ESP_Mode   Mode;
	ESP_APMode APMode;
	ESP_DATA   RecCmd;
	char       SSID[32];
	char       PSWD[32];
	ESP_BOOL   AUTOCONN;
	char       SSID_AP[32];
	char       PSWD_AP[32];
	ESP_BOOL   CIPMode;
	ESP_BOOL   CIPMux;
	ESP_LINK   *Link0;
	ESP_LINK   *Link1;
	ESP_LINK   *Link2;
	ESP_LINK   *Link3;
	ESP_LINK   *Link4;
	ESP_BOOL   Transparent;
}ESP;

extern ESP esp8266;

void    ESP_LowLevelSend          ( char *data, int len);
void    ESP_LowLevelRec           ( ESP *esp , const char *data_in, int len);
void    ESP_PrintLog              ( const char *data );
void    ESP_ClearData             (ESP_DATA *dat);
  
char    ESP_Cmd                   ( ESP *esp , char *cmd, char *reply, unsigned int waittime);

char    ESP_Reset                 ( ESP *esp );
char    ESP_AtTest                ( ESP *esp );
char    ESP_SetMode               ( ESP *esp );
char    ESP_JoinAP                ( ESP *esp );
char    ESP_SetAP                 ( ESP *esp );
char    ESP_AutoCon               ( ESP *esp );
char    ESP_CIPMode               ( ESP *esp );
char    ESP_CIPMux                ( ESP *esp );
char    ESP_MultiTransport        ( ESP *esp );
char    ESP_Transparent           ( ESP *esp );
void    ESP_TransparentEnd        ( ESP *esp );

char    ESP_Set                   ( ESP *esp );
char    ESP_Link                  ( ESP *esp , ESP_LINK *link );
char    ESP_Send                  ( ESP *esp , ESP_LINK *link , char *dat , int len);


#endif
