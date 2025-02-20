#include <stdlib.h>
#include <string.h>
#include <rtthread.h>
#include <at.h>
#include <rtdbg.h>
/* AT+CIFSR            Query local IP address and MAC */
int at_client_test(int argc, char**argv)
{
    at_response_t resp = RT_NULL;
    int result = 0;

    if (argc != 1)
    {
        LOG_E("at_client_test  - AT client send commands to AT server.");
        return -1;
    }

    /* 创建响应结构体，设置最大支持响应数据长度为 256 字节
    （最大响应长度用户根据实际需求自定义），响应数据行数无限制，超时时间为 5 秒 */
    resp = at_create_resp(256, 0, rt_tick_from_millisecond(5000));
    if (resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        return -2;
    }

    /* 关闭回显功能 */
    at_exec_cmd(resp, "ATE0");

    /* AT  Client 发送查询 IP 地址命令并接收 AT Server 响应 */
    /* 响应数据及信息存放在 resp 结构体中 */
    result = at_exec_cmd(resp, "AT+CIFSR");
    if (result != RT_EOK)
    {
        LOG_E("AT client send commands failed or return response error!");
        goto __exit;
    }

    /* 按行数循环打印接收到的响应数据 */
    {
        const char *line_buffer = RT_NULL;

        LOG_D("Response buffer");
        for(rt_size_t line_num = 1; line_num <= resp->line_counts; line_num++)
        {
            if((line_buffer = at_resp_get_line(resp, line_num)) != RT_NULL)
            {
                LOG_D("line %d buffer : %s", line_num, line_buffer);
            }
            else
            {
                LOG_E("Parse line buffer error!");
            }
        }
    }
    /* 按自定义表达式（sscanf 解析方式）解析数据，得到对应数据 */
    {
        char resp_arg[256] = { 0 };
        /* 自定义数据解析表达式 ，用于解析两双引号之间字符串信息 */
        const char * resp_expr = "%*[^\"]\"%[^\"]\"";

        LOG_D("Parse arguments");
        /* 解析响应数据中第一行数据，得到对应 IP 地址 */
        if (at_resp_parse_line_args(resp, 1, resp_expr, resp_arg) == 1)
        {
            LOG_D("Station IP  : %s", resp_arg);
            memset(resp_arg, 0x00, 256);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, 4));
        }

        /* 解析响应数据中第二行数据，得到对应 MAC 地址 */
        if (at_resp_parse_line_args(resp, 2, resp_expr, resp_arg) == 1)
        {
            LOG_D("Station MAC : %s", resp_arg);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp, 5));
            goto __exit;
        }
    }
__exit:
    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return result;
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