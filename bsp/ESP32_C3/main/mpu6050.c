/*
 * 程序清单：这是一个 I2C 设备使用例程
 * 例程导出了 i2c_aht10_sample 命令到控制终端
 * 命令调用格式：i2c_aht10_sample i2c1
 * 命令解释：命令第二个参数是要使用的I2C总线设备名称，为空则使用默认的I2C总线设备
 * 程序功能：通过 I2C 设备读取温湿度传感器 aht10 的温湿度数据并打印
*/
#define DBG_TAG "xxx"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <rtdbg.h>

#include "mpu6050_reg.h"
#include "rttypes.h"

#include <math.h>

//result = i2c_master_write_byte(cmd, msg->addr << 1 | WRITE_BIT, ACK_CHECK_EN);
#define MPU6050_ADDR	0x68 //不是 0xD0

#define MPU6050_I2C_BUS_NAME          "i2c0"  /* 传感器连接的I2C总线设备名称 */
#define AHT10_ADDR                  0x38    /* 从机地址 */
#define AHT10_CALIBRATION_CMD       0xE1    /* 校准命令 */
#define AHT10_NORMAL_CMD            0xA8    /* 一般命令 */
#define AHT10_GET_DATA              0xAC    /* 获取数据命令 */

static struct rt_i2c_bus_device *i2c_bus = RT_NULL;     /* I2C总线设备句柄 */
static rt_bool_t initialized = RT_FALSE;                /* 传感器初始化状态 */

