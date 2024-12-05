#include <stdlib.h>

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

char* xmalloc(size_t size);
void xfree(void *ptr);
void* xrealloc(void* ptr, size_t size);

#endif