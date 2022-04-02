/**
 * @file shell.c
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
#include "shell.h"
#include <ctype.h>
#if defined(SHELL_USING_FS)
#include "ff.h"
#endif

struct shell_syscall *_syscall_table_begin = NULL;
struct shell_syscall *_syscall_table_end = NULL;
struct shell_sysvar *_sysvar_table_begin = NULL;
struct shell_sysvar *_sysvar_table_end = NULL;

struct shell _shell;
static struct shell *shell;
static char *shell_prompt_custom = NULL;

int shell_help(int argc, char **argv)
{
    SHELL_DGB("shell commands list:\r\n");
    {
        struct shell_syscall *index;

        for (index = _syscall_table_begin; index < _syscall_table_end; index++) {
            if (strncmp(index->name, "__cmd_", 6) != 0) {
                continue;
            }

#if defined(SHELL_USING_DESCRIPTION)
            SHELL_DGB("%-16s - %s\r\n", &index->name[6], index->desc);
#else
            SHELL_DGB("%s\r\n", &index->name[6]);
#endif
        }
    }
    SHELL_DGB("\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_help, help, shell help.);

static int shell_memtrace(int argc, char **argv)
{
    uint32_t addr, value;

    /* check args */
    if (argc < 2) {
        SHELL_DGB("write memory: 0x42000000 0xabcd 10\r\n");
        SHELL_DGB("read memory: 0x42000000 10\r\n");
        return 0;
    }

    /* get address */
    addr = strtoll(argv[1], NULL, 16);

    if (argc < 3) {
        /* read word */
        SHELL_DGB("0x%08x\r\n", *(volatile uint32_t *)addr);
        return 0;
    }

    if (argc < 4) {
        uint16_t count = atoi(argv[2]);
        for (int i = 0; i < count; i++) {
            /* read word */
            SHELL_DGB("0x%08x\r\n", *(volatile uint32_t *)(addr + 4 * i));
        }

        return 0;
    }

    if (argc < 5) {
        uint16_t count = atoi(argv[3]);
        /* write value */
        value = strtoll(argv[2], NULL, 16);

        for (int i = 0; i < count; i++) {
            *(volatile uint32_t *)(addr + 4 * i) = (uint32_t)value;
        }
        return 0;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_memtrace, memtrace, memory trace.);

static char *shell_get_prompt(void)
{
    static char shell_prompt[SHELL_CONSOLEBUF_SIZE + 1] = { 0 };

    if (shell_prompt_custom) {
        strcpy(shell_prompt, shell_prompt_custom);
    } else {
        strcpy(shell_prompt, SHELL_DEFAULT_NAME);
    }
#if defined(SHELL_USING_FS)
    /* get current working directory */
    f_getcwd(&shell_prompt[strlen(shell_prompt)],
             SHELL_CONSOLEBUF_SIZE - strlen(shell_prompt));
#endif

    strcat(shell_prompt, "/>");

    return shell_prompt;
}

static int str_common(const char *str1, const char *str2)
{
    const char *str = str1;

    while ((*str != 0) && (*str2 != 0) && (*str == *str2)) {
        str++;
        str2++;
    }

    return (str - str1);
}

static void shell_handle_history(struct shell *shell)
{
    SHELL_PRINTF("\033[2K\r");
    SHELL_PROMPT("%s", shell_get_prompt());
    SHELL_PRINTF("%s", shell->line);
}

static void shell_push_history(struct shell *shell)
{
    if (shell->line_position != 0) {
        /* push history */
        if (shell->history_count >= SHELL_HISTORY_LINES) {
            /* if current cmd is same as last cmd, don't push */
            if (memcmp(&shell->cmd_history[SHELL_HISTORY_LINES - 1], shell->line,
                       SHELL_CMD_SIZE)) {
                /* move history */
                int index;

                for (index = 0; index < SHELL_HISTORY_LINES - 1; index++) {
                    memcpy(&shell->cmd_history[index][0],
                           &shell->cmd_history[index + 1][0], SHELL_CMD_SIZE);
                }

                memset(&shell->cmd_history[index][0], 0, SHELL_CMD_SIZE);
                memcpy(&shell->cmd_history[index][0], shell->line,
                       shell->line_position);

                /* it's the maximum history */
                shell->history_count = SHELL_HISTORY_LINES;
            }
        } else {
            /* if current cmd is same as last cmd, don't push */
            if (shell->history_count == 0 ||
                memcmp(&shell->cmd_history[shell->history_count - 1], shell->line,
                       SHELL_CMD_SIZE)) {
                shell->current_history = shell->history_count;
                memset(&shell->cmd_history[shell->history_count][0], 0, SHELL_CMD_SIZE);
                memcpy(&shell->cmd_history[shell->history_count][0], shell->line,
                       shell->line_position);

                /* increase count and set current history position */
                shell->history_count++;
            }
        }
    }

    shell->current_history = shell->history_count;
}

