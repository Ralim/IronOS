/**
 * *****************************************************************************
 * @file dis_cmd.c
 * @version 0.1
 * @date 2020-09-30
 * @brief
 * *****************************************************************************
 * @attention
 *
 *  <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *    3. Neither the name of Bouffalo Lab nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * *****************************************************************************
 */

#include "ring_buffer.h"
#include "bflb_platform.h"
#include "hal_uart.h"
#include "dis_cmd.h"
#include "GSL61xx.h"

//#undef MSG
//#define MSG(...)

FCSCMD_TypeDef Rx, Tx;
uint8_t image[120 * 104];

uint8_t Run_XOR(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t value;

    for (value = 0, i = 0; i < len; i++) {
        value ^= buf[i];
    }

    return value;
}

uint8_t Run_SUM(uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint8_t value;

    for (value = 0, i = 0; i < len; i++) {
        value += buf[i];
    }

    return (~value);
}

void CMD_Process(PACK_TypeDef *CMD_Pack, PACK_TypeDef *ACK_Pack)
{
    static int index = 0;
    struct device *uart = device_find("debug_log");

    /*TODO ACK Function */
    uint8_t CAP_REP[] = { 0x3A, 0x02, 0x00, 0x00, 0x00, 0x04, 0x3c, 0x00, 0x78, 0x00, 0x68, 0xA3 };
    uint8_t TRANSMIT_REP[] = { 0x3A, 0x06, 0x00, 0x00, 0x00, 0x02, 0x3e, 0x80, 0x00, 0xff };
    uint8_t TRANSMIT_REP2[] = { 0x3A, 0x06, 0x00, 0x00, 0x00, 0x04, 0x38, 0x80, 0x00, 0x0e, 0x8b, 0x6A };
    uint8_t data_XOR = 0;
    uint8_t data_SUM = 0;

    switch (CMD_Pack->Cmd1) {
        case CMD_INT:
            break;

        case CMD_GETINFO:
            break;

        case CMD_CAPTURE:
            /*cap image */
            CaptureGSL61xx(image);

            //device_write(uart, 0, (uint8_t *)CAP_REP, sizeof(CAP_REP));
            for (index = 0; index < 120; index++) {
                device_write(uart, 0, (uint8_t *)&image[index * 104], 104);
            }

            break;

        case CMD_DELETE:
            break;

        case CMD_TRANSMIT:

            device_write(uart, 0, (uint8_t *)TRANSMIT_REP, sizeof(TRANSMIT_REP));
            device_write(uart, 0, (uint8_t *)(image + 256 * index), 256);

            data_XOR = Run_XOR(image + 256 * index, 256);
            data_SUM = Run_SUM(image + 256 * index, 256);
            TRANSMIT_REP2[9] = data_XOR;
            TRANSMIT_REP2[10] = data_SUM;
            TRANSMIT_REP2[11] = Run_SUM(TRANSMIT_REP2, 11);

            device_write(uart, 0, (uint8_t *)TRANSMIT_REP2, 12);
            index++;

            if (index == 49) {
                index = 0;
                //memset(image,0,120*104);
                //Ring_Buffer_Reset(&uartRB);
                GSL61xx_init();
            }

            break;

        case CMD_CONFIG:
            break;

        case CMD_DEBUG:
            break;

        default:
            break;
    }
}

int8_t TASK_Check_CMD(PACK_TypeDef *RxPack, PACK_TypeDef *TxPack)
{
    TxPack->Head = RxPack->Head;
    TxPack->Cmd1 = RxPack->Cmd1;
    TxPack->Len = 0;

    /* TODO ExtData deal function */
    if (RxPack->Cmd1 == 0x02) {
        RxPack->Len >>= 8;

        if (Run_XOR((uint8_t *)RxPack, 6) != RxPack->XOR) {
            TxPack->Cmd2 = ERROR_XOR;
            //MSG("error1\r\n");
            return -1;
        } else if (Run_SUM((uint8_t *)RxPack, RxPack->Len + 7) != RxPack->SUM) {
            TxPack->Cmd2 = ERROR_SUM;
            //MSG("error2\r\n");
            return -1;
        } else {
            return 0;
        }
    } else {
        RxPack->Len >>= 8; //0x0400 -> 0x0004

        //MSG("RxPack->Head = 0x%x\r\n ",RxPack->Head);
        //MSG("RxPack->Cmd1 = 0x%x\r\n ",RxPack->Cmd1);
        //MSG("RxPack->Cmd2 = 0x%x\r\n ",RxPack->Cmd2);
        //MSG("RxPack->Len = 0x%x\r\n ",RxPack->Len);
        //MSG("RxPack->XOR = 0x%x\r\n ",RxPack->XOR);
        //MSG("RxPack->ExtData[0] = 0x%x\r\n ",RxPack->ExtData[0]);
        //MSG("RxPack->ExtData[1] = 0x%x\r\n ",RxPack->ExtData[1]);
        //MSG("RxPack->ExtData[2] = 0x%x\r\n ",RxPack->ExtData[2]);
        //MSG("RxPack->ExtData[3] = 0x%x\r\n ",RxPack->ExtData[3]);
        //MSG("RxPack->SUM = 0x%x\r\n ",RxPack->SUM);

        if (Run_XOR((uint8_t *)RxPack, 6) != RxPack->XOR) {
            TxPack->Cmd2 = ERROR_XOR;
            //MSG("error1\r\n");
            return -1;
        } else if (Run_SUM((uint8_t *)RxPack, RxPack->Len + 7) != RxPack->SUM) {
            TxPack->Cmd2 = ERROR_SUM;
            //MSG("error2\r\n");
            return -1;
        } else {
            return 0;
        }
    }
}

Ring_Buffer_Type uartRB;

void uart_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    uint8_t i = 0;

    if (state == UART_EVENT_RX_FIFO) {
        Ring_Buffer_Write(&uartRB, (uint8_t *)args, size);
    } else if (state == UART_EVENT_RTO) {
        Ring_Buffer_Write(&uartRB, (uint8_t *)args, size);
    }
}

uint8_t ringbuffer[64];
void display_cmd_init(void)
{
    struct device *uart = device_find("debug_log");
    device_set_callback(uart, uart_irq_callback);
    device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));

    Ring_Buffer_Init(&uartRB, ringbuffer, sizeof(ringbuffer), NULL, NULL);
}
void display_cmd_handle(void)
{
    uint16_t ringbuf_len = 0;

    ringbuf_len = Ring_Buffer_Read(&uartRB, (uint8_t *)&Rx.Packet, sizeof(Rx.Packet));

    if (ringbuf_len != 0) {
        //MSG("ringbuf_len =%d\r\n",ringbuf_len );
        if (0 == TASK_Check_CMD(&Rx.Packet, &Tx.Packet)) {
            Ring_Buffer_Reset(&uartRB);
            CMD_Process(&Rx.Packet, &Tx.Packet);
        }
    }
}