#ifndef BEJ_H
#define BEJ_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    enum {
        BEJ_FMT_SET = 0x00,
        BEJ_FMT_ARRAY = 0x01,
        BEJ_FMT_INTEGER = 0x03,
        BEJ_FMT_ENUM = 0x04,
        BEJ_FMT_STRING = 0x05,
        BEJ_FMT_REAL = 0x06,
        BEJ_FMT_BOOLEAN = 0x07,
        BEJ_FMT_CHOICE = 0x09,
        BEJ_FMT_NULL = 0x0A,
        BEJ_FMT_RESOURCE_LINK = 0x0E,
        BEJ_FMT_PROPERTY_ANN = 0x0F
    };

    enum {
        BEJ_SEL_MAJOR = 0,
        BEJ_SEL_ANNOT = 1
    };

    typedef struct {
        uint8_t  format_and_flags;
        uint16_t seq;
        uint16_t child_ptr_bin;
        uint16_t child_count;
        uint8_t  name_len;
        uint16_t name_off;
    } bej_dict_entry_disk_t;

    typedef struct bej_dict_entry_s {
        uint8_t  format;
        uint8_t  flags;
        uint16_t seq;
        int32_t  child_first_idx;
        uint16_t child_count;
        const char* name;
    } bej_dict_entry_t;

    typedef struct {
        uint8_t* bytes;
        size_t   size;

        uint8_t  version_tag;
        uint8_t  dict_flags;
        uint16_t entry_count;
        uint32_t schema_version;
        uint32_t dict_size;

        bej_dict_entry_t* entries;

        uint32_t header_size;
        uint32_t entry_size;
    } bej_dictionary_t;

    typedef struct {
        const uint8_t* p;
        size_t  size;
        size_t  pos;
    } bej_stream_t;

    int bej_decode_to_json(FILE* out,
        const uint8_t* bej, size_t bej_size,
        const bej_dictionary_t* dict_major);

    void bej_stream_init(bej_stream_t* s, const uint8_t* buf, size_t n);
    int  bej_read_nnint(bej_stream_t* s, uint64_t* out);
    int  bej_peek_format(bej_stream_t* s, uint8_t* fmt, uint8_t* flags);
    int  bej_read_sfl(bej_stream_t* s, uint64_t* seq, uint8_t* fmt, uint64_t* len, uint8_t* flags);
    int  bej_expect_bytes(bej_stream_t* s, const uint8_t* bytes, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* BEJ_H */
