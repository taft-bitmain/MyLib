/*******************************************************************************
 * @file     mydsp.h
 * @brief    simple operations to signal
 * @version  V1.3
 * @date     2022.3.11
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
    none
EXAMPLE:
    none

*******************************************************************************/
#ifndef __MYDSP_H
#define __MYDSP_H

#include "stdint.h"

#define     debug_printf        myprintf

uint16_t *  mydsp_32to16        ( uint32_t * data, uint16_t len );
uint8_t *   mydsp_32to8         ( uint32_t * data, uint16_t len );
uint8_t *   mydsp_16to8         ( uint16_t * data, uint16_t len );

void		mydsp_print         ( const char * dataname, uint16_t * data, uint16_t len );

uint16_t    mydsp_filter        ( uint16_t * datain, uint16_t * dataout, uint16_t len, uint16_t n );
uint16_t	mydsp_get_vpp       ( uint16_t * data, uint16_t len, uint16_t * max, uint16_t * min );
uint16_t	mydsp_get_dc        ( uint16_t * data, uint16_t len );
uint16_t    mydsp_get_period	( uint16_t * data, uint16_t len );
int16_t     mydsp_get_phase     ( uint16_t * dat1, uint16_t * dat2, uint16_t len, uint16_t range );
uint16_t	mydsp_get_sinvpp    ( uint16_t * data, uint16_t len );
uint16_t    mydsp_get_duty      ( uint16_t * data, uint16_t len );
uint16_t    mydsp_get_rectcoeff ( uint16_t * data, uint16_t len, uint16_t * max, uint16_t * min, uint16_t * dc, uint16_t * rectcoeff );

uint16_t	mydsp_match         ( const uint16_t * tab_match, uint16_t tab_len, uint16_t tab_wid, const uint16_t * datain, const float * data_weight );

void		mydsp_lineplot      ( const uint16_t * point_x, const uint16_t * point_y, uint16_t point_count, uint16_t * dataout, uint16_t countout );
void 		mydsp_logplot       ( const uint16_t * point_x, const uint32_t * point_logy, uint16_t point_count, uint16_t * dataout, uint16_t countout );
#endif
