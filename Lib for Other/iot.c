/*******************************************************************************
 * @file     iot.c
 * @brief
 * @version  V1.0
 * @date     2021.5.15
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "iot.h"
#include "esp.h"
#include "mygui.h"

MQTT_Packet   Global_Packet;
IOT_JSON      Global_JSON;
char          Global_ConnectState;
uint32_t      Global_TickLastPing;

#define       Global_BufLen 1024
unsigned char Global_Buf[Global_BufLen];


ESP_LINK mylink;


int IOT_TCP_Send(unsigned char *data, int len)
{
  ESP_Send(&esp8266,&mylink,(char *)data,len);
  return len;
}

char IOT_TCP_ClearRec(void)
{
  ESP_ClearData(&esp8266.Link0->RecData);
  return 1;
}

char IOT_Init(void)
{
  esp8266.Mode = STA;
	strcpy(esp8266.SSID, "Taiiwo");
	strcpy(esp8266.PSWD, "zxcv1234");
	esp8266.AUTOCONN = Enable;
	esp8266.CIPMux = Disable;
	esp8266.CIPMode = Enable;
	esp8266.Transparent = Disable;
  

  mylink.LinkID = Single_ID;
  mylink.NetPro = TCP;
  strcpy(mylink.IP,IOT_DOMAIN);
  mylink.Port = IOT_PORT;
  mylink.Keep = 60;
  
  ESP_Set(&esp8266);
  
  ESP_Link(&esp8266,&mylink);
  
  if( IOT_Connect() )
    ;//MYGUI_DrawCircle(115, 8, 4, 1, 1);
  else
    ;//MYGUI_DrawCircle(115, 8, 4, 0, 1);
  //MYGUI_Refresh();
  
  return 1;
}

char IOT_ClearIOTJSON(IOT_JSON *iotjson)
{
  iotjson->method[0] = '\0';
  iotjson->id[0] = '\0';
  iotjson->params1 = 0;
  iotjson->version[0] = '\0';
  iotjson->Status = 0;
  return 1;
}

char IOT_ParsePayload(char *payload,IOT_JSON *iotjson)
{
  char *index;
  cJSON  *root = NULL, *params = NULL;
  
  IOT_ClearIOTJSON(iotjson);
  index = strstr(payload,"{");
  if(index)
  {
    root = cJSON_Parse(index);
  }
  if( root )
  {
    strcpy(iotjson->method, cJSON_GetObjectItem(root, "method")->valuestring);
    strcpy(iotjson->id, cJSON_GetObjectItem(root, "id")->valuestring);
    params = cJSON_GetObjectItem(root, "params");
    if( params )
    {
      if( (params = cJSON_GetObjectItem(params, "LED")) != NULL)
      {
        iotjson->params1 = params->valueint;
        iotjson->Status = 1;
      }
    }
    strcpy(iotjson->version, cJSON_GetObjectItem(root, "version")->valuestring);
  }
  cJSON_Delete(root);
  
  if(iotjson->Status)
    return 1;
  else
    return 0;
}

/*  there is some error, do not use   */
char IOT_CreatePayload(char *payload,IOT_JSON *iotjson)
{
//  char *tmp;
//  cJSON  *root,*params;
//  root = cJSON_CreateObject();
//  params = cJSON_CreateObject();
//  
////  cJSON_DeleteItemFromObject(params, "LED");
////  cJSON_DeleteItemFromObject(params, "LightLux");
////  cJSON_DeleteItemFromObject(params, "IR");
//  
//  cJSON_AddNumberToObject(params, "LED", (double)(iotjson->params1) );
//  cJSON_AddNumberToObject(params, "LightLux", (double)(iotjson->params2) );
//  cJSON_AddNumberToObject(params, "IR", (double)(iotjson->params3) );
//  
////  cJSON_
//  
//  cJSON_AddStringToObject(root, "method", iotjson->method);
//  cJSON_AddStringToObject(root, "id", iotjson->id);
//  cJSON_AddItemToObject(root, "params", params);
//  cJSON_AddStringToObject(root, "version", iotjson->version);
//  
//  tmp = cJSON_PrintUnformatted(root);
//  strcpy(payload,tmp);
//  cJSON_Delete(root);


#define IOT_JSON_PART1 "{\"id\":1623224125231,\"params\":"
#define IOT_JSON_PART2 "{\"RH\":%d,\"temp\":%.1f}"
#define IOT_JSON_PART3 ",\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}"

  char *tmp = payload;
  strcpy(tmp,IOT_JSON_PART1);
  sprintf(tmp + strlen(IOT_JSON_PART1),IOT_JSON_PART2,iotjson->params1,iotjson->params2);
  strcat(tmp,IOT_JSON_PART3);
  return 1;
}

