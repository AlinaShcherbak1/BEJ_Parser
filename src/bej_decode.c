#include "bej.h"
#include "dict.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

static void json_write_escaped(FILE* out, const char* s, size_t n);


static int pp_level = 0;

static void pp_indent(FILE* out) {
    for (int i = 0; i < pp_level * 2; ++i) fputc(' ', out);
}


static void pp_nl(FILE* out) {
    fputc('\n', out);
    pp_indent(out);
}

static int decode_enum_with_dict(FILE* out,
    bej_stream_t* s,
    uint64_t length,
    const bej_dictionary_t* dict,
    const dict_subset_t* current_children,
    uint16_t seq_of_field) {
    (void)length;

    uint64_t val_seq = 0;
    if (bej_read_nnint(s, &val_seq) != 0) return -1;

    const bej_dict_entry_t* enum_field = dict_child_by_seq(current_children, (uint16_t)seq_of_field);
    if (!enum_field) { fprintf(out, "%" PRIu64, val_seq); return 0; }


    dict_subset_t variants = dict_children(dict, (int)(enum_field - dict->entries));
    const bej_dict_entry_t* variant = dict_child_by_seq(&variants, (uint16_t)val_seq);
    if (!variant || !variant->name) {
        fprintf(out, "%" PRIu64, val_seq);
        return 0;
    }
    
    json_write_escaped(out, variant->name, strlen(variant->name));
    return 0;
}




void bej_stream_init(bej_stream_t* s, const uint8_t* buf, size_t n) {
    s->p = buf; s->size = n; s->pos = 0;
}

static int bej_read_u8(bej_stream_t* s, uint8_t* out) {
    if (s->pos >= s->size) return -1;
    *out = s->p[s->pos++];
    return 0;
}

static int bej_read(bej_stream_t* s, void* dst, size_t n) {
    if (s->pos + n > s->size) return -1;
    memcpy(dst, s->p + s->pos, n);
    s->pos += n;
    return 0;
}

int bej_expect_bytes(bej_stream_t* s, const uint8_t* bytes, size_t n) {
    if (s->pos + n > s->size) return -1;
    if (memcmp(s->p + s->pos, bytes, n) != 0) return -1;
    s->pos += n;
    return 0;
}

int bej_read_nnint(bej_stream_t* s, uint64_t* out) {
    uint8_t count = 0;
    if (bej_read_u8(s, &count) != 0) return -1;
    if (count == 0) { *out = 0; return 0; }
    if (count > 8) return -1;
    uint8_t buf[8] = { 0 };
    if (bej_read(s, buf, count) != 0) return -1;
    uint64_t v = 0;
    for (int i = (int)count - 1; i >= 0; --i) v = (v << 8) | buf[i];
    *out = v;
    return 0;
}

int bej_peek_format(bej_stream_t* s, uint8_t* fmt, uint8_t* flags) {
    size_t saved = s->pos;
    uint64_t dummy;
    if (bej_read_nnint(s, &dummy) != 0) { s->pos = saved; return -1; }
    uint8_t ff;
    if (bej_read_u8(s, &ff) != 0) { s->pos = saved; return -1; }
    *fmt = (uint8_t)((ff >> 4) & 0x0F);
    *flags = (uint8_t)(ff & 0x0F);
    s->pos = saved;
    return 0;
}


int bej_read_sfl(bej_stream_t* s, uint64_t* seq, uint8_t* fmt, uint64_t* len, uint8_t* flags) {
    if (bej_read_nnint(s, seq) != 0) return -1;
    uint8_t ff;
    if (bej_read_u8(s, &ff) != 0) return -1;
    *fmt = (uint8_t)((ff >> 4) & 0x0F);
    *flags = (uint8_t)(ff & 0x0F);
    if (bej_read_nnint(s, len) != 0) return -1;
    return 0;
}


static void json_write_escaped(FILE* out, const char* s, size_t n) {
    fputc('"', out);
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)s[i];
        switch (c) {
        case '\"': fputs("\\\"", out); break;
        case '\\': fputs("\\\\", out); break;
        case '\b': fputs("\\b", out); break;
        case '\f': fputs("\\f", out); break;
        case '\n': fputs("\\n", out); break;
        case '\r': fputs("\\r", out); break;
        case '\t': fputs("\\t", out); break;
        default:
            if (c < 0x20) fprintf(out, "\\u%04x", c);
            else fputc(c, out);
        }
    }
    fputc('"', out);
}

