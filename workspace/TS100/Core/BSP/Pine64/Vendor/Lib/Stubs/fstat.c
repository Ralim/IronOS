/* See LICENSE of license details. */
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#undef errno
extern int errno;

 int _fstat(int file, struct stat *st)
{
    if ((STDOUT_FILENO == file) || (STDERR_FILENO == file)) {
        st->st_mode = S_IFCHR;
        return 0;
    } else {
        errno = EBADF;
        return -1;
    }
}
