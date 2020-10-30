/* See LICENSE of license details. */
#include <errno.h>

#undef errno
extern int errno;

int _link(char *old, char *new)
{
    errno = EMLINK;
    return -1;
}
