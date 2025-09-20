#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <stddef.h>

int read_entire_file(const char *path, uint8_t **out_buf, size_t *out_len);

#endif
