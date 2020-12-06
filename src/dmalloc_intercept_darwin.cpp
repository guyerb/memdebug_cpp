#include <cstdlib>
#include "dmalloc.h"

/* from dyld-interposing.h */
#define DYLD_INTERPOSE(_replacment,_replacee)                           \
  __attribute__((used)) static struct{                                  \
    const void* replacment;                                             \
    const void* replacee;                                               \
  } _interpose_##_replacee __attribute__ ((section ("__DATA,__interpose"))) = { \
    (const void*)(unsigned long)&_replacment,                           \
    (const void*)(unsigned long)&_replacee                              \
  };

DYLD_INTERPOSE(dmalloc_calloc, calloc)
DYLD_INTERPOSE(dmalloc_free, free)
DYLD_INTERPOSE(dmalloc_malloc, malloc)
DYLD_INTERPOSE(dmalloc_realloc, realloc)
  
