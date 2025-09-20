#include "bitbuf.h"

void bitbuf_init(bitbuf_t *bb, const void *data, size_t len) {
  bb->p = (const uint8_t*)data;
  bb->end = bb->p + len;
}

size_t bitbuf_remaining(const bitbuf_t *bb) {
  return (size_t)(bb->end - bb->p);
}

int bitbuf_get_u8(bitbuf_t *bb, uint8_t *out) {
  if (bb->p >= bb->end) return -1;
  *out = *bb->p++;
  return 0;
}

int bitbuf_peek_u8(const bitbuf_t *bb, uint8_t *out) {
  if (bb->p >= bb->end) return -1;
  *out = *bb->p;
  return 0;
}

int bitbuf_get_bytes(bitbuf_t *bb, void *dst, size_t n) {
  if ((size_t)(bb->end - bb->p) < n) return -1;
  for (size_t i = 0; i < n; ++i) ((uint8_t*)dst)[i] = bb->p[i];
  bb->p += n;
  return 0;
}

int bitbuf_get_uleb128(bitbuf_t *bb, uint64_t *out) {
  uint64_t v = 0;
  unsigned shift = 0;
  for (;;) {
    uint8_t b;
    if (bitbuf_get_u8(bb, &b) != 0) return -1;
    v |= ((uint64_t)(b & 0x7F)) << shift;
    if ((b & 0x80) == 0) break;
    shift += 7;
    if (shift > 63) return -1; /* overflow */
  }
  *out = v;
  return 0;
}
