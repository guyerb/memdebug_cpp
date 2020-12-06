# -*- MakeFile -*-
target = checks

uname := $(shell uname -s)
ifeq ($(uname), Linux)
	preload = LD_PRELOAD=../src/libdmalloc.so
endif
ifeq ($(uname), Darwin)
	preload = DYLD_INSERT_LIBRARIES=../src/libdmalloc.dylib DYLD_FORCE_FLAT_NAMESPACE=1
endif


%.o: %.c
	gcc -g -std=c99 -Wall -Wextra  -c $<  -o $@

$(target): checks.o
	gcc -g   $^ -lpthread -o $@

check: $(target)
	@echo run with injected malloc calls
	@echo ----------------------------
	 $(preload) ./$(target)
	@echo ----------------------------
	@echo

clean distclean:
	rm -f  *.o  $(target)  *~

