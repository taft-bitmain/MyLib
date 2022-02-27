/*******************************************************************************
 * @file     mytask.c
 * @brief    Put the following functions into mian.c, using Arduino style.
 * @version  V1.0
 * @date     2021.8.1
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "mytask.h"
//#include "base.h"



void setup(void)
{
    myprintf("Program start!\r\n");
}



void loop(void)
{
    static int i;
    myprintf("tick %d\r\n",i++);
    HAL_Delay(1000);
}
