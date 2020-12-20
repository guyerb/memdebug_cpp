/* dmalloc: a simple malloc debugging library.
 *  - gather stats on usage
 *  - print those stats to stderr opportunistically
 */
#include <cstdlib>
#include <ctime>

#include "dmalloc_log.h"
#include "dmalloc_cookie.h"
#include "dmalloc_stat.h"
#include "libc_wrapper.h"

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

void * dmalloc_calloc(size_t count, size_t size)
{
  void *ptr = NULL;
  std::time_t now = time(NULL);
  dmalloc_cookie cookie;

  dputc('c');
  ptr = libc_calloc_wrapper(count, size + cookie.size());
  if (ptr) {
    ptr = cookie.cookie(ptr, now, size);
    gstat.s_alloc(size, now);
  }
  return ptr;
}

void dmalloc_free(void *ptr)
{
  std::time_t now = std::time(NULL);
  dmalloc_cookie cookie;

  dputc('f');
  if (cookie.ours(ptr)) {
    gstat.s_free(cookie.bytes(ptr), now, cookie.birthday(ptr));
    ptr = cookie.base(ptr);
  }
  libc_free_wrapper(ptr);
  return;
}

void * dmalloc_malloc(size_t size)
{
  dmalloc_cookie cookie;
  std::time_t now = time(NULL);
  void *ptr;

  dputc('m');
  ptr = libc_malloc_wrapper(size + cookie.size());
  if (ptr) {
    ptr = cookie.cookie(ptr, now, size);
    gstat.s_alloc(size, now);
  }
  return ptr;
}

void * dmalloc_realloc(void *ptr, size_t size)
{
  void *p;
  std::time_t now = std::time(NULL);
  dmalloc_cookie cookie;

  dputc('r');
  if (ptr && cookie.ours(ptr)) {
    gstat.s_free(cookie.bytes(ptr), now, cookie.birthday(ptr));
    ptr = cookie.base(ptr);
  }

  p = libc_realloc_wrapper(ptr, size + cookie.size());
  if (p) {
    p = cookie.cookie(p, now, size);
    gstat.s_alloc(size, now);
  }
  return p;
}

#endif // DMALLOC_PASSTHROUGH