static int json_write_name(FILE* out, const char* name) {
    if (!name) return -1;
    json_write_escaped(out, name, strlen(name));
    fputc(':', out);
    fputc(' ', out); 
    return 0;
}


static int read_exact(bej_stream_t* s, uint8_t* buf, size_t n) {
    return bej_read(s, buf, n);
}



static int decode_enum(FILE* out, bej_stream_t* s, uint64_t length) {
    (void)length;
    uint64_t val = 0;
    if (bej_read_nnint(s, &val) != 0) return -1;
    fprintf(out, "%" PRIu64, val);
    return 0;
}

static int decode_string(FILE* out, bej_stream_t* s, uint64_t length) {
    if (length == 0 || length > (1ull << 31)) return -1;
    char* tmp = (char*)malloc((size_t)length);
    if (!tmp) return -1;
    if (read_exact(s, (uint8_t*)tmp, (size_t)length) != 0) { free(tmp); return -1; }

    size_t n = (size_t)length;
    if (n && tmp[n - 1] == '\0') n -= 1;
    json_write_escaped(out, tmp, n);
    free(tmp);
    return 0;
}

static int decode_boolean(FILE* out, bej_stream_t* s, uint64_t length) {
    if (length != 1) return -1;
    uint8_t v;
    if (bej_read_u8(s, &v) != 0) return -1;
    fputs(v ? "true" : "false", out);
    return 0;
}

static int decode_integer(FILE* out, bej_stream_t* s, uint64_t length) {
    if (length == 0 || length > 8) return -1;
    uint8_t buf[8] = { 0 };
    if (read_exact(s, buf, (size_t)length) != 0) return -1;
    int64_t v = 0;
    for (int i = (int)length - 1; i >= 0; --i) v = (v << 8) | buf[i];

    if (length < 8 && (buf[length - 1] & 0x80)) {
        for (int i = (int)length; i < 8; ++i) ((uint8_t*)&v)[i] = 0xFF;
    }
    fprintf(out, "%" PRId64, v);
    return 0;
}

static int decode_real(FILE* out, bej_stream_t* s, uint64_t length) {
    size_t start = s->pos;
    uint64_t lenWhole = 0; if (bej_read_nnint(s, &lenWhole) != 0) return -1;
    if (lenWhole > 8) return -1;
    int64_t whole = 0; 
    if (lenWhole) {
        uint8_t w[8] = { 0 }; if (read_exact(s, w, (size_t)lenWhole) != 0) return -1;
        for (int i = (int)lenWhole - 1; i >= 0; --i) whole = (whole << 8) | w[i];
        if (lenWhole < 8 && (w[lenWhole - 1] & 0x80)) { for (int i = (int)lenWhole; i < 8; ++i) ((uint8_t*)&whole)[i] = 0xFF; }
    }
    uint64_t lz = 0; if (bej_read_nnint(s, &lz) != 0) return -1;
    uint64_t fract = 0; if (bej_read_nnint(s, &fract) != 0) return -1;
    uint64_t lenExp = 0; if (bej_read_nnint(s, &lenExp) != 0) return -1;
    int64_t expv = 0;
    if (lenExp) {
        if (lenExp > 8) return -1;
        uint8_t e[8] = { 0 }; if (read_exact(s, e, (size_t)lenExp) != 0) return -1;
        for (int i = (int)lenExp - 1; i >= 0; --i) expv = (expv << 8) | e[i];
        if (lenExp < 8 && (e[lenExp - 1] & 0x80)) { for (int i = (int)lenExp; i < 8; ++i) ((uint8_t*)&expv)[i] = 0xFF; }
    }
  
    fprintf(out, "%lld.", (long long)whole);
    for (uint64_t i = 0; i < lz; ++i) fputc('0', out);
    fprintf(out, "%llu", (unsigned long long)fract);
    if (expv) fprintf(out, "e%lld", (long long)expv);

   
    if ((uint64_t)(s->pos - start) != length) return -1;
    return 0;
}


static int decode_value(FILE* out,
    bej_stream_t* s,
    const bej_dictionary_t* dict,
    const dict_subset_t* current_children,
    bool is_array_elem);

