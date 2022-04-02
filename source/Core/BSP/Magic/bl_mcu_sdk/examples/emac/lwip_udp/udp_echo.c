/**
 * @file udp_echo.c
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

#include "udp_echo.h"

#define UDP_RAWAPI_TEST  0
#define UDP_NETCONN_TEST 1
#define UDP_SOCKET_TEST  2

#define UDP_TEST_SELECT UDP_NETCONN_TEST

// clang-format off
static const uint8_t send_buf[1472] = {
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

    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1',
};
// clang-format on

#if ((UDP_TEST_SELECT == UDP_NETCONN_TEST) || (UDP_TEST_SELECT == UDP_SOCKET_TEST))

static unsigned char recv_buf[2048];
// static struct netbuf *recv_buf;
static StackType_t udp_echo_stack[512];
static StaticTask_t udp_echo_handle;
#endif

#if (UDP_TEST_SELECT == UDP_NETCONN_TEST)

void udp_netconn_send_test(void)
{
    struct netconn *udpconn;
    struct netbuf *udpbuf;

    ip_addr_t remote_ipaddr;
    err_t err;

    IP4_ADDR(&remote_ipaddr, UDP_DST_IP0, UDP_DST_IP1, UDP_DST_IP2, UDP_DST_IP3);

    udpconn = netconn_new(NETCONN_UDP);

    if (udpconn != NULL) {
        err = netconn_bind(udpconn, IP_ADDR_ANY, 3366);
        netconn_connect(udpconn, &remote_ipaddr, UDP_TEST_PORT);
        if (err == ERR_OK) {
            MSG("UDP connect!\r\n");
            while (1) {
                udpbuf = netbuf_new();
                netbuf_alloc(udpbuf, 1472);

                memcpy(udpbuf->p->payload, (void *)send_buf, 1472);

                err = netconn_send(udpconn, udpbuf);

                netbuf_delete(udpbuf);
            }
        } else {
            MSG("UDP bind failed!\r\n");
        }
    }

    // err = netconn_sendto(udpconn, send_buf, &remote_ipaddr, UDP_TEST_PORT);
    // vTaskDelay(20);
}

static void udp_echo_netconn_test(void)
{
    struct netconn *conn;
    struct netbuf *buf;
    err_t err;

    conn = netconn_new(NETCONN_UDP);
    err = netconn_bind(conn, IP_ADDR_ANY, UDP_TEST_PORT);

    if (err != ERR_OK) {
        MSG("UDP netconn bind failed\r\n");
        return;
    }

    while (1) {
        err = netconn_recv(conn, &buf);
        if (err == ERR_OK) {
            if (netbuf_copy(buf, recv_buf, sizeof(recv_buf)) != buf->p->tot_len) {
                MSG("netconn copy failed\r\n");
            } else {
                recv_buf[buf->p->tot_len] = '\0';
                err = netconn_send(conn, buf);
                if (err != ERR_OK) {
                    MSG("UDP netconn send failed\r\n");
                }
                // else {
                // MSG("UDP get:%s\r\n", recv_buf);
                // }
            }
            netbuf_delete(buf);
        }
    }
}

void udp_echo_init(void)
{
    xTaskCreateStatic((void *)udp_echo_netconn_test, (char *)"udp_echo", sizeof(udp_echo_stack) / 4, NULL, osPriorityHigh, udp_echo_stack, &udp_echo_handle);
}
#endif

#if (UDP_TEST_SELECT == UDP_SOCKET_TEST)
void udp_socket_send_test(void)
{
    int sock;
    ip_addr_t remote_ipaddr;
    struct sockaddr_in remote_addr;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sock < 0) {
        MSG("creat socker error\r\n");
        return;
    }

    IP4_ADDR(&remote_ipaddr, UDP_DST_IP0, UDP_DST_IP1, UDP_DST_IP2, UDP_DST_IP3);
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = remote_ipaddr.addr;
    remote_addr.sin_port = htons(UDP_TEST_PORT);
    while (1) {
        sendto(sock, (char *)send_buf, sizeof(send_buf), 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
    }
}

static void udp_echo_socket_test(void)
{
    int sock, recv_len;
    struct sockaddr_in udp_addr, seraddr;
    socklen_t addr_len;

    while (1) {
        if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            MSG("creat socker error\r\n");
            return;
        }
        MSG("udp socket creat success\r\n");
        udp_addr.sin_family = AF_INET;
        udp_addr.sin_addr.s_addr = INADDR_ANY;
        udp_addr.sin_port = htons(UDP_TEST_PORT);
        memset(&(udp_addr.sin_zero), 0, sizeof(udp_addr.sin_zero));

        if (bind(sock, (struct sockaddr *)&udp_addr, sizeof(struct sockaddr)) < 0) {
            MSG("UDP socket unable to bind");
            return;
        }
        MSG("udp socket bind\r\n");

        while (1) {
            recv_len = recvfrom(sock, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&seraddr, &addr_len);
            // MSG("recv from %s\r\n", inet_ntoa(seraddr.sin_addr));
            // MSG("recv:%s \r\n", recv_buf);
            sendto(sock, recv_buf, recv_len, 0, (struct sockaddr *)&seraddr, addr_len);
            // MSG("seraddr %x, addr_len:%d\r\n", seraddr.sin_addr, addr_len);
        }
    }
}

void udp_echo_init(void)
{
    xTaskCreateStatic((void *)udp_echo_socket_test, (char *)"udp_echo", sizeof(udp_echo_stack) / 4, NULL, osPriorityHigh, udp_echo_stack, &udp_echo_handle);
}
#endif

#if (UDP_TEST_SELECT == UDP_RAWAPI_TEST)

#include "lwip/udp.h"

static StackType_t udp_task_stack[512];
static StaticTask_t udp_task_handle;

static struct udp_pcb *udp_echo_raw_pcb;

static void udp_echo_raw_recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    LWIP_UNUSED_ARG(arg);
    if (p != NULL) {
        /* send received packet back to sender */
        udp_sendto(upcb, p, addr, port);
        /* free the pbuf */
        pbuf_free(p);
    }
}

