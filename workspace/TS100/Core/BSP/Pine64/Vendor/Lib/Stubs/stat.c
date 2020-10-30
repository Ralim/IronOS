/* See LICENSE of license details. */
#include <sys/stat.h>

 int _stat(char *file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}