static int decode_set(FILE* out,
    bej_stream_t* s,
    const bej_dictionary_t* dict,
    const dict_subset_t* parent_children) {
    uint64_t seq_with_sel = 0, len = 0;
    uint8_t fmt = 0, flags = 0;
    if (bej_read_sfl(s, &seq_with_sel, &fmt, &len, &flags) != 0) return -1;
    if (fmt != BEJ_FMT_SET) return -1;

    uint64_t count = 0; if (bej_read_nnint(s, &count) != 0) return -1;

    uint16_t seq = (uint16_t)((seq_with_sel >> 1) & 0xFFFF);
    uint8_t selector = (uint8_t)(seq_with_sel & 0x1);
    if (selector != BEJ_SEL_MAJOR) return -2; 

    const bej_dict_entry_t* entry = NULL;
    dict_subset_t children;
    if (parent_children) {
        entry = dict_child_by_seq(parent_children, seq);
        if (!entry) return -1;
        children = dict_children(dict, (int)(entry - dict->entries));
    }
    else {
        children = dict_children(dict, -1);
    }

    fputc('{', out);
    for (uint64_t i = 0; i < count; ++i) {
        if (i) fputc(',', out);
        uint8_t cf = 0, cflags = 0;
        if (bej_peek_format(s, &cf, &cflags) != 0) return -1;

        size_t saved = s->pos;
        uint64_t cseq_sel = 0, clen = 0; uint8_t cfmt = 0, cfl = 0;
        if (bej_read_sfl(s, &cseq_sel, &cfmt, &clen, &cfl) != 0) return -1;
        s->pos = saved;

        uint16_t cseq = (uint16_t)((cseq_sel >> 1) & 0xFFFF);
        uint8_t csel = (uint8_t)(cseq_sel & 0x1);

        if (csel == BEJ_SEL_MAJOR) {
            const bej_dict_entry_t* child_def = dict_child_by_seq(&children, cseq);
            if (!child_def) return -1;
            if (child_def->name && *child_def->name) {
                json_write_name(out, child_def->name);
            }
            else {
                fprintf(out, "\"_%u\":", cseq);
            }
            if (decode_value(out, s, dict, &children, false) != 0) return -1;
        }
        else {

            uint64_t skip_seq = 0, skip_len = 0; uint8_t skip_fmt = 0, skip_flags2 = 0;
            if (bej_read_sfl(s, &skip_seq, &skip_fmt, &skip_len, &skip_flags2) != 0) return -1;

            if (s->pos + skip_len > s->size) return -1;
            s->pos += (size_t)skip_len;

            return -2;
        }
    }
    fputc('}', out);
    return 0;
}

static int decode_array(FILE* out,
    bej_stream_t* s,
    const bej_dictionary_t* dict,
    const dict_subset_t* parent_children) {
    uint64_t seq_with_sel = 0, len = 0; uint8_t fmt = 0, flags = 0;
    if (bej_read_sfl(s, &seq_with_sel, &fmt, &len, &flags) != 0) return -1;
    if (fmt != BEJ_FMT_ARRAY) return -1;

    uint64_t count = 0; if (bej_read_nnint(s, &count) != 0) return -1;

    uint16_t seq = (uint16_t)((seq_with_sel >> 1) & 0xFFFF);
    uint8_t selector = (uint8_t)(seq_with_sel & 0x1);
    if (selector != BEJ_SEL_MAJOR) return -2;

    const bej_dict_entry_t* arr_def = dict_child_by_seq(parent_children, seq);
    if (!arr_def) return -1;

    dict_subset_t elem_sub = dict_children(dict, (int)(arr_def - dict->entries));

    fputc('[', out);
    for (uint64_t i = 0; i < count; ++i) {
        if (i) fputc(',', out);
        if (decode_value(out, s, dict, &elem_sub, true) != 0) return -1;
    }
    fputc(']', out);
    return 0;
}

