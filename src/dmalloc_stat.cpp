#include <mutex>
#include <ctime>
#include <iostream>
#include "dmalloc_stat.h"
#include "dmalloc_log.h"
#include "dmalloc_version.h"
#include "libc_wrapper.h"

std::mutex _s_m;

unsigned  dmalloc_stat::_s_agebucket_largest()
{
  unsigned largest = 0;
  unsigned count = 0;

  count = 0;
  for (int i = 0; i < 10; i++) { count+= _s_agebucket_cnt[i]; }
  if (count > largest) largest = count;

  count = 0;
  for (int i = 10; i < 99; i++) { count+= _s_agebucket_cnt[i]; }
  if (count > largest) largest = count;

  count = 0;
  for (int i = 100; i < 999; i++) { count+= _s_agebucket_cnt[i]; }
  if (count > largest) largest = count;

  count = _s_agebucket_cnt[999];
  if (count > largest) largest = count;

  return largest;

}

unsigned  dmalloc_stat::_s_szebucket_largest(const std::vector<unsigned> &rv)
{
  unsigned largest = 0;

  for (std::vector<unsigned>::size_type i = 0; i < rv.size(); i++)
    if (rv[i] > largest) largest = rv[i];

  return largest;
}

int dmalloc_stat::_s_agebucket_ndx(std::time_t birth)
{
  int ndx = -1;

  if (birth > _s_ageupdate) {
    _s_invalid_birthday++;
    ndx = -1;
  } else {
    ndx = _s_ageupdate - birth;
    if (ndx > 999) ndx = 999;
  }
  return ndx;
}

/* Monitor the age of each allocation by grouping them in buckets,
 * where each bucket represents one second of age. Bump them forward
 * by elapsed time since last update.
 */
void dmalloc_stat::_s_agebucket_update(std::time_t now)
{
  std::time_t elapsed = 0;

  if (now < _s_ageupdate) {
    dputc('U');
    return;
  }
  elapsed = now - _s_ageupdate;
  if (elapsed == 0) {
    return;
  }
  _s_ageupdate = now;

  dputc('u');
  std::vector<int>::size_type i = _s_agebucket_cnt.size();
  for ( ; i > 0; i--) {
    std::vector<int>::size_type src_ndx = i - 1;
    std::vector<int>::size_type dst_ndx = src_ndx + elapsed;
    unsigned src_val = _s_agebucket_cnt[src_ndx];

    if (src_ndx == (_s_agebucket_cnt.size() - 1)) continue;	/* doesn't update */
    _s_agebucket_cnt[src_ndx] = 0;
    if (dst_ndx >= _s_agebucket_cnt.size() - 1) {
      _s_agebucket_cnt[_s_agebucket_cnt.size() - 1] += src_val;
    } else {
      _s_agebucket_cnt[dst_ndx] = src_val;
    }
  }
}

int dmalloc_stat::_s_szebucket_ndx(std::size_t sz)
{
  if (sz >= 0x1000) return BUCKET_4096;
  if (sz & 0x0800) return BUCKET_2048;
  if (sz & 0x0400) return BUCKET_1024;
  if (sz & 0x0200) return BUCKET_0512;
  if (sz & 0x0100) return BUCKET_0256;
  if (sz & 0x0080) return BUCKET_0128;
  if (sz & 0x0040) return BUCKET_0064;
  if (sz & 0x0020) return BUCKET_0032;
  if (sz & 0x0010) return BUCKET_0016;
  if (sz & 0x0008) return BUCKET_0008;
  if (sz & 0x0004) return BUCKET_0004;
  return BUCKET_0000;
}

/* how many items does one '#' represent (given x columns) */
unsigned dmalloc_stat::_s_dump_scaler(unsigned largest, unsigned columns)
{
  unsigned scaler = largest / columns;

  if (largest % columns) scaler ++;
  return scaler;
}

void dmalloc_stat::_s_dump_scaled(std::string &hdr, unsigned count, unsigned scaler)
{
  static int depth = 0;

  if (depth == 0)
    dprintf("%s: ", hdr.c_str());

  if (scaler) {
    if (count < scaler) {
      if (count > 0)
        dprintf(".");	/* partial # */
    } else {
      dprintf("#");
      depth++;
      _s_dump_scaled(hdr, count - scaler, scaler);
      depth--;
    }
  }
  if (depth == 0) dprintf("\n");
}

