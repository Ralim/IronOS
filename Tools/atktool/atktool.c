/*
 * atktool - compact CLI toolkit for the Alientek T90 (Nations N32L40x) soldering iron.
 *
 * Image format (.atk), reverse-engineered + byte-exact round-trip verified against the official
 * factory t90.atk and PanKleszcz's t90_msdmem.atk:
 *
 *   header (13 bytes):
 *     [0:6]  magic = 08 02 0A 01 A0 20
 *     [6:10] payload size  (uint32 little-endian = length of the raw application image)
 *     [10]   year - 2000   (build date, metadata only)
 *     [11]   month
 *     [12]   day
 *   payload: the raw application (linked at 0x08005000) with every byte XOR 0xFF.
 *
 * Commands:
 *   pack   <in.bin> <out.atk> [--date YYYY-MM-DD]   wrap a raw app image into an .atk
 *   unpack <in.atk> <out.bin>                        extract the raw app image from an .atk
 *   info   <file.atk>                                print the header fields
 *   probe                                            (USB) find the iron in update mode
 *   flash  <in.atk> [--vid V] [--pid P] [--force]    (USB) flash an .atk  [EXPERIMENTAL]
 *
 * pack/unpack/info need no libraries. probe/flash need hidapi (build is gated on USE_HIDAPI).
 *
 * SAFETY: the USB flash path replicates the Alientek HID-bootloader protocol reverse-engineered
 * from the T80P (dozed-dev/atk-fw-util). The T90 update-mode VID/PID and exact chunking are not
 * 100%% confirmed publicly, so `flash` is EXPERIMENTAL and refuses to run without --force. ALWAYS
 * keep the factory t90.atk for recovery and verify with a USB capture before trusting it.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t ATK_MAGIC[6] = {0x08, 0x02, 0x0A, 0x01, 0xA0, 0x20};
#define ATK_HEADER_SIZE 13

/* ----- file helpers ----- */
static uint8_t *read_file(const char *path, long *out_len) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    fprintf(stderr, "error: cannot open %s\n", path);
    return NULL;
  }
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);
  uint8_t *buf = (uint8_t *)malloc(len > 0 ? (size_t)len : 1);
  if (!buf || (len > 0 && fread(buf, 1, (size_t)len, f) != (size_t)len)) {
    fprintf(stderr, "error: cannot read %s\n", path);
    free(buf);
    fclose(f);
    return NULL;
  }
  fclose(f);
  *out_len = len;
  return buf;
}

static int write_file(const char *path, const uint8_t *buf, size_t len) {
  FILE *f = fopen(path, "wb");
  if (!f) {
    fprintf(stderr, "error: cannot create %s\n", path);
    return -1;
  }
  int ok = fwrite(buf, 1, len, f) == len;
  fclose(f);
  if (!ok) {
    fprintf(stderr, "error: cannot write %s\n", path);
    return -1;
  }
  return 0;
}

/* ----- .atk format ----- */
static void atk_build_header(uint8_t hdr[ATK_HEADER_SIZE], uint32_t payload_size, int year, int month, int day) {
  memcpy(hdr, ATK_MAGIC, 6);
  hdr[6]  = (uint8_t)(payload_size & 0xFF);
  hdr[7]  = (uint8_t)((payload_size >> 8) & 0xFF);
  hdr[8]  = (uint8_t)((payload_size >> 16) & 0xFF);
  hdr[9]  = (uint8_t)((payload_size >> 24) & 0xFF);
  hdr[10] = (uint8_t)((year - 2000) & 0xFF);
  hdr[11] = (uint8_t)(month & 0xFF);
  hdr[12] = (uint8_t)(day & 0xFF);
}

