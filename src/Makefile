# -*- MakeFile -*-
sources := dmalloc.cpp dmalloc_stat.cpp dmalloc_log.cpp

uname := $(shell uname -s)
ifeq ($(uname), Linux)
	os = LINUX
	sources += dmalloc_intercept_linux.cpp libc_wrapper_linux.cpp
	target = libdmalloc.so
endif
ifeq ($(uname), Darwin)
	os = DARWIN
	sources += dmalloc_intercept_darwin.cpp
	target = libdmalloc.dylib
endif
objects = $(sources:.cpp=.o)
depends = $(objects:.o=.d)

%.o:%.cpp
	g++ -g -std=c++17 -Wall -Wextra -D $(os) -c  $<  -fPIC  -o $@
	@g++ -MM $*.cpp > $*.d

$(target): $(objects)
	g++ -g  $^  -ldl -shared  -o $@

check: $(target)

# don't remove libs on clean so we can build for multiple arch, see distclean
clean:
	rm -f $(objects) $(depends) *~
	make -f Makefile.unit_test clean
	make -f Makefile.unit_test_cookie clean
	make -f Makefile.unit_test_stat clean
	make -f Makefile.unit_test_dmalloc clean

distclean: clean
	rm -f *.dylib *.so
	rm -rf *.dSYM

-include $(depends)

