#include "stub.h"
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

off_t _lseek(int fd, off_t ptr, int dir) { return _stub(EBADF); }
