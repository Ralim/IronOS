/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "main_functions.h"
#include "bflb_platform.h"
#include "hal_uart.h"

/// Global Destructor for C++, which we're not using.
/// See https://alex-robenko.gitbook.io/bare_metal_cpp/compiler_output/static#custom-destructors
void *__dso_handle = NULL;

int main()
{
    bflb_platform_init(0);
    setup();
    printf("model load successfully!!\n");

    while (1) {
        loop();
    }

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
