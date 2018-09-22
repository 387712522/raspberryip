//
// Created by BaoPuCao on 2018/9/1.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include "../include/raspberrypii2c.h"



static I2COperationStruct* createI2COperationStruct[MAX_I2C_BUS_NUM] = {NULL,};
const static char* I2CBusDev[5] = {"/dev/i2c-0", "/dev/i2c-1", "/dev/i2c-2", "/dev/i2c-3", "/dev/i2c-4"};




static inline int32 i2cSMBusAccess (int32 fd, uint8 rw, uint8 command, uint32 size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args;

    args.read_write = rw;
    args.command    = command;
    args.size       = size;
    args.data       = data;
    return ioctl (fd, I2C_SMBUS, &args);
}

int32 I2CAddReference(I2COperationStruct *thiz)
{

    return true;
}

int32 I2CClose(I2COperationStruct *thiz)
{
    I2COperationPriv *pI2COperationPriv = NULL;

    int I2CDeviceId = -1;

    if (NULL == thiz)
    {
        printf("I2CClose Operation Failed , Thiz is NULL \n");
        return -1;
    }

    if (NULL == thiz->priv)
    {
        printf("I2CClose Operation Failed ,thiz->priv is NULL \n");
        return -1;
    }

    pI2COperationPriv = (I2COperationPriv*)thiz->priv;

    if (pI2COperationPriv->fd < 0)
    {
        printf("I2CClose Operation Failed , pI2COperationPriv->fd < 0 \n");
        return -1;
    }

    I2CDeviceId = pI2COperationPriv->I2CDeviceId;
    if ((I2CDeviceId > 4) ||(I2CDeviceId < 0))
    {
        printf("I2CClose Operation Failed, I2CDeviceId is undefined, I2CDeviceId = %d.\n",I2CDeviceId);
        return -1;
    }

    close(pI2COperationPriv->fd);

    if (NULL != pI2COperationPriv->pI2CMutex)
    {
        free(pI2COperationPriv->pI2CMutex);
        pI2COperationPriv->pI2CMutex = NULL;
    }

    free(pI2COperationPriv);
    pI2COperationPriv = NULL;


    free(createI2COperationStruct[I2CDeviceId]);
    createI2COperationStruct[I2CDeviceId] = NULL;

    return true;
}

int32 I2COpen(I2COperationStruct *thiz)
{
    int32 fd = -1;
    I2COperationPriv *pI2COperationPriv = NULL;

    if (NULL == thiz)
    {
        printf("I2c Open Failed, thiz is NULL \n");
        return -1;
    }

    if (NULL == thiz->priv)
    {
        printf("I2C Open Failed, thiz->priv is NULL \n");
        return -1;
    }

    pI2COperationPriv = (I2COperationPriv*)thiz->priv;

    fd = open(I2CBusDev[pI2COperationPriv->I2CDeviceId],O_RDWR);
    //printf("fd = %d \n",fd);
    if (fd < 0)
    {
        printf("I2C device open Failed, fd = %d\n",fd);
        return -1;
    }

    pI2COperationPriv->fd = fd;
    return fd;
}

int32 I2CReadReg8(I2COperationStruct *thiz, int32 addr, int32 reg)
{
    I2COperationPriv *pI2COperationPriv = NULL;
    union i2c_smbus_data data;

    if (NULL == thiz)
    {
        printf("I2CReadReg8 I2COperationStruct is NULL \n");
        return false;
    }

    pI2COperationPriv = (I2COperationPriv*)thiz->priv;
    if (NULL == pI2COperationPriv)
    {
        printf("I2CReadReg8 I2COperationPriv is NULL \n");
        return -1;
    }

    if (pI2COperationPriv->fd <= 0)
    {
        printf("I2CReadReg8 I2COperationPriv fd is NULL \n");
        return -1;
    }

    if (ioctl(pI2COperationPriv->fd, I2C_SLAVE, addr) < 0)
    {
        printf("I2CReadReg8 mpu9255ID set failed \n");
        return -1;
    }

    if (i2cSMBusAccess (pI2COperationPriv->fd, I2C_SMBUS_READ, (uint8)reg, I2C_SMBUS_BYTE_DATA, &data))
        return -1;
    else
        return data.byte & 0xFF;
}


