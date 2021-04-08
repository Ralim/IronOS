/*
 * PD Buddy Firmware Library - USB Power Delivery for everyone
 * Copyright 2017-2018 Clayton G. Hobbs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PDB_FUSB_USER_H
#define PDB_FUSB_USER_H

#include <stdint.h>
uint8_t fusb_read_byte(uint8_t addr);
bool    fusb_read_buf(uint8_t addr, uint8_t size, uint8_t *buf);
bool    fusb_write_byte(uint8_t addr, uint8_t byte);
bool    fusb_write_buf(uint8_t addr, uint8_t size, const uint8_t *buf);
uint8_t fusb302_detect();
void    setupFUSBIRQ();

#endif /* PDB_FUSB302B_H */
