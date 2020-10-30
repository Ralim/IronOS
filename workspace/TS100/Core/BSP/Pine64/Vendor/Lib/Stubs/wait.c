/* See LICENSE of license details. */
#include <sys/stat.h>
#include <errno.h>

#undef errno
extern int errno;

 int _wait(int *status)
{
    errno = ECHILD;
    return -1;
}
