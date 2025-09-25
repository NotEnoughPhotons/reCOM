#ifndef ZSYSTEM_ALLOC_H
#define ZSYSTEM_ALLOC_H
/// -------------------------------------------
/// ALLOCATION MACROS
/// -------------------------------------------
#define zmalloc(size) __malloc(size, __FILE__, __LINE__)
#define zcalloc(num, size) __calloc(num, size, __FILE__, __LINE__)
#define zrealloc(ptr, new_size) __realloc(ptr, new_size, __FILE__, __LINE__)
#define zmemalign(alignment, size) __memalign(alignment, size, __FILE__, __LINE__)
#define zstrdup(str) __strdup(str, __FILE__, __LINE__)
#define zfree(block) __free(block, __FILE__, __LINE__)
#endif