// intercept routine for malloc calls.

#include <cstddef>

#include "dmalloc.h"

extern "C" void * calloc(std::size_t count, std::size_t size)
{
  return dmalloc_calloc(count, size);
}

extern "C" void free(void *ptr)
{
  dmalloc_free(ptr);
}

extern "C" void * malloc(std::size_t size)
{
  return dmalloc_malloc(size);
}

extern "C" void * realloc(void *ptr, std::size_t size)
{
  return dmalloc_realloc(ptr, size);
}
