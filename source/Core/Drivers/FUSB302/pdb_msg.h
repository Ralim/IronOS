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

#ifndef PDB_MSG_H
#define PDB_MSG_H

#include <stdint.h>

/*
 * PD message union
 *
 * This can be safely read from or written to in any form without any
 * transformations because everything in the system is little-endian.
 *
 * Two bytes of padding are required at the start to prevent problems due to
 * alignment.  Specifically, without the padding, &obj[0] != &bytes[2], making
 * the statement in the previous paragraph invalid.
 */
union pd_msg {
  struct {
    uint8_t _pad1[2];
    uint8_t bytes[30];
  } __attribute__((packed));
  struct {
    uint8_t  _pad2[2];
    uint16_t hdr;
    union {
      uint32_t obj[7];
      struct {
        uint16_t exthdr;
        uint8_t  data[26];
      };
    };
  } __attribute__((packed));
};

#endif /* PDB_MSG_H */
