#ifndef BEJ_H
#define BEJ_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "bej_dict.h"

/**\file bej.h
 * Subset BEJ decoder (Sets, Integers, Strings, Arrays of Integers).
 *
 * The real DSP0218 format is richer; this provides a stepping stone.
 */

typedef struct bej_decoder {
  int verbose;
} bej_decoder_t;

/** Decode BEJ buffer to JSON text, using a dictionary for field names. */
int bej_decode_to_json(const uint8_t *buf, size_t len,
                       const bej_dict_t *dict,
                       FILE *out,
                       int skip_annotations,
                       int verbose);

#endif
