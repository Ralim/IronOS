/**
  ******************************************************************************
  * @file    blsp_eflash_loader_uart.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include "bflb_eflash_loader.h"
#include "bflb_platform.h"
#include "blsp_port.h"
#include "blsp_common.h"
#include "partition.h"
#include "hal_uart.h"
#include "drv_device.h"
#include "hal_boot2.h"

static uint32_t g_detected_baudrate;

static void bflb_eflash_loader_usart_if_deinit();
struct device *download_uart = NULL;

void bflb_dump_data(uint8_t *buf, uint32_t size)
{
    for (int i = 0; i < size; i++) {
        if (i % 16 == 0)
            MSG("\r\n");
        MSG("%02x ", buf[i]);
    }
    MSG("\r\n");
}
static void ATTR_TCM_SECTION uart0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint8_t *buf = g_eflash_loader_readbuf[g_rx_buf_index];
    if (state == UART_EVENT_RX_FIFO) {
        //g_rx_buf_len += device_read(download_uart,0,buf,BFLB_EFLASH_LOADER_READBUF_SIZE-g_rx_buf_len);
        arch_memcpy(buf + g_rx_buf_len, args, size);
        g_rx_buf_len += size;
    } else if (state == UART_EVENT_RTO) {
        //g_rx_buf_len += device_read(download_uart,0,buf,BFLB_EFLASH_LOADER_READBUF_SIZE-g_rx_buf_len);
        arch_memcpy(buf + g_rx_buf_len, args, size);
        g_rx_buf_len += size;
    }
}

static void bflb_eflash_loader_usart_if_init(uint32_t bdrate)
{
    hal_boot2_uart_gpio_init();
#if ((BLSP_BOOT2_MODE == BOOT2_MODE_RELEASE) || (BLSP_BOOT2_MODE == BOOT2_MODE_DEBUG))
    uart_register(0, "debug_log");
    download_uart = device_find("debug_log");

    if (download_uart) {
        UART_DEV(download_uart)->fifo_threshold = 16;
        device_open(download_uart, DEVICE_OFLAG_STREAM_TX);
    }
#endif

#if (BLSP_BOOT2_MODE == BOOT2_MODE_DEEP_DEBUG)
    uart_register(0, "iap_download");
    download_uart = device_find("iap_download");

    if (download_uart) {
        UART_DEV(download_uart)->fifo_threshold = 16;
        device_open(download_uart, DEVICE_OFLAG_STREAM_TX);
    }

    uart_register(1, "debug_log");
    struct device *uart = device_find("debug_log");

    if (uart) {
        device_open(uart, DEVICE_OFLAG_STREAM_TX);
    }
#endif
}

void bflb_eflash_loader_usart_if_enable_int(void)
{
    if (download_uart) {
        device_close(download_uart);
        device_open(download_uart, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_INT_RX);
        device_set_callback(download_uart, uart0_irq_callback);
        device_control(download_uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));
    }
}

void bflb_eflash_loader_usart_if_send(uint8_t *data, uint32_t len)
{
    if (download_uart) {
        device_write(download_uart, 0, data, len);
    }
}

int32_t bflb_eflash_loader_usart_if_wait_tx_idle(uint32_t timeout)
{
    /*UART now can't judge tx idle now*/
    bflb_platform_delay_ms(timeout);

    return 0;
}

static uint32_t *bflb_eflash_loader_usart_if_receive(uint32_t *recv_len, uint16_t maxlen, uint16_t timeout)
{
    uint8_t *buf = (uint8_t *)g_eflash_loader_readbuf[g_rx_buf_index];
    uint16_t datalen = 0;
    uint64_t time_now;

    time_now = bflb_platform_get_time_ms();

    do {
        if (g_rx_buf_len >= 4) {
            /* receive cmd id and data len*/
            datalen = buf[2] + (buf[3] << 8);

            if (g_rx_buf_len == datalen + 4) {
                /*receive all the payload,return */
                /* move on to next buffer */
                g_rx_buf_index = (g_rx_buf_index + 1) % 2;
                g_rx_buf_len = 0;

                if (datalen <= BFLB_EFLASH_LOADER_CMD_DATA_MAX_LEN) {
                    *recv_len = datalen + 4;
                    return (uint32_t *)buf;
                } else {
                    *recv_len = 0;
                    return NULL;
                }
            }
        }
    } while (bflb_platform_get_time_ms() - time_now < timeout);

    *recv_len = 0;
    return NULL;
}

static void bflb_eflash_loader_usart_if_deinit()
{
    struct device *uart;
    uart = device_find("iap_download");

    if (uart) {
        device_close(uart);
    }

    uart = device_find("debug_log");

    if (uart) {
        device_close(uart);
    }
}

