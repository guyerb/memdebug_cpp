#ifndef DMALLOC_COOKIE_H
#define DMALLOC_COOKIE_H

class dmalloc_cookie {
 private:
  /* | MAGIC | TSTAMP | SIZE | USER DATA | */
  const uint32_t _magic = {0xDACABEEF};
  const uint32_t _msize = {sizeof(uint32_t)};
  const uint32_t _tsize = {sizeof(time_t)};
  const uint32_t _ssize = {sizeof(size_t)};
  const uint32_t _dsize = {(sizeof(time_t) + sizeof(uint32_t) + sizeof(size_t))};
 public:
  dmalloc_cookie() = default;
  ~dmalloc_cookie() {};

  void * cookie(void *, time_t, size_t);
  size_t bytes(void *);
  uint32_t size() {return _dsize;}
  time_t birthday(void *);
  bool ours(void *);
  void * base(void *);
  uint32_t magic(void *);
};

void *dmalloc_cookie::cookie(void *ptr, time_t birthday, size_t sz)
{
  void *p = NULL;

  if (ptr) {
    *(uint32_t *)ptr = _magic;
    *(time_t *)((uint8_t *)ptr + _msize) = birthday;
    *(size_t *)((uint8_t *)ptr + _msize + _tsize) = sz;
    p = (void *)((uint8_t *)ptr + _dsize);
  }
  return p;
}

size_t dmalloc_cookie::bytes(void *ptr)
{
  size_t sz = 0;

  if (ptr && ours(ptr)) {
    time_t   *ps = (time_t *)((uint8_t *)ptr - _ssize);
    sz = *ps;
  }
  return sz;
}

time_t dmalloc_cookie::birthday(void *ptr)
{
  time_t birthday = 0;

  if (ptr and ours(ptr)) {
    time_t   *pt = (time_t *)((uint8_t *)ptr - _ssize - _tsize);
    birthday = *pt;
  }
  return birthday;
}

bool dmalloc_cookie::ours(void *ptr)
{
  bool ours = false;

  if (ptr) {
    uint32_t *p = (uint32_t *)base(ptr);
    if (p) {
      if (*p == _magic)
	ours = true;
    }
  }
  return ours;
}

void * dmalloc_cookie::base(void *ptr)
{
  return (void *)((uint8_t *)ptr - _dsize);
}

uint32_t dmalloc_cookie::magic(void *ptr)
{
  uint32_t magic = 0;

  if (ptr) {
    uint32_t *p = (uint32_t *)base(ptr);
    magic = *p;
  }
  return magic;
}

#endif	/* DMALLOC_COOKIE_H */
