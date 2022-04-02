/**
 * @file font.h
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

#ifndef __FONT_FONT_ASCII_16X8_H__
#define __FONT_FONT_ASCII_16X8_H__

#define FONT_ASCII_16X8  1
#define FONT_ASCII_32X16 0
#define FONT_ASCII_64X32 0

#if FONT_ASCII_16X8

extern const unsigned char font_ascii_16x8[];
#endif

#if FONT_ASCII_64X32
extern const unsigned char ascii_64x32[];
#endif

extern const unsigned char asc2_1206[95][12];
extern const unsigned char asc2_1608[95][16];
extern const unsigned char asc2_2412[95][36];
extern const unsigned char Hzk[][32];
extern const unsigned char c_chFont1608[95][16];
#endif
