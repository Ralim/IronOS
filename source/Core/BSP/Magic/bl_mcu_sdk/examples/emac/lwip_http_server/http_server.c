/**
 * @file http_server.c
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
// #include "lwip/inet.h"
// #include "lwip/netif.h"
#include "lwip/tcpbase.h"
#include "lwip/tcp.h"
#include "lwip/apps/fs.h"
#include "lwip/apps/httpd.h"

#include "FreeRTOS.h"
#include "task.h"

#include "http_server.h"

const HtmlServerRouter_TypeDef httpRouter[] = {
    { "/", "/index.html" },
    { "/favicon.ico", "/asset/favicon.ico" },
    { "/js/main.js", "/js/main.js" },
    { "/css/style.css", "/css/style.css" },
    { "/css/asset/off.png", "/asset/off.png" },
    { "/asset/off.png", "/asset/off.png" },
    { "/css/asset/on.png", "/asset/on.png" },
    { "/asset/on.png", "/asset/on.png" },
    { "/asset/ok.json", "/asset/ok.json" },
    { "*", "/404.html" }
};

SemaphoreHandle_t console_lock = NULL;
sys_sem_t s_x_btn_semaphore = NULL;
char button_clicked_flag = false;

static struct netconn *http_active_conn = NULL;
static StackType_t http_server_stack[512];
static StaticTask_t http_server_handle;

void send_file(const char *path)
{
    struct fs_file file;
    err_t error = ERR_OK;

    error = fs_open(&file, path);

    if (error == ERR_OK) {
        netconn_write(http_active_conn, (const unsigned char *)(file.data), (size_t)file.len, NETCONN_NOCOPY);
    } else {
        OS_MSG("[ERROR] send file read failed, path: %s, error code: %d\r\n", path, error);
    }

    fs_close(&file);
}

err_t try_send_file(const char *path)
{
    struct fs_file file;
    err_t error = ERR_OK;

    error = fs_open(&file, path);

    if (error == ERR_OK) {
        netconn_write(http_active_conn, (const unsigned char *)(file.data), (size_t)file.len, NETCONN_NOCOPY);
    } else {
        OS_MSG("[ERROR] try send file read failed, path: %s, error code: %d\r\n", path, error);
    }

    fs_close(&file);

    return error;
}

void serch_send_file(const char *url)
{
    static uint16_t router_length = sizeof(httpRouter) / sizeof(HtmlRequestMethod_TypeDef);
    uint16_t i = 0;

    for (i = 0; i < router_length; i++) {
        if (!strcmp(url, httpRouter[i].url)) {
            send_file(httpRouter[i].path);
            break;
        }
    }

    /* send 404 */
    if (i >= router_length) {
        OS_MSG("[WARNING] Not found path %s, sended 404 page.\r\n", url);
        send_file(httpRouter[router_length - 1].path);
    }
}

void http_post_btn_process(const char *url, const char *body)
{
    if (strstr(body, "true") == NULL) {
        button_clicked_flag = false;
    } else {
        button_clicked_flag = true;
    }

    xSemaphoreGive(s_x_btn_semaphore);
}

void process_http_get(const char *url, const char *body)
{
    serch_send_file(url);
}

void process_http_post(const char *url, const char *body)
{
    if (!strcmp(url, "/button_clicked")) {
        http_post_btn_process(url, body);
        serch_send_file("/asset/ok.json");
    }
}

void process_http_request(const char *requset, uint16_t length)
{
    char *mem = NULL;
    char *str = NULL;
    char *body = NULL;
    char *url = NULL;

    mem = malloc(length);
    memset(mem, 0, length);

    str = mem;
    body = mem;

    memcpy(str, requset, sizeof(char) * length);

    HtmlRequestMethod_TypeDef method = HTTP_UNKNOWN;

    /* get http request method */
    if (str != NULL) {
        str = strtok(str, " ");
        if (!strcmp(str, "GET")) {
            method = HTTP_GET;
            OS_MSG("[LOG] Methon: GET\r\n");
        } else if (!strcmp(str, "POST")) {
            method = HTTP_POST;
            OS_MSG("[LOG] Methon: POST\r\n");
        } else {
            method = HTTP_UNKNOWN;
            OS_MSG("[LOG] Methon: UNKNOWN\r\n");
        }
    }

    /* get url */
    if (str != NULL) {
        str = strtok(NULL, " ");
        if (str != NULL) {
            url = malloc(strlen(str) + 1);
            memset(url, 0, strlen(str) + 1);
            strcpy(url, str);
            OS_MSG("[LOG] URL: %s\r\n", url);
        }
    }

    memcpy(body, requset, sizeof(char) * length);

    /* process http requset */
    switch (method) {
        case HTTP_GET:
            process_http_get(url, body);
            break;
        case HTTP_POST:
            process_http_post(url, body);
            break;
        default:
            break;
    }

    free(url);
    free(mem);
}

