/* dmalloc: a simple malloc debugging library.
 *  - gather stats on usage
 *  - print those stats to stderr opportunistically
 */
#include <cstdlib>

#include "dmalloc_logs.h"
#include "dmalloc_cookie.h"
#include "libc_wrappers.h
#ifdef DMALLOC_PASSTHROUGH
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
#else  // DMALLOC_PASSTHROUGH

void * dmalloc_calloc_intercept(size_t count, size_t size)
{
  void *ptr = NULL;
  time_t now = time(NULL);
  dmalloc_cookie cookie;

  dputc('c', stderr);
  ptr = libc_calloc_wrapper(count, size + DSIZE);
  if (ptr) {
    ptr = cookie.cookie(ptr, now, size);
    dmalloc_stats_alloc(size, now);
  }
  return ptr;
}

void dmalloc_free_intercept(void *ptr)
{
  dmalloc_cookie cookie;

  dputc('f', stderr);
  if (cookie.ours(ptr)) {
    dmalloc_stats_free(dmalloc_size_get(ptr), time(NULL), \
		       dmalloc_birthday_get(ptr));
    ptr = cookie.base(ptr);
  }
  libc_free_wrapper(ptr);
  return;
}

void * dmalloc_malloc_intercept(size_t size)
{
  dmalloc_cookie cookie;
  time_t now = time(NULL);
  void *ptr;

  dputc('m', stderr);
  ptr = libc_malloc_wrapper(size + DSIZE);
  if (ptr) {
    ptr = cookie.cookie(ptr, now, size);
    dmalloc_stats_alloc(size, now);
  }
  return ptr;
}

void * dmalloc_realloc_intercept(void *ptr, size_t size)
{
  void *p;
  time_t now = time(NULL);
  dmalloc_cookie cookie;

  dputc('r', stderr);
  if (ptr && dmalloc_ptr_ours(ptr)) {
    dmalloc_stats_free(cookie.bytes(ptr), time(NULL),	\
		       cookie.birthday(ptr));
    ptr = cookie.base(ptr);
  }

  p = libc_realloc_wrapper(ptr, size + cookies.size());
  if (p) {
    p = cookie.cookie(p, now, size);
    dmalloc_stats_alloc(size, now);
  }
  return p;
}

#endif // DMALLOC_PASSTHROUGH

