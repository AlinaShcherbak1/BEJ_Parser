#ifndef DICT_H
#define DICT_H

#include "bej.h"

int dict_load(const char* path, bej_dictionary_t* out);
void dict_free(bej_dictionary_t* d);

typedef struct {
    const bej_dict_entry_t* list;
    uint16_t count;
} dict_subset_t;

dict_subset_t dict_children(const bej_dictionary_t* d, int parent_idx);
const bej_dict_entry_t* dict_child_by_seq(const dict_subset_t* sub, uint16_t seq);

#endif /* DICT_H */
