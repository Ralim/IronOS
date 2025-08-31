/* Includes */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>

/* Functions */
void initialise_monitor_handles() {}

/* Syscalls (stub implementations to avoid compile warnings and possibe future problems) */
int _getpid(void) { return 1; }

#if defined(MODEL_Pinecil) || defined(MODEL_Pinecilv2)
// do nothing here because some stubs and real implementations added for Pinecils already
#else
off_t   _lseek(int fd, off_t ptr, int dir) { return -1; }
ssize_t _read(int fd, void *ptr, size_t len) { return -1; }
ssize_t _write(int fd, const void *ptr, size_t len) { return -1; }
int     _close(int fd) { return -1; }
#endif
