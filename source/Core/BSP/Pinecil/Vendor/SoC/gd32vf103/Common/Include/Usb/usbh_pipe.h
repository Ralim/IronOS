/*!
    \file  usbh_pipe.h
    \brief USB host mode pipe header file

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

#ifndef __USBH_PIPE_H
#define __USBH_PIPE_H

#include "usbh_core.h"

#define HC_MAX 8U

#define HC_OK        0x0000U
#define HC_USED      0x8000U
#define HC_ERROR     0xFFFFU
#define HC_USED_MASK 0x7FFFU

/* allocate a new pipe */
uint8_t usbh_pipe_allocate(usb_core_driver *pudev, uint8_t ep_addr);

/* delete all USB host pipe */
uint8_t usbh_pipe_delete(usb_core_driver *pudev);

/* free a pipe */
uint8_t usbh_pipe_free(usb_core_driver *pudev, uint8_t pp_num);

/* create a pipe */
uint8_t usbh_pipe_create(usb_core_driver *pudev, usb_dev_prop *udev, uint8_t pp_num, uint8_t ep_type, uint16_t ep_mpl);

/* modify a pipe */
uint8_t usbh_pipe_update(usb_core_driver *pudev, uint8_t pp_num, uint8_t dev_addr, uint32_t dev_speed, uint16_t ep_mpl);

#endif /* __USBH_PIPE_H */
