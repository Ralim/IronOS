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
/*
 * Read multiple bytes from the FUSB302B
 *
 * cfg: The FUSB302B to communicate with
 * addr: The memory address from which to read
 * size: The number of bytes to read
 * buf: The buffer into which data will be read
 */
bool fusb_read_buf(uint8_t addr, uint8_t size, uint8_t *buf);
/*
 * Write multiple bytes to the FUSB302B
 *
 * cfg: The FUSB302B to communicate with
 * addr: The memory address to which we will write
 * size: The number of bytes to write
 * buf: The buffer to write
 */
bool fusb_write_buf(uint8_t addr, uint8_t size, const uint8_t *buf);
// Used to poll for the device existing on the I2C bus. This should return non-zero if the device is responding on the bus
bool fusb302_detect();
// Once this is called IRQ's should be enabled and routed to the IRQ handler thread
void setupFUSBIRQ();
// This should return true if the IRQ line for the FUSB302 is still held low
bool getFUS302IRQLow();

#endif /* PDB_FUSB302B_H */