char IOT_SendJSON(IOT_JSON *iotjson)
{
  char payload[256];
  IOT_CreatePayload(payload,iotjson);
  IOT_Publish(IOT_PTOPIC,payload);
  return 1;
}

char IOT_MainTask(void)
{
  IOT_Ping(0);
  
  if(IOT_TCP_RecFlag)
  {
    switch(IOT_PktTypeRead(IOT_TCP_RecDat))
    {
      case PUBLISH:
      {
        if(IOT_PubPktRead(&Global_Packet))
        {
          IOT_PublishAck(Global_Packet.packetid);

//          if(IOT_ParsePayload((char *)Global_Packet.payload,&Global_JSON))
//          {
//            IOT_printf("Log:method  %s\r\n",Global_JSON.method);
//            IOT_printf("Log:id      %s\r\n",Global_JSON.id);
//            IOT_printf("Log:LED     %d\r\n",Global_JSON.params1);
//            IOT_printf("Log:version %s\r\n",Global_JSON.version);
//          }
        }
        break;
      }
      case SUBSCRIBE:
      {
        if(IOT_SubPktRead(&Global_Packet))
        {
          IOT_SubscribeAck(Global_Packet.packetid);
        }
        break;
      }
      case PINGREQ:
      {
        IOT_Pong();
        break;
      }
      case PINGRESP:
      {
        IOT_printf("Log:Pong once\r\n");
        Global_TickLastPing = HAL_GetTick();
        break;
      }
      case SUBACK:
      {
        IOT_printf("Log:Subscribe successfully\r\n");
        break;
      }
      
      default: break;
    }
    IOT_TCP_ClearRec();
    IOT_TCP_RecFlag = 0;
  }
  return 1;
}

char IOT_TCP_Wait(void)
{
  uint32_t tickstart;
  tickstart = HAL_GetTick();
  while ((HAL_GetTick() - tickstart) < IOT_TIMEOUT)
  {
    if(IOT_TCP_RecFlag)
    {
      IOT_TCP_RecFlag = 0;
      return 1;;
    }
  }
  IOT_TCP_RecFlag = 0;
  return 0;
}

char IOT_Connect(void)
{
  int len;

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

  data.MQTTVersion = 3;
	data.clientID.cstring = IOT_CLIENTID;
	data.keepAliveInterval = 60;
	data.cleansession = 1;
	data.username.cstring = IOT_USERNAME;
	data.password.cstring = IOT_PASSWORD;

  len = MQTTSerialize_connect(Global_Buf, Global_BufLen, &data);
  
  IOT_TCP_ClearRec();
  IOT_TCP_Send(Global_Buf,len);
  
  if(IOT_TCP_Wait())
  {
    
    if( IOT_PktTypeRead(IOT_TCP_RecDat)== CONNACK)
    {
      IOT_printf("Log:Connected MQTT!\r\n");
      Global_ConnectState = 1;
      Global_TickLastPing = HAL_GetTick();
      IOT_TCP_ClearRec();
      return 1;
    }
    else
    {
      IOT_printf("Log:Connected MQTT fail\r\n");
      Global_ConnectState = 0;
      IOT_TCP_ClearRec();
      return 0;
    }
  }
  else
  {
    IOT_printf("Log:Connected MQTT timeout\r\n");
    Global_ConnectState = 0;
    return 0;
  }
}

