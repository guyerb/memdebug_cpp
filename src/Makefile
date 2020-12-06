# -*- MakeFile -*-
sources := dmalloc.cpp
objects = $(sources:.cpp=.o)
depends := $(objects:.o=.d)

uname := $(shell uname -s)
ifeq ($(uname), Linux)
	os = LINUX
	sources += dmalloc_intercept_linux.cpp libc_wrappers_linux.cpp
	target = libdmalloc.so
endif
ifeq ($(uname), Darwin)
	os = DARWIN
	sources += dmalloc_intercept_darwin.cpp
	target = libdmalloc.dylib
endif

%.o:%.cpp
	g++ -g -std=c++14 -Wall -Wextra -D DMALLOC_DEBUG -D $(os) -c  $<  -fPIC  -o $@
	@g++ -MM $*.cpp > $*.d

$(target): $(objects)
	g++ -g  $^  -ldl -shared  -o $@

check: $(target)

# don't remove libs on clean so we can build for multiple arch, see distclean
clean:
	rm -f $(objects) $(depends) $(target) *~

distclean: clean
	rm -f *.dylib *.so
	rm -rf *.dSYM

-include $(depends)

