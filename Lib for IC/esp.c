/*******************************************************************************
 * @file     esp.c
 * @brief    
 * @version  V1.1
 * @date     2021.6.2
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 *******************************************************************************/
#include "esp.h"
#include "stdio.h"
#include "base.h"
#include "usbd_cdc_if.h"

ESP esp8266;

//todo
void USART2_IDLE_Callback(uint8_t *data, uint16_t len)
{
	//CDC_Transmit_FS((uint8_t*) data, len);
  LEDTOGGLE(LED);
	ESP_LowLevelRec(&esp8266, (char*) data, len);
}

void ESP_LowLevelSend( char *data, int len)
{
	//todo
	HAL_UART_Transmit(&huart2, (uint8_t*) data, len, 1000);
	//CDC_Transmit_FS((uint8_t*) data, len);
}

void ESP_LowLevelRec( ESP *esp , const char *data_in, int len)
{
	if(esp->Transparent == Enable)
	{
		if(esp->Link0 == NULL)
		{
			ESP_PrintLog("Log:ESP8266:Link data is received, but no buff to store\r\n");
			return ;
		}
		else
		{
			ESP_DATA *tmp = &(esp->Link0->RecData);
			if (len > (ESP_RXLEN - tmp->Count))
				ESP_PrintLog(
						"Log:ESP8266:Link data is received, but buff is not enough\r\n");
			else
			{
				memcpy(tmp->Head, data_in, len);
				tmp->Count += len;
				tmp->Head += len;
				*(tmp->Head) = '\0';
			}
		}
	}
	else
	{
		//todo
		//check the link id, and copy data to every link.
		ESP_DATA *tmp = &(esp->RecCmd);
		if (len > (ESP_RXLEN - tmp->Count))
			ESP_PrintLog(
					"Log:ESP8266:cmd data is received, but buff is not enough\r\n");
		else
		{
			memcpy(tmp->Head, data_in, len);
			tmp->Count += len;
			tmp->Head += len;
			*(tmp->Head) = '\0';
		}
	}
	return ;
}

void ESP_PrintLog(const char *data)
{
	//todo
	//HAL_UART_Transmit(&huart1,(char *)data,strlen(data),1000);
	CDC_Transmit_FS((uint8_t*) data, strlen(data));
	/* Or like this.*/
	//UNUSED(data);
}


void ESP_ClearData(ESP_DATA *dat)
{
	dat->Count = 0;
	dat->Head = dat->Tail = dat->Data;
	//memset(dat->Data, 0, ESP_RXLEN);
}

char ESP_Cmd( ESP *esp , char *cmd, char *reply, unsigned int waittime)
{
	ESP_DATA *tmp = &(esp->RecCmd);
	ESP_ClearData(tmp);

	//发送命令
	ESP_LowLevelSend(cmd, strlen(cmd));

	//不期待回复
	if (reply == NULL || waittime == 0)
		return 1;

	//期待回复
	uint32_t tickstart = HAL_GetTick();
	while ((HAL_GetTick() - tickstart) < waittime)
	{
		if (tmp->Count)
		{
			//得到回复
			if (strstr(tmp->Tail, reply) != NULL)
			{
				return 1;
			}
			else
			{
				ESP_ClearData(tmp);
			}
		}
	}
	//超时
	ESP_PrintLog(esp->RecCmd.Data);
	return 0;
}

char ESP_Reset( ESP *esp )
{
#if (ESP_USE_RSTPIN == 1)
{
	HAL_GPIO_WritePin(GPIO_Port_ESPRst,GPIO_Pin_ESPRst,GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIO_Port_ESPRst,GPIO_Pin_ESPRst,GPIO_PIN_SET);
  HAL_Delay(3000);
  return 1;
}
#else
  ESP_TransparentEnd(esp);
  HAL_Delay(1100);
	return ESP_Cmd(esp,"AT+RST\r\n", "ready", 3000);
#endif
}

char ESP_AtTest( ESP *esp )
{
	if( ESP_Cmd(esp,"AT\r\n","OK",500) )
	{
		return 1;
	}
	else
	{
		ESP_PrintLog("Log:ESP8266:AT test fail\r\n");
		return 0;
	}
}

char ESP_SetMode( ESP *esp )
{
	char cmd[80];
	sprintf(cmd, "AT+CWMODE=%d\r\n",(int)esp->Mode);
	if( ESP_Cmd(esp, cmd, "OK", 1000) )
	{
		return 1;
	}
	else
	{
		ESP_PrintLog("Log:ESP8266:Set mode fail\r\n");
		return 0;
	}
}

char ESP_JoinAP( ESP *esp )
{
	if(esp->Mode == STA || esp->Mode == AP_STA)
	{
		//todo
		// check the AP which has been connected before, to save time.
		char cmd[80];
		sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n",esp->SSID,esp->PSWD);
		if( ESP_Cmd(esp, cmd, "OK", 10000) )
		{
			return 1;
		}
		else
		{
			ESP_PrintLog("Log:ESP8266:Join AP fail\r\n");
			return 0;
		}
	}
	else
	{
		//ESP_PrintLog("Log:ESP8266:Join AP fail, it works in AP mode\r\n");
		return 1;
	}
}

