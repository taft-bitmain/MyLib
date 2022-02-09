/*******************************************************************************
 * @file     mpu6050.c
 * @brief
 * @version  V1.0
 * @date     2022.1.20
 * @author   RainingCats 1466586342@qq.com
 * @code     UTF-8
*******************************************************************************/

#include "base.h"
#include "mpu6050.h"


/***************** Basic Interface *************************/

inline uint8_t MPU6050_ReadReg ( uint8_t regaddr )
{
    return MyI2C_ReadReg(&mpu6050_i2c,regaddr);
}

inline uint8_t MPU6050_ReadRegs ( uint8_t regaddr,uint8_t * pdat,uint16_t len )
{
    return MyI2C_ReadMem(&mpu6050_i2c,regaddr,pdat,len);
}

inline uint8_t MPU6050_WriteReg ( uint8_t regaddr,uint8_t dat )
{
    return MyI2C_WriteReg(&mpu6050_i2c,regaddr,dat);
}

inline void MPU6050_DelayMs ( uint16_t ms )
{
    HAL_Delay(ms);
}


/****************** Main Functions *************************/

uint8_t MPU6050_Init (void)
{

    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x80);     // reset
    MPU6050_DelayMs(200);
    
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);     // wake up, Internal 8MHz oscillator
    MPU6050_DelayMs(200);
    
    if( MPU6050_ReadReg(MPU6050_WHO_AM_I) != MPU6050_I2CADDR )
        return 0;
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	// Gyroscope Full Scale Range: ±2000dps
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00);	// accelerometer Full Scale Range: ±2g
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x07 );    // sample rate 125Hz
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);	        // DLPF 5Hz
    //MPU6050_WriteReg(MPU6050_INT_ENABLE,0X00);	  // disable all interrupts
	//MPU6050_WriteReg(MPU6050_USER_CTRL,0X00);	      // I2C master close
	//MPU6050_WriteReg(MPU6050_FIFO_EN,0X00);	      // disable FIFO
	//MPU6050_WriteReg(MPU6050_INT_PIN_CONFIG,0X80);  // PIN INT activate low
    return 1;
}



uint8_t MPU6050_GetAccel (uint16_t *Accelx, uint16_t *Accely, uint16_t *Accelz)
{
    uint8_t dat[6];
    MPU6050_ReadRegs(MPU6050_ACCEL_OUT,dat,6);
    *Accelx = dat[0]<<8 | dat[1];
    *Accely = dat[0]<<8 | dat[1];
    *Accelz = dat[0]<<8 | dat[1];
    return 1;
}

uint16_t MPU6050_GetTemp (void)
{
    uint8_t dat[2];
    MPU6050_ReadRegs(MPU6050_TEMP_OUT,dat,2);
    return dat[0]<<8 | dat[1];
}

uint8_t MPU6050_GetGyro (uint16_t *Gyrox, uint16_t *Gyroy, uint16_t *Gyroz)
{
    uint8_t dat[6];
    MPU6050_ReadRegs(MPU6050_GYRO_OUT,dat,6);
    *Gyrox = dat[0]<<8 | dat[1];
    *Gyroy = dat[2]<<8 | dat[3];
    *Gyroz = dat[4]<<8 | dat[5];
    return 1;
}
uint8_t MPU6050_GetAll (uint16_t *alldata)
{
    uint8_t dat[14];
    MPU6050_ReadRegs(MPU6050_ACCEL_OUT,dat,14);
    alldata[0] = dat[0]<<8 | dat[1];
    alldata[1] = dat[2]<<8 | dat[3];
    alldata[2] = dat[4]<<8 | dat[5];
    
    alldata[3] = dat[6]<<8 | dat[7];
    
    alldata[4] = dat[8]<<8 | dat[9];
    alldata[5] = dat[10]<<8 | dat[11];
    alldata[6] = dat[12]<<8 | dat[13];
    return 1;
}
