#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Our subset BEJ: elements encoded as [tag:1][sid:uleb128][len:uleb128][value...] */
/* Tags (must match decoder): */
#define T_SET   0x30
#define T_INT   0x10
#define T_STR   0x40
#define T_ARR   0x60

static void put_u8(FILE *f, uint8_t b){ fputc(b, f); }

static void put_uleb128(FILE *f, uint64_t v){
  do {
    uint8_t b = v & 0x7F;
    v >>= 7;
    if (v) b |= 0x80;
    fputc(b, f);
  } while (v);
}

/* Helper to write one element with raw value buffer */
static void write_elem(FILE *f, uint8_t tag, uint64_t sid, const uint8_t *val, size_t vlen){
  put_u8(f, tag);
  put_uleb128(f, sid);
  put_uleb128(f, (uint64_t)vlen);
  if (vlen) fwrite(val, 1, vlen, f);
}

/* Build an integer value (ULEB128) into a temp buffer and write */
static void write_int(FILE *f, uint64_t sid, uint64_t value){
  uint8_t tmp[16];
  size_t n = 0;
  uint64_t v = value;
  do {
    uint8_t b = v & 0x7F;
    v >>= 7;
    if (v) b |= 0x80;
    tmp[n++] = b;
  } while (v);
  write_elem(f, T_INT, sid, tmp, n);
}

/* Build a UTF-8 string value */
static void write_str(FILE *f, uint64_t sid, const char *s){
  write_elem(f, T_STR, sid, (const uint8_t*)s, strlen(s));
}

/* Build an array of ints */
static void write_arr_int(FILE *f, uint64_t sid, const uint64_t *vals, size_t count){
  /* First, build the payload (concatenated ULEB128 ints) into a small buffer */
  uint8_t buf[256];
  size_t n = 0;
  for (size_t i = 0; i < count; ++i){
    uint64_t v = vals[i];
    do {
      uint8_t b = v & 0x7F;
      v >>= 7;
      if (v) b |= 0x80;
      if (n < sizeof(buf)) buf[n++] = b;
    } while (v);
  }
  write_elem(f, T_ARR, sid, buf, n);
}

/* Build a nested set for MemoryLocation { Channel, Slot } */
static void write_memory_location(FILE *f){
  /* Build the inner set into a temp buffer to know its length */
  uint8_t buf[64];
  size_t n = 0;
  FILE *mem = tmpfile();
  if (!mem) { fprintf(stderr,"tmpfile failed\n"); exit(1); }

  /* Channel (sid=6) */
  /* write_elem writes [tag][sid][len][value]; here we write into mem, but need only payload of the set later */
  write_int(mem, 6, 0);
  /* Slot (sid=7) */
  write_int(mem, 7, 0);

  /* Extract bytes from mem */
  fflush(mem);
  fseek(mem, 0, SEEK_END);
  long len = ftell(mem);
  fseek(mem, 0, SEEK_SET);
  if (len < 0 || len > (long)sizeof(buf)) { fprintf(stderr,"tmp too big\n"); exit(1); }
  if (fread(buf, 1, (size_t)len, mem) != (size_t)len) { fprintf(stderr,"tmp read fail\n"); exit(1); }
  fclose(mem);

  /* Now wrap it as a set with sid=5 */
  write_elem(f, T_SET, 5, buf, (size_t)len);
}

int main(int argc, char **argv){
  const char *out = (argc > 1) ? argv[1] : "examples/example_bej.bin";
  FILE *f = fopen(out, "wb");
  if (!f) { fprintf(stderr, "open %s failed\n", out); return 1; }

  /* Build top-level set as concatenation of its member elements */
  uint8_t top[1024];
  size_t tn = 0;
  FILE *tmp = tmpfile();
  if (!tmp) { fprintf(stderr,"tmpfile failed\n"); fclose(f); return 1; }

  /* CapacityMiB (sid=1) = 65536 */
  write_int(tmp, 1, 65536);
  /* DataWidthBits (sid=2) = 64 */
  write_int(tmp, 2, 64);
  /* AllowedSpeedsMHz (sid=3) = [2400, 3200] */
  uint64_t arr[] = {2400, 3200};
  write_arr_int(tmp, 3, arr, 2);
  /* ErrorCorrection (sid=4) = "NoECC" */
  write_str(tmp, 4, "NoECC");
  /* MemoryLocation (sid=5) = set { Channel(sid=6)=0, Slot(sid=7)=0 } */
  write_memory_location(tmp);

  fflush(tmp);
  fseek(tmp, 0, SEEK_END);
  long tlen = ftell(tmp);
  fseek(tmp, 0, SEEK_SET);
  if (tlen < 0 || tlen > (long)sizeof(top)) { fprintf(stderr,"top too big\n"); fclose(tmp); fclose(f); return 1; }
  if (fread(top, 1, (size_t)tlen, tmp) != (size_t)tlen) { fprintf(stderr,"read tmp failed\n"); fclose(tmp); fclose(f); return 1; }
  fclose(tmp);

  /* Wrap the top payload as a Set with a synthetic SID=0 (not used by decoder for object key) */
  /* Our decoder expects the full buffer to be a SET payload (it immediately calls decode_set on the buffer),
     so to be compatible we can just write the inner members directly (no outer element). */
  /* However, to keep symmetry, let's write an outer set element and then decode_set will work if the file contains only members.
     The current decoder reads the whole buffer as a set payload (no header), so we need to write only the members.
     => Write only the concatenated members (no outer [tag sid len]). */

  /* So just dump the members: */
  if (fwrite(top, 1, (size_t)tlen, f) != (size_t)tlen) { fprintf(stderr,"write fail\n"); fclose(f); return 1; }
  fclose(f);
  fprintf(stderr, "Wrote %ld bytes to %s\n", tlen, out);
  return 0;
}