unsigned dmalloc_stat::_s_dump_range_cnt(std::vector<unsigned> &rv, unsigned floor, unsigned ceiling)
{
  unsigned count = 0;

  for ( unsigned i=floor; i <= ceiling; i++) {
    count += rv[i];
  }
  return count;
}
void dmalloc_stat::_s_dump_range_scaled(std::string &hdr, std::vector<unsigned> &rv, unsigned floor, unsigned ceiling, unsigned scaler)
{
  unsigned count = _s_dump_range_cnt(rv, floor, ceiling);
  _s_dump_scaled(hdr, count, scaler);
}

/* value formatter (e.g. 1,000) from the interwebz */
void dmalloc_stat::_s_dump_with_sep (unsigned n, char sep)
{
  if (n < 1000) {
    dprintf ("%d", n);
    return;
  }
  _s_dump_with_sep(n/1000);
  dprintf ("%c%03d", sep, n%1000);
}

void dmalloc_stat::_s_dump_self(std::time_t now) noexcept
{
#ifdef LINUX
  struct tm *pgm = nullptr;
  char * pgm_str = nullptr;

  dputc('!');
  pgm = std::gmtime(&now);
  pgm_str= std::asctime(pgm);
  pgm_str[24] = '\0';		/* kill the newline with bravado */
#endif
#ifdef DARWIN			// BUGBUG
  (void)(now);
  const char * pgm_str = "gmtime hangs on Darwin in sharedlib";
#endif

  dprintf("========== %s: UTC %s ==========\n", DMALLOC_VERSION_STRING, pgm_str);
  dprintf("%-26s", "overall allocations:" );
  _s_dump_with_sep(_s_allt_alloc); dprintf("\n");
  dprintf("%-26s", "current allocations:" );
  _s_dump_with_sep(_s_curr_alloc); dprintf("\n");
  dprintf("%-26s", "current alloc bytes:" );
  _s_dump_with_sep(_s_curr_bytes); dprintf("\n");

#ifdef DMALLOC_INTERNALS
  dprintf("\ninternals:\n");
  dprintf("%-25s %d\n", "age underruns:", _s_underrun_age);
  dprintf("%-25s %d\n", "age failed ins:", _s_failed_age);
  dprintf("%-25s %d\n", "size underruns:", _s_underrun_bytes);
  dprintf("%-25s %d\n", "invalid_birthday:", _s_invalid_birthday);
  dprintf("\n");
#endif

  std::vector<std::string> sze_hdr = {
    "0    -    4",  "4    -    8", "8    -   16",  "16   -   32",
    "32   -   64",  "64   -  128", "128  -  256",  "256  -  512",
    "512  - 1024",  "1024 - 2048", "2048 - 4096",  "4096 - INFI",
  };

  /* number of allocations living in size bucket*/
  unsigned scaler_cnt = _s_dump_scaler(_s_szebucket_largest(_s_szebucket_cnt), 68);
  dprintf("histogram: allocs: (one # represents approx %d allocs)\n", scaler_cnt);
  for (unsigned i = 0; i < szesz; i++) {
    _s_dump_scaled(sze_hdr[i],	_s_szebucket_cnt[i], scaler_cnt);
  }
  dprintf("\n");

  /* number of bytes living in size bucket. */
  unsigned scaler_sze = _s_dump_scaler(_s_szebucket_largest(_s_szebucket_sze), 68);
  dprintf("histogram: bytes: (one # represents approx %d bytes)\n", scaler_sze);
  for (unsigned i = 0; i < szesz; i++) {
    _s_dump_scaled(sze_hdr[i],	_s_szebucket_sze[i], scaler_sze);
  }
  dprintf("\n");

  /* dump age buckets */
  unsigned scaler_age = _s_dump_scaler(_s_agebucket_largest(), 68);
  std::vector<std::string> age_hdr = {
    "<    10 sec", "<   100 sec", "<  1000 sec", ">= 1000 sec"
  };
  dprintf("histogram: alloc ages: (one # represents appox %d  allocs)\n", scaler_age);
  _s_dump_range_scaled(age_hdr[0], _s_agebucket_cnt, 0, 9, scaler_age);
  _s_dump_range_scaled(age_hdr[1], _s_agebucket_cnt, 10, 99, scaler_age);
  _s_dump_range_scaled(age_hdr[2], _s_agebucket_cnt, 100, 998, scaler_age);
  _s_dump_range_scaled(age_hdr[3], _s_agebucket_cnt, 999, 999, scaler_age);
}

