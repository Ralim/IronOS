/**
 * @file tcp_client.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "bflb_platform.h"

#include "lwip/api.h"
#include "lwip/arch.h"
#include "lwip/opt.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "FreeRTOS.h"
#include "task.h"

#include "tcp_client.h"

#define USE_RAWAPI_TEST  0
#define USE_NETCONN_TEST 1
#define USE_SOCKET_TEST  2

#define TCP_TEST_SELECT USE_SOCKET_TEST

static StackType_t tcp_client_stack[512];
static StaticTask_t tcp_client_handle;

// clang-format off
static const uint8_t send_buf[1460] = {
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',

    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',

    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9','0','1','2','3','4','5','6','7','8','9',
    '0','1','2','3','4','5','6','7','8','9',
};
// clang-format on

#if (TCP_TEST_SELECT == USE_NETCONN_TEST)
#if LWIP_NETCONN

static void tcp_client_netconn_test(void)
{
    struct netconn *conn;
    ip4_addr_t remote_ipaddr;
    err_t err;

    conn = netconn_new(NETCONN_TCP);
    if (conn == NULL) {
        MSG("Create conn failed!\r\n");
        while (1) {
            vTaskDelay(10);
        }
    }

    IP4_ADDR(&remote_ipaddr, SERVER_IP_ADDR0, SERVER_IP_ADDR1, SERVER_IP_ADDR2, SERVER_IP_ADDR3);
    while (1) {
        err = netconn_connect(conn, &remote_ipaddr, TCP_CLIENT_TEST_PORT);

        MSG("Create client success, connect remote ip&port:%d.%d.%d.%d:%d\r\n", SERVER_IP_ADDR0, SERVER_IP_ADDR1, SERVER_IP_ADDR2, SERVER_IP_ADDR3, TCP_CLIENT_TEST_PORT);

        if (err == ERR_OK) {
            MSG("connet to server success!\r\n");

            while (1) {
                netconn_write(conn, send_buf, sizeof(send_buf), NETCONN_NOCOPY);
            }
        } else {
            netconn_close(conn);
            netconn_delete(conn);
            MSG("connect failed! err:%d\r\n", err);
        }
    }
}

#endif

/**
 * @brief Initialize the tcp client (start its thread)
 * @param none
 *
 */
void tcp_client_init()
{
    xTaskCreateStatic((void *)tcp_client_netconn_test, (char *)"tcp_client", sizeof(tcp_client_stack) / 4, NULL, osPriorityHigh, tcp_client_stack, &tcp_client_handle);
}
#endif

#if (TCP_TEST_SELECT == USE_SOCKET_TEST)

static void tcp_client_socket_test(void)
{
    int sock;
    ip4_addr_t remote_ipaddr;
    struct sockaddr_in remote_addr;
    IP4_ADDR(&remote_ipaddr, SERVER_IP_ADDR0, SERVER_IP_ADDR1, SERVER_IP_ADDR2, SERVER_IP_ADDR3);

    while (1) {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            MSG("TCP Client create socket error\r\n");
            return;
        }
        remote_addr.sin_family = AF_INET;
        remote_addr.sin_port = htons(TCP_CLIENT_TEST_PORT);
        remote_addr.sin_addr.s_addr = remote_ipaddr.addr;
        memset(&(remote_addr.sin_zero), 0, sizeof(remote_addr.sin_zero));
        MSG("Create client success, connect remote ip&port:%d.%d.%d.%d:%d\r\n", SERVER_IP_ADDR0, SERVER_IP_ADDR1, SERVER_IP_ADDR2, SERVER_IP_ADDR3, TCP_CLIENT_TEST_PORT);

        if (connect(sock, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
            MSG("TCP client connect server falied!\r\n");
            closesocket(sock);
            return;
        }

        MSG("TCP client connect server success!\r\n");
        while (1) {
            if (write(sock, send_buf, sizeof(send_buf)) < 0) {
                MSG("write falied!\r\n");
                break;
            }
        }
        closesocket(sock);
    }
}

/**
 * @brief Initialize the tcp client (start its thread)
 * @param none
 *
 */
void tcp_client_init()
{
    xTaskCreateStatic((void *)tcp_client_socket_test, (char *)"tcp_client", sizeof(tcp_client_stack) / 4, NULL, osPriorityHigh, tcp_client_stack, &tcp_client_handle);
}
#endif

#if (TCP_TEST_SELECT == USE_RAWAPI_TEST)

#include "lwip/tcp.h"

static struct tcp_pcb *client_pcb = NULL;

static void tcp_client_err(void *arg, err_t err)
{
    MSG("connect error! close by core!\r\n");
    MSG("try to connect to server again!\r\n");
    tcp_close(client_pcb);

    tcp_client_raw_init();
}

static err_t tcp_client_send(void *arg, struct tcp_pcb *t_pcb)
{
    tcp_write(t_pcb, send_buf, sizeof(send_buf), NETCONN_COPY);

    return ERR_OK;
}

static err_t tcp_client_recv(void *arg, struct tcp_pcb *t_pcb, struct pbuf *p, err_t err)
{
    if (p != NULL) {
        // update wnd
        tcp_recved(t_pcb, p->tot_len);
        // recv data

        tcp_write(t_pcb, p->payload, p->tot_len, NETCONN_COPY);

        memset(p->payload, 0, p->tot_len);
        pbuf_free(p);
    } else if (err == ERR_OK) {
        // disconnect server
        MSG("server has been disconnect!\r\n");
        tcp_close(t_pcb);

        // reconnect
        tcp_client_raw_init();
    }
    return ERR_OK;
}

static err_t tcp_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    MSG("connect ok!\r\n");

    // register callback
    tcp_poll(pcb, tcp_client_send, 2);
    // register recv callback
    tcp_recv(pcb, tcp_client_recv);

    return ERR_OK;
}

void tcp_client_raw_init(void)
{
    ip4_addr_t server_ip;

    // creat tcp pcb
    client_pcb = tcp_new();
    if (client_pcb == NULL) {
        MSG("creat client pcb failed!\r\n");
        return;
    }

    IP4_ADDR(&server_ip, SERVER_IP_ADDR0, SERVER_IP_ADDR1, SERVER_IP_ADDR2, SERVER_IP_ADDR3);

    MSG("client start connect!\r\n");

    // connect
    tcp_connect(client_pcb, &server_ip, TCP_CLIENT_TEST_PORT, tcp_client_connected);

    // register error process
    tcp_err(client_pcb, tcp_client_err);
}

#endif
