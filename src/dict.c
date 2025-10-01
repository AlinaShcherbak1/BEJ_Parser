#include "dict.h"
#include "io.h"
#include <stdlib.h>
#include <string.h>

static uint16_t rd16(const uint8_t* p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t rd32(const uint8_t* p) { return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24)); }

int dict_load(const char* path, bej_dictionary_t* out) {
    memset(out, 0, sizeof(*out));
    if (read_file_all(path, &out->bytes, &out->size) != 0) return -1;
    if (out->size < 12) return -1;

    const uint8_t* p = out->bytes;
    out->version_tag = p[0];
    out->dict_flags = p[1];
    out->entry_count = rd16(p + 2);
    out->schema_version = rd32(p + 4);
    out->dict_size = rd32(p + 8);

    out->header_size = 1 + 1 + 2 + 4 + 4; 
    out->entry_size = 1 + 2 + 2 + 2 + 1 + 2;

    size_t need = out->header_size + (size_t)out->entry_count * out->entry_size;
    if (out->size < need) return -1;

    out->entries = (bej_dict_entry_t*)calloc(out->entry_count, sizeof(bej_dict_entry_t));
    if (!out->entries) return -1;

    const uint8_t* pool = out->bytes;
    for (uint16_t i = 0; i < out->entry_count; ++i) {
        const uint8_t* e = p + out->header_size + (size_t)i * out->entry_size;
        uint8_t fmtf = e[0];
        out->entries[i].format = (fmtf >> 4) & 0x0F;
        out->entries[i].flags = fmtf & 0x0F;
        out->entries[i].seq = rd16(e + 1);

        uint16_t child_ptr_bin = rd16(e + 3);
        uint16_t child_count = rd16(e + 5);
        uint8_t  name_len = e[7];
        uint16_t name_off = rd16(e + 8);


        if (child_ptr_bin != 0) {
            int32_t idx = (int32_t)((child_ptr_bin - out->header_size) / out->entry_size);
            out->entries[i].child_first_idx = idx;
            out->entries[i].child_count = child_count;
        }
        else {
            out->entries[i].child_first_idx = -1;
            out->entries[i].child_count = 0;
        }

        if (name_off) {
            out->entries[i].name = (const char*)(pool + name_off);
        }
        else {
            out->entries[i].name = NULL;
        }
    }
    return 0;
}

void dict_free(bej_dictionary_t* d) {
    if (!d) return;
    free(d->entries);
    free(d->bytes);
    memset(d, 0, sizeof(*d));
}

dict_subset_t dict_children(const bej_dictionary_t* d, int parent_idx) {
    if (parent_idx < 0) {
        if (d->entry_count == 0) return (dict_subset_t) { 0 };
        const bej_dict_entry_t* root = &d->entries[0];
        if (root->child_first_idx >= 0 && root->child_count) {
            return (dict_subset_t) { .list = &d->entries[root->child_first_idx], .count = root->child_count };
        }
        return (dict_subset_t) { .list = NULL, .count = 0 };
    }
    const bej_dict_entry_t* e = &d->entries[parent_idx];
    if (e->child_first_idx >= 0 && e->child_count) {
        return (dict_subset_t) { .list = &d->entries[e->child_first_idx], .count = e->child_count };
    }
    return (dict_subset_t) { .list = NULL, .count = 0 };
}

const bej_dict_entry_t* dict_child_by_seq(const dict_subset_t* sub, uint16_t seq) {
    if (!sub || !sub->list) return NULL;
    for (uint16_t i = 0; i < sub->count; ++i) {
        if (sub->list[i].seq == seq) return &sub->list[i];
    }
    return NULL;
}
