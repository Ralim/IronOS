/* See LICENSE of license details. */

#include "gd32vf103_usart.h"
#include <errno.h>
#include <nuclei_sdk_hal.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

ssize_t _write(int fd, const void *ptr, size_t len) { return -1; }
