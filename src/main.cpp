#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <memory.h>
#include <math.h>
#include <sys/time.h>
//#include <linux/i2c.h>
//#include <linux/i2c-dev.h>
#include "../include/raspberrypii2c.h"
#include "../include/Kalman.h"


#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02



#define XG_OFFSET_H 0x13
#define XG_OFFSET_L 0x14
#define YG_OFFSET_H 0x15
#define YG_OFFSET_L 0x16
#define ZG_OFFSET_H 0x17
#define ZG_OFFSET_L 0x18



#define	SMPLRT_DIV		0x19	//Sample Rate Divider. Typical values:0x07(125Hz) 1KHz internal sample rate
#define	CONFIG			0x1A	//Low Pass Filter.Typical values:0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//Gyro Full Scale Select. Typical values:0x10(1000dps)
#define	ACCEL_CONFIG	0x1C	//Accel Full Scale Select. Typical values:0x01(2g)

#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48


#define MAG_XOUT_L		0x03
#define MAG_XOUT_H		0x04
#define MAG_YOUT_L		0x05
#define MAG_YOUT_H		0x06
#define MAG_ZOUT_L		0x07
#define MAG_ZOUT_H		0x08




#define	PWR_MGMT_1		0x6B	//Power Management. Typical values:0x00(run mode)
#define	PWR_MGMT_2		0x6C

#define	WHO_AM_I		  0x75	//identity of the device


#define	GYRO_ADDRESS   0xD0	  //Gyro and Accel device address
#define MAG_ADDRESS    0x18   //compass device address
#define ACCEL_ADDRESS  0xD0

#define ADDRESS_AD0_LOW     0xD0 //address pin low (GND), default for InvenSense evaluation board
#define ADDRESS_AD0_HIGH    0xD1 //address pin high (VCC)
#define DEFAULT_ADDRESS     GYRO_ADDRESS
#define WHO_AM_I_VAL				0x73 //identity of MPU9250 is 0x71. identity of MPU9255 is 0x73.

#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define mpu9255ID 0x68