static int cmd_pack(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: atktool pack <in.bin> <out.atk> [--date YYYY-MM-DD]\n");
    return 2;
  }
  int year = 2000, month = 1, day = 1;
  for (int i = 2; i < argc; i++) {
    if (strcmp(argv[i], "--date") == 0 && i + 1 < argc) {
      sscanf(argv[++i], "%d-%d-%d", &year, &month, &day);
    }
  }
  long len = 0;
  uint8_t *bin = read_file(argv[0], &len);
  if (!bin) return 1;
  uint8_t *out = (uint8_t *)malloc(ATK_HEADER_SIZE + (size_t)len);
  atk_build_header(out, (uint32_t)len, year, month, day);
  for (long i = 0; i < len; i++) out[ATK_HEADER_SIZE + i] = (uint8_t)(bin[i] ^ 0xFF);
  int rc = write_file(argv[1], out, ATK_HEADER_SIZE + (size_t)len);
  if (rc == 0) printf("packed %ld bytes -> %s (13-byte header + XOR 0xFF payload)\n", len, argv[1]);
  free(bin);
  free(out);
  return rc ? 1 : 0;
}

static int atk_check_magic(const uint8_t *atk, long len) {
  if (len < ATK_HEADER_SIZE || memcmp(atk, ATK_MAGIC, 6) != 0) {
    fprintf(stderr, "error: not a T90 .atk (bad magic / too short)\n");
    return -1;
  }
  return 0;
}

static int cmd_unpack(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: atktool unpack <in.atk> <out.bin>\n");
    return 2;
  }
  long len = 0;
  uint8_t *atk = read_file(argv[0], &len);
  if (!atk) return 1;
  if (atk_check_magic(atk, len)) { free(atk); return 1; }
  uint32_t size = (uint32_t)atk[6] | ((uint32_t)atk[7] << 8) | ((uint32_t)atk[8] << 16) | ((uint32_t)atk[9] << 24);
  long payload = len - ATK_HEADER_SIZE;
  if ((long)size != payload) fprintf(stderr, "warning: header size %u != payload %ld\n", size, payload);
  uint8_t *bin = (uint8_t *)malloc(payload > 0 ? (size_t)payload : 1);
  for (long i = 0; i < payload; i++) bin[i] = (uint8_t)(atk[ATK_HEADER_SIZE + i] ^ 0xFF);
  int rc = write_file(argv[1], bin, (size_t)payload);
  if (rc == 0) printf("unpacked %ld bytes -> %s\n", payload, argv[1]);
  free(atk);
  free(bin);
  return rc ? 1 : 0;
}

static int cmd_info(int argc, char **argv) {
  if (argc < 1) {
    fprintf(stderr, "usage: atktool info <file.atk>\n");
    return 2;
  }
  long len = 0;
  uint8_t *atk = read_file(argv[0], &len);
  if (!atk) return 1;
  if (atk_check_magic(atk, len)) { free(atk); return 1; }
  uint32_t size = (uint32_t)atk[6] | ((uint32_t)atk[7] << 8) | ((uint32_t)atk[8] << 16) | ((uint32_t)atk[9] << 24);
  long payload = len - ATK_HEADER_SIZE;
  /* decode the first 8 payload bytes (XOR 0xFF) -> Cortex-M vector table */
  uint32_t sp = 0, reset = 0;
  if (payload >= 8) {
    for (int i = 0; i < 4; i++) sp |= (uint32_t)(atk[ATK_HEADER_SIZE + i] ^ 0xFF) << (8 * i);
    for (int i = 0; i < 4; i++) reset |= (uint32_t)(atk[ATK_HEADER_SIZE + 4 + i] ^ 0xFF) << (8 * i);
  }
  printf("file:         %s (%ld bytes)\n", argv[0], len);
  printf("magic:        ok (08 02 0A 01 A0 20)\n");
  printf("payload size: %u  (file payload %ld, %s)\n", size, payload, ((long)size == payload) ? "match" : "MISMATCH");
  printf("build date:   %04d-%02d-%02d\n", 2000 + atk[10], atk[11], atk[12]);
  printf("vector[0] SP: 0x%08X  %s\n", sp, (sp >= 0x20000000 && sp <= 0x20008000) ? "(plausible SRAM)" : "(?)");
  printf("vector[1] PC: 0x%08X  %s\n", reset, (reset >= 0x08005000 && reset < 0x08020000) ? "(plausible app region)" : "(?)");
  free(atk);
  return 0;
}

