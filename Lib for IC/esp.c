/*******************************************************************************
 * @file     esp.c
 * @brief    Connect esp8266 module with AT commands through UART.
 * @version  V1.2
 * @date     2021.12.2
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
 *******************************************************************************/
#include "esp.h"
#include <stdio.h>
#include <stdarg.h>
#include "base.h"

ESP esp8266;

//todo
void UART2_IDLE_Callback(uint8_t *data, uint16_t len)
{    
	ESP_LowLevelRec(&esp8266, (char*) data, len);
}

void ESP_LowLevelSend( char *data, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)data, len, 1000);
}

void ESP_LowLevelRec( ESP *esp , const char *data_in, int len)
{
	if(esp->Transparent == Enable)
	{
		if(esp->Link0 == NULL)
		{
			ESP_DebugLog("Log:ESP8266:Link data is received, but no buff to store\r\n");
			return ;
		}
		else
		{
			ESP_Data *tmp = &(esp->Link0->RecData);
			if (len > (ESP_RXBUFF_LEN - tmp->Count))
				ESP_DebugLog(
						"Log:ESP8266 Link data is received, but buff is not enough\r\n");
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
		ESP_Data *tmp = &(esp->RecCmd);
		if (len > (ESP_RXBUFF_LEN - tmp->Count))
			ESP_DebugLog(
					"Log:ESP8266 cmd data is received, but buff is not enough\r\n");
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


void ESP_ClearData(ESP_Data *dat)
{
	dat->Count = 0;
	dat->Head = dat->Tail = dat->Data;
}

#define ESP_CMD_NONE        0x00
#define ESP_CMD_TIMEOUT     0x01
#define ESP_CMD_BUSY        0x02
#define ESP_CMD_OK          0x04
#define ESP_CMD_ERROR       0x08

static char _CmdBuff[128];

char ESP_Cmd( ESP *esp , unsigned int waittime, char *cmd,...)
{
	ESP_Data *tmp = &(esp->RecCmd);
	ESP_ClearData(tmp);
    
    int nBytes ;
    
    va_list arg_ptr;
    va_start(arg_ptr, cmd);
    nBytes = vsprintf(_CmdBuff, cmd, arg_ptr);
    va_end(arg_ptr);
    
	ESP_LowLevelSend(_CmdBuff, nBytes);

	if (waittime == 0)
		return ESP_CMD_NONE;

	uint32_t tickstart = HAL_GetTick();
	while ((HAL_GetTick() - tickstart) < waittime)
	{
		if (tmp->Count)
		{
            if (strstr(tmp->Tail, "OK") != NULL)
			{
				return ESP_CMD_OK;
			}
            else if(strstr(tmp->Tail, "ERROR") != NULL)
            {
                return ESP_CMD_ERROR;
            }
            else if(strstr(tmp->Tail, "busy p...") != NULL)
            {
                return ESP_CMD_BUSY;
            }
		}
	}
	return ESP_CMD_TIMEOUT;
}

char ESP_Reset( ESP *esp )
{
#if (ESP_USE_PINRST == 1)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
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
    ESP_DebugLog("Log:ESP8266 AT test...\r\n");
    
	if( ESP_Cmd(esp,2000,"AT\r\n") == ESP_CMD_OK )
	{
		return 1;
	}
	else
	{
		ESP_DebugLog("Log:ESP8266 AT test fail\r\n");
		return 0;
	}
}

char ESP_SetMode( ESP *esp )
{
	if( ESP_Cmd(esp,1000,"AT+CWMODE=%d\r\n",(int)(esp->Mode) ) == ESP_CMD_OK )
	{
		return 1;
	}
	else
	{
		ESP_DebugLog("Log:ESP8266 Set mode fail\r\n");
		return 0;
	}
}

char ESP_JoinAP( ESP *esp )
{
    ESP_DebugLog("Log:ESP8266 join AP...\r\n"); 
    ESP_DebugLog("\tSSID: %s\r\n",esp->SSID);
    ESP_DebugLog("\tPSWD: %s\r\n",esp->PSWD);
    
	if(esp->Mode == STA || esp->Mode == AP_STA)
	{
        if( (ESP_Cmd(esp, 1000, "AT+CWJAP?\r\n") == ESP_CMD_OK) && ( strstr(esp->RecCmd.Tail, esp->SSID ) != NULL) )
        {
            ESP_DebugLog("Log:ESP8266 has been joined AP\r\n");
            esp->Connected = Enable;
            return 1;
        }
        
        if( ESP_Cmd(esp,10000, "AT+CWJAP=\"%s\",\"%s\"\r\n",esp->SSID,esp->PSWD ) == ESP_CMD_OK )
        {
            ESP_DebugLog("Log:ESP8266 Join AP success\r\n");
            esp->Connected = Enable;
            return 1;
        }
        else
        {
            esp->Connected = Disable;
            ESP_DebugLog("Log:ESP8266 Join AP error\r\n");
            return 0;
        }
	}
	else
	{
		ESP_DebugLog("Log:ESP8266 Join AP fail, it works in AP mode\r\n");
		return 1;
	}
}

char ESP_SetAP( ESP *esp )
{
	if(esp->Mode == AP || esp->Mode == AP_STA)
	{
		if( ESP_Cmd(esp, 1000,"AT+CWSAP=\"%s\",\"%s\",1,%d\r\n",esp->SSID_AP,esp->PSWD_AP,(int)esp->APMode )==ESP_CMD_OK )
		{
			return 1;
		}
		else
		{
			ESP_DebugLog("Log:ESP8266 Set AP fail\r\n");
			return 0;
		}
	}
	else
	{
		ESP_DebugLog("Log:ESP8266 Set AP fail, it works in STA mode\r\n");
		return 1;
	}
}

char ESP_AutoCon( ESP *esp )
{
	if(esp->AutoConnect == Enable)
    {
        if( ESP_Cmd(esp, 1000, "AT+CWAUTOCONN=1\r\n")==ESP_CMD_OK )
            return 1;
        else
            return 0;
    }
            
	else
    {
		if( ESP_Cmd(esp, 1000, "AT+CWAUTOCONN=0\r\n")==ESP_CMD_OK )
            return 1;
        else
            return 0;
    }
}

char ESP_CIPMode( ESP *esp )
{
	if(esp->CIPMode == Enable)
    {
        if( ESP_Cmd(esp, 1000, "AT+CIPMODE=1\r\n")==ESP_CMD_OK )
            return 1;
        else
            return 0;
    }
	else
    {
        if( ESP_Cmd(esp, 1000, "AT+CIPMODE=0\r\n")==ESP_CMD_OK )
            return 1;
        else
            return 0;
    }
}

char ESP_CIPMux( ESP *esp )
{
	if(esp->CIPMux == Enable)
    {
        if( ESP_Cmd(esp, 1000, "AT+CIPMUX=1\r\n")==ESP_CMD_OK )
            return 1;
        else
            return 0;
    }
	else
	{
        if( ESP_Cmd(esp, 1000, "AT+CIPMUX=0\r\n")==ESP_CMD_OK )
            return 1;
        else
            return 0;
    }
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
        if( ESP_Cmd(esp,1000,"AT+CIPSEND\r\n") == ESP_CMD_OK )
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
	esp->Transparent = Disable;
    ESP_Cmd(esp,300,"+++");
}

char ESP_Set( ESP *esp )
{
    
//	if( !ESP_Reset(esp) )
//        return 0;
    if( !ESP_AtTest(esp) )
        return 0;
	if( !ESP_SetMode(esp) )
        return 0;
//	if( !ESP_SetAP(esp) )
//        return 0;
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

char ESP_Link_Set( ESP *esp , ESP_Link *link )
{
	char cmd[128];
	char *cmdp = cmd;
    char ret;
    
    if(esp == NULL || link == NULL)
    {
        ESP_DebugLog("Log:ESP8266 wrong parameter for connecting\r\n");
        return 0;
    }
    if( !esp->Connected )
    {
        ESP_DebugLog("Log:ESP8266 can't set link as it is not connect to AP\r\n");
        return 0;
    }
    ESP_DebugLog("Log:ESP8266 link to %s:%d\r\n",link->IP,link->Port);
    
    ESP_ClearData( &(link->RecData) );

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
    ret = ESP_Cmd(esp,2000,"%s",cmd);
	if(  ret == ESP_CMD_OK || 
        (ret == ESP_CMD_ERROR && strstr(esp->RecCmd.Tail, "ALREADY CONNECTED") != NULL) )
	{
        link->Linked = Enable;
		switch(link->LinkID)
		{
			case Multiple_ID_0  :esp->Link0 = link;break;
			case Multiple_ID_1  :esp->Link1 = link;break;
			case Multiple_ID_2  :esp->Link2 = link;break;
			case Multiple_ID_3  :esp->Link3 = link;break;
			case Multiple_ID_4  :esp->Link4 = link;break;
			case Single_ID      :esp->Link0 = link;break;
		}
        ESP_DebugLog("Log:ESP8266 link success\r\n");
		return 1;
	}
	else
	{
		ESP_DebugLog("Log:ESP8266 link fail\r\n");
		return 0;
	}
    
}
char ESP_Link_Close ( ESP *esp , ESP_Link *link )
{
    if(link->Linked == Enable)
    {
        if( ESP_Cmd(esp,1000,"AT+CIPSLOSE\r\n") == ESP_CMD_OK )
        {
            link->Linked = Disable;
            ESP_DebugLog("Log:ESP8266 Link closed\r\n");
            return 1;
        }
        ESP_DebugLog("Log:ESP8266 Link failed\r\n");
        return 0;
    }
    return 1;
}

char ESP_Link_Send( ESP *esp , ESP_Link *link , char *dat , int len)
{
    if( link == NULL || link->Linked == Disable)
    {
        ESP_DebugLog("Log:ESP8266 can't send as link is not set\r\n");
        return 0;
    }
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
