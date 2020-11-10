/* See LICENSE of license details. */
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>

extern int _gettimeofday(struct timeval *, void *);

 clock_t _times(struct tms *buf)
{
    static struct timeval t0;
    struct timeval t;
    long long utime;

    /* When called for the first time, initialize t0. */
    if (t0.tv_sec == 0 && t0.tv_usec == 0) {
        _gettimeofday(&t0, 0);
    }

    _gettimeofday(&t, 0);

    utime = (t.tv_sec - t0.tv_sec) * 1000000 + (t.tv_usec - t0.tv_usec);
    buf->tms_utime = utime * CLOCKS_PER_SEC / 1000000;
    buf->tms_stime = buf->tms_cstime = buf->tms_cutime = 0;

    return buf->tms_utime;
}
