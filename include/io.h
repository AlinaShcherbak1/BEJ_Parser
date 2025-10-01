#ifndef IO_H
#define IO_H

#include <stddef.h>
#include <stdint.h>

int read_file_all(const char* path, uint8_t** out_data, size_t* out_size);

int write_file_all(const char* path, const char* data, size_t size);

#endif