/* ----- USB flash (HID bootloader protocol; EXPERIMENTAL for T90) ----- */
#ifdef USE_HIDAPI
#include <hidapi.h>

#define ATK_VID 0x413D
#define ATK_PID 0x2107
#define DEV_ADDR 0xFB
#define FN_GET_INFO 0x10
#define FN_SET_FW_INFO 0x11
#define FN_START 0x12
#define FN_DATA 0x13
#define FN_END 0x14
#define CHUNK 58

static uint16_t crc16_modbus(const uint8_t *d, size_t n) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < n; i++) {
    crc ^= d[i];
    for (int b = 0; b < 8; b++) crc = (crc & 1) ? (uint16_t)((crc >> 1) ^ 0xA001) : (uint16_t)(crc >> 1);
  }
  return crc;
}

/* build a 64-byte packet, prefixed with HID report id 0 -> 65 bytes for hid_write */
static void build_packet(uint8_t out[65], uint8_t fn, uint8_t seq, const uint8_t *data, uint8_t dlen) {
  memset(out, 0, 65);
  out[0] = 0x00; /* report id */
  uint8_t *p = out + 1;
  p[0] = DEV_ADDR;
  p[1] = fn;
  p[2] = seq;
  p[3] = dlen;
  if (dlen && data) memcpy(p + 4, data, dlen);
  uint16_t crc = crc16_modbus(p, (size_t)(4 + dlen));
  p[4 + dlen] = (uint8_t)(crc & 0xFF);
  p[4 + dlen + 1] = (uint8_t)(crc >> 8);
}

static int xfer(hid_device *h, uint8_t fn, uint8_t seq, const uint8_t *data, uint8_t dlen, int expect_reply) {
  uint8_t pkt[65];
  build_packet(pkt, fn, seq, data, dlen);
  if (hid_write(h, pkt, 65) < 0) {
    fprintf(stderr, "error: hid_write failed: %ls\n", hid_error(h));
    return -1;
  }
  if (expect_reply) {
    uint8_t rsp[64];
    int n = hid_read_timeout(h, rsp, sizeof(rsp), 1000);
    if (n <= 0) {
      fprintf(stderr, "error: no reply to fn 0x%02X\n", fn);
      return -1;
    }
  }
  return 0;
}

static hid_device *open_dev(unsigned short vid, unsigned short pid) {
  if (hid_init()) return NULL;
  hid_device *h = hid_open(vid, pid, NULL);
  if (!h) fprintf(stderr, "error: iron not found in update mode (VID %04X PID %04X).\n"
                          "       enter update mode: hold the B button, plug USB-C, power on (~4s -> 'Upgrade...').\n",
                  vid, pid);
  return h;
}

static int cmd_probe(int argc, char **argv) {
  unsigned short vid = ATK_VID, pid = ATK_PID;
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--vid") && i + 1 < argc) vid = (unsigned short)strtol(argv[++i], NULL, 0);
    if (!strcmp(argv[i], "--pid") && i + 1 < argc) pid = (unsigned short)strtol(argv[++i], NULL, 0);
  }
  hid_device *h = open_dev(vid, pid);
  if (!h) return 1;
  printf("found iron in update mode at VID %04X PID %04X\n", vid, pid);
  hid_close(h);
  hid_exit();
  return 0;
}

