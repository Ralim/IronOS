/**
 * @file shell.h
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
#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "shell_config.h"
#include "bflb_platform.h"

#ifdef SHELL_USING_COLOR
/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */
#define _SHELL_COLOR_HDR(n) shell->shell_printf("\033[" #n "m")
#define _SHELL_COLOR_END    shell->shell_printf("\033[0m")

#define shell_print(color_n, fmt, ...)           \
    do {                                         \
        _SHELL_COLOR_HDR(color_n);               \
        shell->shell_printf(fmt, ##__VA_ARGS__); \
        _SHELL_COLOR_END;                        \
    } while (0)

#define SHELL_PRINTF(fmt, ...) shell_print(0, fmt, ##__VA_ARGS__)
#define SHELL_PROMPT(fmt, ...) shell_print(34, fmt, ##__VA_ARGS__)
#define SHELL_DGB(fmt, ...)    shell_print(32, fmt, ##__VA_ARGS__)
#define SHELL_CMD(fmt, ...)    shell_print(33, fmt, ##__VA_ARGS__)
#define SHELL_E(fmt, ...)      shell_print(31, fmt, ##__VA_ARGS__)

#else
#define SHELL_PRINTF(fmt, ...) shell->shell_printf(fmt, ##__VA_ARGS__)
#define SHELL_PROMPT(fmt, ...) shell->shell_printf(fmt, ##__VA_ARGS__)
#define SHELL_DGB(fmt, ...)    shell->shell_printf(fmt, ##__VA_ARGS__)
#define SHELL_CMD(fmt, ...)    shell->shell_printf(fmt, ##__VA_ARGS__)
#define SHELL_E(fmt, ...)      shell->shell_printf(fmt, ##__VA_ARGS__)
#endif

#define SHELL_MALLOC malloc
#define SHELL_FREE   free

typedef int (*syscall_func)(void);
typedef int (*cmd_function_t)(int argc, char **argv);

enum input_stat {
    WAIT_NORMAL,
    WAIT_SPEC_KEY,
    WAIT_FUNC_KEY,
};

struct shell {
    enum input_stat stat;

    uint16_t current_history;
    uint16_t history_count;
    char cmd_history[SHELL_HISTORY_LINES][SHELL_CMD_SIZE];

    char line[SHELL_CMD_SIZE];
    uint16_t line_position;
    uint16_t line_curpos;

#ifdef SHELL_USING_AUTH
    char password[SHELL_PASSWORD_MAX];
#endif

#if defined(SHELL_USING_FS)

#endif
    void (*shell_printf)(char *fmt, ...);
};

/* system call table */
struct shell_syscall {
    const char *name; /* the name of system call */
#if defined(SHELL_USING_DESCRIPTION)
    const char *desc; /* description of system call */
#endif
    syscall_func func; /* the function address of system call */
};

/* system variable table */
struct shell_sysvar {
    const char *name; /* the name of variable */
#if defined(SHELL_USING_DESCRIPTION)
    const char *desc; /* description of system variable */
#endif
    uint8_t type; /* the type of variable */
    void *var;    /* the address of variable */
};

#ifdef SHELL_USING_DESCRIPTION
#define SHELL_FUNCTION_EXPORT_CMD(name, cmd, desc)                                                        \
    const char __fsym_##cmd##_name[] __attribute__((section(".rodata.name"))) = #cmd;                     \
    const char __fsym_##cmd##_desc[] __attribute__((section(".rodata.name"))) = #desc;                    \
    __attribute__((used)) const struct shell_syscall __fsym_##cmd __attribute__((section("FSymTab"))) = { \
        __fsym_##cmd##_name,                                                                              \
        __fsym_##cmd##_desc,                                                                              \
        (syscall_func)&name                                                                               \
    };

#define SHELL_VAR_EXPORT(name, type, desc)                                                                \
    const char __vsym_##name##_name[] __attribute__((section(".rodata.name"))) = #name;                   \
    const char __vsym_##name##_desc[] __attribute__((section(".rodata.name"))) = #desc;                   \
    __attribute__((used)) const struct shell_sysvar __vsym_##name __attribute__((section("VSymTab"))) = { \
        __vsym_##name##_name,                                                                             \
        __vsym_##name##_desc,                                                                             \
        type,                                                                                             \
        (void *)&name                                                                                     \
    };
#else
#define SHELL_FUNCTION_EXPORT_CMD(name, cmd, desc)                                                        \
    const char __fsym_##cmd##_name[] = #cmd;                                                              \
    __attribute__((used)) const struct shell_syscall __fsym_##cmd __attribute__((section("FSymTab"))) = { \
        __fsym_##cmd##_name,                                                                              \
        (syscall_func)&name                                                                               \
    };

#define SHELL_VAR_EXPORT(name, type, desc)                                                                \
    const char __vsym_##name##_name[] = #name;                                                            \
    __attribute__((used)) const struct shell_sysvar __vsym_##name __attribute__((section("VSymTab"))) = { \
        __vsym_##name##_name,                                                                             \
        type,                                                                                             \
        (void *)&name                                                                                     \
    };
#endif /* end of SHELL_USING_DESCRIPTION */

/**
 * @ingroup shell
 *
 * This macro exports a command to module shell.
 *
 * @param command the name of command.
 * @param desc the description of command, which will show in help.
 */
#define SHELL_CMD_EXPORT(command, desc) \
    SHELL_FUNCTION_EXPORT_CMD(command, __cmd_##command, desc)
#define SHELL_CMD_EXPORT_ALIAS(command, alias, desc) \
    SHELL_FUNCTION_EXPORT_CMD(command, __cmd_##alias, desc)

void shell_handler(uint8_t data);
int shell_set_prompt(const char *prompt);
int shell_set_print(void (*shell_printf)(char *fmt, ...));
void shell_init(void);
#endif