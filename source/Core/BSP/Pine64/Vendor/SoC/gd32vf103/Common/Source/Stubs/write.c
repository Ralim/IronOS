/* See LICENSE of license details. */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <nuclei_sdk_hal.h>
#include "gd32vf103_usart.h"

ssize_t _write(int fd, const void* ptr, size_t len) {
	return -1;
}
