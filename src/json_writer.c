#include "json_writer.h"
#include <string.h>

static void jsonw_write_comma(jsonw_t *w) {
  if (w->depth > 0 && w->need_comma[w->depth - 1]) {
    fputc(',', w->f);
    fputc('\n', w->f);
    w->need_comma[w->depth - 1] = 0;
  }
}

void jsonw_init(jsonw_t *w, FILE *f) {
  w->f = f;
  memset(w->need_comma, 0, sizeof(w->need_comma));
  w->depth = 0;
}

void jsonw_begin_obj(jsonw_t *w) {
  jsonw_write_comma(w);
  fputc('{', w->f);
  fputc('\n', w->f);
  w->need_comma[w->depth++] = 0;
}

void jsonw_end_obj(jsonw_t *w) {
  fputc('\n', w->f);
  fputc('}', w->f);
  if (w->depth > 0) w->depth--;
  if (w->depth > 0) w->need_comma[w->depth - 1] = 1;
}

void jsonw_begin_arr(jsonw_t *w) {
  jsonw_write_comma(w);
  fputc('[', w->f);
  fputc('\n', w->f);
  w->need_comma[w->depth++] = 0;
}

void jsonw_end_arr(jsonw_t *w) {
  fputc('\n', w->f);
  fputc(']', w->f);
  if (w->depth > 0) w->depth--;
  if (w->depth > 0) w->need_comma[w->depth - 1] = 1;
}

void jsonw_key(jsonw_t *w, const char *key) {
  jsonw_write_comma(w);
  fprintf(w->f, "  \"%s\": ", key);
}

static void jsonw_escape_and_write(FILE *f, const char *s) {
  fputc('\"', f);
  for (; *s; ++s) {
    unsigned char c = (unsigned char)*s;
    switch (c) {
      case '\"': fputs("\\\"", f); break;
      case '\\': fputs("\\\\", f); break;
      case '\n': fputs("\\n", f); break;
      case '\r': fputs("\\r", f); break;
      case '\t': fputs("\\t", f); break;
      default:
        if (c < 0x20) fprintf(f, "\\u%04x", (unsigned)c);
        else fputc(c, f);
    }
  }
  fputc('\"', f);
}

void jsonw_string(jsonw_t *w, const char *s) {
  jsonw_write_comma(w);
  jsonw_escape_and_write(w->f, s);
  if (w->depth > 0) w->need_comma[w->depth - 1] = 1;
}

void jsonw_int64(jsonw_t *w, long long v) {
  jsonw_write_comma(w);
  fprintf(w->f, "%lld", v);
  if (w->depth > 0) w->need_comma[w->depth - 1] = 1;
}
