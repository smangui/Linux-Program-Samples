
#include <errno.h>      // for errno
#include <string.h>     // for strerror
#include <stdio.h>
#include "utils.h"

void ErrorString(const char *str)
{
   printf("%s failed: errno %d: %s\n", str, errno, strerror(errno));
}