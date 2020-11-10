
/* See LICENSE of license details. */
#include <errno.h>

#undef errno
extern int errno;

 int _unlink(const char *name)
{
    return -1;
}
