/*!
    \file  drv_usb_host.h
    \brief USB host mode low level driver header file

    \version 2019-6-5, V1.0.0, firmware for GD32 USBFS&USBHS
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#ifndef __DRV_USB_HOST_H
#define __DRV_USB_HOST_H

#include "drv_usb_core.h"
#include "drv_usb_regs.h"
#include "usb_ch9_std.h"

typedef enum _usb_pipe_status {
  PIPE_IDLE = 0U,
  PIPE_XF,
  PIPE_HALTED,
  PIPE_NAK,
  PIPE_NYET,
  PIPE_STALL,
  PIPE_TRACERR,
  PIPE_BBERR,
  PIPE_REQOVR,
  PIPE_DTGERR,
} usb_pipe_staus;

typedef enum _usb_pipe_mode { PIPE_PERIOD = 0U, PIPE_NON_PERIOD = 1U } usb_pipe_mode;

typedef enum _usb_urb_state { URB_IDLE = 0U, URB_DONE, URB_NOTREADY, URB_ERROR, URB_STALL } usb_urb_state;

typedef struct _usb_pipe {
  uint8_t  in_used;
  uint8_t  dev_addr;
  uint32_t dev_speed;

  struct {
    uint8_t  num;
    uint8_t  dir;
    uint8_t  type;
    uint16_t mps;
  } ep;

  uint8_t  ping;
  uint32_t DPID;

  uint8_t *xfer_buf;
  uint32_t xfer_len;
  uint32_t xfer_count;

  uint8_t data_toggle_in;
  uint8_t data_toggle_out;

  __IO uint32_t       err_count;
  __IO usb_pipe_staus pp_status;
  __IO usb_urb_state  urb_state;
} usb_pipe;

typedef struct _usb_host_drv {
  uint8_t       rx_buf[512U];
  __IO uint32_t connect_status;
  __IO uint32_t port_enabled;
  __IO uint32_t backup_xfercount[USBFS_MAX_TX_FIFOS];

  usb_pipe pipe[USBFS_MAX_TX_FIFOS];
} usb_host_drv;

typedef struct _usb_core_driver {
  usb_core_basic bp;

  usb_core_regs regs;

  usb_host_drv host;
} usb_core_driver;

/* initializes USB core for host mode */
usb_status usb_host_init(usb_core_driver *pudev);

/* initialize host pipe */
usb_status usb_pipe_init(usb_core_driver *pudev, uint8_t pipe_num);

/* prepare host pipe for transferring packets */
usb_status usb_pipe_xfer(usb_core_driver *pudev, uint8_t pipe_num);

/* halt host pipe */
usb_status usb_pipe_halt(usb_core_driver *pudev, uint8_t pipe_num);

/* configure host pipe to do ping operation */
usb_status usb_pipe_ping(usb_core_driver *pudev, uint8_t pipe_num);

/* reset host port */
uint32_t usb_port_reset(usb_core_driver *pudev);

/* control the VBUS to power */
void usb_portvbus_switch(usb_core_driver *pudev, uint8_t state);

/* stop the USB host and clean up FIFO */
void usb_host_stop(usb_core_driver *pudev);

//__STATIC_INLINE uint8_t usb_frame_even (usb_core_driver *pudev)
uint32_t usb_frame_even(usb_core_driver *pudev);
//{
//  return !(pudev->regs.hr->HFINFR & 0x01U);
//}

//__STATIC_INLINE void usb_phyclock_config (usb_core_driver *pudev, uint8_t clock)
void usb_phyclock_config(usb_core_driver *pudev, uint8_t clock);
//{
// pudev->regs.hr->HCTL &= ~HCTL_CLKSEL;
// pudev->regs.hr->HCTL |= clock;
//}

uint32_t usb_port_read(usb_core_driver *pudev);
// inline uint32_t usb_port_read (usb_core_driver *pudev)
//{
// return *pudev->regs.HPCS & ~(HPCS_PE | HPCS_PCD | HPCS_PEDC);
//}

uint32_t usb_curspeed_get(usb_core_driver *pudev);

// inline uint32_t usb_curspeed_get (usb_core_driver *pudev)
//{
// return *pudev->regs.HPCS & HPCS_PS;
//}

//__STATIC_INLINE uint32_t usb_curframe_get (usb_core_driver *pudev)
uint32_t usb_curframe_get(usb_core_driver *pudev);
//{
//  return (pudev->regs.hr->HFINFR & 0xFFFFU);
//}

#endif /* __DRV_USB_HOST_H */