#ifdef SHELL_USING_FS

void shell_auto_complete_path(char *path)
{
    DIR dir;
    FILINFO fno;
    char *full_path, *ptr, *index;
    char str_buff[256];

    if (!path)
        return;

    full_path = str_buff;

    // if (*path != '/') {
    //     f_getcwd(full_path, 256);
    //     if (full_path[strlen(full_path) - 1] != '/')
    //         strcat(full_path, "/");
    // } else
    *full_path = '\0';

    index = NULL;
    ptr = path;
    for (;;) {
        if (*ptr == '/')
            index = ptr + 1;
        if (!*ptr)
            break;

        ptr++;
    }
    if (index == NULL)
        index = path;

    if (index != NULL) {
        char *dest = index;

        /* fill the parent path */
        ptr = full_path;
        while (*ptr)
            ptr++;

        for (index = path; index != dest;)
            *ptr++ = *index++;
        *(ptr - 1) = '\0';

        if (f_opendir(&dir, full_path)) /* open directory failed! */
        {
            return;
        }

        /* restore the index position */
        index = dest;
    }

    /* auto complete the file or directory name */
    if (*index == '\0') /* display all of files and directories */
    {
        f_rewinddir(&dir);
        for (;;) {
            f_readdir(&dir, &fno);
            if (fno.fname[0] == '\0')
                break;

            SHELL_PRINTF("%s%s%s\r\n", fno.fname, (fno.fattrib & AM_DIR) ? "/" : "", (fno.fattrib & AM_HID) ? "(Hidden)" : "");
        }
    } else {
        uint32_t length, min_length = 0;

        f_rewinddir(&dir);
        for (;;) {
            f_readdir(&dir, &fno);
            if (fno.fname[0] == '\0')
                break;

            /* matched the prefix string */
            if (strncmp(index, fno.fname, strlen(index)) == 0) {
                if (min_length == 0) {
                    min_length = strlen(fno.fname);
                    /* save dirent name */
                    strcpy(full_path, fno.fname);
                }

                length = str_common(fno.fname, full_path);

                if (length < min_length) {
                    min_length = length;
                }
            }
        }

        if (min_length) {
            if (min_length < strlen(full_path)) {
                /* list the candidate */
                f_rewinddir(&dir);
                for (;;) {
                    f_readdir(&dir, &fno);
                    if (fno.fname[0] == '\0')
                        break;

                    if (strncmp(index, fno.fname, strlen(index)) == 0)
                        SHELL_PRINTF("%s%s%s\r\n", fno.fname, (fno.fattrib & AM_DIR) ? "/" : "", (fno.fattrib & AM_HID) ? "(Hidden)" : "");
                }
            }

            length = index - path;
            memcpy(index, full_path, min_length);
            path[length + min_length] = '\0';
        }
    }

    f_closedir(&dir);
}
#endif

static void shell_auto_complete(char *prefix)
{
    int length, min_length;
    const char *name_ptr, *cmd_name;
    struct shell_syscall *index;

    min_length = 0;
    name_ptr = NULL;

    SHELL_PRINTF("\r\n");

    if (*prefix == '\0') {
        shell_help(0, NULL);
        return;
    }

#ifdef SHELL_USING_FS
    /* check whether a spare in the command */
    {
        char *ptr;

        ptr = prefix + strlen(prefix);

        while (ptr != prefix) {
            if (*ptr == ' ') {
                shell_auto_complete_path(ptr + 1);
                break;
            }

            ptr--;
        }
    }
#endif

    /* checks in internal command */
    {
        for (index = _syscall_table_begin; index < _syscall_table_end; index++) {
            /* skip finsh shell function */
            if (strncmp(index->name, "__cmd_", 6) != 0) {
                continue;
            }

            cmd_name = (const char *)&index->name[6];

            if (strncmp(prefix, cmd_name, strlen(prefix)) == 0) {
                if (min_length == 0) {
                    /* set name_ptr */
                    name_ptr = cmd_name;
                    /* set initial length */
                    min_length = strlen(name_ptr);
                }

                length = str_common(name_ptr, cmd_name);

                if (length < min_length) {
                    min_length = length;
                }

                SHELL_CMD("%s\r\n", cmd_name);
            }
        }
    }

    /* auto complete string */
    if (name_ptr != NULL) {
        strncpy(prefix, name_ptr, min_length);
    }

    SHELL_PROMPT("%s", shell_get_prompt());
    SHELL_PRINTF("%s", prefix);
    return;
}

