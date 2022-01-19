/*******************************************************************************
 * @file     mydsp.c
 * @brief    simple operations to signal
 * @version  V1.2
 * @date     2021.5.29
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "mydsp.h"
#include "math.h"
#include "base.h"

uint16_t MyDSP_Filter(uint16_t *datain,uint16_t *dataout, uint16_t len,uint16_t n)
{
	uint16_t i,n_tmp;
	uint32_t sum = 0;
	if( len <= n )
		return 0;
	n |= 0x01;
	n_tmp = n/2;
	for( i = 0; i < len + n; i++ )
	{
		if(i < n)
		{
			sum += datain[i];
			if(i%2 == 0)
				dataout[i/2] = sum/(i + 1);
		}
		else if(i < len)
		{
			sum += datain[i];
			sum -= datain[i-n];
			dataout[i-n_tmp] = sum/n;
		}
		else
		{
			sum -= datain[i-n];
			if(i%2 == 1)
				dataout[len - n_tmp + (i - len)/2 ] = sum/(len + n - i-1);
		}
	}
	return 1;
}

uint16_t MyDSP_GetCycleLen(uint16_t *data, uint16_t len)
{
	uint16_t i,CycleLen;
	uint16_t flag1 = 0,flag2 = 0;
	int64_t AutoCor0 = 0,AutoCor1 = 0,AutoCor2;
	len /= 2;
	for(CycleLen = 0;CycleLen < len; CycleLen++)
	{
		AutoCor2 = 0;
		for(i = 0;i < len; i++)
			AutoCor2 += data[i]*data[i+CycleLen];
		if(CycleLen > 2 && AutoCor1>AutoCor0 && AutoCor1>AutoCor2)
		{
			if(flag1 == 0)
				flag1 = CycleLen-1;
			else if(flag2 == 0)
				flag2 = CycleLen-1;
			else
				return flag2 - flag1;
		}
		AutoCor0 = AutoCor1;
		AutoCor1 = AutoCor2;
	}
	return 0;
}

// range <- cyclelen
int16_t MyDSP_GetPhase(uint16_t *dat1,uint16_t *dat2, uint16_t len,uint16_t range)
{
  
  int16_t i,j;
  int16_t min_index = 0; 
  uint32_t min_sum = 0;
  
  range = ( range > len ) ? len : range;
  
  for( i = -range/2; i < range/2; i++ )
  {
    uint32_t sum;
    int16_t tmp;
    sum = 0;
  	for( j = len/2-range/2; j < len/2+range/2; j++)
    {
      tmp = dat1[j] - dat2[j+i];
      if(tmp > 0)
        sum += tmp;
      else
        sum += (-tmp);
    }
    if(min_sum == 0)
    {
      min_sum = sum;
    }
    else
    {
      if(min_sum > sum)
      {
        min_index = i;
        min_sum = sum;
      }
    }
  }
  return min_index;
}

uint16_t MyDSP_GetVpp(uint16_t *data, uint16_t len,uint16_t *max,uint16_t *min)
{
	uint16_t i;
	for(*min = data[0],*max = data[0],i = 1;i < len; i++)
	{
	  if(*min > data[i]) *min = data[i];
	  if(*max < data[i]) *max = data[i];
	}
	return *max-*min;
}

uint16_t MyDSP_Get(uint16_t *data, uint16_t len,uint16_t *max,uint16_t *min,uint16_t *dc,uint16_t *rectcoeff)
{
	uint16_t i;
  uint32_t count;
	for(*min = data[0],*max = data[0],i = 1,count = data[0];i < len; i++)
	{
	  if(*min > data[i]) *min = data[i];
	  if(*max < data[i]) *max = data[i];
    count += data[i];
	}
  *dc = count/len;
  count = 0;
  for(i = 0;i < len; i++)
	{
    if( data[i] >= *dc)
      count += (data[i] - *dc);
    else
      count += (*dc - data[i]);
	}
  *rectcoeff = (uint16_t)((float)count / len / (*max - *min) * 200);
	return 1;
}

uint16_t MyDSP_GetDuty(uint16_t *data, uint16_t len,uint16_t dc)
{
	uint16_t i,up = 0,cycle = 0; 
  for(i = 0 ;(i < len) && (data[i] > dc); i++);
  for( ;(i < len) && (data[i] < dc); i++);
  for( ;(i < len) && (data[i] > dc); i++)
  {
    up++;
    cycle++;
  }
  for( ;(i < len) && (data[i] < dc); i++)
  {
    cycle++;
  }
  printf("dc = %d up = %d cycle = %d\r\n",dc,up,cycle);
  
  if(i < len)
    return up * 100 /cycle;
  
  return 0;
}


// y = x / a + b;
uint16_t MyDSP_Correct(uint16_t *data, uint16_t len,int16_t a,float b,int16_t c)
{
	uint16_t i; 
  for(i = 0 ; i < len ; i++)
    data[i] = (uint16_t)((data[i] + a) / b + c);
  return 1;
}

uint16_t MyDSP_GetDCLevel(uint16_t *data, uint16_t len)
{
	uint16_t i,dc_level;
	uint32_t total_sum = 0;
	for(i = 0;i < len; i++)
		total_sum += data[i];
	dc_level = total_sum/len;
	return dc_level;
}

uint16_t MyDSP_GetSinVpp(uint16_t *data,uint16_t len,uint16_t *dc_level)
{
	uint16_t cycle_len,dc,i;
	uint32_t ac_sum = 0;
	
	cycle_len = MyDSP_GetCycleLen(data,len);
	////////////////////////
	//printf("cycle_len=%d\r\n",cycle_len);
	
	dc = MyDSP_GetDCLevel(data,len-len%cycle_len);
	*dc_level = dc;

	for(i = 0;i < cycle_len; i++)
	{
		if(data[i] >= dc)
			ac_sum += (data[i] - dc);
		else
			ac_sum += (dc - data[i]);
	}
	////////////////////
	//MyDSP_Print("X1",data,len);
	return (uint16_t)(ac_sum*3.14159/cycle_len);
}

void MyDSP_Print(const char *dataname,uint16_t *data, uint16_t len)
{
	uint16_t i;
	for(i = 0;i < len; i++)
		printf("%s=%d,",dataname,*data++);
	printf("\r\n");
}

void MyDSP_B16toB8(uint16_t *data,uint16_t len)
{
	uint16_t i;
	uint8_t *ptmp = (uint8_t *)data;
	for( i = 0; i < len + len; i++ )
	{
    if(data[i] > 255)
      ptmp[i] = 0;
		else
      ptmp[i] = data[i];
	}
}

void MyDSP_Point2Point(uint16_t *data,uint16_t len,uint16_t len2)
{
	uint16_t i;
	uint8_t *ptmp = (uint8_t *)data;
	for( i = 0; i < len + len; i++ )
	{
    if(data[i] > 255)
      ptmp[i] = 0;
		else
      ptmp[i] = data[i];
	}
}

uint16_t MyDSP_Match(const uint16_t *matchTab,uint16_t len,uint16_t wid,const uint16_t *dataIn,const float *dataWei)
{
	uint16_t match_index,i,j;
	uint32_t match_tmp,match_result;
	for(i = 0 ;i < len; i++)
	{
		for(match_tmp = 0,j = 0;j < wid; j++)
		{
			match_tmp += (matchTab[wid*i + j] - dataIn[j]) * (matchTab[wid*i + j] - dataIn[j]) * dataWei[j];
		}
		if(match_tmp < match_result)
		{
			match_index = i;
			match_result = match_tmp;
		}
	}
	return match_index;
}

void MyDSP_LinePlot(const uint16_t *xin,const uint16_t *yin,const uint16_t countin,uint16_t *dataout,const  uint16_t countout)
{
	uint16_t Next_x,Next_y;
	uint16_t Prev_x,Prev_y;
	float dy;
	
	for(index = 0;index < len; index++)
	{
		
		Prev_x = xin[index];
		Prev_y = yin[index];
		
		Next_x = xin[index + 1];
		Next_y = yin[index + 1];
		
		for( i = Prev_x; i < Next_x; i++ )
		{
			if(i > w_max)
				return;
			dy = ((float)Next_y - Prev_y) / (Next_x - Prev_x);
			
			dataout[i] = (uint16_t)(dy * (i - Prev_x) + Prev_y);
		}
		
	}
}

void MyDSP_LogPlot(const uint16_t *datain,const uint32_t *login,const uint16_t countin,uint16_t *dataout,const uint16_t countout)
{
	uint16_t linein[countin],index,first = login[0];
	for(index = 0;index < countin; index++)
	{
		linein[index] = (uint16_t)(log(login[index]/first)/(log(10))*100 + 0.5);
		if(linein[index] > countout)
			return;
	}
	MyDSP_LinePlot(datain,linein,countin,dataout,countout);
}