void http_server_process(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    uint16_t buflen;

    if (netconn_recv(conn, &inbuf) == ERR_OK) {
        if (netconn_err(conn) == ERR_OK) {
            netbuf_data(inbuf, (void **)&buf, &buflen);
            process_http_request(buf, buflen);
        }
        netconn_close(conn);
        netbuf_delete(inbuf);
    }
}

void http_server_thread(void *arg)
{
    // httpd_init();
    struct netconn *conn, *newconn;
    LWIP_UNUSED_ARG(arg);

    /* create a new tcp connection handle */
    conn = netconn_new(NETCONN_TCP);

    if (conn != NULL) {
        /* bind to port 80 HTTP whit default IP addr */
        if (netconn_bind(conn, IP4_ADDR_ANY, HTTP_SERVER_PORT) == ERR_OK) {
            /* start listening port */
            netconn_listen(conn);
            OS_MSG("http server listen :%d", HTTP_SERVER_PORT);

            while (1) {
                if (netconn_accept(conn, &newconn) == ERR_OK) {
                    if (newconn == NULL) {
                        OS_MSG("[ERROR]");
                    }

                    http_active_conn = newconn;
                    http_server_process(newconn);

                    http_active_conn = NULL;
                    netconn_delete(newconn);
                }
            }
        }
    }
}

void http_server_init(void)
{
    // sys_thread_new("http_server", http_server_thread, NULL, 512, 2);
    xTaskCreateStatic((void *)http_server_thread, (char *)"http_server", sizeof(http_server_stack) / 4, NULL, osPriorityAboveNormal, http_server_stack, &http_server_handle);
}

#if LWIP_NETCONN

#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG LWIP_DBG_OFF
#endif

static const char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static const char http_index_html[] = "<html><head><title>Congrats!</title></head><body><h1>Welcome to our lwIP HTTP server!</h1><p>This is a small test page, served by httpserver-netconn.</body></html>";

/** Serve one HTTP connection accepted in the http thread */
static void http_server_netconn_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;

    /* Read the data from the port, blocking if nothing yet there.
   We assume the request (the part we care about) is in one netbuf */
    err = netconn_recv(conn, &inbuf);

    if (err == ERR_OK) {
        netbuf_data(inbuf, (void **)&buf, &buflen);

        /* Is this an HTTP GET command? (only check the first 5 chars, since
    there are other formats for GET, and we're keeping it very simple )*/
        if (buflen >= 5 &&
            buf[0] == 'G' &&
            buf[1] == 'E' &&
            buf[2] == 'T' &&
            buf[3] == ' ' &&
            buf[4] == '/') {
            /* Send the HTML header
             * subtract 1 from the size, since we dont send the \0 in the string
             * NETCONN_NOCOPY: our data is const static, so no need to copy it
       */
            err = netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_COPY);
            OS_MSG("netconn write!conn:0x%x err:%d\r\n", conn, err);
            /* Send our HTML page */
            err = netconn_write(conn, http_index_html, sizeof(http_index_html) - 1, NETCONN_NOCOPY);
            OS_MSG("netconn write!conn:0x%x err:%d\r\n", conn, err);
        }
    }
    /* Close the connection (server closes in HTTP) */
    netconn_close(conn);

    /* Delete the buffer (netconn_recv gives us ownership,
   so we have to make sure to deallocate the buffer) */
    netbuf_delete(inbuf);
}

/** The main function, never returns! */
static void http_server_netconn_thread(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err;
    LWIP_UNUSED_ARG(arg);

    /* Create a new TCP connection handle */
    /* Bind to port 80 (HTTP) with default IP address */

    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, IP_ADDR_ANY, 80);

    LWIP_ERROR("http_server: invalid conn", (conn != NULL), return;);

    /* Put the connection into LISTEN state */
    netconn_listen(conn);
    OS_MSG("http server listen port: 80\r\n");
    do {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK) {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
        }
    } while (err == ERR_OK);
    LWIP_DEBUGF(HTTPD_DEBUG, ("http_server_netconn_thread: netconn_accept received error %d, shutting down", err));
    netconn_close(conn);
    netconn_delete(conn);
}

/** Initialize the HTTP server (start its thread) */
void http_server_netconn_init(void)
{
    sys_thread_new("http_server_netconn", http_server_netconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, osPriorityAboveNormal);
}

#endif /* LWIP_NETCONN*/