void udp_echo_raw_init(void)
{
    // ip_addr_t remote_ipaddr;
    err_t err;
    // udp_echo_raw_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    udp_echo_raw_pcb = udp_new();
    // IP4_ADDR(&remote_ipaddr, UDP_DST_IP0, UDP_DST_IP1, UDP_DST_IP2, UDP_DST_IP3);

    if (udp_echo_raw_pcb != NULL) {
        err = udp_bind(udp_echo_raw_pcb, IP_ANY_TYPE, UDP_TEST_PORT);

        if (err == ERR_OK) {
            // MSG("bind UDP PCB:0x%x!\r\n", udp_echo_raw_pcb);
            udp_recv(udp_echo_raw_pcb, udp_echo_raw_recv, NULL);
        } else {
            MSG("UDP bind error,err:%d\r\n", err);
        }
    } else {
        MSG("Creat UDP falied!\r\n");
    }
}

void udp_client_send(ip_addr_t *dst_ipaddr, uint32_t dst_port)
{
    struct pbuf *p;

    // allocate pbuf from pool
    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(send_buf), PBUF_RAM);

    if (p != NULL) {
        // copy data to pbuf
        pbuf_take(p, (char *)send_buf, sizeof(send_buf));
        // MSG("udp send\r\n");
        // send udp data
        // udp_send(udp_echo_raw_pcb, p);
        udp_sendto(udp_echo_raw_pcb, p, dst_ipaddr, dst_port);

        // free pbuf
        pbuf_free(p);
    }
}

void udp_task_test(void)
{
    ip_addr_t remote_ipaddr;
    err_t err;
    udp_echo_raw_pcb = udp_new();
    IP4_ADDR(&remote_ipaddr, UDP_DST_IP0, UDP_DST_IP1, UDP_DST_IP2, UDP_DST_IP3);
    if (udp_echo_raw_pcb != NULL) {
        err = udp_connect(udp_echo_raw_pcb, &remote_ipaddr, UDP_TEST_PORT);

        if (err != ERR_OK) {
            MSG("UDP connect error,err:%d\r\n", err);
            return;
        }
        MSG("local port is %d\r\n", udp_echo_raw_pcb->local_port);

        while (1) {
            udp_client_send(&remote_ipaddr, UDP_TEST_PORT);
        }
    } else {
        MSG("Creat UDP falied!\r\n");
    }
}

void udp_echo_init(void)
{
    xTaskCreateStatic((void *)udp_task_test, (char *)"udp_task", sizeof(udp_task_stack) / 4, NULL, osPriorityHigh, udp_task_stack, &udp_task_handle);
}
#endif
