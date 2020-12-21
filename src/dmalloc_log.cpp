#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "dmalloc_log.h"

void dprintf( const char* format, ... )
{
  va_list args;

  va_start(args, format);
#ifdef LINUX
  vfprintf(stderr, format, args);
#else
  malloc_printf(format, args);
#endif
  va_end( args );
}
