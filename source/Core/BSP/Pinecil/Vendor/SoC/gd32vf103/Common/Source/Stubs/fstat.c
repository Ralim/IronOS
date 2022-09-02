#include "stub.h"
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

int _fstat(int fd, struct stat *st) { return _stub(EBADF); }