static int shell_split(char *cmd, uint32_t length, char *argv[SHELL_ARG_NUM])
{
    char *ptr;
    uint32_t position;
    uint32_t argc;
    uint32_t i;

    ptr = cmd;
    position = 0;
    argc = 0;

    while (position < length) {
        /* strip bank and tab */
        while ((*ptr == ' ' || *ptr == '\t') && position < length) {
            *ptr = '\0';
            ptr++;
            position++;
        }

        if (argc >= SHELL_ARG_NUM) {
            SHELL_E("Too many args ! We only Use:\r\n");

            for (i = 0; i < argc; i++) {
                SHELL_E("%s ", argv[i]);
            }

            SHELL_E("\r\n");
            break;
        }

        if (position >= length) {
            break;
        }

        /* handle string */
        if (*ptr == '"') {
            ptr++;
            position++;
            argv[argc] = ptr;
            argc++;

            /* skip this string */
            while (*ptr != '"' && position < length) {
                if (*ptr == '\\') {
                    if (*(ptr + 1) == '"') {
                        ptr++;
                        position++;
                    }
                }

                ptr++;
                position++;
            }

            if (position >= length) {
                break;
            }

            /* skip '"' */
            *ptr = '\0';
            ptr++;
            position++;
        } else {
            argv[argc] = ptr;
            argc++;

            while ((*ptr != ' ' && *ptr != '\t') && position < length) {
                ptr++;
                position++;
            }

            if (position >= length) {
                break;
            }
        }
    }

    return argc;
}

static cmd_function_t shell_get_cmd(char *cmd, int size)
{
    struct shell_syscall *index;
    cmd_function_t cmd_func = NULL;

    for (index = _syscall_table_begin; index < _syscall_table_end; index++) {
        if (strncmp(index->name, "__cmd_", 6) != 0) {
            continue;
        }

        if (strncmp(&index->name[6], cmd, size) == 0 &&
            index->name[6 + size] == '\0') {
            cmd_func = (cmd_function_t)index->func;
            break;
        }
    }

    return cmd_func;
}

static int shell_exec_cmd(char *cmd, uint32_t length, int *retp)
{
    int argc;
    uint32_t cmd0_size = 0;
    cmd_function_t cmd_func;
    char *argv[SHELL_ARG_NUM];

    // ASSERT(cmd);
    // ASSERT(retp);

    /* find the size of first command */
    while ((cmd[cmd0_size] != ' ' && cmd[cmd0_size] != '\t') &&
           cmd0_size < length) {
        cmd0_size++;
    }

    if (cmd0_size == 0) {
        return -1;
    }

    cmd_func = shell_get_cmd(cmd, cmd0_size);

    if (cmd_func == NULL) {
        return -1;
    }

    /* split arguments */
    memset(argv, 0x00, sizeof(argv));
    argc = shell_split(cmd, length, argv);

    if (argc == 0) {
        return -1;
    }

    /* exec this command */
    *retp = cmd_func(argc, argv);
    return 0;
}

#if defined(SHELL_USING_LWIP) && defined(SHELL_USING_FS)
static int shell_exec_lwp(char *cmd, uint32_t length)
{
    int argc;
    int cmd0_size = 0;
    char *argv[SHELL_ARG_NUM];
    int fd = -1;
    char *pg_name;

    extern int exec(char *, int, char **);

    /* find the size of first command */
    while ((cmd[cmd0_size] != ' ' && cmd[cmd0_size] != '\t') &&
           cmd0_size < length) {
        cmd0_size++;
    }

    if (cmd0_size == 0) {
        return -1;
    }

    /* split arguments */
    rt_memset(argv, 0x00, sizeof(argv));
    argc = msh_split(cmd, length, argv);

    if (argc == 0) {
        return -1;
    }

    pg_name = argv[0];
    /* try to open program */
    fd = open(pg_name, O_RDONLY, 0);

    if (fd < 0) {
        return -1;
    }

    /* found program */
    close(fd);
    exec(pg_name, argc, argv);

    return 0;
}
#endif

