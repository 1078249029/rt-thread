/*
 * Copyright (c) 2021-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2022-04-24     supperthomas  first version
 * 2022-06-02     supperthomas  fix version
 * 2023-10-20     WCX1024979076 add wifi application
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "/home/lzx/rt-thread/rt-thread/bsp/ESP32_C3/packages/ESP-IDF-latest/tools/mocks/hal/include/hal/gpio_types.h"
#include "/home/lzx/rt-thread/rt-thread/bsp/ESP32_C3/drivers/drv_spi.h"

#define W25Q_SPI_DEVICE_NAME "spi20"
#define SPI_BUS_NAME "spi2"

void spi_w25q_sample(void)
{
    rt_hw_spi_device_attach(SPI_BUS_NAME, W25Q_SPI_DEVICE_NAME, GPIO_NUM_10 );

    struct rt_spi_device *spi_w25q;
    struct rt_spi_configuration cfg;
    
    rt_uint8_t w25x_read_id[5] = {0x9f, 0xff, 0xff, 0x00, 0xff};
    rt_uint8_t id[5] = {0x00,0x00,0x00,0x00,0x00};

    spi_w25q = (struct rt_spi_device*)rt_device_find(W25Q_SPI_DEVICE_NAME);
    if(!spi_w25q)
    {
        rt_kprintf("spi asmple run failed! can't find %s device\n",W25Q_SPI_DEVICE_NAME);
    }
    else 
    {    
        cfg.max_hz = 0;
        cfg.data_width = 7;
        cfg.mode = RT_SPI_MODE_0;
        rt_spi_configure(spi_w25q, &cfg);

        struct rt_spi_message msg1;

        msg1.send_buf   = &w25x_read_id;
        msg1.recv_buf   = &id;
        msg1.length     = 7;//
        msg1.cs_take    = 1;
        msg1.cs_release = 0;
        msg1.next       = RT_NULL;

        rt_spi_transfer_message(spi_w25q, &msg1);
        rt_kprintf("use rt_spi_transfer_message() read w25q ID is:%x%x\n", id[2], id[3]);
    }
    for(int a = 0; a < sizeof(id); a++)
    {
        rt_kprintf("id is:%x\t", id[a]);
    }
}
MSH_CMD_EXPORT(spi_w25q_sample, spi w25q sample);

/*
 *todo: todo: iic read data by mpu6050
 *
 */

void read_mpu6050_sample(void)
{

}
MSH_CMD_EXPORT(read_mpu6050_sample, read mpu6050 sample);

#ifdef BSP_USING_BLE
void app_main()
{
    while(1)
    {

    }
}
#endif /* BSP_USING_BLE */

// #define USING_WIFI_IN_MAIN

void test(void);

int main(void)
{
    rt_kprintf("%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
    test();
    rt_kprintf("%s, %s, %d\n", __FILE__, __FUNCTION__, __LINE__);
    int kdb_sys_init(void);
    void kdb_sys_start(void);
    rt_kprintf("Hello!RT-THREAD!\r\n");
    rt_pin_mode(RT_BSP_LED_PIN, PIN_MODE_OUTPUT);

#ifdef BSP_USING_BLE
    extern void app_main(void);
    esp_timer_init();
    app_main(); //该函数为nimble样例程序入口
#endif /* BSP_USING_BLE */

#if defined (RT_USING_WIFI) && defined (USING_WIFI_IN_MAIN)
    /* set wifi work mode */
    rt_wlan_set_mode(RT_WLAN_DEVICE_STA_NAME, RT_WLAN_STATION);
    rt_wlan_set_mode(RT_WLAN_DEVICE_AP_NAME, RT_WLAN_AP);
#endif /* RT_USING_WIFI */

    while (1)
    {
        rt_pin_write(RT_BSP_LED_PIN, PIN_HIGH);
        rt_thread_mdelay(1000);
        rt_pin_write(RT_BSP_LED_PIN, PIN_LOW);
        rt_thread_mdelay(1000);
    }
}