/* 写传感器寄存器 */
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data)
{
    rt_uint8_t buf[2] = {0};
    struct rt_i2c_msg msgs;
    rt_uint32_t buf_size = 1;

    buf[0] = reg; //cmd == reg_addr
    if (data != RT_NULL)
    {
        buf[1] = data[0]; //send/receive data
        //buf[2] = data[1];
        buf_size = 2;
    }

    msgs.addr = MPU6050_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = buf_size;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {    
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

/* 读传感器寄存器数据 */
static rt_err_t read_reg(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs;

    msgs.addr = MPU6050_ADDR;
    msgs.flags = RT_I2C_RD;
    msgs.buf = buf;
    msgs.len = len;

    /* 调用I2C设备接口传输数据 */
    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
    rt_kprintf("In function %s, line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);
}

void read_id(rt_uint8_t* buf)
{
    write_reg(i2c_bus,MPU6050_WHO_AM_I,RT_NULL);
	read_reg(i2c_bus, 1, buf);
}


void read_data(rt_int16_t *AccX,rt_int16_t *AccY,rt_int16_t *AccZ,
			    rt_int16_t *GyroX,rt_int16_t *GyroY,rt_int16_t *GyroZ)
{
    rt_uint8_t DataH,DataL;
	write_reg(i2c_bus, MPU6050_ACCEL_XOUT_H,RT_NULL);
    read_reg(i2c_bus, sizeof(DataH),&DataH);
	write_reg(i2c_bus, MPU6050_ACCEL_XOUT_L,RT_NULL);
    read_reg(i2c_bus, sizeof(DataL),&DataL);
	*AccX = (DataH << 8) | DataL;

    write_reg(i2c_bus, MPU6050_ACCEL_YOUT_H,RT_NULL);
    read_reg(i2c_bus, sizeof(DataH),&DataH);
	write_reg(i2c_bus, MPU6050_ACCEL_YOUT_L,RT_NULL);
    read_reg(i2c_bus, sizeof(DataL),&DataL);
	*AccY = (DataH << 8) | DataL;

    write_reg(i2c_bus, MPU6050_ACCEL_ZOUT_H,RT_NULL);
    read_reg(i2c_bus, sizeof(DataH),&DataH);
	write_reg(i2c_bus, MPU6050_ACCEL_ZOUT_L,RT_NULL);
    read_reg(i2c_bus, sizeof(DataL),&DataL);
	*AccZ = (DataH << 8) | DataL;

    write_reg(i2c_bus, MPU6050_GYRO_XOUT_H,RT_NULL);
    read_reg(i2c_bus, sizeof(DataH),&DataH);
	write_reg(i2c_bus, MPU6050_GYRO_XOUT_L,RT_NULL);
    read_reg(i2c_bus, sizeof(DataL),&DataL);
	*GyroX = (DataH << 8) | DataL;

    write_reg(i2c_bus, MPU6050_GYRO_YOUT_H,RT_NULL);
    read_reg(i2c_bus, sizeof(DataH),&DataH);
	write_reg(i2c_bus, MPU6050_GYRO_YOUT_L,RT_NULL);
    read_reg(i2c_bus, sizeof(DataL),&DataL);
	*GyroY = (DataH << 8) | DataL;

    write_reg(i2c_bus, MPU6050_GYRO_ZOUT_H,RT_NULL);
    read_reg(i2c_bus, sizeof(DataH),&DataH);
	write_reg(i2c_bus, MPU6050_GYRO_ZOUT_L,RT_NULL);
    read_reg(i2c_bus, sizeof(DataL),&DataL);
	*GyroZ = (DataH << 8) | DataL;
}

void read_signed_data(float *data)
{
    //1. 原始数据获取
	float accx1,accy1,accz1;//三方向角加速度值
    float accx2,accy2,accz2;//三方向角加速度值
    rt_int16_t raw_data1[6];
    rt_int16_t raw_data2[6];

	//获取加速度传感器数据
    read_data(&raw_data1[0], &raw_data1[1], &raw_data1[2], &raw_data1[3], &raw_data1[4], &raw_data1[5]);
    read_data(&raw_data2[0], &raw_data2[1], &raw_data2[2], &raw_data2[3], &raw_data2[4], &raw_data2[5]);

	float accel1_x = raw_data1[0];//x轴加速度值暂存
	float accel1_y = raw_data1[1];//y轴加速度值暂存
	float accel1_z = raw_data1[2];//z轴加速度值暂存
	float gyro1_x  = raw_data1[3];//x轴陀螺仪值暂存
	float gyro1_y  = raw_data1[4];//y轴陀螺仪值暂存
	float gyro1_z  = raw_data1[5];//z轴陀螺仪值暂存
    float accel2_x = raw_data2[0];//x轴加速度值暂存
	float accel2_y = raw_data2[1];//y轴加速度值暂存
	float accel2_z = raw_data2[2];//z轴加速度值暂存
	float gyro2_x  = raw_data2[3];//x轴陀螺仪值暂存
	float gyro2_y  = raw_data2[4];//y轴陀螺仪值暂存
	float gyro2_z  = raw_data2[5];//z轴陀螺仪值暂存

	//2.角加速度原始值处理过程	
	//加速度传感器配置寄存器0X1C内写入0x01,设置范围为±16g。换算关系：2^16/16 = 2048LSB/g
	if(accel1_x<32764) accx1=accel1_x/2048.0;//计算x轴加速度
	else               accx1=1-(accel1_x-65536)/2048.0;
	if(accel1_y<32764) accy1=accel1_y/2048.0;//计算y轴加速度
	else               accy1=1-(accel1_y-65536)/2048.0;
	if(accel1_z<32764) accz1=accel1_z/2048.0;//计算z轴加速度
	else               accz1=(accel1_z-65536)/2048.0;
	
    if(accel2_x<32764) accx2=accel2_x/2048.0;//计算x轴加速度
	else               accx2=1-(accel2_x-65536)/2048.0;
	if(accel2_y<32764) accy2=accel2_y/2048.0;//计算y轴加速度
	else               accy2=1-(accel2_y-65536)/2048.0;
	if(accel2_z<32764) accz2=accel2_z/2048.0;//计算z轴加速度
	else               accz2=(accel2_z-65536)/2048.0;

	//3.角速度原始值处理过程
	//陀螺仪配置寄存器0X1B内写入0x18，设置范围为±2000deg/s。换算关系：2^16/4000=16.4LSB/(°/S)
	////计算角速度
	if(gyro1_x<32768) gyro1_x=-(gyro1_x/16.4);
	if(gyro1_x>32768) gyro1_x=+(65535-gyro1_x)/16.4;
	if(gyro1_y<32768) gyro1_y=-(gyro1_y/16.4);
	if(gyro1_y>32768) gyro1_y=+(65535-gyro1_y)/16.4;
	if(gyro1_z<32768) gyro1_z=-(gyro1_z/16.4);
	if(gyro1_z>32768) gyro1_z=+(65535-gyro1_z)/16.4;


    if(gyro2_x<32768) gyro2_x=-(gyro2_x/16.4);
	if(gyro2_x>32768) gyro2_x=+(65535-gyro2_x)/16.4;
	if(gyro2_y<32768) gyro2_y=-(gyro2_y/16.4);
	if(gyro2_y>32768) gyro2_y=+(65535-gyro2_y)/16.4;
	if(gyro2_z<32768) gyro2_z=-(gyro2_z/16.4);
	if(gyro2_z>32768) gyro2_z=+(65535-gyro2_z)/16.4;

    data[0] = accx1;
    data[1] = accy1;
    data[2] = accz1;
    data[3] = gyro1_x;
    data[4] = gyro1_y;
    data[5] = gyro1_z;
    data[6] = accx2;
    data[7] = accy2;
    data[8] = accz2;
    data[9] = gyro2_x;
    data[10] = gyro2_y;
    data[11] = gyro2_z;
}

void mpu6050_init(const char *name)
{
    rt_uint8_t temp[2] = {0, 0};

    /* 查找I2C总线设备，获取I2C总线设备句柄 */
    i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(name);

    if (i2c_bus == RT_NULL)
    {
        rt_kprintf("can't find %s device!\n", name);
    }
    else
    {
        rt_uint8_t data = 0x01;
        write_reg(i2c_bus,MPU6050_PWR_MGMT_1,&data);
        data = 0x00;
        write_reg(i2c_bus,MPU6050_PWR_MGMT_2,&data);
        data = 0x09;
        write_reg(i2c_bus,MPU6050_SMPLRT_DIV,&data);
        data = 0x06;
        write_reg(i2c_bus,MPU6050_CONFIG,&data);
        data = 0x18;    //range:± 2000 °/s sensitivity:16.4 LSB/°/s
        write_reg(i2c_bus,MPU6050_GYRO_CONFIG,&data);
        data = 0x18;    //range:± 16g
        write_reg(i2c_bus,MPU6050_ACCEL_CONFIG,&data); 

        initialized = RT_TRUE;
    }
}

static void i2c_mpu6050_sample(int argc, char *argv[])
{
    rt_int16_t raw_AX,raw_AY,raw_AZ,raw_GX,raw_GY,raw_GZ;
    char name[RT_NAME_MAX];
    rt_uint8_t buf;
    if (argc == 2)
    {
        rt_strncpy(name, argv[1], RT_NAME_MAX);
    }
    else
    {
        rt_strncpy(name, MPU6050_I2C_BUS_NAME, RT_NAME_MAX);
    }
    rt_base_t level = rt_hw_interrupt_disable();
    if (!initialized)
    {
        /* 传感器初始化 */
        mpu6050_init(name);
    }
    if (initialized)
    {
        read_id(&buf);

        /* 读取温湿度数据 */
        read_data(&raw_AX,&raw_AY,&raw_AZ,\
            &raw_GX,&raw_GY,&raw_GZ);

        rt_kprintf("Ax:%5d\tAY:%5d\tAZ:%5d\r\nGx:%5d\tGY:%5d\tGZ:%5d\r\n", raw_AX,raw_AY,raw_AZ,raw_GX,raw_GY,raw_GZ);
        rt_kprintf("id is %x\n", buf);
    }
    else
    {
        rt_kprintf("initialize sensor failed!\n");
    }
    rt_hw_interrupt_enable(level);
    rt_kprintf("In function %s, line %d, file: %s\n", __FUNCTION__, __LINE__, __FILE__);
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(i2c_mpu6050_sample, i2c mpu6050 sample);
