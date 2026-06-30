#ifndef STRCASECMP_H
#define STRCASECMP_H

#include <cstdint>

int
strcasecmp(const char *s1, const char *s2);

int
strncasecmp(const char *s1, const char *s2, size_t n);
#endif