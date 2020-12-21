#ifndef DMALLOC_COOKIE_H
#define DMALLOC_COOKIE_H

#include <cstdint>

class dmalloc_cookie {
 private:
  /* | MAGIC | TSTAMP | SIZE | USER DATA | */
  const uint32_t _magic = {0xDACABEEF};
  const uint32_t _msize = {sizeof(_magic)};
  const uint32_t _tsize = {sizeof(std::time_t)};
  const uint32_t _ssize = {sizeof(std::size_t)};
  const uint32_t _csize = {_msize + _tsize + _ssize};
 public:
  dmalloc_cookie() = default;
  ~dmalloc_cookie() {};

  void * cookie(void *, std::time_t, size_t);
  std::size_t bytes(void *);
  uint32_t size() {return _csize;}
  std::time_t birthday(void *);
  bool ours(void *);
  void * base(void *);
  uint32_t magic(void *);
};

void *dmalloc_cookie::cookie(void *ptr, std::time_t birthday, std::size_t sz)
{
  void *p = NULL;

  if (ptr) {
    *(uint32_t *)ptr = _magic;
    *(std::time_t *)((uint8_t *)ptr + _msize) = birthday;
    *(std::size_t *)((uint8_t *)ptr + _msize + _tsize) = sz;
    p = (void *)((uint8_t *)ptr + _csize);
  }
  return p;
}

std::size_t dmalloc_cookie::bytes(void *ptr)
{
  std::size_t sz = 0;

  if (ptr && ours(ptr)) {
    std::time_t   *ps = (std::time_t *)((uint8_t *)ptr - _ssize);
    sz = *ps;
  }
  return sz;
}

std::time_t dmalloc_cookie::birthday(void *ptr)
{
  std::time_t birthday = 0;

  if (ptr and ours(ptr)) {
    std::time_t   *pt = (std::time_t *)((uint8_t *)ptr - _ssize - _tsize);
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
  return (void *)((uint8_t *)ptr - _csize);
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
