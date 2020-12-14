#include <cstdio>
#include <iostream>
#include <string>
#include <cstdint>

#include "unit_test.h"

using namespace::std;

int main()
{
  unit_test ut;
  uint32_t i = 0;
  uint64_t j = 0;

  ut.ut_start_unit("unit test self check", '^');
  ut.ut_start_section("exercise ut_check method");
  ut.ut_check("ut_check self test int", 1, 1);
  ut.ut_check_ne("ut_check self test != int", 0, 1);
  ut.ut_check_ne("ut_check self test != int", 1, 0);
  ut.ut_check("ut_check self test u32", i, i);
  ut.ut_check("ut_check self test u64", j, j);
  time_t now = time(nullptr);
  ut.ut_check("ut_check self test tm", now, now);
  ut.ut_check_ne("ut_check self test != tm", now, now + 1);
  ut.ut_check("ut_check self test str", "a string", "a string");
  ut.ut_check("ut_check self test str", "a string", "b string");
  ut.ut_check_ne("ut_check self test != str", "a string", "b string");
  ut.ut_finish_section();

  ut.ut_start_section("fail checks");
  ut.ut_check("ut_check self int fail", 1, 0);
  ut.ut_check("ut_check self int fail", 0, 1);
  ut.ut_check("ut_check self test tm", now, now + 1);
  ut.ut_finish_section();
  ut.ut_finish_unit();

}
