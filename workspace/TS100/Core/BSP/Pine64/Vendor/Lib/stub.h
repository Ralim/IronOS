#ifdef __cplusplus
extern "C" {
#endif
/* See LICENSE of license details. */
#ifndef _NUCLEI_SYS_STUB_H
#define _NUCLEI_SYS_STUB_H

#include <stdint.h>
#include <unistd.h>

void write_hex(int fd, unsigned long int hex);

static inline int _stub(int err) {
  return -1;
}

void _exit(int code);

#endif /* _NUCLEI_SYS_STUB_H */
#ifdef __cplusplus
}
#endif