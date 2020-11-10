/* See LICENSE of license details. */
#include <errno.h>

#undef errno
extern int errno;

 int _close(int fd)
{
    errno = EBADF;
    return -1;
}
