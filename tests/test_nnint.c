#include "bej.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void make_nnint(uint8_t* buf, size_t* n, uint64_t v, uint8_t len) {
    buf[0] = len;
    for (uint8_t i = 0; i < len; i++) buf[1 + i] = (uint8_t)((v >> (8 * i)) & 0xFF);
    *n = (size_t)1 + len;
}

int main(void) {

    uint8_t tmp[16]; size_t n = 0;
    uint64_t out = 0;
    bej_stream_t s;

    make_nnint(tmp, &n, 0, 1);
    bej_stream_init(&s, tmp, n); assert(bej_read_nnint(&s, &out) == 0 && out == 0);

    make_nnint(tmp, &n, 65, 1);
    bej_stream_init(&s, tmp, n); assert(bej_read_nnint(&s, &out) == 0 && out == 65);

    make_nnint(tmp, &n, 1337, 2);
    bej_stream_init(&s, tmp, n); assert(bej_read_nnint(&s, &out) == 0 && out == 1337);

    puts("OK");
    return 0;
}
