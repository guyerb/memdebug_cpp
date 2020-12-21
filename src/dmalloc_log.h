#ifndef DMALLOC_LOG_H
#define DMALLOC_LOG_H

#include <cstdio>

#ifdef DMALLOC_DEBUG
#define dputc(x)  putc((x), stderr)
#else
#define dputc(x) do {} while(0)
#endif	/* DMALLOC_DEBUG */

void dprintf( const char* format, ... );

#endif	/* DMALLOC_LOG_H */
