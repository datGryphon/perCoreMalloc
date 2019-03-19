#include "malloc.h"
#include "calloc.h"
#include "realloc.h"
#include "free.h"

int posix_memalign(void **memptr, size_t alignment, size_t size);
void * aligned_alloc(size_t alignment, size_t size);
void * valloc(size_t size);
void * reallocarray(void *ptr, size_t nmemb, size_t size);
void * memalign(size_t alignment, size_t size);
void * pvalloc(size_t size);