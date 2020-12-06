#ifndef DMALLOC_LOGS_H
#define DMALLOC_LOGS_H

#include <cstdio>

#ifdef DMALLOC_DEBUG
#define dputc(x)  putc((x), stderr)
#else
#define dputc(x) do {} while(0)
#endif	/* DMALLOC_DEBUG */

#endif	/* DMALLOC_LOGS_H */