char ESP_SetAP( ESP *esp )
{
	if(esp->Mode == AP || esp->Mode == AP_STA)
	{
		char cmd[128];
		sprintf(cmd, "AT+CWSAP=\"%s\",\"%s\",1,%d\r\n",esp->SSID_AP,esp->PSWD_AP,(int)esp->APMode);
		if( ESP_Cmd(esp, cmd, "OK", 1000) )
		{
			return 1;
		}
		else
		{
			ESP_PrintLog("Log:ESP8266:Set AP fail\r\n");
			return 0;
		}
	}
	else
	{
		//ESP_PrintLog("Log:ESP8266:Set AP fail, it works in STA mode\r\n");
		return 1;
	}
}

char ESP_AutoCon( ESP *esp )
{
	if(esp->AUTOCONN == Enable)
		return ESP_Cmd(esp, "AT+CWAUTOCONN=1\r\n", "OK", 1000);
	else
		return ESP_Cmd(esp, "AT+CWAUTOCONN=0\r\n", "OK", 1000);
}

char ESP_CIPMode( ESP *esp )
{
	if(esp->CIPMode == Enable)
		return ESP_Cmd(esp, "AT+CIPMODE=1\r\n", "OK", 1000);
	else
		return ESP_Cmd(esp, "AT+CIPMODE=0\r\n", "OK", 1000);
}

char ESP_CIPMux( ESP *esp )
{
	if(esp->CIPMux == Enable)
		return ESP_Cmd(esp, "AT+CIPMUX=1\r\n", "OK", 1000);
	else
		return ESP_Cmd(esp, "AT+CIPMUX=0\r\n", "OK", 1000);
}

char ESP_MultiTransport( ESP *esp )
{
	//todo
	return 0;
}

char ESP_Transparent( ESP *esp )
{
  if(esp->Transparent == Disable)
  {
    if( ESP_Cmd(esp,"AT+CIPSEND\r\n", ">", 1000) )
    {
      esp->Transparent = Enable;
      return 1;
    }
    return 0;
  }
	return 1;
}

void ESP_TransparentEnd( ESP *esp )
{
	ESP_Cmd(esp,"+++", NULL, 0);
	esp->Transparent = Disable;
}

char ESP_Set( ESP *esp )
{
	if( !ESP_Reset(esp) )
    return 0;
	if( !ESP_AtTest(esp) )
    return 0;
	if( !ESP_SetMode(esp) )
    return 0;
	if( !ESP_SetAP(esp) )
    return 0;
	if( !ESP_JoinAP(esp) )
    return 0;
	if( !ESP_AutoCon(esp) )
    return 0;
	if( !ESP_CIPMode(esp) )
    return 0;
	if( !ESP_CIPMux(esp) )
    return 0;
	return 1;
}

char ESP_Link( ESP *esp , ESP_LINK *link )
{
	char cmd[128];
	char *cmdp = cmd;

	if(esp->CIPMux == Enable)
	{
		cmdp += sprintf(cmdp,"AT+CIPSTART=%d,",(int)link->LinkID);
	}
	else
	{
		cmdp += sprintf(cmdp,"AT+CIPSTART=");
	}

	switch(link->NetPro)
	{
		case TCP:
			cmdp += sprintf(cmdp, "\"%s\",\"%s\",%d","TCP",link->IP,link->Port);
			goto keep_alive;
		case UDP:
			cmdp += sprintf(cmdp, "\"%s\",\"%s\",%d,%d,0\r\n","UDP",link->IP,link->Port,link->LocalPort);
			goto send;
		case SSL:
			cmdp += sprintf(cmdp, "\"%s\",\"%s\",%d","SSL",link->IP,link->Port);
			goto keep_alive;
		default : return 0;
	}

keep_alive:
	if(link->Keep > 0)
	{
		link->Keep = (link->Keep > 7200) ? 7200 : link->Keep;
		cmdp += sprintf(cmdp,",%d",link->Keep);
	}
	sprintf(cmdp,"\r\n");
send:
	if (ESP_Cmd(esp, cmd, "OK", 2000))
	{
		switch(link->LinkID)
		{
			case Multiple_ID_0:esp->Link0 = link;break;
			case Multiple_ID_1:esp->Link1 = link;break;
			case Multiple_ID_2:esp->Link2 = link;break;
			case Multiple_ID_3:esp->Link3 = link;break;
			case Multiple_ID_4:esp->Link4 = link;break;
			case Single_ID:esp->Link0 = link;break;
		}
		return 1;
	}
	else
	{
		ESP_PrintLog("Log:ESP8266:link fail\r\n");
		return 0;
	}
}

char ESP_Send( ESP *esp , ESP_LINK *link , char *dat , int len)
{
	switch(link->LinkID)
	{
		case Multiple_ID_0:
		case Multiple_ID_1:
		case Multiple_ID_2:
		case Multiple_ID_3:
		case Multiple_ID_4:
			goto multiple_link;
		case Single_ID:
			goto single_link;
		default: return 0;
	}
multiple_link:
	if( ESP_MultiTransport(esp) )
  {
    ESP_LowLevelSend(dat,len);
    return 1;
  }
	return 0;
single_link:
	if( ESP_Transparent(esp) )
  {
    ESP_LowLevelSend(dat,len);
    return 1;
  }
	return 0;
}
