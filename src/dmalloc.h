#ifndef DMALLOC_H
#define DMALLOC_H

#include <cstddef>

extern void * dmalloc_calloc(std::size_t , std::size_t );
extern void   dmalloc_free(void *);
extern void * dmalloc_malloc(std::size_t );
extern void * dmalloc_realloc(void *, std::size_t );

#endif	/* DMALLOC_H */
