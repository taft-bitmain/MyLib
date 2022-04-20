/*******************************************************************************
 * @file     mlx90614.h
 * @brief    Drive the IC mlx90614
 * @version  V1.1
 * @date     2022.4.17
 * @author   RainingRabbits 1466586342@qq.com
 * @code     UTF-8
******************************************************************************/
/*******************************************************************************
TIPS:
	1. The I2C clock must be lower than 150KHz.
EXAMPLE:
	float temp = MLX90614_RegRead(MLX90614_CMD_RAM|MLX90614_RAM_TOBJ1) * 0.02 - 273.15;
    myprintf("temp = :%f \r\n", temp);
*******************************************************************************/
#ifndef __MLX90614_H
#define __MLX90614_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"
#include "stm32f1xx.h"

#define MLX90614_I2C_ADDR      0x5A

#define MLX90614_I2C_SOFTWARE
//#define MLX90614_I2C_HARDWARE

/***************** Basic Interface *************************/

#ifdef MLX90614_I2C_SOFTWARE /* software i2c */

#include "myi2c.h"

static myi2c mlx90614_i2c = {
	.SCL_port = GPIOB,.SCL_pin = GPIO_PIN_0,
	.SDA_port = GPIOB,.SDA_pin = GPIO_PIN_1,
	.speed = 2 ,.slaver_addr = MLX90614_I2C_ADDR
};

static inline uint8_t MLX90614_I2C_Write ( uint8_t cmd, uint8_t * pdatain, uint8_t len )
{
    return myi2c_write_bytes( &mlx90614_i2c, cmd, 1, pdatain, len );
}

static inline uint8_t MLX90614_I2C_Read ( uint8_t cmd, uint8_t * pdataout, uint8_t len )
{
    return myi2c_read_bytes( &mlx90614_i2c, cmd, 1, pdataout, len );
}

#endif

#ifdef MLX90614_I2C_HARDWARE   /* hardware i2c */

#define MLX90614_I2C_HANDLE        hi2cx

#include "i2c.h"


static inline uint8_t MLX90614_I2C_Write ( uint8_t cmd, uint8_t * pdatain, uint8_t len )
{
    return HAL_I2C_Mem_Write( &MLX90614_I2C_HANDLE , MLX90614_I2C_ADDR<<1, cmd, I2C_MEMADD_SIZE_8BIT , pdatain, len, 500 );
}

static inline uint8_t MLX90614_I2C_Read ( uint8_t cmd, uint8_t * pdataout, uint8_t len )
{
    return HAL_I2C_Mem_Read( &MLX90614_I2C_HANDLE , MLX90614_I2C_ADDR<<1, cmd, I2C_MEMADD_SIZE_8BIT , pdataout, len, 500 );
}

#endif


/****************** Main Functions *************************/

uint8_t     MLX90614_Init       ( void );
uint8_t     MLX90614_PEC_Cal    ( const uint8_t * data, uint16_t len, uint16_t polynomial ); // polynomial is 0x107
uint16_t    MLX90614_RegRead    ( uint8_t addr );
uint8_t     MLX90614_RegWrite   ( uint8_t addr , uint16_t dat);

float       MLX90614_GetTemp    ( void );

/***************** Registers Definition ********************/

#define     MLX90614_EEPROM_TOMAX       0x00
#define     MLX90614_EEPROM_TOMIN       0x01
#define     MLX90614_EEPROM_PWMCTRL     0x02
#define     MLX90614_EEPROM_TARANGE     0x03
#define     MLX90614_EEPROM_EMICORCOEFF 0x04
#define     MLX90614_EEPROM_CONFIGREG   0x05
#define     MLX90614_EEPROM_SMBADDR     0x0E
#define     MLX90614_EEPROM_ID0         0x1C
#define     MLX90614_EEPROM_ID1         0x1D
#define     MLX90614_EEPROM_ID2         0x1E
#define     MLX90614_EEPROM_ID3         0x1F

#define     MLX90614_RAM_RAWIR1         0x04
#define     MLX90614_RAM_RAWIR2         0x05
#define     MLX90614_RAM_TA             0x06
#define     MLX90614_RAM_TA             0x06
#define     MLX90614_RAM_TOBJ1          0x07
#define     MLX90614_RAM_TOBJ2          0x08

#define     MLX90614_CMD_RAM            0x00
#define     MLX90614_CMD_EEPROM         0x20
#define     MLX90614_CMD_READFLAGS      0xF0
#define     MLX90614_CMD_SLEEP          0xFF

#ifdef __cplusplus
}
#endif
#endif
