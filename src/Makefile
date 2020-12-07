# -*- MakeFile -*-
sources := dmalloc.cpp
objects = $(sources:.cpp=.o)
depends := $(objects:.o=.d)

uname := $(shell uname -s)
ifeq ($(uname), Linux)
	os = LINUX
	sources += dmalloc_intercept_linux.cpp libc_wrapper.cpp
	target = libdmalloc.so
endif
ifeq ($(uname), Darwin)
	os = DARWIN
	sources += dmalloc_intercept_darwin.cpp
	target = libdmalloc.dylib
endif

%.o:%.cpp
	g++ -g -std=c++14 -Wall -Wextra -D $(os) -c  $<  -fPIC  -o $@
	@g++ -MM $*.cpp > $*.d

$(target): $(objects)
	g++ -g  $^  -ldl -shared  -o $@

check: $(target)

# don't remove libs on clean so we can build for multiple arch, see distclean
clean:
	rm -f $(objects) $(depends) $(target) *~
	make -f unit_test.Makefile clean
	make -f unit_test_cookie.Makefile clean
	make -f unit_test_stat.Makefile clean

distclean: clean
	rm -f *.dylib *.so
	rm -rf *.dSYM

-include $(depends)