int32_t bflb_eflash_loader_uart_init()
{
    bflb_eflash_loader_usart_if_deinit();
    bflb_eflash_loader_usart_if_init(0);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_uart_handshake_poll(uint32_t timeout)
{
    uint8_t buf[UART_FIFO_LEN];
    uint32_t i;
    uint32_t handshake_count = 0;
    uint32_t rcv_buf_len = 0;
    //rcv_buf_len = UART_ReceiveData(g_uart_if_id,buf,128);
    //struct device *download_uart = device_find("download_uart");
    if (download_uart) {
        rcv_buf_len = device_read(download_uart, 0, buf, UART_FIFO_LEN);
    }

    //while(1)
    {
        if (rcv_buf_len >= 16) {
            for (i = 0; i < 16; i++) {
                if (buf[i] == BFLB_EFLASH_LOADER_HAND_SHAKE_BYTE) {
                    handshake_count++;

                    if (handshake_count > BFLB_EFLASH_LAODER_HAND_SHAKE_SUSS_COUNT) {
                        break;
                    }
                }
            }
        }
    }

    if (handshake_count >= BFLB_EFLASH_LAODER_HAND_SHAKE_SUSS_COUNT) {
        //reinit uart
        MSG("iap handshake %d 0x55 rcv\r\n", handshake_count);

    } else {
        MSG("iap handshake err\r\n");
        return -1;
    }

    /*receive shake hanad signal*/
    bflb_eflash_loader_usart_if_send((uint8_t *)"OK", 2);
    //arch_delay_ms(400);
    bflb_platform_delay_ms(400);
    /* consume the remaining bytes when shake hand(0x55) if needed */
    rcv_buf_len = device_read(download_uart, 0, buf, UART_FIFO_LEN); //UART_ReceiveData(g_uart_if_id,buf,128);
    //MSG("bflb_eflash_loader_usart_if_receive len %d\r\n", rcv_buf_len);
    /*init rx info */
    g_rx_buf_index = 0;
    g_rx_buf_len = 0;

#if (BLSP_BOOT2_MODE == BOOT2_MODE_DEBUG)
    bflb_platform_print_set(1);
    device_unregister("debug_log");
    uart_register(0, "iap_download");
    download_uart = device_find("iap_download");

    if (download_uart) {
        UART_DEV(download_uart)->fifo_threshold = 16;
        device_open(download_uart, DEVICE_OFLAG_STREAM_TX);
    }
#endif

    simple_malloc_init(g_malloc_buf, sizeof(g_malloc_buf));
    g_eflash_loader_readbuf[0] = vmalloc(BFLB_EFLASH_LOADER_READBUF_SIZE);
    g_eflash_loader_readbuf[1] = vmalloc(BFLB_EFLASH_LOADER_READBUF_SIZE);
    arch_memset(g_eflash_loader_readbuf[0], 0, BFLB_EFLASH_LOADER_READBUF_SIZE);
    arch_memset(g_eflash_loader_readbuf[1], 0, BFLB_EFLASH_LOADER_READBUF_SIZE);

    bflb_eflash_loader_usart_if_enable_int();
    return 0;
}

uint32_t *bflb_eflash_loader_uart_recv(uint32_t *recv_len, uint32_t maxlen, uint32_t timeout)
{
    return bflb_eflash_loader_usart_if_receive(recv_len, maxlen, timeout);
}

int32_t bflb_eflash_loader_uart_send(uint32_t *data, uint32_t len)
{
    bflb_eflash_loader_usart_if_send((uint8_t *)data, len);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_usart_wait_tx_idle(uint32_t timeout)
{
    return bflb_eflash_loader_usart_if_wait_tx_idle(timeout);
}

int32_t bflb_eflash_loader_uart_change_rate(uint32_t oldval, uint32_t newval)
{
    uint32_t b = (uint32_t)((g_detected_baudrate * 1.0 * newval) / oldval);

    //MSG("BDR:");
    //MSG(oldval);
    //MSG(newval);
    //MSG(g_detected_baudrate);
    //MSG(b);

    bflb_eflash_loader_usart_if_wait_tx_idle(BFLB_EFLASH_LOADER_IF_TX_IDLE_TIMEOUT);

    bflb_eflash_loader_usart_if_init(b);
    bflb_eflash_loader_usart_if_send((uint8_t *)"OK", 2);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_uart_deinit()
{
    /* delete uart deinit, when uart tx(gpio16) set input function, uart send 0xFF to uart tx fifo
    bflb_eflash_loader_deinit_uart_gpio(g_abr_gpio_sel);

    bflb_eflash_loader_usart_if_deinit();
    */

    return BFLB_EFLASH_LOADER_SUCCESS;
}
