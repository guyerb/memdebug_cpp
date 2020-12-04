#include <cstdio>
#include <iostream>
#include <string>
#include <cstdint>

#include "unit_test.h"

using namespace::std;


#ifdef DMALLOC_UNIT_STATS


int main()
{
  unit_test ut;

  ut.start_unit("STATS");
  
}
#endif	// DMALLOC_UNIT_STATS
