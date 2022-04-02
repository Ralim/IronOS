#ifndef __BFLB_EFLASH_LOADER_INTERFACE_H__
#define __BFLB_EFLASH_LOADER_INTERFACE_H__

#include "bflb_eflash_loader.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"


#define BFLB_EFLASH_LOADER_IF_TX_IDLE_TIMEOUT            4  /*ms*/
#define BFLB_EFLASH_LOADER_HAND_SHAKE_BYTE              0x55
#define BFLB_EFLASH_LAODER_HAND_SHAKE_SUSS_COUNT        5

typedef  enum tag_eflash_loader_if_type_t
{
	//BFLB_EFLASH_LOADER_IF_FLASH=0x01,
	BFLB_EFLASH_LOADER_IF_UART=0x0,
	BFLB_EFLASH_LOADER_IF_JLINK,
	BFLB_EFLASH_LOADER_IF_SDIO,
	BFLB_EFLASH_LOADER_IF_USB,	
	BFLB_EFLASH_LOADER_IF_ALL,
}eflash_loader_if_type_t;

typedef  int32_t (*boot_if_init_p)(void);
typedef  int32_t (*boot_if_handshake_poll_p)(uint32_t timeout);
typedef  uint32_t *(*boot_if_read_p)(uint32_t *len,uint32_t maxlen,uint32_t timeout);
typedef  int32_t (*boot_if_write_p)(uint32_t *data, uint32_t len);
typedef  int32_t (*boot_if_wait_tx_idle_p)(uint32_t timeout);
typedef  int32_t (*boot_if_deinit_p)(void);
typedef  int32_t (*boot_if_change_rate_p)(uint32_t oldval, uint32_t newval);

typedef  struct tag_eflash_loader_if_cfg_t
{
	uint8_t if_type;            				//interface type
	uint8_t if_type_onfail;     				//if fail move to this interface
    uint8_t disabled;                           //enable this if
    uint8_t rsvd;

	//pentry_t         boot_entry[2];

	uint16_t          maxlen;             				//max len for one buffer
	uint16_t          timeout;            				//ms

	boot_if_init_p boot_if_init;          				//init function pointer
	boot_if_handshake_poll_p boot_if_handshake_poll;    //shake hand poll function pointer
	boot_if_read_p boot_if_recv;          				//read function pointer
	boot_if_write_p boot_if_send;        				//write function pointer
    boot_if_wait_tx_idle_p boot_if_wait_tx_idle;        //wait tx idle function pointer
	boot_if_deinit_p boot_if_deinit;      				//deinit function pointer
	boot_if_change_rate_p boot_if_changerate;  			//change rate function pointer

}eflash_loader_if_cfg_t;


eflash_loader_if_cfg_t * bflb_eflash_loader_if_set(eflash_loader_if_type_t type);
int32_t bflb_eflash_loader_if_init();
int32_t bflb_eflash_loader_if_handshake_poll(uint32_t timeout);
uint32_t *bflb_eflash_loader_if_read(uint32_t *read_len);
int32_t bflb_eflash_loader_if_write(uint32_t *data,uint32_t len);
int32_t bflb_eflash_loader_if_wait_tx_idle(uint32_t timeout);
int32_t bflb_eflash_loader_if_deinit();
int32_t bflb_eflash_loader_main(void);

extern uint8_t *g_eflash_loader_readbuf[2];
/*read data buffer from flash or boot interface*/
extern volatile uint32_t g_rx_buf_index;
extern volatile uint32_t g_rx_buf_len;
extern uint32_t g_eflash_loader_cmd_ack_buf[16];

#endif
