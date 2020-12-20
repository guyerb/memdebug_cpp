#ifndef DMALLOC_STAT_H
#define DMALLOC_STAT_H

#include <ctime>
#include <mutex>
#include <vector>

enum bucket_index {
  BUCKET_0000, BUCKET_0004, BUCKET_0008, BUCKET_0016, BUCKET_0032, BUCKET_0064,
  BUCKET_0128, BUCKET_0256, BUCKET_0512, BUCKET_1024, BUCKET_2048, BUCKET_4096,
};

constexpr std::vector<unsigned>::size_type  agesz =  1000;
constexpr std::vector<unsigned>::size_type  szesz =  12;

extern class dmalloc_stat stat;

class dmalloc_stat {

#ifdef DMALLOC_UNIT_TEST
 public:
#endif
  std::mutex _s_m;

  unsigned _s_curr_bytes {0};
  unsigned _s_curr_alloc {0};
  unsigned _s_allt_alloc {0};
  std::vector<unsigned> _s_szebucket_cnt = std::vector<unsigned>(szesz, 0);
  std::vector<unsigned> _s_szebucket_sze = std::vector<unsigned>(szesz, 0);
  std::vector<unsigned> _s_agebucket_cnt = std::vector<unsigned>(agesz, 0);
  std::time_t   _s_ageupdate;
  std::time_t   _s_logupdate;

  /* internal debugging */
  unsigned _s_null_free {0};
  unsigned _s_fail_alloc {0};
  unsigned _s_underrun_age {0};
  unsigned _s_underrun_bytes {0};
  unsigned _s_invalid_birthday {0};

  int  _s_agebucket_ndx(std::time_t now, std::time_t birth);
  void _s_agebucket_update(std::time_t now);

  unsigned  _s_agebucket_largest();
  unsigned  _s_szebucket_largest();
  unsigned  _s_scaler(int largest, int columns);
  int s_szebucket_ndx(std::size_t sz);

  void _s_dump_scaled(char *hdr, int count, int scale);
  void _s_dump_range_scaled(char *hdr, int *p, int floor, int ceiling, int scale);
  void _s_dump_value_with_commas(int n);
  void _s_dump_self(std::time_t);

 public:
  void s_agebucket_insert(std::time_t birth);
  void s_agebucket_delete(std::time_t now, std::time_t birth);

  void s_alloc(std::size_t sz, std::time_t now);
  void s_free(std::size_t sz, std::time_t now, std::time_t birth);
  void s_dump(std::time_t now);

};

#endif /* DMALLOC_STAT_H */

