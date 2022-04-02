/**
 * @file iperf_server.c
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
#include "drv_mmheap.h"

#include "lwip/api.h"
#include "lwip/arch.h"
#include "lwip/opt.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/sockets.h"

#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "FreeRTOS.h"
#include "task.h"

#include "iperf_server.h"

static StackType_t iperf_sever_stack[256];
static StaticTask_t iperf_server_handle;

static void lwiperf_report(void *arg,
                           enum lwiperf_report_type report_type,
                           const ip_addr_t *local_addr,
                           u16_t local_port,
                           const ip_addr_t *remote_addr,
                           u16_t remote_port,
                           u32_t bytes_transferred,
                           u32_t ms_duration,
                           u32_t bandwidth_kbitpsec)
{
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(local_addr);
    LWIP_UNUSED_ARG(local_port);

    MSG("IPERF report: type=%d, remote: %s:%d, total bytes: %" U32_F
        ", duration in ms: %" U32_F ", kbits/s: %" U32_F "\n",
        (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port,
        bytes_transferred, ms_duration, bandwidth_kbitpsec);
}

/**
 * @brief
 *
 * @param arg
 */
static void iperf_server_thread(void *arg)
{
    struct netif *tmpnetif;
    uint8_t iptxt[20];
    tmpnetif = netif_find("bl");
    lwiperf_start_tcp_server(&tmpnetif->ip_addr, IPERF_SERVER_PORT, lwiperf_report,
                             NULL);

    sprintf((char *)iptxt, "%s", ip4addr_ntoa(&tmpnetif->ip_addr));
    MSG("lwiperf tcp server test IP address & port: %s:%d\r\n", iptxt, IPERF_SERVER_PORT);

    while (1) {
        vTaskDelay(2000);
    }
}

static void my_iperf_server(void)
{
    struct netconn *conn, *newconn;
    err_t err;
    void *recv_data;

    recv_data = (void *)pvPortMalloc(IPERF_BUFSZ);
    if (recv_data == NULL) {
        MSG("No memory\r\n");
    }
    // creat tcp server
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, IPERF_SERVER_PORT);

    MSG("local port:%d\r\n", IPERF_SERVER_PORT);
    // LWIP_ERROR("tcpecho: invalid conn", (conn != NULL), return;);

    // set listen
    netconn_listen(conn);
    // wait connect
    err = netconn_accept(conn, &newconn);
    while (1) {
        // process connect
        if (err == ERR_OK) {
            struct netbuf *buf;
            // void *data;
            u16_t len;
            // loopback recv data
            while ((err = netconn_recv(newconn, &buf)) == ERR_OK) {
                do {
                    netbuf_data(buf, (void *)&recv_data, &len);
                    // err = netconn_write(newconn, recv_data, len, NETCONN_COPY);
                } while (netbuf_next(buf) >= 0);
                netbuf_delete(buf);
            }
            // delete connect
            netconn_close(newconn);
            // netconn_shutdown(newconn, 1, 1);  // two-way port close
            netconn_delete(newconn);
        }
    }
}

void my_iperf_socket_test(void)
{
    int sock, newconn, size, byte_recv;
    struct sockaddr_in address, remotehost;

    MSG("TCP Server socket\r\n");
    void *recv_data;

    recv_data = (void *)pvPortMalloc(IPERF_BUFSZ);
    if (recv_data == NULL) {
        MSG("No memory\r\n");
    }

    /* create a TCP socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return;
    }

    /* bind to test port at any interface */
    address.sin_family = AF_INET;
    address.sin_port = htons(IPERF_SERVER_PORT);
    address.sin_addr.s_addr = INADDR_ANY;
    memset(&(address.sin_zero), 0, sizeof(address.sin_zero));

    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0) {
        return;
    }

    /* listen for incoming connections (TCP listen backlog = 5) */
    listen(sock, 5);

    size = sizeof(remotehost);

    while (1) {
        newconn = accept(sock, (struct sockaddr *)&remotehost, (socklen_t *)&size);
        while (1) {
            byte_recv = recv(newconn, recv_data, IPERF_BUFSZ, 0);
            if (byte_recv <= 0)
                break;
        }

        if (newconn >= 0) {
            closesocket(newconn);
            newconn = -1;
        }
    }
}

void iperf_server_init(void)
{
    xTaskCreateStatic((void *)iperf_server_thread, (char *)"iperf", sizeof(iperf_sever_stack) / 4, NULL, osPriorityHigh, iperf_sever_stack, &iperf_server_handle);
}
