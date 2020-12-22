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

dmalloc_stat *pstat = nullptr;
void __attribute__ ((constructor)) dmalloc_stat_init(void)
{
  dputc('&');
  pstat = new (dmalloc_stat);
}

void * dmalloc_calloc(size_t count, size_t size)
{
  void *ptr = nullptr;
  std::time_t now = time(nullptr);
  dmalloc_cookie cookie;

  dputc('c');
  ptr = libc_calloc_wrapper(count, size + cookie.size());
  if (pstat && ptr) {
    ptr = cookie.cookie(ptr, now, size);
    pstat->s_alloc(size, now);
  }
  return ptr;
}

void dmalloc_free(void *ptr)
{
  std::time_t now = std::time(nullptr);
  dmalloc_cookie cookie;

  dputc('f');
  if (pstat && ptr && cookie.ours(ptr)) {
    pstat->s_free(cookie.bytes(ptr), now, cookie.birthday(ptr));
    ptr = cookie.base(ptr);
  }
  libc_free_wrapper(ptr);
  return;
}

void * dmalloc_malloc(size_t size)
{
  dmalloc_cookie cookie;
  std::time_t now = time(nullptr);
  void *ptr;

  dputc('m');
  ptr = libc_malloc_wrapper(size + cookie.size());
  if (pstat && ptr) {
    ptr = cookie.cookie(ptr, now, size);
    pstat->s_alloc(size, now);
  }
  return ptr;
}

void * dmalloc_realloc(void *ptr, size_t size)
{
  void *p;
  std::time_t now = std::time(nullptr);
  dmalloc_cookie cookie;

  dputc('r');
  if (pstat && ptr && cookie.ours(ptr)) {
    pstat->s_free(cookie.bytes(ptr), now, cookie.birthday(ptr));
    ptr = cookie.base(ptr);
  }

  p = libc_realloc_wrapper(ptr, size + cookie.size());
  if (pstat && p) {
    p = cookie.cookie(p, now, size);
    pstat->s_alloc(size, now);
  }
  return p;
}

#endif // DMALLOC_PASSTHROUGH

/* ************************************************************************** */
#ifdef DMALLOC_UNIT_TEST_DMALLOC

#include "unit_test.h"

unit_test ut;

int main()
{
  void *ptr = dmalloc_malloc(32);

  ut.ut_check_ne("alloc 32 bytes", (void *)0L, ptr);

  ptr = dmalloc_realloc(ptr, 64);
  ut.ut_check_ne("realloc 64 bytes", (void *)0L, ptr);
  dmalloc_free(ptr);
  dmalloc_free(nullptr);
}


#endif // DMALLOC_UNIT_TEST_DMALLOC