int shell_exec(char *cmd, uint32_t length)
{
    int cmd_ret;

    /* strim the beginning of command */
    while (*cmd == ' ' || *cmd == '\t') {
        cmd++;
        length--;
    }

    if (length == 0) {
        return 0;
    }

    /* Exec sequence:
   * 1. built-in command
   * 2. module(if enabled)
   */
    if (shell_exec_cmd(cmd, length, &cmd_ret) == 0) {
        return cmd_ret;
    }

#ifdef SHELL_USING_FS
    // extern int shell_exec_script(char *cmd, uint32_t length);

    // if (shell_exec_script(cmd, length) == 0) {
    //     return 0;
    // }

#endif

#ifdef SHELL_USING_LWIP

    if (shell_exec_lwp(cmd, length) == 0) {
        return 0;
    }

#endif

    /* truncate the cmd at the first space. */
    {
        char *tcmd;
        tcmd = cmd;

        while (*tcmd != ' ' && *tcmd != '\0') {
            tcmd++;
        }

        *tcmd = '\0';
    }
    SHELL_E("%s: command not found.\r\n", cmd);
    return -1;
}

void shell_handler(uint8_t data)
{
    /*
   * handle control key
   * up key  : 0x1b 0x5b 0x41
   * down key: 0x1b 0x5b 0x42
   * right key:0x1b 0x5b 0x43
   * left key: 0x1b 0x5b 0x44
   */
    if (data == 0x1b) {
        shell->stat = WAIT_SPEC_KEY;
        return;
    } else if (shell->stat == WAIT_SPEC_KEY) {
        if (data == 0x5b) {
            shell->stat = WAIT_FUNC_KEY;
            return;
        }

        shell->stat = WAIT_NORMAL;
    } else if (shell->stat == WAIT_FUNC_KEY) {
        shell->stat = WAIT_NORMAL;

        if (data == 0x41) /* up key */
        {
            /* prev history */
            if (shell->current_history > 0) {
                shell->current_history--;
            } else {
                shell->current_history = 0;
                return;
            }

            /* copy the history command */
            memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
                   SHELL_CMD_SIZE);
            shell->line_curpos = shell->line_position = strlen(shell->line);
            shell_handle_history(shell);

            return;
        } else if (data == 0x42) /* down key */
        {
            /* next history */
            if (shell->current_history < shell->history_count - 1) {
                shell->current_history++;
            } else {
                /* set to the end of history */
                if (shell->history_count != 0) {
                    shell->current_history = shell->history_count - 1;
                } else {
                    return;
                }
            }

            memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
                   SHELL_CMD_SIZE);
            shell->line_curpos = shell->line_position = strlen(shell->line);
            shell_handle_history(shell);

            return;
        } else if (data == 0x44) /* left key */
        {
            if (shell->line_curpos) {
                SHELL_PRINTF("\b");
                shell->line_curpos--;
            }

            return;
        } else if (data == 0x43) /* right key */
        {
            if (shell->line_curpos < shell->line_position) {
                SHELL_PRINTF("%c", shell->line[shell->line_curpos]);
                shell->line_curpos++;
            }
            return;
        }
    }

    /* received null or error */
    if (data == '\0' || data == 0xFF) {
        return;
    }
    /* handle tab key */
    else if (data == '\t') {
        int i;

        /* move the cursor to the beginning of line */
        for (i = 0; i < shell->line_curpos; i++) {
            SHELL_PRINTF("\b");
        }

        /* auto complete */
        shell_auto_complete(&shell->line[0]);
        /* re-calculate position */
        shell->line_curpos = shell->line_position = strlen(shell->line);

        return;
    }
    /* handle backspace key */
    else if (data == 0x7f || data == 0x08) {
        /* note that shell->line_curpos >= 0 */
        if (shell->line_curpos == 0) {
            return;
        }

        shell->line_position--;
        shell->line_curpos--;

        if (shell->line_position > shell->line_curpos) {
            int i;

            memmove(&shell->line[shell->line_curpos],
                    &shell->line[shell->line_curpos + 1],
                    shell->line_position - shell->line_curpos);
            shell->line[shell->line_position] = 0;

            SHELL_PRINTF("\b%s  \b", &shell->line[shell->line_curpos]);

            /* move the cursor to the origin position */
            for (i = shell->line_curpos; i <= shell->line_position; i++) {
                SHELL_PRINTF("\b");
            }
        } else {
            SHELL_PRINTF("\b \b");
            shell->line[shell->line_position] = 0;
        }

        return;
    }

    /* handle end of line, break */
    if (data == '\r' || data == '\n') {
        shell_push_history(shell);

        SHELL_PRINTF("\r\n");
        shell_exec(shell->line, shell->line_position);

        SHELL_PROMPT(shell_get_prompt());
        memset(shell->line, 0, sizeof(shell->line));
        shell->line_curpos = shell->line_position = 0;
        return;
    }

    /* it's a large line, discard it */
    if (shell->line_position >= SHELL_CMD_SIZE) {
        shell->line_position = 0;
    }

    /* normal character */
    if (shell->line_curpos < shell->line_position) {
        int i;

        memmove(&shell->line[shell->line_curpos + 1],
                &shell->line[shell->line_curpos],
                shell->line_position - shell->line_curpos);
        shell->line[shell->line_curpos] = data;

        SHELL_PRINTF("%s", &shell->line[shell->line_curpos]);

        /* move the cursor to new position */
        for (i = shell->line_curpos; i < shell->line_position; i++) {
            SHELL_PRINTF("\b");
        }
    } else {
        shell->line[shell->line_position] = data;
        SHELL_PRINTF("%c", data);
    }

    data = 0;
    shell->line_position++;
    shell->line_curpos++;

    if (shell->line_position >= SHELL_CMD_SIZE) {
        /* clear command line */
        shell->line_position = 0;
        shell->line_curpos = 0;
    }
}

