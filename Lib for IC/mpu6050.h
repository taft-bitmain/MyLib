/*******************************************************************************
 * @file     mpu6050.h
 * @brief
 * @version  V1.0
 * @date     2022.1.20
 * @author   RainingCats 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/
/*******************************************************************************
TIPS:
	
EXAMPLE:
    MPU6050_Init();
    while(1)
    {
        static uint16_t alldata[7];
        MPU6050_GetAll(alldata);
        myprintf("accel: %04X,%04X,%04X\r\n",alldata[0],alldata[1],alldata[2]);
        HAL_Delay(10);
        myprintf("temp : %04X\r\n",alldata[4]);
        HAL_Delay(10);
        myprintf("gyro : %04X,%04X,%04X\r\n",alldata[4],alldata[5],alldata[6]);
    }
*******************************************************************************/
#ifndef __MPU6050_H
#define __MPU6050_H
#ifdef __cplusplus
extern "c" {
#endif

#include "stdint.h"


/***************** Basic Interface *************************/

#include "myi2c.h"

#define     MPU6050_I2CADDR             0x68 

static MyI2C mpu6050_i2c = {
	.SCL_Port = GPIOB,.SCL_Bit = GPIO_PIN_10,
	.SDA_Port = GPIOB,.SDA_Bit = GPIO_PIN_0,
	.Speed = 3 ,.DevAddr = MPU6050_I2CADDR << 1
};

uint8_t     MPU6050_ReadReg     ( uint8_t regaddr );
uint8_t     MPU6050_ReadRegs    ( uint8_t regaddr,uint8_t * pdat,uint16_t len );
uint8_t     MPU6050_WriteReg    ( uint8_t regaddr,uint8_t dat );
void        MPU6050_DelayMs     ( uint16_t ms );

/****************** Main Functions *************************/

uint8_t     MPU6050_Init        ( void );
uint8_t     MPU6050_GetAccel    ( uint16_t *Accelx, uint16_t *Accely, uint16_t *Accelz );
uint16_t    MPU6050_GetTemp     ( void );
uint8_t     MPU6050_GetGyro     ( uint16_t *Gyrox, uint16_t *Gyroy, uint16_t *Gyroz );
uint8_t     MPU6050_GetAll      ( uint16_t *alldata );

/***************** Registers Definition ********************/
#define     MPU6050_SELF_TEST_X         0x0D
#define     MPU6050_SELF_TEST_Y         0x0E
#define     MPU6050_SELF_TEST_Z         0x0F
#define     MPU6050_SELF_TEST_A         0x10
#define     MPU6050_SMPLRT_DIV          0x19
#define     MPU6050_CONFIG              0x1A
#define     MPU6050_GYRO_CONFIG         0x1B
#define     MPU6050_ACCEL_CONFIG        0x1C
#define     MPU6050_MOT_THR             0x1F

#define     MPU6050_FIFO_EN             0x23
#define     MPU6050_INT_PIN_CONFIG      0x37
#define     MPU6050_INT_ENABLE          0x38
#define     MPU6050_INT_STATUS          0x3A

#define     MPU6050_ACCEL_OUT           0x3B
    //          MPU6050_ACCEL_XOUT_H        0x3B
    //          MPU6050_ACCEL_XOUT_L        0x3C
    //          MPU6050_ACCEL_YOUT_H        0x3D
    //          MPU6050_ACCEL_YOUT_L        0x3E
    //          MPU6050_ACCEL_ZOUT_H        0x3F
    //          MPU6050_ACCEL_ZOUT_L        0x40
#define     MPU6050_TEMP_OUT            0x41
    //          MPU6050_TEMP_OUT_H          0x41
    //          MPU6050_TEMP_OUT_L          0x42
#define     MPU6050_GYRO_OUT            0x43
    //          MPU6050_GYRO_XOUT_H         0x43
    //          MPU6050_GYRO_XOUT_L         0x44
    //          MPU6050_GYRO_YOUT_H         0x45
    //          MPU6050_GYRO_YOUT_L         0x46
    //          MPU6050_GYRO_ZOUT_H         0x47
    //          MPU6050_GYRO_ZOUT_L         0x48
    
#define     MPU6050_SIGNAL_PATH_RESET   0x68
#define     MPU6050_MOT_DETTECT_CTRL    0x69
#define     MPU6050_USER_CTRL           0x6A
#define     MPU6050_PWR_MGMT_1          0x6B
#define     MPU6050_PWR_MGMT_2          0x6C
#define     MPU6050_FIFO_COUNT          0x72
    //          MPU6050_FIFO_COUNT_H        0x72
    //          MPU6050_FIFO_COUNT_L        0x73
#define     MPU6050_FIFO                0x74
#define     MPU6050_WHO_AM_I            0x75

#ifdef __cplusplus
}
#endif
#endif
