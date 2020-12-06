# -*- MakeFile -*-

sources := dmalloc_stats.cpp
objects := $(sources:.cpp=.o)
depends := $(objects:.o=.d)

target := stats-unit_test

%.o:%.cpp
	g++ -std=c++14 -g -Wall -Wextra -Werror -c $< -o $@
	@gcc -MM $*.cpp > $*.d

$(target):$(objects)
	g++ -g $^ -o $@

clean:
	rm -f *.o *.d *~ $(target)


-include $(depends)

