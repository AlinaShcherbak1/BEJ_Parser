/** @file main.c
 *  @brief CLI: bej2json -s <schema.bin> -b <payload.bej> -o <out.json>
 */

#include "dict.h"
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char* prog) {
    fprintf(stderr,
        "Usage:\n"
        "  %s -s <schema_dict.bin> -b <payload.bej> -o <out.json>\n"
        "Options:\n"
        "  -s   Path to major schema binary dictionary (.bin)\n"
        "  -b   Path to BEJ-encoded payload\n"
        "  -o   Output JSON file (UTF-8)\n"
        "Notes:\n"
        "  * Annotation dictionary is ignored in this simplified build.\n", prog);
}

int main(int argc, char** argv) {
    const char* dict_path = NULL;
    const char* bej_path = NULL;
    const char* out_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-s") && i + 1 < argc) dict_path = argv[++i];
        else if (!strcmp(argv[i], "-b") && i + 1 < argc) bej_path = argv[++i];
        else if (!strcmp(argv[i], "-o") && i + 1 < argc) out_path = argv[++i];
        else { usage(argv[0]); return 2; }
    }
    if (!dict_path || !bej_path || !out_path) { usage(argv[0]); return 2; }

    bej_dictionary_t dict = { 0 };
    if (dict_load(dict_path, &dict) != 0) {
        fprintf(stderr, "Failed to load dictionary: %s\n", dict_path);
        return 1;
    }

    uint8_t* bej = NULL; size_t bej_sz = 0;
    if (read_file_all(bej_path, &bej, &bej_sz) != 0) {
        fprintf(stderr, "Failed to read BEJ payload: %s\n", bej_path);
        dict_free(&dict);
        return 1;
    }

    FILE* out = fopen(out_path, "wb");
    if (!out) {
        fprintf(stderr, "Cannot open output: %s\n", out_path);
        free(bej); dict_free(&dict);
        return 1;
    }

    int rc = bej_decode_to_json(out, bej, bej_sz, &dict);
    fclose(out);
    free(bej);
    dict_free(&dict);

    if (rc != 0) {
        fprintf(stderr, "Decode failed (code %d)\n", rc);
        return 1;
    }
    return 0;
}
