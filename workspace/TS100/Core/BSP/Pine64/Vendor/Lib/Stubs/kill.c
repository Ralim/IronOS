/* See LICENSE of license details. */
#include <errno.h>
#undef errno
extern int errno;

 int _kill(int pid, int sig)
{
    errno = EINVAL;
    return -1;
}
