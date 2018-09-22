//
// Created by BaoPuCao on 2018/9/1.
//create i2c operation struct
//

#ifndef __RASPBERRYPII2C_H__
#define __RASPBERRYPII2C_H__
#ifdef __cplusplus
extern "C"{
#endif

#include "def.h"
#include <pthread.h>

/*
///设置I2C属性
typedef struct I2CAttributeStruct
{
    int I2CDeviceId;
    int I2CRetries;         ///接收不到ACK是的重试次数
    int I2CTimeout;         ///设置超时时限的jiffies
    int I2CSlave;           ///设置从机地址
    int I2CSlaveForce;      ///强制设置从机地址
    int I2CTenbit;          ///选择地址位长 : 0 表示7bit长度，!=0表示10bit
    int I2CFuncs;           ///获取适配支持的功能
    int I2CRDWR;            ///支持读写传输
    int I2CPEC;             ///!=0 使用PEC SMBus
    int I2CSMBUS;           ///SMBus传输
    int reserved[54];       ///保留
}I2CAttributeStruct;

typedef struct I2CMassage
{
    int addr;           ///设备从地址
    int flags;          ///通讯标准，发送数据为0,接收数据为I2C_M_RD
    int len;            ///数据长度
    int *buff;           ///传输数据
}I2CMassage;
*/
typedef struct I2CDeviceDesc
{
    int32 deviceId;
    int32 reserved[31];
}I2CDeviceDesc;

typedef struct I2COperationPriv
{
    int32 fd;
    pthread_mutex_t *pI2CMutex;
    int32 I2CDeviceId;
    int32 reserved[29];
}I2COperationPriv;

#define MAX_I2C_BUS_NUM 5
typedef enum I2CDeviceList
{
    I2CDevice_0 = 0,
    I2CDevice_1,
    I2CDevice_2,
    I2CDevice_3,
    I2CDevice_4
}I2CDeviceList;



typedef struct I2COperationStruct
{
    void *priv;  //

    int32 (*addReference)(struct I2COperationStruct *thiz);

    int32 (*release)(struct I2COperationStruct *thiz);

    int32 (*open)(struct I2COperationStruct *thiz);

    int32 (*close)(struct I2COperationStruct *thiz);

    int32 (*readReg8)(struct I2COperationStruct *thiz, int32 addr, int32 reg);

    int32 (*readReg16)(struct I2COperationStruct *thiz, int32 addr, int32 reg);

    int32 (*writeReg8)(struct I2COperationStruct *thiz, int32 addr, int32 reg, int32 value);

    int32 (*writeReg16)(struct I2COperationStruct *thiz, int32 addr, int32 reg, int32 value);

    int32 (*purge)(struct I2COperationStruct *thiz);

    int32 *reserved[55];
}I2COperationStruct;

int createI2COperation(I2CDeviceDesc *i2cDevDesc, I2COperationStruct **i2cOperationStruct);

#ifdef __cplusplus
}
#endif

#endif //__RASPBERRYPII2C_H__