static int cmd_flash(int argc, char **argv) {
  if (argc < 1) {
    fprintf(stderr, "usage: atktool flash <in.atk> [--vid V] [--pid P] [--force]\n");
    return 2;
  }
  unsigned short vid = ATK_VID, pid = ATK_PID;
  int force = 0;
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--force")) force = 1;
    else if (!strcmp(argv[i], "--vid") && i + 1 < argc) vid = (unsigned short)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--pid") && i + 1 < argc) pid = (unsigned short)strtol(argv[++i], NULL, 0);
  }
  if (!force) {
    fprintf(stderr,
            "REFUSING: flash is EXPERIMENTAL for the T90 (update-mode VID/PID + chunking not fully\n"
            "confirmed). Keep the factory t90.atk for recovery, verify the protocol with a USB capture,\n"
            "then re-run with --force.\n");
    return 1;
  }
  long len = 0;
  uint8_t *atk = read_file(argv[0], &len);
  if (!atk) return 1;
  if (atk_check_magic(atk, len)) { free(atk); return 1; }

  hid_device *h = open_dev(vid, pid);
  if (!h) { free(atk); return 1; }

  /* protocol (from the Alientek HID bootloader, T80P reference): info -> set-fw-info(header) ->
   * start -> stream payload chunks -> end. The header sent is the 13-byte T90 header; the streamed
   * bytes are the (already XOR'd) payload exactly as stored in the .atk. */
  int rc = 0;
  rc |= xfer(h, FN_GET_INFO, 0, NULL, 0, 1);
  rc |= xfer(h, FN_SET_FW_INFO, 0, atk, ATK_HEADER_SIZE, 1);
  rc |= xfer(h, FN_START, 0, NULL, 0, 1);
  long off = ATK_HEADER_SIZE;
  uint8_t seq = 0;
  while (off < len && rc == 0) {
    uint8_t dlen = (uint8_t)((len - off) >= CHUNK ? CHUNK : (len - off));
    rc |= xfer(h, FN_DATA, seq, atk + off, dlen, 1);
    off += dlen;
    seq++;
    if ((off % 4096) < CHUNK) printf("\r  flashing %ld/%ld bytes...", off - ATK_HEADER_SIZE, len - ATK_HEADER_SIZE), fflush(stdout);
  }
  /* END: device may not reply */
  xfer(h, FN_END, 0, NULL, 0, 0);
  printf("\n%s\n", rc == 0 ? "flash complete (verify the iron booted; if not, re-flash the factory t90.atk)" : "flash FAILED");
  hid_close(h);
  hid_exit();
  free(atk);
  return rc ? 1 : 0;
}
#else
static int cmd_probe(int argc, char **argv) {
  (void)argc; (void)argv;
  fprintf(stderr, "probe/flash need hidapi: install it (brew install hidapi / apt install libhidapi-dev) and rebuild.\n");
  return 1;
}
static int cmd_flash(int argc, char **argv) { return cmd_probe(argc, argv); }
#endif

static int usage(void) {
  fprintf(stderr,
          "atktool - Alientek T90 firmware image toolkit\n\n"
          "  pack    <in.bin> <out.atk> [--date YYYY-MM-DD]   wrap raw app -> .atk\n"
          "  unpack  <in.atk> <out.bin>                        .atk -> raw app\n"
          "  info    <file.atk>                                show header + vector table\n"
          "  probe   [--vid V] [--pid P]                       find the iron in update mode (hidapi)\n"
          "  flash   <in.atk> [--vid V] [--pid P] --force      flash over USB (hidapi, EXPERIMENTAL)\n");
  return 2;
}

int main(int argc, char **argv) {
  if (argc < 2) return usage();
  const char *cmd = argv[1];
  if (!strcmp(cmd, "pack")) return cmd_pack(argc - 2, argv + 2);
  if (!strcmp(cmd, "unpack") || !strcmp(cmd, "decrypt")) return cmd_unpack(argc - 2, argv + 2);
  if (!strcmp(cmd, "info")) return cmd_info(argc - 2, argv + 2);
  if (!strcmp(cmd, "probe")) return cmd_probe(argc - 2, argv + 2);
  if (!strcmp(cmd, "flash")) return cmd_flash(argc - 2, argv + 2);
  return usage();
}
