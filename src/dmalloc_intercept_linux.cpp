// intercept routine for malloc calls.

#include "dmalloc.h"

void * calloc(std::size_t count, std::size_t size)
{
  return dmalloc_calloc(count, size);
}

void free(void *ptr)
{
  dmalloc_free(ptr);
}

void * malloc(std::size_t size)
{
  return dmalloc_malloc(size);
}

void * realloc(void *ptr, std::size_t size)
{
  return dmalloc_realloc(ptr, size);
}
