/* Includes */
#include <errno.h>
#include <stdio.h>

/* Variables */
extern int errno;
register char *stack_ptr asm("sp");

/* Functions */

/**
 _sbrk
 Increase program data space. Malloc and related functions depend on this
 **/
caddr_t _sbrk(int incr) {
	return (void*) -1;
}