char IOT_DisConnect(void)
{
  int len;
  len = MQTTSerialize_disconnect(Global_Buf, Global_BufLen);
  IOT_TCP_Send(Global_Buf,len);
  Global_ConnectState = 0;
  return 1;
}

char IOT_Ping(char IsNow)
{
  int len;  
  if(IsNow || HAL_GetTick() > (Global_TickLastPing+50000) || (HAL_GetTick() < Global_TickLastPing) )
  {
    len = MQTTSerialize_pingreq(Global_Buf, Global_BufLen);
    IOT_TCP_Send(Global_Buf, len);
    Global_TickLastPing = HAL_GetTick();
    IOT_printf("Log:Ping\r\n");
  }
  return 0;
}

char IOT_Pong(void)
{
  int len;
  len = MQTTSerialize_pingresp(Global_Buf, Global_BufLen);
  IOT_TCP_Send(Global_Buf, len);
  return 1;
}

char IOT_Publish(char *pTopic,char *payload)
{
  int len;
  if(Global_ConnectState)
  {
    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = pTopic;
    len = MQTTSerialize_publish(Global_Buf, Global_BufLen, 0, 0, 0, 0, topicString, (unsigned char*)payload, strlen(payload));
    IOT_TCP_Send(Global_Buf, len);
    
    Global_TickLastPing = HAL_GetTick();// May be usefull  
 
    IOT_printf("Log:Published once\r\n");
    return 1;
  }
  return 0;
}

char IOT_PublishAck(unsigned short packetid)
{
  int len;
  len = MQTTSerialize_puback(Global_Buf, Global_BufLen,packetid);
  IOT_TCP_Send(Global_Buf, len);
  IOT_printf("Log:Publish ack once\r\n");
  return 1;
}

char IOT_Subscribe(char *pTopic)
{
  int len;
  if(Global_ConnectState)
  {
    MQTTString topicString = MQTTString_initializer;
    topicString.cstring = pTopic;
    len = MQTTSerialize_subscribe(Global_Buf, Global_BufLen, 0, 1, 1, &topicString, 0);

    IOT_TCP_Send(Global_Buf, len);
    
  }
  else
  {
    IOT_printf("Log:Subscribe: Not Connect\r\n");
    return 0;
  }
  return 1;
}

char IOT_SubscribeAck(unsigned short packetid)
{
  int len;
  int grantedQoSs[2] = {0,0};
  len = MQTTSerialize_suback(Global_Buf, Global_BufLen,packetid,2,grantedQoSs);
  IOT_TCP_Send(Global_Buf, len);
  IOT_printf("Log:Subscribe ack once\r\n");
  return 1;
  
}

int IOT_PktTypeRead(char *buf)
{
  MQTTHeader header;
  header.byte = buf[0];
  IOT_printf("Log:Rec a Packet.Type is %d.\r\n",header.bits.type);
  return header.bits.type;
}

char IOT_PubPktRead(MQTT_Packet *pkt)
{
  return MQTTDeserialize_publish(&(pkt->dup),&(pkt->qos),&(pkt->retained),
    &(pkt->packetid),&(pkt->topicName),&(pkt->payload),&(pkt->payloadlen),
    (unsigned char *)IOT_TCP_RecDat,IOT_TCP_RecLen);
}
char IOT_SubPktRead(MQTT_Packet *pkt)
{
  int maxcount = 10;
  int count;
  MQTTString topicFilters[10];
  int requestedQoSs[2];
  return MQTTDeserialize_subscribe(&(pkt->dup),&(pkt->packetid),maxcount,&count,
    topicFilters,requestedQoSs,(unsigned char *)IOT_TCP_RecDat,IOT_TCP_RecLen);
}