void dmalloc_stat::s_agebucket_insert(std::time_t now)
{
  dputc('i');
  _s_agebucket_update(now);

  /* the agebucket is guaranteed up to date but maybe in the future so
     compute age ndx rather than assuming 0 */
  int ndx = _s_agebucket_ndx(now);
  if (ndx >= 0) 
    _s_agebucket_cnt[ndx]++;
  else
    _s_failed_age++;
}

void dmalloc_stat::s_agebucket_delete(std::time_t now, std::time_t birth)
{
  dputc('d');
  _s_agebucket_update(now);
  int ndx = _s_agebucket_ndx(birth);
  if (ndx != -1 && _s_agebucket_cnt[ndx] != 0) {
    _s_agebucket_cnt[ndx]--;
  } else {
    _s_underrun_age++;
  }
}

void dmalloc_stat::s_alloc(std::size_t sz, std::time_t now)
{
  int ndx = _s_szebucket_ndx(sz);
  std::unique_lock lck {_s_m};

  dputc('+');
  _s_curr_bytes += sz;
  _s_curr_alloc ++;
  _s_allt_alloc ++;
  _s_szebucket_cnt[ndx] += 1;
  _s_szebucket_sze[ndx] += sz;
  s_agebucket_insert(now);
  lck.unlock();
  s_dump(now);
}

void dmalloc_stat::s_free(std::size_t sz, std::time_t now, std::time_t birth)
{
  int ndx = _s_szebucket_ndx(sz);
  std::unique_lock lck {_s_m};

  dputc('-');
  if ((_s_curr_bytes >= sz) && _s_szebucket_cnt[ndx] && (_s_szebucket_sze[ndx] >= sz)) {
    _s_curr_bytes -= sz;
    _s_curr_alloc--;
    _s_szebucket_cnt[ndx]--;
    _s_szebucket_sze[ndx] -= sz;
    s_agebucket_delete(now, birth);
  } else {
    _s_underrun_bytes++;
  }
  lck.unlock();
  s_dump(now);
}

void dmalloc_stat::s_dump(std::time_t now)
{
  std::unique_lock lck {_s_m};

  if (_s_logupdate == 0) {
    _s_logupdate = now;
    lck.unlock();
    return;
  }
  if (_s_logupdate > now) {
    // old caller who was blocked
    dputc('P');
    lck.unlock();
    return;
  }
  if ((now - _s_logupdate) < 5) {
    lck.unlock();
    return;
  }
  _s_logupdate = now;
  lck.unlock();

  dputc('p');
  dmalloc_stat copy = *this;
  copy._s_dump_self(now);
}

/* ************************************************************************** */
#ifdef DMALLOC_UNIT_TEST

#include "unit_test.h"

unit_test ut;