int32 I2CReadReg16(I2COperationStruct *thiz, int32 addr, int32 reg)
{
    I2COperationPriv *pI2COperationPriv = NULL;
    union i2c_smbus_data data;

    if (NULL == thiz)
    {
        printf("I2CReadReg16 I2COperationStruct is NULL \n");
        return -1;
    }

    pI2COperationPriv = (I2COperationPriv*)thiz->priv;
    if (NULL == pI2COperationPriv)
    {
        printf("I2CReadReg16 I2COperationPriv is NULL \n");
        return -1;
    }

    if (pI2COperationPriv->fd <= 0)
    {
        printf("I2CReadReg16 I2COperationPriv fd is NULL \n");
        return -1;
    }

    if (ioctl(pI2COperationPriv->fd, I2C_SLAVE, addr) < 0)
    {
        printf("I2CReadReg16 I2C_SLAVE set failed \n");
        return -1;
    }

    if (i2cSMBusAccess(pI2COperationPriv->fd, I2C_SMBUS_READ, reg, I2C_SMBUS_WORD_DATA, &data))
        return -1 ;
    else
        return data.word & 0xFFFF ;

}

int32 I2CWriteReg8(I2COperationStruct *thiz, int32 addr, int32 reg, int32 value)
{
    I2COperationPriv *pI2COperationPriv = NULL;
    union i2c_smbus_data data;

    if (NULL == thiz)
    {
        printf("I2CWriteReg16 I2COperationStruct is NULL \n");
        return -1;
    }

    if (NULL == thiz->priv)
    {
        printf("I2CWriteReg16 I2COperationStruct priv is NULL \n");
        return -1;
    }

    pI2COperationPriv = (I2COperationPriv*)thiz->priv;

    if (pI2COperationPriv->fd < 0)
    {
        printf("I2CWriteReg16 pI2COperationPriv->fd is NULL \n");
        return -1;
    }
   // pthread_mutex_lock(pI2COperationPriv->pI2CMutex);
    if (ioctl(pI2COperationPriv->fd, I2C_SLAVE, addr) < 0)
    {
        printf("I2CWriteReg8 I2C_SLAVE set failed \n");
       // pthread_mutex_unlock(pI2COperationPriv->pI2CMutex);
        return -1;
    }
    data.byte = (uint8)value;
   // pthread_mutex_unlock(pI2COperationPriv->pI2CMutex);
    return i2cSMBusAccess(pI2COperationPriv->fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data) ;
}

int32 I2CWriteReg16(I2COperationStruct *thiz, int32 addr, int32 reg, int32 value)
{

    I2COperationPriv *pI2COperationPriv = NULL;
    union i2c_smbus_data data;

    if (NULL == thiz)
    {
        printf("I2CWriteReg16 I2COperationStruct is NULL \n");
        return -1;
    }

    if (NULL == thiz->priv)
    {
        printf("I2CWriteReg16 I2COperationStruct priv is NULL \n");
        return -1;
    }

    pI2COperationPriv = (I2COperationPriv*)thiz->priv;

    if (pI2COperationPriv->fd < 0)
    {
        printf("I2CWriteReg16 pI2COperationPriv->fd is NULL \n");
        return -1;
    }

    if (ioctl(pI2COperationPriv->fd, I2C_SLAVE, addr) < 0)
    {
        printf("I2CWriteReg16 I2C_SLAVE set failed \n");
        return -1;
    }

    data.word = (uint16)value;
    return i2cSMBusAccess(pI2COperationPriv->fd, I2C_SMBUS_WRITE, reg, I2C_SMBUS_WORD_DATA, &data) ;
}


int32 I2CRelease(I2COperationStruct *thiz)
{
    return true;
}

int32 I2CPurge(I2COperationStruct *thiz)
{
    return true;
}


