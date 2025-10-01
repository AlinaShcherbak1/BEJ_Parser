#include "io.h"
#include <stdio.h>
#include <stdlib.h>

int read_file_all(const char* path, uint8_t** out_data, size_t* out_size) {
    *out_data = NULL; *out_size = 0;
    FILE* f = fopen(path, "rb");
    if (!f) return -1;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return -1; }
    rewind(f);
    uint8_t* buf = (uint8_t*)malloc((size_t)sz);
    if (!buf) { fclose(f); return -1; }
    size_t n = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (n != (size_t)sz) { free(buf); return -1; }
    *out_data = buf;
    *out_size = (size_t)sz;
    return 0;
}

int write_file_all(const char* path, const char* data, size_t size) {
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    size_t n = fwrite(data, 1, size, f);
    fclose(f);
    return n == size ? 0 : -1;
}
