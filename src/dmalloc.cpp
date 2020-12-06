/* dmalloc: a simple malloc debugging library.
 *
 *  - gather stats on usage
 *  - print those stats to stderr opportunistically
 */
#include <cstdlib>

#include "dmalloc_logs.h"
#include "libc_wrappers.h"

void * dmalloc_calloc(size_t count, size_t size)
{
  dputc('c');
  return libc_calloc_wrapper(count, size);
}

void dmalloc_free(void *ptr)
{
  dputc('f');
  libc_free_wrapper(ptr);
  return;
}

void * dmalloc_malloc(size_t size)
{
  dputc('m');
  return libc_malloc_wrapper(size);
}

void * dmalloc_realloc(void *ptr, size_t size)
{
  dputc('r');
  return libc_realloc_wrapper(ptr, size);
}

