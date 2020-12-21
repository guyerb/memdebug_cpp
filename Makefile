# -*- MakeFile -*-

subdirs = src checks

all:
	for d in $(subdirs); do [ -d $$d ] && $(MAKE) -C $$d; done

check:
	for d in $(subdirs); do [ -d $$d ] && $(MAKE) -C $$d check; done

clean:
	for d in $(subdirs); do [ -d $$d ] && $(MAKE) -C $$d clean; done
	rm -f *~

distclean: clean
	for d in $(subdirs); do [ -d $$d ] && $(MAKE) -C $$d distclean; done
	rm -f cscope.*

.phony: check
