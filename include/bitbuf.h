#ifndef BITBUF_H
#define BITBUF_H

#include <stddef.h>
#include <stdint.h>

/** Simple byte-buffer reader with bounds checks. */
typedef struct bitbuf {
  const uint8_t *p;
  const uint8_t *end;
} bitbuf_t;

void bitbuf_init(bitbuf_t *bb, const void *data, size_t len);
size_t bitbuf_remaining(const bitbuf_t *bb);
int bitbuf_get_u8(bitbuf_t *bb, uint8_t *out);
int bitbuf_peek_u8(const bitbuf_t *bb, uint8_t *out);
int bitbuf_get_bytes(bitbuf_t *bb, void *dst, size_t n);

/** Read unsigned LEB128 (varint). Returns 0 on success. */
int bitbuf_get_uleb128(bitbuf_t *bb, uint64_t *out);

#endif
