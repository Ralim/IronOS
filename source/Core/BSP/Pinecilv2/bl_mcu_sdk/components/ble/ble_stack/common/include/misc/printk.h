/* printk.h - low-level debug output */

/*
 * Copyright (c) 2010-2012, 2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _PRINTK_H_
#define _PRINTK_H_

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#include <zephyr.h>

#ifdef __cplusplus
extern "C" {
#endif

#define snprintk snprintf
#define printk   printf

#ifdef __cplusplus
}
#endif

#endif
