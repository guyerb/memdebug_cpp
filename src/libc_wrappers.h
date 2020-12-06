#ifndef LIBC_WRAPPERS_H
#define LIBC_WRAPPERS_H

#include <cstdlib>

#ifdef LINUX
extern void * libc_calloc_wrapper(size_t, size_t);
extern void libc_free_wrapper(void *);
extern void * libc_malloc_wrapper(size_t);
extern void * libc_realloc_wrapper(void *, size_t);
#endif

#ifdef DARWIN
#define libc_calloc_wrapper		calloc
#define libc_free_wrapper		free
#define libc_malloc_wrapper		malloc
#define libc_realloc_wrapper		realloc
#endif

#endif	/* LIBC_WRAPPERS_H */
