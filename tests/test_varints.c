#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "unity.h" /* single TU include of the minimal tester */
#include "bitbuf.h"

TEST_CASE(test_uleb128_basic) {
  uint8_t buf1[] = { 0x7F }; /* 127 */
  bitbuf_t bb; bitbuf_init(&bb, buf1, sizeof(buf1));
  uint64_t v=0; TEST_ASSERT_TRUE(bitbuf_get_uleb128(&bb, &v) == 0); TEST_ASSERT_EQ_U64(v, 127);
}

TEST_CASE(test_uleb128_multibyte) {
  uint8_t buf[] = { 0xE5, 0x8E, 0x26 }; /* 624485 (Wiki) */
  bitbuf_t bb; bitbuf_init(&bb, buf, sizeof(buf));
  uint64_t v=0; TEST_ASSERT_TRUE(bitbuf_get_uleb128(&bb, &v) == 0); TEST_ASSERT_EQ_U64(v, 624485ULL);
}

int main(void) {
  run(test_uleb128_basic, "ULEB128 basic");
  run(test_uleb128_multibyte, "ULEB128 multibyte");
  return unity_finish();
}
