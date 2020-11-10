/* See LICENSE of license details. */

#include "n200_func.h"
 void _exit(int fd)
{
    while(1) {
        __WFI();
    }
}
