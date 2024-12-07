#ifndef __MPU6050_H
#define __MPU6050_H

#include "rttypes.h"

void MPU6050_Init(void);
void MPU6050_WriteReg(rt_uint8_t RegAddr,rt_uint8_t Data);
rt_uint8_t MPU6050_ReadReg(rt_uint8_t RegAddr);
void MPU6050_GetData(rt_int16_t *AccX,rt_int16_t *AccY,rt_int16_t *AccZ,
						rt_int16_t *GyroX,rt_int16_t *GyroY,rt_int16_t *GyroZ);

#endif