int32 createI2COperation(I2CDeviceDesc *i2cDevDesc, I2COperationStruct **i2cOperationStruct)
{

    I2COperationPriv *pI2COperationPriv = NULL;

    if ((NULL == i2cDevDesc)&&(i2cDevDesc->deviceId < 0 || i2cDevDesc->deviceId > 4))
    {
        printf("(%s|%s|%d)error:I2CDeviceDesc is NULL or i2cDevDesc->deviceId = %d Nonsupport!! \n",__FILE__,__FUNCTION__,__LINE__,i2cDevDesc->deviceId);
        return -1;
    }

    pI2COperationPriv = (I2COperationPriv*)malloc(sizeof(I2COperationPriv));
    if (NULL == pI2COperationPriv)
    {
        printf("(%s|%s|%d)error:malloc pI2COperationPriv Failed \n",__FILE__,__FUNCTION__,__LINE__);
        goto failedToCreateI2COperation;
    }
    memset(pI2COperationPriv, 0, sizeof(I2COperationPriv));


    pI2COperationPriv->pI2CMutex = malloc(sizeof(pthread_mutex_t));
    if (NULL == pI2COperationPriv->pI2CMutex)
    {
        printf("createI2COperation Operation Failed, pI2COperationPriv->pI2CMutex malloc Failed \n");
        goto failedToCreateI2COperation;
    }
    if (pthread_mutex_init(pI2COperationPriv->pI2CMutex, NULL) < 0)
    {
        printf("pI2COperationPriv->pI2CMutex Init Failed \n");
        goto failedToCreateI2COperation;
    }


    pthread_mutex_lock(pI2COperationPriv->pI2CMutex);

    if (createI2COperationStruct[i2cDevDesc->deviceId] != NULL)
    {
        printf("i2cDevDesc->deviceId = %d is already Created \n");
        *i2cOperationStruct = createI2COperationStruct[i2cDevDesc->deviceId];
        pthread_mutex_unlock(pI2COperationPriv->pI2CMutex);
        goto failedToCreateI2COperation;
    }

    if (NULL == createI2COperationStruct[i2cDevDesc->deviceId])
    {
        createI2COperationStruct[i2cDevDesc->deviceId] = (I2COperationStruct*)malloc(sizeof(I2COperationStruct));
        if (NULL == createI2COperationStruct[i2cDevDesc->deviceId])
        {
            printf("(%s|%s|%d)error:malloc I2COperationStruct Failed \n",__FILE__,__FUNCTION__,__LINE__);
            pthread_mutex_unlock(pI2COperationPriv->pI2CMutex);
            goto failedToCreateI2COperation;
        }
        memset(createI2COperationStruct[i2cDevDesc->deviceId], 0, sizeof(I2COperationStruct));


        printf("pi2cOperationStruct = %p \n",createI2COperationStruct[i2cDevDesc->deviceId]);

        pI2COperationPriv->I2CDeviceId = i2cDevDesc->deviceId;

        createI2COperationStruct[i2cDevDesc->deviceId]->addReference = I2CAddReference;
        createI2COperationStruct[i2cDevDesc->deviceId]->open = I2COpen;
        createI2COperationStruct[i2cDevDesc->deviceId]->close = I2CClose;
        createI2COperationStruct[i2cDevDesc->deviceId]->readReg8 = I2CReadReg8;
        createI2COperationStruct[i2cDevDesc->deviceId]->readReg16 = I2CReadReg16;
        createI2COperationStruct[i2cDevDesc->deviceId]->writeReg8 = I2CWriteReg8;
        createI2COperationStruct[i2cDevDesc->deviceId]->writeReg16 = I2CWriteReg16;
        createI2COperationStruct[i2cDevDesc->deviceId]->release = I2CRelease;
        createI2COperationStruct[i2cDevDesc->deviceId]->purge = I2CPurge;
        createI2COperationStruct[i2cDevDesc->deviceId]->priv = pI2COperationPriv;

        *i2cOperationStruct = createI2COperationStruct[i2cDevDesc->deviceId];

    }
    pthread_mutex_unlock(pI2COperationPriv->pI2CMutex);
    return 0;


failedToCreateI2COperation:
    *i2cOperationStruct = NULL;

    if (NULL != pI2COperationPriv)
    {
        if (NULL == pI2COperationPriv->pI2CMutex)
        {
            pthread_mutex_destroy(pI2COperationPriv->pI2CMutex);
            free(pI2COperationPriv->pI2CMutex);
            pI2COperationPriv->pI2CMutex = NULL;
        }
        free(pI2COperationPriv);
        pI2COperationPriv = NULL;
    }
    if (NULL != createI2COperationStruct[i2cDevDesc->deviceId])
    {
        free(createI2COperationStruct[i2cDevDesc->deviceId]);
        createI2COperationStruct[i2cDevDesc->deviceId] = NULL;
    }
    return -1;
}



