/*******************************************************************************
 * @file     mydsp.c
 * @brief    simple operations to signal
 * @version  V1.3
 * @date     2022.3.11
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "mydsp.h"
#include "math.h"
#include "base.h"



uint16_t * mydsp_32to16 ( uint32_t * data, uint16_t len )
{
    uint16_t i;
    uint16_t * ptmp = ( uint16_t * )data;

    for( i = 0; i < len; i++ )
        ptmp[i] = data[i];
    return ( uint16_t * )data;
}

uint8_t * mydsp_32to8 ( uint32_t * data, uint16_t len )
{
    uint16_t i;
    uint8_t * ptmp = ( uint8_t * )data;

    for( i = 0; i < len; i++ )
        ptmp[i] = data[i];
    return ( uint8_t * )data;
}
uint8_t * mydsp_16to8 ( uint16_t * data, uint16_t len )
{
    uint16_t i;
    uint8_t * ptmp = ( uint8_t * )data;

    for( i = 0; i < len; i++ )
        ptmp[i] = data[i];
    return ( uint8_t * )data;
}


void mydsp_print ( const char * dataname, uint16_t * data, uint16_t len )
{
    uint16_t i;

    for( i = 0; i < len; i++ )
        debug_printf( "%s=%d,", dataname, *data++ );

    debug_printf( "\r\n" );
}


// The return length will be reduced by n
uint16_t mydsp_filter ( uint16_t * datain, uint16_t * dataout, uint16_t len, uint16_t n )
{
    uint16_t i;
    uint32_t sum = 0;

    if( len == 0 || n == 0 || len <= n  )
        return 0;

    for( i = 1; i < n ; i++ )
        sum += datain[i];

    for( ; i < len; i++ )
    {
        sum += datain[i];
        sum -= datain[i-n];
        dataout[i-n] = sum / n;
    }
    return 1;
}


uint16_t mydsp_get_vpp( uint16_t * data, uint16_t len, uint16_t * max, uint16_t * min )
{
    uint16_t i;

    for( *min = data[0], *max = data[0], i = 1; i < len; i++ )
    {
        if( *min > data[i] ) *min = data[i];

        if( *max < data[i] ) *max = data[i];
    }

    return *max - *min;
}

uint16_t mydsp_get_dc ( uint16_t * data, uint16_t len )
{
    uint16_t i, dc_level;
    uint32_t total_sum = 0;

    for( i = 0; i < len; i++ )
        total_sum += data[i];

    dc_level = total_sum / len;
    return dc_level;
}

uint16_t mydsp_get_period( uint16_t * data, uint16_t len )
{
    uint16_t i, CycleLen;
    uint16_t flag1 = 0, flag2 = 0;
    int64_t AutoCor0 = 0, AutoCor1 = 0, AutoCor2;
    len /= 2;

    for( CycleLen = 0; CycleLen < len; CycleLen++ )
    {
        AutoCor2 = 0;

        for( i = 0; i < len; i++ )
            AutoCor2 += data[i] * data[i + CycleLen];

        if( CycleLen > 2 && AutoCor1 > AutoCor0 && AutoCor1 > AutoCor2 )
        {
            if( flag1 == 0 )
                flag1 = CycleLen - 1;

            else if( flag2 == 0 )
                flag2 = CycleLen - 1;

            else
                return flag2 - flag1;
        }

        AutoCor0 = AutoCor1;
        AutoCor1 = AutoCor2;
    }

    return 0;
}

// range == period
int16_t mydsp_get_phase( uint16_t * dat1, uint16_t * dat2, uint16_t len, uint16_t range )
{

    int16_t i, j;
    int16_t min_index = 0;
    uint32_t min_sum = 0;

    range = ( range > len ) ? len : range;

    for( i = -range / 2; i < range / 2; i++ )
    {
        uint32_t sum;
        int16_t tmp;
        sum = 0;

        for( j = len / 2 - range / 2; j < len / 2 + range / 2; j++ )
        {
            tmp = dat1[j] - dat2[j + i];

            if( tmp > 0 )
                sum += tmp;

            else
                sum += ( -tmp );
        }

        if( min_sum == 0 )
        {
            min_sum = sum;
        }

        else
        {
            if( min_sum > sum )
            {
                min_index = i;
                min_sum = sum;
            }
        }
    }

    return min_index;
}

uint16_t mydsp_get_sinvpp( uint16_t * data, uint16_t len )
{
    uint16_t period, dc, i;
    uint32_t ac_sum = 0;

    period = mydsp_get_period( data, len );
    
    ////////////////////////
    //printf("cycle_len=%d\r\n",cycle_len);

    dc = mydsp_get_dc( data, len - len % period );

    for( i = 0; i < period; i++ )
    {
        if( data[i] >= dc )
            ac_sum += ( data[i] - dc );

        else
            ac_sum += ( dc - data[i] );
    }

    ////////////////////
    //MyDSP_Print("X1",data,len);
    return ( uint16_t )( ac_sum * 3.14159 / period );
}


uint16_t mydsp_get_duty( uint16_t * data, uint16_t len )
{
    uint16_t dc, i ,up_count, all_count, period;
    
    period = mydsp_get_period( data, len );
    
    all_count = len - len % period;
    
    dc = mydsp_get_dc( data, all_count );

    for( i = 0; i < all_count; i++ )
    {
        if( data[i] >= dc )
            up_count++;
    }

    return up_count*100/all_count;

}

uint16_t mydsp_get_rectcoeff( uint16_t * data, uint16_t len, uint16_t * max,
                    uint16_t * min, uint16_t * dc, uint16_t * rectcoeff )
{
    uint16_t i;
    uint32_t count;

    for( *min = data[0], *max = data[0], i = 1, count = data[0]; i < len; i++ )
    {
        if( *min > data[i] ) *min = data[i];

        if( *max < data[i] ) *max = data[i];

        count += data[i];
    }

    *dc = count / len;
    count = 0;

    for( i = 0; i < len; i++ )
    {
        if( data[i] >= *dc )
            count += ( data[i] - *dc );

        else
            count += ( *dc - data[i] );
    }

    *rectcoeff = ( uint16_t )( ( float )count / len / ( *max - *min ) * 200 );
    return 1;
}


uint16_t mydsp_match( const uint16_t * tab_match, uint16_t tab_len, uint16_t tab_wid, const uint16_t * datain, const float * data_weight )
{
    uint16_t match_index, i, j;
    uint32_t match_tmp, match_result;

    for( i = 0 ; i < tab_len; i++ )
    {
        for( match_tmp = 0, j = 0; j < tab_wid; j++ )
        {
            match_tmp += ( tab_match[tab_wid * i + j] - datain[j] ) * ( tab_match[tab_wid * i + j] -
                         datain[j] ) * data_weight[j];
        }

        if( match_tmp < match_result )
        {
            match_index = i;
            match_result = match_tmp;
        }
    }

    return match_index;
}

void mydsp_lineplot( const uint16_t * point_x, const uint16_t * point_y, uint16_t point_count, uint16_t * dataout, uint16_t countout )
{
    uint16_t i, index;
    uint16_t Next_x, Next_y;
    uint16_t Prev_x, Prev_y;
    float dy;

    for( index = 0; index < point_count; index++ )
    {

        Prev_x = point_x[index];
        Prev_y = point_y[index];

        Next_x = point_x[index + 1];
        Next_y = point_y[index + 1];

        for( i = Prev_x; i < Next_x; i++ )
        {
            if( i > countout )
                return;

            dy = ( ( float )Next_y - Prev_y ) / ( Next_x - Prev_x );

            dataout[i] = ( uint16_t )( dy * ( i - Prev_x ) + Prev_y );
        }
    }
}

void mydsp_logplot( const uint16_t * point_x, const uint32_t * point_logy, uint16_t point_count, uint16_t * dataout, uint16_t countout )
{
    uint16_t point_y[point_count], count, first = point_logy[0];

    for( count = 0; count < point_count; count++ )
    {
        point_y[count] = ( uint16_t )( log( point_logy[count] / first ) / ( log( 10 ) ) * 100 + 0.5 );

        if( point_y[count] > countout )
            return;
    }

    mydsp_lineplot( point_x, point_y, point_count, dataout, countout );
}
