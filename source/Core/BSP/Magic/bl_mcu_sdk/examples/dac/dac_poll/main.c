/**
 * @file main.c
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
#include "hal_dac.h"

uint16_t sinList[] = {
    500, 506, 513, 519, 525, 531, 538, 544, 550, 556,
    563, 569, 575, 581, 587, 594, 600, 606, 612, 618,
    624, 630, 636, 642, 648, 654, 660, 666, 672, 678,
    684, 690, 696, 701, 707, 713, 718, 724, 730, 735,
    741, 746, 752, 757, 762, 768, 773, 778, 784, 789,
    794, 799, 804, 809, 814, 819, 823, 828, 833, 838,
    842, 847, 851, 856, 860, 864, 869, 873, 877, 881,
    885, 889, 893, 897, 901, 904, 908, 912, 915, 919,
    922, 925, 929, 932, 935, 938, 941, 944, 947, 950,
    952, 955, 958, 960, 962, 965, 967, 969, 971, 973,
    975, 977, 979, 981, 983, 984, 986, 987, 989, 990,
    991, 992, 993, 994, 995, 996, 997, 997, 998, 999,
    999, 999, 1000, 1000, 1000, 1000, 1000, 1000, 1000,
    999, 999, 999, 998, 998, 997, 996, 995, 994, 993,
    992, 991, 990, 989, 987, 986, 984, 983, 981, 979,
    978, 976, 974, 972, 970, 967, 965, 963, 960, 958, 955, 953, 950, 947, 944, 941, 938, 935,
    932, 929, 926, 922, 919, 916, 912, 908, 905, 901, 897, 893, 890, 886, 882, 878, 873, 869,
    865, 861, 856, 852, 847, 843, 838, 833, 829, 824, 819, 814, 809, 804, 799, 794, 789, 784,
    779, 774, 768, 763, 758, 752, 747, 741, 736, 730, 725, 719, 714, 708, 702, 696, 691, 685, 679,
    673, 667, 661, 655, 649, 643, 637, 631, 625, 619, 613, 607, 601, 594, 588, 582, 576, 570, 563,
    557, 551, 545, 538, 532, 526, 520, 513, 507, 501, 495, 488, 482, 476, 469, 463, 457, 451, 444,
    438, 432, 426, 420, 413, 407, 401, 395, 389, 383, 376, 370, 364, 358, 352, 346, 340, 334, 329,
    323, 317, 311, 305, 299, 294, 288, 282, 277, 271, 265, 260, 254, 249, 244, 238, 233, 228, 222,
    217, 212, 207, 202, 197, 192, 187, 182, 177, 172, 168, 163, 158, 154, 149, 145, 141, 136, 132, 128, 124,
    119, 115, 111, 107, 104, 100, 96, 92, 89, 85, 82, 78, 75, 72, 69, 65, 62, 59, 56, 54, 51, 48, 45, 43, 40, 38,
    36, 33, 31, 29, 27, 25, 23, 21, 19, 18, 16, 14, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6, 8, 9, 10, 11,
    12, 14, 15, 17, 19, 20, 22, 24, 26, 28, 30, 32, 35, 37, 39, 42, 44, 47, 50, 52, 55, 58, 61, 64, 67, 71, 74, 77, 81, 84, 87, 91, 95, 98, 102, 106, 110, 114, 118, 122,
    126, 130, 135, 139, 143, 148, 152, 157, 161, 166, 171, 175, 180, 185, 190, 195, 200, 205, 210, 215, 220, 226, 231, 236, 242, 247, 252, 258, 263, 269, 274, 280, 286,
    291, 297, 303, 309, 315, 320, 326, 332, 338, 344, 350, 356, 362, 368, 374, 380, 386, 392, 399, 405, 411, 417, 423, 430, 436, 442, 448, 455, 461, 467, 473, 480, 486, 492
};

int main(void)
{
    bflb_platform_init(0);
    dac_register(DAC0_INDEX, "dac");
    struct device *dac = device_find("dac");

    if (dac) {
        device_open(dac, DEVICE_OFLAG_STREAM_TX);
        MSG("device find success\r\n");
    }

    while (1) {
        device_write(dac, DAC_CHANNEL_0, sinList, sizeof(sinList) / sizeof(uint16_t));
    }
}