static void shell_function_init(const void *begin, const void *end)
{
    _syscall_table_begin = (struct shell_syscall *)begin;
    _syscall_table_end = (struct shell_syscall *)end;
}

static void shell_var_init(const void *begin, const void *end)
{
    _sysvar_table_begin = (struct shell_sysvar *)begin;
    _sysvar_table_end = (struct shell_sysvar *)end;
}

int shell_set_prompt(const char *prompt)
{
    if (shell_prompt_custom) {
        SHELL_FREE(shell_prompt_custom);
        shell_prompt_custom = NULL;
    }

    /* strdup */
    if (prompt) {
        shell_prompt_custom = (char *)SHELL_MALLOC(strlen(prompt) + 1);
        if (shell_prompt_custom) {
            strcpy(shell_prompt_custom, prompt);
        }
    }

    return 0;
}

int shell_set_print(void (*shell_printf)(char *fmt, ...))
{
    if (shell_printf) {
        shell->shell_printf = shell_printf;
        return 0;
    } else
        return -1;
}

/*
 * @ingroup shell
 *
 * This function will initialize shell
 */
void shell_init(void)
{
#if defined(__CC_ARM) || defined(__CLANG_ARM) /* ARM C Compiler */
    extern const int FSymTab$$Base;
    extern const int FSymTab$$Limit;
    extern const int VSymTab$$Base;
    extern const int VSymTab$$Limit;
    shell_function_init(&FSymTab$$Base, &FSymTab$$Limit);
    shell_var_init(&VSymTab$$Base, &VSymTab$$Limit);
#elif defined(__ICCARM__) || defined(__ICCRX__) /* for IAR Compiler */
    shell_function_init(__section_begin("FSymTab"), __section_end("FSymTab"));
    shell_var_init(__section_begin("VSymTab"), __section_end("VSymTab"));
#elif defined(__GNUC__)
    /* GNU GCC Compiler and TI CCS */
    extern const int __fsymtab_start;
    extern const int __fsymtab_end;
    extern const int __vsymtab_start;
    extern const int __vsymtab_end;
    shell_function_init(&__fsymtab_start, &__fsymtab_end);
    shell_var_init(&__vsymtab_start, &__vsymtab_end);
#endif
    shell = &_shell;
    shell_set_prompt(SHELL_DEFAULT_NAME);
    shell_set_print(bflb_platform_printf);
}