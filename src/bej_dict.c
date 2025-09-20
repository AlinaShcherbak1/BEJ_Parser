#include "bej_dict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bej_dict {
  bej_dict_entry_t *entries;
  size_t count;
  char *arena;
  size_t arena_len;
};

static int parse_tag(const char *s) {
  if (!s) return -1;
  if (strcmp(s, "int") == 0) return 0x10;
  if (strcmp(s, "string") == 0) return 0x40;
  if (strcmp(s, "set") == 0) return 0x30;
  if (strcmp(s, "array-int") == 0) return 0x60; /* array of int (simplified) */
  return -1;
}

bej_dict_t* bej_dict_load_csv(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  bej_dict_t *d = (bej_dict_t*)calloc(1, sizeof(*d));
  if (!d) { fclose(f); return NULL; }

  /* First pass: count lines */
  size_t lines = 0;
  char buf[512];
  while (fgets(buf, sizeof(buf), f)) {
    if (buf[0] == '#' || buf[0] == '\n' || buf[0] == '\r') continue;
    ++lines;
  }
  rewind(f);

  d->entries = (bej_dict_entry_t*)calloc(lines, sizeof(*d->entries));
  d->count = 0;

  /* Arena for strings */
  size_t arena_cap = lines * 64u;
  d->arena = (char*)malloc(arena_cap);
  d->arena_len = 0;

  while (fgets(buf, sizeof(buf), f)) {
    if (buf[0] == '#' || buf[0] == '\n' || buf[0] == '\r') continue;
    char *sid_s = strtok(buf, ",\r\n");
    char *name  = strtok(NULL, ",\r\n");
    char *type  = strtok(NULL, ",\r\n");
    if (!sid_s || !name) continue;
    unsigned long sid_ul = strtoul(sid_s, NULL, 10);
    int tag = parse_tag(type);

    size_t nlen = strlen(name);
    if (d->arena_len + nlen + 1 > arena_cap) {
      size_t newcap = arena_cap * 2 + 128;
      char *na = (char*)realloc(d->arena, newcap);
      if (!na) { fclose(f); return d; }
      d->arena = na; arena_cap = newcap;
    }
    char *dst = d->arena + d->arena_len;
    memcpy(dst, name, nlen + 1);
    d->arena_len += nlen + 1;

    d->entries[d->count].sid = (uint32_t)sid_ul;
    d->entries[d->count].name = dst;
    d->entries[d->count].expected_tag = tag;
    d->count++;
  }
  fclose(f);
  return d;
}

bej_dict_t* bej_dict_load_dsp8010_bin(const char *path) {
  (void)path; /* not implemented in phase 1 */
  return NULL;
}

void bej_dict_free(bej_dict_t *d) {
  if (!d) return;
  free(d->entries);
  free(d->arena);
  free(d);
}

const bej_dict_entry_t* bej_dict_find(const bej_dict_t *d, uint32_t sid) {
  if (!d) return NULL;
  for (size_t i = 0; i < d->count; ++i) {
    if (d->entries[i].sid == sid) return &d->entries[i];
  }
  return NULL;
}
