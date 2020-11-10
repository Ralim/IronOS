/* See LICENSE of license details. */
#include <errno.h>
#include <time.h>
#include <sys/time.h>

extern int _gettimeofday(struct timeval *tp, void *tzp);

/* Get current value of CLOCK and store it in tp.  */
 int clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    struct timeval tv;
    int retval = -1;

    retval = _gettimeofday(&tv, NULL);
    if (retval == 0) {
        TIMEVAL_TO_TIMESPEC(&tv, tp);
    }

    return retval;
}
