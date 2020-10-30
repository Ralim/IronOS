/* See LICENSE of license details. */
#include <errno.h>

#undef errno
extern int errno;

 int fork(void)
{
    errno = EAGAIN;
    return -1;
}