static int decode_value(FILE* out,
    bej_stream_t* s,
    const bej_dictionary_t* dict,
    const dict_subset_t* current_children,
    bool is_array_elem) {
    uint64_t seq_sel = 0, len = 0; uint8_t fmt = 0, flags = 0;
    if (bej_read_sfl(s, &seq_sel, &fmt, &len, &flags) != 0) return -1;

    switch (fmt) {
    case BEJ_FMT_SET: {
        uint64_t count = 0;
        if (bej_read_nnint(s, &count) != 0) return -1;

        uint16_t seq = (uint16_t)((seq_sel >> 1) & 0xFFFF);

        dict_subset_t kids = (dict_subset_t){ 0 };
        int have_schema = 0;

        if (!current_children) {
            kids = dict_children(dict, -1);
            have_schema = 1;
        }
        else {
            const bej_dict_entry_t* def = dict_child_by_seq(current_children, seq);
            if (def) {
                kids = dict_children(dict, (int)(def - dict->entries));
                have_schema = 1;
            }
        }

        fputc('{', out);
        int emitted = 0;
        if (count == 0) { fputc('}', out); return 0; }
        pp_level++;
        pp_nl(out);

        for (uint64_t i = 0; i < count; ++i) {
            size_t saved = s->pos;
            uint64_t cseq_sel = 0, clen = 0; uint8_t cfmt = 0, cfl = 0;
            if (bej_read_sfl(s, &cseq_sel, &cfmt, &clen, &cfl) != 0) return -1;
            s->pos = saved;

            uint16_t cseq = (uint16_t)((cseq_sel >> 1) & 0xFFFF);
            uint8_t  csel = (uint8_t)(cseq_sel & 0x1);

            if (csel == BEJ_SEL_MAJOR) {
                const char* nm = NULL;
                if (have_schema) {
                    const bej_dict_entry_t* child_def = dict_child_by_seq(&kids, cseq);
                    if (child_def && child_def->name && *child_def->name) nm = child_def->name;
                }
                if (emitted++) { fputc(',', out); pp_nl(out); }
                if (nm) {
                    json_write_escaped(out, nm, strlen(nm));
                    fputc(':', out); fputc(' ', out);
                    if (decode_value(out, s, dict, &kids, false) != 0) return -1;
                }
                else {
                    fprintf(out, "\"_%u\": ", cseq);
                    if (decode_value(out, s, dict, NULL, false) != 0) return -1;
                }
            }
            else {
                uint64_t skip_seq = 0, skip_len = 0; uint8_t skip_fmt = 0, skip_flags2 = 0;
                if (bej_read_sfl(s, &skip_seq, &skip_fmt, &skip_len, &skip_flags2) != 0) return -1;
                if (s->pos + skip_len > s->size) return -1;
                s->pos += (size_t)skip_len;
            }
        }
        pp_level--;
        pp_nl(out);
        fputc('}', out);
        return 0;
    }

    case BEJ_FMT_ARRAY: {
        uint64_t count = 0;
        if (bej_read_nnint(s, &count) != 0) return -1;

        uint16_t seq = (uint16_t)((seq_sel >> 1) & 0xFFFF);

        dict_subset_t elem_sub = (dict_subset_t){ 0 };
        int have_schema = 0;
        if (current_children) {
            const bej_dict_entry_t* def = dict_child_by_seq(current_children, seq);
            if (def) {
                elem_sub = dict_children(dict, (int)(def - dict->entries));
                have_schema = 1;
            }
        }

        fputc('[', out);
        if (count == 0) { fputc(']', out); return 0; }
        pp_level++;
        pp_nl(out);

        for (uint64_t i = 0; i < count; ++i) {
            if (i) { fputc(',', out); pp_nl(out); }
            if (decode_value(out, s, dict, have_schema ? &elem_sub : NULL, true) != 0) return -1;
        }
        pp_level--;
        pp_nl(out);
        fputc(']', out);
        return 0;
    }

    case BEJ_FMT_STRING:  return decode_string(out, s, len);
    case BEJ_FMT_INTEGER: return decode_integer(out, s, len);
    case BEJ_FMT_BOOLEAN: return decode_boolean(out, s, len);
    case BEJ_FMT_REAL:    return decode_real(out, s, len);

    case BEJ_FMT_ENUM: {
        
        uint16_t seq_field = (uint16_t)((seq_sel >> 1) & 0xFFFF);
        return decode_enum_with_dict(out, s, len, dict, current_children, seq_field);
    }
    case BEJ_FMT_NULL: {

        if (s->pos + len > s->size) return -1;
        s->pos += (size_t)len;
        fputs("null", out);
        return 0;
    }

    default:
        return -1;
    }
}

int bej_decode_to_json(FILE* out,
    const uint8_t* bej, size_t bej_size,
    const bej_dictionary_t* dict_major) {
    bej_stream_t ss; bej_stream_init(&ss, bej, bej_size);

    uint8_t ver[4];
    if (bej_read(&ss, ver, 4) != 0) return -1;

    uint8_t flags[2]; if (bej_read(&ss, flags, 2) != 0) return -1;
    uint8_t schemaClass; if (bej_read(&ss, &schemaClass, 1) != 0) return -1;
    if (!(schemaClass == 0x00 || schemaClass == 0x01)) {
        return -2;
    }
    return decode_value(out, &ss, dict_major, NULL, false);
}
