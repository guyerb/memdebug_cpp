#include <iostream>
#include <string>
#include <cstdint>

#include "unit_test.h"
#include "dmalloc_cookie.h"

  /* | MAGIC | TSTAMP | SIZE | USER DATA | */
const uint32_t magic = {0xDACABEEF};
const uint32_t dsize = {(sizeof(time_t) + sizeof(uint32_t) + sizeof(size_t))};

int main()
{
  unit_test ut;
  dmalloc_cookie cookie;
  uint8_t buf[64] = {0};
  time_t now = time(NULL);

  ut.ut_start_unit("dmalloc_cookies unit test");
  ut.ut_start_section("exercise cookies");
  ut.ut_check("check buf ours", false, cookie.ours(&buf[32]));
  ut.ut_check("check cookies sz", dsize, cookie.size());
  ut.ut_check("install cookies", (void *)&buf[dsize], cookie.cookie(buf, now, 64));
  ut.ut_check("check buf ours", true, cookie.ours(&buf[dsize]));
  ut.ut_check("check buf size", (size_t)64, cookie.bytes(&buf[dsize]));
  ut.ut_check("check buf time", now, cookie.birthday(&buf[dsize]));
  ut.ut_check("check buf base", (void *)buf, cookie.base(&buf[dsize]));
  ut.ut_check("check buf magic", magic, cookie.magic(&buf[dsize]));
  ut.ut_finish_section();
}
