# -*- MakeFile -*-
# unit test for stats modules
#

sources := dmalloc_stat.cpp
objects := $(sources:.cpp=.o)
depends := $(objects:.o=.d)

target := unit_test_stat

%.o:%.cpp
	g++ -std=c++17 -g -D DMALLOC_UNIT_TEST -Wall -Wextra -Werror -c $< -o $@
	@gcc -MM $*.cpp > $*.d

$(target):$(objects)
	g++ -g $^ -o $@

clean:
	rm -f *.o *.d *~ $(target)

-include $(depends)

