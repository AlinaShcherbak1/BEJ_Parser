#include "bej.h"
#include "bej_types.h"
#include "bitbuf.h"
#include "json_writer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int decode_value(bitbuf_t *bb, jsonw_t *jw, const bej_dict_t *dict, int tag, int depth, int verbose);

/* In subset, each element is encoded as: [tag:u8][sid:uleb128][len:uleb128][value...] */
static int decode_element(bitbuf_t *bb, jsonw_t *jw, const bej_dict_t *dict, int as_member, int verbose) {
  uint8_t tag;
  if (bitbuf_get_u8(bb, &tag) != 0) return -1;

  uint64_t sid = 0, len = 0;
  if (bitbuf_get_uleb128(bb, &sid) != 0) return -1;
  if (bitbuf_get_uleb128(bb, &len) != 0) return -1;

  const bej_dict_entry_t *de = bej_dict_find(dict, (uint32_t)sid);
  const char *name = de ? de->name : "unknown";

  if (as_member) jsonw_key(jw, name);
  if (verbose) {
    fprintf(stderr, "tag=0x%02X sid=%llu len=%llu name=%s\n", (unsigned)tag, (unsigned long long)sid, (unsigned long long)len, name);
  }

  /* Create a bounded view for the value */
  bitbuf_t vb = *bb;
  if (bitbuf_remaining(&vb) < len) return -1;
  vb.end = vb.p + len;

  int rc = decode_value(&vb, jw, dict, tag, 0, verbose);
  if (rc != 0) return rc;

  /* Advance parent buffer by len */
  bb->p += len;
  return 0;
}

static int decode_set(bitbuf_t *bb, jsonw_t *jw, const bej_dict_t *dict, int verbose) {
  jsonw_begin_obj(jw);
  while (bitbuf_remaining(bb) > 0) {
    int rc = decode_element(bb, jw, dict, 1, verbose);
    if (rc != 0) return rc;
  }
  jsonw_end_obj(jw);
  return 0;
}

static int decode_array_int(bitbuf_t *bb, jsonw_t *jw) {
  jsonw_begin_arr(jw);
  while (bitbuf_remaining(bb) > 0) {
    uint64_t v;
    if (bitbuf_get_uleb128(bb, &v) != 0) return -1;
    jsonw_int64(jw, (long long)v);
  }
  jsonw_end_arr(jw);
  return 0;
}

static int decode_value(bitbuf_t *bb, jsonw_t *jw, const bej_dict_t *dict, int tag, int depth, int verbose) {
  (void)depth; (void)verbose; (void)dict;
  switch (tag) {
    case BEJ_TAG_SET:
      return decode_set(bb, jw, dict, verbose);
    case BEJ_TAG_INTEGER: {
      uint64_t v;
      if (bitbuf_get_uleb128(bb, &v) != 0) return -1;
      jsonw_int64(jw, (long long)v);
      return 0;
    }
    case BEJ_TAG_STRING: {
      size_t n = bitbuf_remaining(bb);
      if (n > 1<<20) return -1; /* sanity */
      char *tmp = (char*)malloc(n + 1);
      if (!tmp) return -1;
      if (bitbuf_get_bytes(bb, tmp, n) != 0) { free(tmp); return -1; }
      tmp[n] = '\0';
      jsonw_string(jw, tmp);
      free(tmp);
      return 0;
    }
    case BEJ_TAG_ARRAY: {
      return decode_array_int(bb, jw);
    }
    default:
      return -1; /* unsupported */
  }
}

int bej_decode_to_json(const uint8_t *buf, size_t len,
                       const bej_dict_t *dict,
                       FILE *out,
                       int skip_annotations,
                       int verbose) {
  (void)skip_annotations; /* ignored in subset */
  jsonw_t jw; jsonw_init(&jw, out);
  bitbuf_t bb; bitbuf_init(&bb, buf, len);

  /* Top-level is a Set in our subset */
  int rc = decode_set(&bb, &jw, dict, verbose);
  fputc('\n', out);
  return rc;
}
