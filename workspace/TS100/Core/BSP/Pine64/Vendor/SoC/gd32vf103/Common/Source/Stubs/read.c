#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include "nuclei_sdk_hal.h"
#include "gd32vf103_usart.h"

// #define UART_AUTO_ECHO

ssize_t _read(int fd, void* ptr, size_t len) {
	return -1;
}
