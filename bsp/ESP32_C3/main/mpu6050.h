#ifndef __MPU6050_H
#define __MPU6050_H

#include "rttypes.h"
#include <rtdevice.h>

struct rt_i2c_bus_device * mpu6050_init(const char *name);
void read_id(struct rt_i2c_bus_device *i2c_bus, rt_uint8_t* buf);
void read_data(struct rt_i2c_bus_device *i2c_bus, rt_int16_t *AccX,rt_int16_t *AccY,rt_int16_t *AccZ,
			    rt_int16_t *GyroX,rt_int16_t *GyroY,rt_int16_t *GyroZ);
void read_signed_data(struct rt_i2c_bus_device *i2c_bus, float *data);
	
#endif
