/* See LICENSE of license details. */
#include <errno.h>
#include <time.h>
#include <stdint.h>
#include "system_gd32vf103.h"
/* Get resolution of clock. */
 int clock_getres(clockid_t clock_id, struct timespec *res)
{
    res->tv_sec = 0;
    res->tv_nsec = 1000000000 / SystemCoreClock;

    return 0;
}