int main()
{
  std::time_t t_0    = std::time(nullptr);
  std::time_t t_1    = t_0 + 1;
  std::time_t t_5    = t_0 + 5;
  std::time_t t_999  = t_0 + 999;
  std::time_t t_1004 = t_0 + 1004;
  std::time_t t_1500 = t_0 + 1500;
  std::time_t t_2000 = t_0 + 2000;

  ut.ut_start_unit("");

  ut.ut_start_section("exercise empty dump");
  dmalloc_stat my_stat0;
  my_stat0.s_dump(t_0);
  my_stat0.s_dump(t_5);

  dmalloc_stat my_stat1;
  ut.ut_start_section("exercise my_stat1 primitives");
  ut.ut_mark("mark 0");
  ut.ut_check("empty sz size is 0", (unsigned)0, \
              my_stat1._s_szebucket_largest(my_stat1._s_szebucket_sze));
  ut.ut_check("empty sz count is 0", (unsigned)0, \
              my_stat1._s_szebucket_largest(my_stat1._s_szebucket_cnt));
  my_stat1.s_alloc(68, t_0);
  ut.ut_check("current allocated bytes", (unsigned)68, my_stat1._s_curr_bytes);
  ut.ut_check("current allocations", (unsigned)1, my_stat1._s_curr_alloc);
  ut.ut_check("alltime allocations", (unsigned)1, my_stat1._s_allt_alloc);
  ut.ut_check("agebucket largest", (unsigned)1, my_stat1._s_agebucket_largest());
  ut.ut_check("largest sz size is 68", (unsigned)68, \
              my_stat1._s_szebucket_largest(my_stat1._s_szebucket_sze));
  ut.ut_check("empty sz count is 1", (unsigned)1, \
              my_stat1._s_szebucket_largest(my_stat1._s_szebucket_cnt));

  ut.ut_check("agebucket scaler", (unsigned)1, my_stat1._s_dump_scaler(68, 68));
  my_stat1.s_free(68, t_0, t_0);
  my_stat1.s_alloc(69, t_0);
  ut.ut_check("current allocated bytes", (unsigned)69, my_stat1._s_curr_bytes);
  ut.ut_check("current allocations", (unsigned)1, my_stat1._s_curr_alloc);
  ut.ut_check("alltime allocations", (unsigned)2, my_stat1._s_allt_alloc);
  ut.ut_check("agebucket scaler", (unsigned)2, my_stat1._s_dump_scaler(69, 68));
  ut.ut_check("agebucket scaler", (unsigned)2, my_stat1._s_dump_scaler(136, 68));
  ut.ut_check("agebucket scaler", (unsigned)3, my_stat1._s_dump_scaler(137, 68));

  for (int i=0; i< 30000; i++) {
    my_stat1.s_alloc(1, t_0);
  }
  ut.ut_mark("mark 1");
  my_stat1.s_alloc(1, t_999);
  ut.ut_check("current allocated bytes", (unsigned)30070, my_stat1._s_curr_bytes);
  ut.ut_check("age bucket 999", (unsigned)30001, my_stat1._s_agebucket_cnt[999]);

  ut.ut_check("age bucket range 1", (unsigned) 1, my_stat1._s_dump_range_cnt(my_stat1._s_agebucket_cnt, 0, 9));
  ut.ut_check("age bucket range 2", (unsigned) 0, my_stat1._s_dump_range_cnt(my_stat1._s_agebucket_cnt, 10, 99));
  ut.ut_check("age bucket range 3", (unsigned) 0, my_stat1._s_dump_range_cnt(my_stat1._s_agebucket_cnt, 100, 998));
  ut.ut_check("age bucket range 4", (unsigned) 30001, my_stat1._s_dump_range_cnt(my_stat1._s_agebucket_cnt, 999, 999));

  ut.ut_mark("mark 2");
  for (int i=0; i< 10000; i++) {
    my_stat1.s_alloc(128, t_999);
  }
  my_stat1.s_alloc(128, t_1004);

  /*alloc sz	birthday
    0		t_0
    1		t_0
    2		t_0
    4		t_0
    8		t_0
    16		t_0
    32		t_0
    64		t_0
    128		t_0
    256		t_0
    512		t_0
    1024	t_0
    2048	t_0
    4096	t_0
    8192	t_1
    8192	t_999
    8192	t_1500
    8192	t_2000
  */

  dmalloc_stat new_stat;
  ut.ut_start_section("exercise new_stat._s_szebucket_ndx()");
  ut.ut_check("   0 byte bucket is ", 0, new_stat._s_szebucket_ndx(0));
  ut.ut_check("   1 byte bucket is ", 0, new_stat._s_szebucket_ndx(1));
  ut.ut_check("   2 byte bucket is ", 0, new_stat._s_szebucket_ndx(2));
  ut.ut_check("   3 byte bucket is ", 0, new_stat._s_szebucket_ndx(3));
  ut.ut_check("   4 byte bucket is ", 1, new_stat._s_szebucket_ndx(4));
  ut.ut_check("   5 byte bucket is ", 1, new_stat._s_szebucket_ndx(5));
  ut.ut_check("   6 byte bucket is ", 1, new_stat._s_szebucket_ndx(6));
  ut.ut_check("   7 byte bucket is ", 1, new_stat._s_szebucket_ndx(7));
  ut.ut_check("   8 byte bucket is ", 2, new_stat._s_szebucket_ndx(8));
  ut.ut_check("  15 byte bucket is ", 2, new_stat._s_szebucket_ndx(15));
  ut.ut_check("2047 byte bucket is ", 9, new_stat._s_szebucket_ndx(2047));
  ut.ut_check("2049 byte bucket is ", 10, new_stat._s_szebucket_ndx(2049));
  ut.ut_check("4095 byte bucket is ", 10, new_stat._s_szebucket_ndx(4095));
  ut.ut_check("4096 byte bucket is ", 11, new_stat._s_szebucket_ndx(4096));
  ut.ut_check("4097 byte bucket is ", 11, new_stat._s_szebucket_ndx(4097));
  ut.ut_check("1234567 byte bucket is ", 11, new_stat._s_szebucket_ndx(1234567));
  ut.ut_check("empty sz size is 0", (unsigned)0, new_stat._s_szebucket_largest(new_stat._s_szebucket_sze));
  ut.ut_check("empty sz count is 0", (unsigned)0, new_stat._s_szebucket_largest(new_stat._s_szebucket_cnt));
  ut.ut_finish_section();
  ut.ut_start_section("exercise age and size buckets");
  new_stat.s_alloc(   0, t_0);
  new_stat.s_alloc(   1, t_0);
  new_stat.s_alloc(   2, t_0);
  new_stat.s_alloc(   4, t_0);
  new_stat.s_alloc(   8, t_0);
  new_stat.s_alloc(  16, t_0);
  new_stat.s_alloc(  32, t_0);
  new_stat.s_alloc(  64, t_0);
  new_stat.s_alloc( 128, t_0);
  new_stat.s_alloc( 256, t_0);
  new_stat.s_alloc( 512, t_0);
  new_stat.s_alloc(1024, t_0);
  new_stat.s_alloc(2048, t_0);
  new_stat.s_alloc(4096, t_0);
  new_stat.s_alloc(8192, t_0);
  unsigned bytes = 0 + 1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024  \
    + 2048 +4096 + 8192;
  ut.ut_mark("mark 5");
  ut.ut_check("current allocated bytes", bytes, new_stat._s_curr_bytes);
  ut.ut_check("current allocations", (unsigned)15, new_stat._s_curr_alloc);
  ut.ut_check("alltime allocations", (unsigned)15, new_stat._s_allt_alloc);
  ut.ut_check("age  bucket 0",  (unsigned)15, new_stat._s_agebucket_cnt[BUCKET_0000]);
  ut.ut_check("size bucket 0",  (unsigned)3, new_stat._s_szebucket_cnt[BUCKET_0000]);
  ut.ut_check("size bucket 1",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_0004]);
  ut.ut_check("size bucket 2",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_0008]);
  ut.ut_check("size bucket 3",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_0016]);
  ut.ut_check("size bucket 4",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_0032]);
  ut.ut_check("size bucket 5",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_0064]);
  ut.ut_check("size bucket 6",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_0128]);
  ut.ut_check("size bucket 7",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_0256]);
  ut.ut_check("size bucket 8",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_0512]);
  ut.ut_check("size bucket 9",  (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_1024]);
  ut.ut_check("size bucket 10", (unsigned)1, new_stat._s_szebucket_cnt[BUCKET_2048]);
  ut.ut_check("size bucket 11", (unsigned)2, new_stat._s_szebucket_cnt[BUCKET_4096]);
  new_stat.s_alloc(8192, t_1);
  bytes += 8192;
  ut.ut_mark("mark 6");
  ut.ut_check("size bucket 11", (unsigned)3, new_stat._s_szebucket_cnt[BUCKET_4096]);
  ut.ut_check("age  bucket 0",  (unsigned)1, new_stat._s_agebucket_cnt[0]);
  ut.ut_check("age  bucket 1",  (unsigned)15, new_stat._s_agebucket_cnt[1]);
  ut.ut_check("current allocated bytes", bytes, new_stat._s_curr_bytes);
  ut.ut_check("alltime allocations", (unsigned)16, new_stat._s_allt_alloc);

  new_stat.s_alloc(8192, t_999);
  bytes += 8192;
  ut.ut_mark("mark 7");
  ut.ut_check("current allocated bytes", bytes, new_stat._s_curr_bytes);
  ut.ut_check("size bucket 11",  (unsigned)4, new_stat._s_szebucket_cnt[BUCKET_4096]);
  ut.ut_check("age  bucket 0",   (unsigned)1, new_stat._s_agebucket_cnt[0]);
  ut.ut_check("age  bucket 998", (unsigned)1, new_stat._s_agebucket_cnt[998]);
  ut.ut_check("age  bucket 999", (unsigned)15, new_stat._s_agebucket_cnt[999]);

  new_stat.s_alloc(8192, t_1500);
  bytes += 8192;
  ut.ut_mark("mark 8");
  ut.ut_check("size bucket 11",  (unsigned)5, new_stat._s_szebucket_cnt[BUCKET_4096]);
  ut.ut_check("age  bucket 0",   (unsigned)1, new_stat._s_agebucket_cnt[0]);
  ut.ut_check("age  bucket 501",   (unsigned)1, new_stat._s_agebucket_cnt[501]);
  ut.ut_check("age  bucket 999", (unsigned)16, new_stat._s_agebucket_cnt[999]);

  new_stat.s_alloc(8192, t_2000);
  bytes += 8192;
  ut.ut_mark("mark 9");
  ut.ut_check("size bucket 11",  (unsigned)6, new_stat._s_szebucket_cnt[BUCKET_4096]);
  ut.ut_check("age  bucket 0",   (unsigned)1, new_stat._s_agebucket_cnt[0]);
  ut.ut_check("age  bucket 999", (unsigned)17, new_stat._s_agebucket_cnt[999]);
  ut.ut_check("current allocated bytes", bytes, new_stat._s_curr_bytes);
  ut.ut_check("alltime allocations", (unsigned)19, new_stat._s_curr_alloc);
  ut.ut_check("current allocated bytes", bytes, new_stat._s_curr_bytes);
  ut.ut_check("current allocations", (unsigned)19, new_stat._s_curr_alloc);

  new_stat.s_free(bytes * 2, t_2000, t_0);
  ut.ut_mark("mark 10");
  ut.ut_check("current allocated bytes", bytes, new_stat._s_curr_bytes);
  ut.ut_check("alltime allocations", (unsigned)19, new_stat._s_allt_alloc);
  ut.ut_check("current allocations", (unsigned)19, new_stat._s_curr_alloc);
  ut.ut_check("size underrun", (unsigned)1, (unsigned)new_stat._s_underrun_bytes);
  new_stat.s_free(8192, t_2000, t_2000);
  new_stat.s_free(8192, t_2000, t_1500);
  new_stat.s_free(8192, t_2000, t_999);
  new_stat.s_free(8192, t_2000, t_1);
  new_stat.s_free(8192, t_2000, t_0);
  bytes -= 8192 * 5;
  new_stat.s_free(4096, t_2000, t_0);
  bytes -= 4096;
  ut.ut_check("current allocated bytes", bytes, new_stat._s_curr_bytes);
  ut.ut_check("alltime allocations", (unsigned)19, new_stat._s_allt_alloc);
  ut.ut_check("current allocations", (unsigned)13, new_stat._s_curr_alloc);
  ut.ut_check("age  bucket 999", (unsigned)13, new_stat._s_agebucket_cnt[999]);
  ut.ut_check("age  bucket 0", (unsigned)0, new_stat._s_agebucket_cnt[0]);
  new_stat.s_free(4096, t_2000, t_0);
  ut.ut_check("current allocated bytes", bytes, new_stat._s_curr_bytes);
  ut.ut_check("age  bucket 0", (unsigned)0, new_stat._s_agebucket_cnt[0]);
  new_stat.s_free(2048, t_2000, t_0);
  new_stat.s_free(1024, t_2000, t_0);
  new_stat.s_free(512, t_2000, t_0);
  new_stat.s_free(256, t_2000, t_0);
  new_stat.s_free(128, t_2000, t_0);
  new_stat.s_free(64, t_2000, t_0);
  new_stat.s_free(32, t_2000, t_0);
  new_stat.s_free(16, t_2000, t_0);
  new_stat.s_free(8, t_2000, t_0);
  new_stat.s_free(4, t_2000, t_0);
  new_stat.s_free(2, t_2000, t_0);
  new_stat.s_free(1, t_2000, t_0);
  new_stat.s_free(0, t_2000, t_0);
  ut.ut_check("current allocated bytes", (unsigned)0, new_stat._s_curr_bytes);
  ut.ut_check("alltime allocations", (unsigned)19, new_stat._s_allt_alloc);
  ut.ut_check("current allocations", (unsigned)0, new_stat._s_curr_alloc);
  ut.ut_check("age bucket 999", (unsigned)0, new_stat._s_agebucket_cnt[999]);
  ut.ut_check("age bucket 0", (unsigned)0, new_stat._s_agebucket_cnt[0]);

  unsigned population = 0;
  for (const auto& i : new_stat._s_agebucket_cnt) {
      std::cout << i << ' ';
      population += i;
  }
  std::cout << std::endl;
  ut.ut_check("age bucket population", (unsigned)0, population);

  population = 0;
  for (const auto& i : new_stat._s_szebucket_cnt) {
    std::cout << i << ' ';
    population += i;
  }
  std::cout << std::endl;
  ut.ut_check("size bucket population", (unsigned)0, population);
  ut.ut_finish_section();
  ut.ut_finish_unit();

  /*------------------------------------------------------------------------- */
}
#endif	/* DMALLOC_UNIT_TEST */
