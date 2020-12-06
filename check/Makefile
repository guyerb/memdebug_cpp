# -*- MakeFile -*-
target = checks

uname := $(shell uname -s)
ifeq ($(uname), Linux)
	preload = LD_PRELOAD=../src/libdmalloc.so
endif
ifeq ($(uname), Darwin)
	preload = DYLD_INSERT_LIBRARIES=../src/libdmalloc.dylib DYLD_FORCE_FLAT_NAMESPACE=1
endif


%.o: %.cpp
	g++ -g -std=c++17 -Wall -Wextra  -c $<  -o $@

$(target): checks.o
	g++ -g   $^ -pthread -o $@

check: $(target)
	@echo run with injected malloc calls
	@echo ----------------------------
	 $(preload) ./$(target)
	@echo ----------------------------
	@echo

clean distclean:
	rm -f  *.o  $(target)  *~

