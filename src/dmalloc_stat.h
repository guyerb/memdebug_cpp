#ifndef DMALLOC_STAT_H
#define DMALLOC_STAT_H

#include <ctime>
#include <mutex>
#include <vector>

enum bucket_index {
  BUCKET_0000, BUCKET_0004, BUCKET_0008, BUCKET_0016, BUCKET_0032, BUCKET_0064,
  BUCKET_0128, BUCKET_0256, BUCKET_0512, BUCKET_1024, BUCKET_2048, BUCKET_4096,
};

constexpr std::vector<unsigned>::size_type  sizesz =  1000;
constexpr std::vector<unsigned>::size_type  agesz =  12;

extern class dmalloc_stat stat;

class dmalloc_stat {

#ifdef DMALLOC_UNIT_TEST
 public:
#endif
  std::mutex _m;

  unsigned _s_curr_bytes {0};
  unsigned _s_curr_alloc {0};
  unsigned _s_allt_alloc {0};
  std::vector<unsigned> _s_szebucket = std::vector<unsigned>(agesz, 0);
  std::vector<unsigned> _s_agebucket = std::vector<unsigned>(sizesz, 0);
  std::time_t   _s_ageupdate;
  std::time_t   _s_logupdate;

  /* internal debugging */
  unsigned _s_null_free {0};
  unsigned _s_fail_alloc {0};
  unsigned _s_underrun_age {0};
  unsigned _s_underrun_bytes {0};
  unsigned _s_errorlock {0};
  unsigned _s_invalid_birthday {0};

  int  _agebucket_ndx(std::time_t now, std::time_t birth);
  void _agebucket_update(std::time_t now);

  int  _lock();
  void _unlock();

 public:
  void agebucket_insert(std::time_t birth);
  void agebucket_delete(std::time_t now, std::time_t birth);


  void alloc(std::size_t sz, std::time_t now);
  void free(std::size_t sz, std::time_t now, std::time_t birth);

  int size_bucket_ndx(std::size_t sz); /* debug use */
};

#endif /* DMALLOC_STAT_H */

