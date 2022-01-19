/*******************************************************************************
 * @file     iot.h
 * @brief    
 * @version  V1.0
 * @date     2021.5.15
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE CODE:
	
*******************************************************************************/
#ifndef __IOT_H
#define __IOT_H
#ifdef __cplusplus
extern "c" {
#endif

#include  "stm32f4xx_hal.h"
#include  "base.h"
#include  "MQTTPacket.h"
#include  "cJSON.h"

#define IOT_printf        Myprintf

//{"id":1623224125231,"params":{"RH":17,"temp":66.5},"version":"1.0","method":"thing.event.property.post"}

/******************************* MQTT cilent *********************************/
#define  IOT_DOMAIN         "a1qxEw5H3D2.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define  IOT_PORT 	        1883
#define  IOT_CLIENTID 			"12345|securemode=3,signmethod=hmacsha1|"
#define  IOT_USERNAME 			"esp8266&a1qxEw5H3D2"
#define  IOT_PASSWORD 			"D408662CEB43233E137ADA152719D65C9A3D0D75"

#define  IOT_PTOPIC         "/sys/a1qxEw5H3D2/esp8266/thing/event/property/post"
#define  IOT_STOPIC1        "/sys/a1qxEw5H3D2/esp8266/thing/service/property/set"


#define  IOT_TIMEOUT        3000

#define  IOT_TCP_RecFlag     mylink.RecData.Count //must be volatile
#define  IOT_TCP_RecDat      mylink.RecData.Tail  //must be char*
#define  IOT_TCP_RecLen      mylink.RecData.Count

typedef struct
{
	char method[50];
	char id[20];
/***** params *****/
	int params1;
  float params2;
/******************/
  char version[10];
	volatile char Status;
}IOT_JSON;

extern IOT_JSON      Global_JSON;

int      IOT_TCP_Send        (unsigned char *data, int len);
char     IOT_TCP_ClearRec    (void);
char     IOT_Init            (void);

char     IOT_ClearIOTJSON    (IOT_JSON *iotjson);
char     IOT_ParsePayload    (char *payload,IOT_JSON *iotjson);
char     IOT_CreatePayload   (char *payload,IOT_JSON *iotjson);
char     IOT_SendJSON        (IOT_JSON *iotjson);
char     IOT_MainTask        (void);

/******************************* DON't Modify ! *******************************/

typedef struct
{
	unsigned char dup;
  int qos;
  unsigned char retained;
  unsigned short packetid;
  MQTTString topicName;
  unsigned char *payload;
  int payloadlen;
}MQTT_Packet;

extern MQTT_Packet   Global_Packet;

char     IOT_TCP_Wait        (void);
char     IOT_Connect         (void);
char     IOT_DisConnect      (void);
char     IOT_Ping            (char IsNow);
char     IOT_Pong            (void);
char     IOT_Publish         (char *pTopic,char *payload);
char     IOT_PublishAck      (unsigned short packetid);
char     IOT_Subscribe       (char *pTopic);
char     IOT_SubscribeAck    (unsigned short packetid);
int      IOT_PktTypeRead     (char *buf);
char     IOT_PubPktRead      (MQTT_Packet *pkt);
char     IOT_SubPktRead      (MQTT_Packet *pkt);



#ifdef __cplusplus
}
#endif
#endif
