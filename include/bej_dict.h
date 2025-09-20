#ifndef BEJ_DICT_H
#define BEJ_DICT_H

#include <stdint.h>

/** A minimal dictionary entry mapping a SID -> name + expected type. */
typedef struct bej_dict_entry {
  uint32_t sid;
  const char *name;   /* points into an internal arena or static buffer */
  int expected_tag;   /* from enum bej_tag */
} bej_dict_entry_t;

/** Opaque dictionary handle. */
typedef struct bej_dict bej_dict_t;

bej_dict_t* bej_dict_load_csv(const char *path);
bej_dict_t* bej_dict_load_dsp8010_bin(const char *path); /* stub: returns NULL in phase 1 */
void bej_dict_free(bej_dict_t *d);

/** Lookup by SID, return NULL if not found. */
const bej_dict_entry_t* bej_dict_find(const bej_dict_t *d, uint32_t sid);

#endif
