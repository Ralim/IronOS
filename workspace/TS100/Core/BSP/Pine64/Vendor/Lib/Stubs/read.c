/* See LICENSE of license details. */
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>


ssize_t _read(int fd, void* ptr, size_t len) {
	return -1;
}
