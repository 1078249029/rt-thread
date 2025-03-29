#include "klibc/kstring.h"
#include "rthw.h"
#include "rttypes.h"
#include <stdlib.h>
#include <stdio.h>
//#include <string.h>
#include <rtthread.h>
#include <at.h>
#include <rtdbg.h>

#include "/home/lzx/rt-thread/rt-thread/components/libc/compilers/common/include/posix/string.h"
#include "mpu6050.h"


int at_cwmode(rt_int8_t mode)
{
    at_response_t resp = RT_NULL;
    int result = 0;
    resp = at_create_resp(256, 0, rt_tick_from_millisecond(10000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }

    switch (mode) {
        case 1:
        {
            result = at_exec_cmd(resp, "AT+CWMODE=1");
            if (result != RT_EOK)
            {
                LOG_E("at_cwmode=1 error!");
                goto __exit;
            }
            break;
        }
        case 2:
        {
            result = at_exec_cmd(resp, "AT+CWMODE=2");
            if (result != RT_EOK)
            {
                LOG_E("at_cwmode=2 error!");
                goto __exit;
            }
            break;
        }

        case 3:
        {
            result = at_exec_cmd(resp, "AT+CWMODE=3");
            if (result != RT_EOK)
            {
                LOG_E("at_cwmode=3 error!,error code = %d",result);
                goto __exit;
            }
            break;
        }
    }
    
    char resp_arg[256] = { 0 };
    const char * resp_expr = "%*[^\"]\"%[^\"]\"";

    LOG_D("Parse arguments");
    if (at_resp_parse_line_args(resp, 1, resp_expr, resp_arg) > 0)
    {
        LOG_D("AT+CWMODE  : %s", resp_arg);
        memset(resp_arg, 0x00, 256);
    }
    else
    {
        LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, 1));
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

int at_cwjap(void)
{
    at_response_t resp = RT_NULL;
    int result = 0;
    resp = at_create_resp(256, 0, rt_tick_from_millisecond(10000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }

    result = at_exec_cmd(resp, "AT+CWJAP=\"rtthread\",\"12345678\"");
    if (result != RT_EOK)
    {
        LOG_E("at_cwjap error!");
        goto __exit;
    }

    char resp_arg[256] = { 0 };
    const char * resp_expr = "%*[^\"]\"%[^\"]\"";

    LOG_D("Parse arguments");
    for(int i = 1; i < 4; i++){
        if (at_resp_parse_line_args(resp, i, resp_expr, resp_arg) > 0)
        {
            LOG_D("%s", resp_arg);
            memset(resp_arg, 0x00, 256);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, i));
        }
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

int at_cifsr(void)
{
    at_response_t resp = RT_NULL;
    int result = 0;
    resp = at_create_resp(256, 0, rt_tick_from_millisecond(5000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }

    result = at_exec_cmd(resp, "AT+CIFSR");
    if (result != RT_EOK)
    {
        LOG_E("at_cifsr error!");
        goto __exit;
    }

    char resp_arg[256] = { 0 };
    const char * resp_expr = "%*[^\"]\"%[^\"]\"";

    LOG_D("Parse arguments");
    for(int i = 1; i < 7; i++){
        if (at_resp_parse_line_args(resp, i, resp_expr, resp_arg) > 0)
        {
            LOG_D("%s", resp_arg);
            memset(resp_arg, 0x00, 256);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, i));
        }
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

int at_cipstart(char* protol, char* ip, char* port)
{
    at_response_t resp = RT_NULL;
    int result = 0;
    resp = at_create_resp(256, 0, rt_tick_from_millisecond(5000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }

    char dest[128] = {"AT+CIPSTART="};
    char separate[]={","};
    strcat(dest, protol);
    strcat(dest, separate);
    strcat(dest, ip);
    strcat(dest, separate);
    strcat(dest, port);

    result = at_exec_cmd(resp, dest);
    if (result != RT_EOK)
    {
        LOG_E("at_cipstart error!");
        goto __exit;
    }

    char resp_arg[256] = { 0 };
    const char * resp_expr = "%*[^\"]\"%[^\"]\"";

    LOG_D("Parse arguments");
    for(int i = 1; i < 2; i++){
        if (at_resp_parse_line_args(resp, i, resp_expr, resp_arg) > 0)
        {
            LOG_D("%s %s %s: %s", protol, ip, port,resp_arg);
            memset(resp_arg, 0x00, 256);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, i));
        }
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

int at_cipsend(void)
{
    at_response_t resp = RT_NULL;
    int result = 0;
    resp = at_create_resp(256, 0, rt_tick_from_millisecond(5000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }
    
    result = at_exec_cmd(resp, "AT+CIPSEND");
    if (result != RT_EOK)
    {
        LOG_E("at_cipsend error!");
        goto __exit;
    }

    char resp_arg[256] = { 0 };
    const char * resp_expr = "%*[^\"]\"%[^\"]\"";

    LOG_D("Parse arguments");
    for(int i = 1; i < 2; i++){
        if (at_resp_parse_line_args(resp, i, resp_expr, resp_arg) > 0)
        {
            LOG_D("at_cipsend: %s",resp_arg);
            memset(resp_arg, 0x00, 256);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, i));
        }
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

int at_senddata(char* data)
{
    at_response_t resp = RT_NULL;
    int result = 0;

    if(sizeof(data) > 1460)
    {
        LOG_E("Data lenth over limit!");
        goto __exit;
    }

    resp = at_create_resp(256, 0, rt_tick_from_millisecond(5000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }
    
    result = at_exec_cmd(resp, data);
    if (result != RT_EOK)
    {
        LOG_E("at_senddata error!, error code = %d",result);
        goto __exit;
    }

    char resp_arg[256] = { 0 };
    const char * resp_expr = "%*[^\"]\"%[^\"]\"";

    LOG_D("Parse arguments");
    for(int i = 1; i < 2; i++){
        if (at_resp_parse_line_args(resp, i, resp_expr, resp_arg) > 0)
        {
            LOG_D("at_senddata: %s",resp_arg);
            memset(resp_arg, 0x00, 256);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, i));
        }
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

int at_stopsend(void)
{
    at_response_t resp = RT_NULL;
    int result = 0;

    resp = at_create_resp(256, 0, rt_tick_from_millisecond(5000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }
    
    result = at_exec_cmd_noln(resp, "+++");
    if (result != RT_EOK)
    {
        LOG_E("at_stopsend error!");
        goto __exit;
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

int at_cipmode(rt_int8_t mode)
{
    at_response_t resp = RT_NULL;
    int result = 0;
    resp = at_create_resp(256, 0, rt_tick_from_millisecond(5000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }

    switch (mode) {
        case 0:
        {
            result = at_exec_cmd(resp, "AT+CIPMODE=0");
            if (result != RT_EOK)
            {
                LOG_E("at_cipmode=0 error!");
                goto __exit;
            }
            break;
        }

        case 1:
        {
            result = at_exec_cmd(resp, "AT+CIPMODE=1");
            if (result != RT_EOK)
            {
                LOG_E("at_cipmode=1 error!");
                goto __exit;
            }
            break;
        }
    }
    
    char resp_arg[256] = { 0 };
    const char * resp_expr = "%*[^\"]\"%[^\"]\"";

    LOG_D("Parse arguments");
    for(int i = 1; i < 3; i++){
        if (at_resp_parse_line_args(resp, i, resp_expr, resp_arg) > 0)
        {
            LOG_D("%s", resp_arg);
            memset(resp_arg, 0x00, 256);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, i));
        }
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

int at_rst(void)
{
    at_response_t resp = RT_NULL;
    int result = 0;

    resp = at_create_resp(256, 0, rt_tick_from_millisecond(5000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        goto __exit;
    }
    
    result = at_exec_cmd(resp, "AT+RST");
    if (result != RT_EOK)
    {
        LOG_E("at_rst error!");
        goto __exit;
    }
    at_delete_resp(resp);
    return 0;

__exit:

    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return -1;
}

#define COMMAND_DELAY 1000
#define WIFI_DELAY    15000  
#define DATA_DELAY    10  

int at_client_test(void)
{
    char protol[]={"\"TCP\""};
    char ip[]={"\"192.168.40.190\""};
    char port[]={"8080"};
    float mpu6050_data[14]={0};
    
    char ax1_char[6];
    char ay1_char[6];
    char az1_char[6];
    char gyrox1_char[6];
    char gyroy1_char[6];
    char gyroz1_char[6];

    float ax1_float;
    float ay1_float;
    float az1_float;
    float gyrox1_float;
    float gyroy1_float;
    float gyroz1_float;
    
    char ax2_char[6] = {0};
    char ay2_char[6] = {0};
    char az2_char[6] = {0};
    char gyrox2_char[6] = {0};
    char gyroy2_char[6] = {0};
    char gyroz2_char[6] = {0};

    float ax2_float;
    float ay2_float;
    float az2_float;
    float gyrox2_float;
    float gyroy2_float;
    float gyroz2_float;

    char dest[100] = {0};
    char comma[]={","};

    rt_uint8_t buf;
    at_client_init("uart1",512,512);
    char mpu6050_id_char[5];

    mpu6050_init("i2c0");
    mpu6050_init("i2c1");

    at_cwmode(3);
    at_rst();
    rt_thread_mdelay(COMMAND_DELAY);
    at_cwjap();
    rt_thread_mdelay(WIFI_DELAY);
    at_cifsr();
    rt_thread_mdelay(COMMAND_DELAY);
    at_cipstart(protol, ip, port);
    rt_thread_mdelay(COMMAND_DELAY);
    at_cipmode(1);
    rt_thread_mdelay(COMMAND_DELAY);
    at_cipsend();
    rt_thread_mdelay(COMMAND_DELAY);
    
    read_id(&buf);
    utoa((unsigned int)buf, &mpu6050_id_char[0], 10);
    at_senddata("id: ");
    at_senddata(mpu6050_id_char);
    at_senddata("data: ");
    for(int i = 0; i < 1500; i++){
        /* 读取温湿度数据 */
        read_signed_data(mpu6050_data);
        ax1_float = (mpu6050_data[0]);
        ay1_float = (mpu6050_data[1]);
        az1_float = (mpu6050_data[2]);

        gyrox1_float = (mpu6050_data[3]);
        gyroy1_float = (mpu6050_data[4]);
        gyroz1_float = (mpu6050_data[5]);

        ax2_float = (mpu6050_data[6]);
        ay2_float = (mpu6050_data[7]);
        az2_float = (mpu6050_data[8]);

        gyrox2_float = (mpu6050_data[9]);
        gyroy2_float = (mpu6050_data[10]);
        gyroz2_float = (mpu6050_data[11]);
        sprintf(ax1_char,"%.2f",ax1_float);
        sprintf(ay1_char,"%.2f",ay1_float);
        sprintf(az1_char,"%.2f",az1_float);
        sprintf(gyrox1_char,"%.2f",gyrox1_float);
        sprintf(gyroy1_char,"%.2f",gyroy1_float);
        sprintf(gyroz1_char,"%.2f",gyroz1_float);

        sprintf(ax2_char,"%.2f",ax2_float);
        sprintf(ay2_char,"%.2f",ay2_float);
        sprintf(az2_char,"%.2f",az2_float);
        sprintf(gyrox2_char,"%.2f",gyrox2_float);
        sprintf(gyroy2_char,"%.2f",gyroy2_float);
        sprintf(gyroz2_char,"%.2f",gyroz2_float);
        strcat(dest, ax1_char);
        strcat(dest, comma);
        strcat(dest, ay1_char);
        strcat(dest, comma);
        strcat(dest, az1_char);
        strcat(dest, comma);
        strcat(dest, gyrox1_char);
        strcat(dest, comma);
        strcat(dest, gyroy1_char);
        strcat(dest, comma);
        strcat(dest, gyroz1_char);
        strcat(dest, comma);

        strcat(dest, ax2_char);
        strcat(dest, comma);
        strcat(dest, ay2_char);
        strcat(dest, comma);
        strcat(dest, az2_char);
        strcat(dest, comma);
        strcat(dest, gyrox2_char);
        strcat(dest, comma);
        strcat(dest, gyroy2_char);
        strcat(dest, comma);
        strcat(dest, gyroz2_char);
        at_senddata(dest);
        rt_memset(dest, 0, sizeof(dest));
        rt_thread_mdelay(20);
    }

    rt_thread_mdelay(COMMAND_DELAY);
    at_stopsend();
    rt_thread_mdelay(COMMAND_DELAY);
    at_cipmode(0);
    rt_thread_mdelay(COMMAND_DELAY);
    return 0;
}

/* 设置当前 AT 客户端最大支持的一次接收数据的长度 */
#define AT_CLIENT_RECV_BUFF_LEN         512
#define AT_CLIENT_SEND_BUFF_LEN         512
int at_client_test_init(int argc, char**argv)
{
    if (argc != 2)
    {
        rt_kprintf("at_client_init <dev_name>   -- AT client initialize.\n");
        return -RT_ERROR;
    }

    at_client_init(argv[1], AT_CLIENT_RECV_BUFF_LEN,AT_CLIENT_SEND_BUFF_LEN);

    return RT_EOK;
}
#ifdef FINSH_USING_MSH
#include <finsh.h>
/* 添加 AT Client 测试命令到 shell  */
MSH_CMD_EXPORT(at_client_test, AT client send cmd and get response);
/* 添加 AT Client 初始化命令到 shell  */
MSH_CMD_EXPORT_ALIAS(at_client_test_init, at_client_init, initialize AT client);
#endif