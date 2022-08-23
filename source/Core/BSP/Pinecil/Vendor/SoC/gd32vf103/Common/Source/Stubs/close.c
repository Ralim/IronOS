#include "stub.h"
#include <errno.h>

int _close(int fd) { return _stub(EBADF); }
