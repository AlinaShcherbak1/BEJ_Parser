#include "bej.h"
#include "bej_dict.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *argv0) {
  fprintf(stderr, "Usage: %s -d <dict.csv> -b <bej.bin> -o <out.json> [--skip-anno] [-v]\n", argv0);
}

int main(int argc, char **argv) {
  const char *dict_path = NULL;
  const char *bej_path = NULL;
  const char *out_path = NULL;
  int verbose = 0;

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) dict_path = argv[++i];
    else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) bej_path = argv[++i];
    else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) out_path = argv[++i];
    else if (strcmp(argv[i], "-v") == 0) verbose = 1;
    else if (strcmp(argv[i], "--skip-anno") == 0) { /* ignored in subset */ }
    else { usage(argv[0]); return 2; }
  }
  if (!dict_path || !bej_path || !out_path) { usage(argv[0]); return 2; }

  bej_dict_t *dict = NULL;
  if (strstr(dict_path, ".csv")) dict = bej_dict_load_csv(dict_path);
  else dict = bej_dict_load_dsp8010_bin(dict_path);
  if (!dict) { fprintf(stderr, "Failed to load dictionary: %s\n", dict_path); return 1; }

  uint8_t *data = NULL; size_t len = 0;
  if (read_entire_file(bej_path, &data, &len) != 0) { fprintf(stderr, "Failed to read %s\n", bej_path); bej_dict_free(dict); return 1; }

  FILE *out = fopen(out_path, "wb");
  if (!out) { fprintf(stderr, "Failed to open %s\n", out_path); free(data); bej_dict_free(dict); return 1; }

  int rc = bej_decode_to_json(data, len, dict, out, 1, verbose);
  fclose(out);
  free(data);
  bej_dict_free(dict);
  return rc == 0 ? 0 : 1;
}