int main()
{
    uint8 tempHValue = 0;
    uint8 tempLValue = 0;
    int16 xAxisAccelValue = 0;
    int16 yAxisAccelValue = 0;
    int16 zAxisAccelValue = 0;

    float xAxisAccel = 0.0;
    float yAxisAccel = 0.0;
    float zAxisAccel = 0.0;

    int16 xAxisGyroValue = 0;
    int16 yAxisGyroValue = 0;
    int16 zAxisGyroValue = 0;

    float xAxisGyro = 0.0;
    float yAxisGyro = 0.0;
    float zAxisGyro = 0.0;

    int16 xAxisMagnetoValue = 0;
    int16 yAxisMagnetoValue = 0;
    int16 zAxisMagnetoValue = 0;

    I2COperationStruct *i2cOperation = NULL;
    int32 fd = -1;
    int32 createI2c = 0;
    I2CDeviceDesc i2cDevDesc;

    //struct timeval tv;

    Kalman cKalmanFilter;
    struct timeval sTimeVal;
    ulong currentMilliSecond = 0,milliSecondBackup = 0;


    printf("Hello, World!\n");
    printf("i2cOperation = %p \n",i2cOperation);


    i2cDevDesc.deviceId = I2CDevice_1;

    createI2c = createI2COperation(&i2cDevDesc, &i2cOperation);
    printf("i2cOperation = %p \n",i2cOperation);
    if (NULL == i2cOperation)
    {
        printf("i2cOperation is NULL \n");
        return 0;
    }

    i2cOperation->open(i2cOperation);
    i2cOperation->writeReg16(i2cOperation, mpu9255ID, PWR_MGMT_1, 0x10);
    i2cOperation->writeReg16(i2cOperation, mpu9255ID, PWR_MGMT_1, 0x00);
    i2cOperation->writeReg16(i2cOperation, mpu9255ID, PWR_MGMT_2, 0x00);
    i2cOperation->writeReg16(i2cOperation, mpu9255ID, SMPLRT_DIV, 0x07);
    i2cOperation->writeReg16(i2cOperation, mpu9255ID, CONFIG, 0x06);
    i2cOperation->writeReg16(i2cOperation, mpu9255ID, GYRO_CONFIG, 0x10);
    i2cOperation->writeReg16(i2cOperation, mpu9255ID, ACCEL_CONFIG, 0x00);


    usleep(10*1000);   //延时10ms

    int32 whoami = i2cOperation->readReg8(i2cOperation, mpu9255ID, 0x75);
    printf("whoami = 0x%x \n",whoami);
    gettimeofday(&sTimeVal, NULL);
    currentMilliSecond = milliSecondBackup = (ulong)(sTimeVal.tv_sec*1000 + sTimeVal.tv_usec/1000);
    while(1)
    {
        tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, ACCEL_XOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, ACCEL_XOUT_H);
        xAxisAccelValue = (tempHValue << 8) + tempLValue;
        xAxisAccel = (float)(xAxisAccelValue/16384.0);
       // printf("xAxisAccelValue = %f \n",xAxisAccel);

        tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, ACCEL_YOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, ACCEL_YOUT_H);
        yAxisAccelValue = (tempHValue << 8) + tempLValue;
        yAxisAccel = (float)(yAxisAccelValue/16384.0);
        //printf("yAxisAccelValue = %f \n",yAxisAccel);

        tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, ACCEL_ZOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, ACCEL_ZOUT_H);
        zAxisAccelValue = (tempHValue << 8) + tempLValue;
        zAxisAccel = (float)(zAxisAccelValue/16384.0);
       // printf("zAxisAccelValue = %f \n",zAxisAccel);
        double xAngle = atan(xAxisAccel/zAxisAccel)*180.0/3.1416;
        printf("xAngle = %f \n", xAngle);
        double yAngle = atan(yAxisAccel/zAxisAccel)*180.0/3.1416;
        printf("yAngle = %f \n", yAngle);
        //printf("---------------------\n");
        //cKalmanFilter.setAngle(xAngle);

        tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, GYRO_XOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, GYRO_XOUT_H);
        xAxisGyroValue = (tempHValue << 8) + tempLValue;
        xAxisGyro = (float)(xAxisGyroValue/32.8);
        printf("xAxisGyroValue = %f \n",xAxisGyro);

        tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, GYRO_YOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, GYRO_YOUT_H);
        yAxisGyroValue = (tempHValue << 8) + tempLValue;
        yAxisGyro = (float)(yAxisGyroValue/32.8);
        printf("yAxisGyroValue = %f \n",yAxisGyro);

        /*tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, GYRO_ZOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, GYRO_ZOUT_H);
        zAxisGyroValue = (tempHValue << 8) + tempLValue;
        zAxisGyro = (float)(zAxisGyroValue/32.8);
        printf("zAxisGyroValue = %f \n",zAxisGyro);
        printf("---------------------\n");

        tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, MAG_XOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, MAG_XOUT_H);
        xAxisMagnetoValue = (tempHValue << 8) + tempLValue;
        printf("xAxisGyroValue = 0x%d \n",xAxisMagnetoValue);

        tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, MAG_YOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, MAG_YOUT_H);
        yAxisMagnetoValue = (tempHValue << 8) + tempLValue;
        printf("yAxisGyroValue = 0x%d \n",yAxisMagnetoValue);

        tempLValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, MAG_ZOUT_L);
        tempHValue = (unsigned char)i2cOperation->readReg8(i2cOperation, mpu9255ID, MAG_ZOUT_H);
        zAxisMagnetoValue = (tempHValue << 8) + tempLValue;
        printf("zAxisGyroValue = 0x%d \n",zAxisMagnetoValue);
        printf("---------------------\n");
*/
        gettimeofday(&sTimeVal, NULL);
        currentMilliSecond = (ulong)(sTimeVal.tv_sec*1000 + sTimeVal.tv_usec/1000);
        printf("currentMilliSecond = %ld \n", currentMilliSecond);
        printf("milliSecondBackup = %ld \n",milliSecondBackup);
        printf("currentMilliSecond - milliSecondBackup = %ld \n", currentMilliSecond - milliSecondBackup);
        currentMilliSecond = currentMilliSecond - milliSecondBackup;
        float xAxisGyroAngle = xAxisGyro*currentMilliSecond;
        float kalmanXAngle = cKalmanFilter.getAngle(xAngle, xAxisGyroAngle, currentMilliSecond);
        printf("kalmanXAngle = %f \n",kalmanXAngle);
        printf("---------------------\n");
        usleep(1000*10);
    }



    return 0;
}