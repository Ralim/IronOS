/**
 * @file tcp_server.c
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

#include "tcp_server.h"

#define USE_RAWAPI_TEST  0
#define USE_NETCONN_TEST 1
#define USE_SOCKET_TEST  2

#define TCP_TEST_SELECT USE_NETCONN_TEST

struct netif *tmpnetif;
static uint8_t iptxt[20];

static StackType_t tcp_server_stack[512];
static StaticTask_t tcp_server_handle;

#if (TCP_TEST_SELECT == USE_NETCONN_TEST)
#if LWIP_NETCONN
// clang-format off
static const uint8_t send_buf[1460] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
};
// clang-format on
void tcp_netconn_send_test(void)
{
    struct netconn *conn, *newconn;
    err_t err;

    // creat tcp server
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, TCP_SERVER_TEST_PORT);

    // set listen
    netconn_listen(conn);

    tmpnetif = netif_find("bl");
    sprintf((char *)iptxt, "%s", ip4addr_ntoa(&tmpnetif->ip_addr));
    MSG("Create server success, server ip & listen port:%s:%d\r\n", iptxt, TCP_SERVER_TEST_PORT);

    while (1) {
        // wait connect
        err = netconn_accept(conn, &newconn);
        MSG("server recv a connection!\r\n");

        // process connect
        if (err == ERR_OK) {
            MSG("netconn send!\r\n");

            while (1) {
                netconn_write(newconn, send_buf, sizeof(send_buf), NETCONN_NOCOPY);
            }
        }
        // delete connect
        netconn_close(newconn);
        // netconn_shutdown(newconn, 1, 1); // two-way port close
        netconn_delete(newconn);
    }
}

/**
 * @brief implementing tcp loopback use the netconn api
 *
 */
void tcp_server_netconn_test(void)
{
    struct netconn *conn, *newconn;
    err_t err;

    // creat tcp server
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, TCP_SERVER_TEST_PORT);

    // set listen
    netconn_listen(conn);

    tmpnetif = netif_find("bl");
    sprintf((char *)iptxt, "%s", ip4addr_ntoa(&tmpnetif->ip_addr));
    MSG("Create server success, server ip & listen port:%s:%d\r\n", iptxt, TCP_SERVER_TEST_PORT);

    while (1) {
        // wait connect
        err = netconn_accept(conn, &newconn);
        MSG("server recv a connection!\r\n");

        // process connect
        if (err == ERR_OK) {
            struct netbuf *buf;
            void *data;
            uint16_t len;

            // loopback recv data
            while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
                do {
                    netbuf_data(buf, &data, &len);
                    err = netconn_write(newconn, data, len, NETCONN_NOCOPY); // tcp echo
                    // if (err != ERR_OK) {
                    //     MSG("write falied! err:%d\r\n", err);
                    // }
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }
        }
        // delete connect
        netconn_close(newconn);
        // netconn_shutdown(newconn, 1, 1); // two-way port close
        netconn_delete(newconn);
    }
}
#endif
/**
  * @brief  Initialize the tcp server (start its thread)
  * @param  none
  * @retval None
  */
void tcp_server_init()
{
    xTaskCreateStatic((void *)tcp_server_netconn_test, (char *)"tcp_server", sizeof(tcp_server_stack) / 4, NULL, osPriorityHigh, tcp_server_stack, &tcp_server_handle);
}
#endif

#if (TCP_TEST_SELECT == USE_SOCKET_TEST)

static unsigned char recv_buffer[2048];
/**
  * @brief serve tcp connection
  * @param conn: connection socket
  * @retval None
  */
void tcp_server_process(int conn)
{
    int buflen;
    // int ret;
    int flag;
    setsockopt(conn, IPPROTO_TCP, TCP_NODELAY, (void *)&flag, sizeof(int));

    while (1) {
        buflen = recv(conn, recv_buffer, sizeof(recv_buffer), 0);
        if (buflen <= 0) {
            break;
        }
        write(conn, recv_buffer, buflen);
    }
    if (conn >= 0) {
        closesocket(conn);
    }
    // /* Read in the request */
    // ret = read(conn, recv_buffer, buflen);
    // if (ret > 0) {
    //     write(conn, (const unsigned char *)(recv_buffer), (size_t)ret);
    // }
    // /* Close connection socket */
    // close(conn);
}

/**
  * @brief  tcp server thread
  * @param arg: pointer on argument(not used here)
  * @retval None
  */
static void tcp_server_socket_thread(void *arg)
{
    int sock, newconn, size;
    struct sockaddr_in address, remotehost;

    MSG("TCP Server create socket\r\n");

    /* create a TCP socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return;
    }

    /* bind to test port at any interface */
    address.sin_family = AF_INET;
    address.sin_port = htons(TCP_SERVER_TEST_PORT);
    address.sin_addr.s_addr = INADDR_ANY;
    memset(&(address.sin_zero), 0, sizeof(address.sin_zero));

    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return;
    }

    /* listen for incoming connections (TCP listen backlog = 5) */
    listen(sock, 5);

    tmpnetif = netif_find("bl");
    sprintf((char *)iptxt, "%s", ip4addr_ntoa(&tmpnetif->ip_addr));
    MSG("Create server success, server ip & listen port:%s:%d\r\n", iptxt, TCP_SERVER_TEST_PORT);

    size = sizeof(remotehost);

    while (1) {
        newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
        MSG("TCP Server deal one client\r\n");
        tcp_server_process(newconn);
        if (newconn >= 0) {
            closesocket(newconn);
            newconn = -1;
        }
    }
}

/**
  * @brief  Initialize the tcp server (start its thread)
  * @param  none
  * @retval None
  */
void tcp_server_init()
{
    xTaskCreateStatic((void *)tcp_server_socket_thread, (char *)"tcp_server", sizeof(tcp_server_stack) / 4, NULL, 16, tcp_server_stack, &tcp_server_handle);
}
#endif

#if (TCP_TEST_SELECT == USE_RAWAPI_TEST)

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

static err_t tcp_echo_recv(void *arg, struct tcp_pcb *t_pcb, struct pbuf *p, err_t err)
{
    if (p != NULL) {
        // update wnd
        tcp_recved(t_pcb, p->tot_len);

        // recv data and write
        tcp_write(t_pcb, p->payload, p->tot_len, NETCONN_COPY);

        // memset(p->payload, 0, p->tot_len);
        pbuf_free(p);

    } else if (err == ERR_OK) {
        return tcp_close(t_pcb);
    }
    return ERR_OK;
}

static err_t tcp_echo_process(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, tcp_echo_recv);
    return ERR_OK;
}

void tcp_server_raw_init(void)
{
    struct tcp_pcb *new_tcp = NULL;
    err_t err;

    // creat new tcp pcb
    new_tcp = tcp_new();
    if (new_tcp == NULL) {
        MSG("new tcp pcb falied!\r\n");
    }

    // bind tcp pcb
    err = tcp_bind(new_tcp, IP_ADDR_ANY, TCP_SERVER_TEST_PORT);
    if (err != ERR_OK) {
        MSG("tcp bind failed!\r\n");
    }

    // listen port
    new_tcp = tcp_listen(new_tcp);
    MSG("tcp listen ...\r\n");

    // process some connect
    tcp_accept(new_tcp, tcp_echo_process);
}

#endif
