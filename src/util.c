#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int read_entire_file(const char *path, uint8_t **out_buf, size_t *out_len) {
  *out_buf = NULL; *out_len = 0;
  FILE *f = fopen(path, "rb");
  if (!f) return -1;
  if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
  long n = ftell(f);
  if (n < 0) { fclose(f); return -1; }
  rewind(f);
  uint8_t *buf = (uint8_t*)malloc((size_t)n);
  if (!buf) { fclose(f); return -1; }
  if (fread(buf, 1, (size_t)n, f) != (size_t)n) { free(buf); fclose(f); return -1; }
  fclose(f);
  *out_buf = buf; *out_len = (size_t)n;
  return 0;
}
