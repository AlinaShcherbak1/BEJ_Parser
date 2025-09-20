#ifndef JSON_WRITER_H
#define JSON_WRITER_H

#include <stdio.h>

/** Minimal JSON writer with streaming output. */
typedef struct jsonw {
  FILE *f;
  int need_comma[32];
  int depth;
} jsonw_t;

void jsonw_init(jsonw_t *w, FILE *f);
void jsonw_begin_obj(jsonw_t *w);
void jsonw_end_obj(jsonw_t *w);
void jsonw_begin_arr(jsonw_t *w);
void jsonw_end_arr(jsonw_t *w);
void jsonw_key(jsonw_t *w, const char *key);
void jsonw_string(jsonw_t *w, const char *s);
void jsonw_int64(jsonw_t *w, long long v);

#endif
