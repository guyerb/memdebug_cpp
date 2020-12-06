# -*- MakeFile -*-
# unit test for unit test itself
#

sources := unit_test_cookies.cpp
objects := $(sources:.cpp=.o)
depends := $(objects:.o=.d)

target := unit_test_cookies

%.o:%.cpp
	g++ -std=c++14 -g -Wall -Wextra -Werror -c $< -o $@
	@gcc -MM $*.cpp > $*.d

$(target):$(objects)
	g++ -g $^ -o $@

clean:
	rm -f *.o *.d *~ $(target)

-include $(depends)

