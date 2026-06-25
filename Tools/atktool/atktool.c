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
#ifdef USE_HIDAPI
#include <unistd.h> /* usleep for inter-command flash-timing delays */
#endif

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

/* T90 update-mode bootloader enumerates as "N32L40xCustm HID", serial ATK-T90,
 * manufacturer ALIENTEK, at VID 0x19F5 / PID 0x3245 (confirmed from the real device).
 * The older 0x413D/0x2107 was the T80P-era guess and is wrong for the T90. */
#define ATK_VID 0x19F5
#define ATK_PID 0x3245
/* host->device target address. the T90 bootloader only processes commands sent to 0xAF
 * (reverse-engineered: a sweep of all 256 addresses showed only 0xAF returns real replies;
 * other addresses get a canned "DF fn seq 01 05" reject). the T80 used 0xFB. */
#define DEV_ADDR 0xAF
#define SRC_ADDR 0xDF     /* SOURCE (device -> host); confirmed from live T90 replies */
#define RESULT_OK 0x00    /* device result code for success (T90); nonzero = error code  */
#define FN_GET_INFO 0x10    /* GetDevInfo    -> 21-byte DevInfo                       */
#define FN_SET_FW_INFO 0x11  /* SetFwInfo     -> 13-byte FirmInfo (= the .atk header)  */
#define FN_START 0x12        /* SetStartSendFW                                         */
#define FN_DATA 0x13         /* SetSendData   -> one chunk of FirmInfo.dataLenght bytes*/
#define FN_END 0x14          /* SetSendDataEnd                                         */
#define FN_SET_BOOT 0x15     /* SetBoot -> COMMIT: FINALIZE freshly-flashed image (set app-valid
                              * flag) + reboot into it. replies DF 15 .. then resets. THIS is the
                              * correct post-flash commit (matches the T80 reimpl's boot path). */
#define FN_START_APP 0x16    /* SetStartAPP -> jump-only: boots an ALREADY-valid resident app
                              * WITHOUT finalizing (no reply). a fresh flash committed with 0x16
                              * never boots -> use 0x15. kept for the reboot-resident-app primitive. */
#define CHUNK 58             /* frame max data; the real chunk size comes from dataLenght */

/* the T90 bootloader uses a 1-byte additive checksum: every frame byte
 * (addr,fn,seq,len,data...,chk) sums to 0 mod 256. confirmed from live replies:
 *   GET_INFO    -> DF 10 00 01 05 0B   (DF+10+00+01+05+0B = 0x100)
 *   SET_FW_INFO -> DF 11 00 01 05 0A
 *   START       -> DF 12 00 01 05 09
 * (the T80P used CRC-16/MODBUS; that does NOT apply to the T90.) */
static uint8_t checksum8(const uint8_t *d, size_t n) {
  uint8_t s = 0;
  for (size_t i = 0; i < n; i++) s = (uint8_t)(s + d[i]);
  return (uint8_t)(0 - s); /* two's complement: frame incl this byte wraps to 0 */
}

/* check a received frame: header+data+chk sum to 0 mod 256 */
static int frame_ok(const uint8_t *p, int n) {
  if (n < 6) return 0;
  uint8_t dlen = p[3];
  if (4 + dlen + 1 > n) return 0;
  uint8_t s = 0;
  for (int i = 0; i < 4 + dlen + 1; i++) s = (uint8_t)(s + p[i]);
  return s == 0;
}

static uint8_t g_dev_addr = DEV_ADDR; /* host->device target address, overridable via --addr */

/* build a 64-byte frame, prefixed with HID report id 0 -> 65 bytes for hid_write:
 * [0x00][addr][fn][seq][len][data...][chk][pad..] */
static void build_packet(uint8_t out[65], uint8_t fn, uint8_t seq, const uint8_t *data, uint8_t dlen) {
  memset(out, 0, 65);
  out[0] = 0x00; /* hid report id */
  uint8_t *p = out + 1;
  p[0] = g_dev_addr;
  p[1] = fn;
  p[2] = seq;
  p[3] = dlen;
  if (dlen && data) memcpy(p + 4, data, dlen);
  p[4 + dlen] = checksum8(p, (size_t)(4 + dlen));
}

/* send one frame; if expect_reply, read the reply into *reply (cap bytes) and return its length.
 * returns -1 on a write error or (when a reply is expected) no reply. 0 when no reply expected. */
static int xfer(hid_device *h, uint8_t fn, uint8_t seq, const uint8_t *data, uint8_t dlen,
                int expect_reply, uint8_t *reply, int cap) {
  uint8_t pkt[65];
  build_packet(pkt, fn, seq, data, dlen);
  if (hid_write(h, pkt, 65) < 0) {
    fprintf(stderr, "error: hid_write failed: %ls\n", hid_error(h));
    return -1;
  }
  if (!expect_reply) return 0;
  uint8_t rsp[64];
  int n = hid_read_timeout(h, rsp, sizeof(rsp), 1000);
  if (n <= 0) {
    fprintf(stderr, "error: no reply to fn 0x%02X\n", fn);
    return -1;
  }
  if (reply && cap > 0) memcpy(reply, rsp, n < cap ? (size_t)n : (size_t)cap);
  return n;
}

static hid_device *open_dev(unsigned short vid, unsigned short pid) {
  if (hid_init()) return NULL;
  hid_device *h = hid_open(vid, pid, NULL);
  if (!h) fprintf(stderr, "error: iron not found in update mode (VID %04X PID %04X).\n"
                          "       enter update mode: hold the B button, plug USB-C, power on (~4s -> 'Upgrade...').\n",
                  vid, pid);
  return h;
}

static void hexdump(const char *label, const uint8_t *d, int n) {
  printf("%s (%d bytes):", label, n);
  for (int i = 0; i < n; i++) {
    if (i % 16 == 0) printf("\n  ");
    printf("%02X ", d[i]);
  }
  printf("\n");
}

/* probe: open the device, dump its identity + HID report descriptor, then do a READ-ONLY
 * GET_INFO handshake and validate the reply's CRC. This confirms the Alientek bootloader
 * application-layer protocol is really speaking on this HID transport BEFORE any flash write. */
static int cmd_probe(int argc, char **argv) {
  unsigned short vid = ATK_VID, pid = ATK_PID;
  int badcrc = 0;
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--vid") && i + 1 < argc) vid = (unsigned short)strtol(argv[++i], NULL, 0);
    if (!strcmp(argv[i], "--pid") && i + 1 < argc) pid = (unsigned short)strtol(argv[++i], NULL, 0);
    if (!strcmp(argv[i], "--addr") && i + 1 < argc) g_dev_addr = (uint8_t)strtol(argv[++i], NULL, 0);
    if (!strcmp(argv[i], "--badcrc")) badcrc = 1;
  }
  hid_device *h = open_dev(vid, pid);
  if (!h) return 1;
  printf("found device at VID %04X PID %04X%s\n", vid, pid, badcrc ? "  [--badcrc: sending corrupted CRC]" : "");

  wchar_t s[256];
  if (hid_get_manufacturer_string(h, s, 256) == 0) printf("  manufacturer: %ls\n", s);
  if (hid_get_product_string(h, s, 256) == 0)      printf("  product:      %ls\n", s);
  if (hid_get_serial_number_string(h, s, 256) == 0) printf("  serial:       %ls\n", s);

  unsigned char rd[512];
  int rn = hid_get_report_descriptor(h, rd, sizeof(rd));
  if (rn > 0) hexdump("  report descriptor", rd, rn);
  else printf("  report descriptor: unavailable (%d)\n", rn);

  /* GET_INFO is a query, not a write - safe to send for verification */
  uint8_t pkt[65];
  build_packet(pkt, FN_GET_INFO, 0, NULL, 0);
  if (badcrc) pkt[1 + 4] ^= 0xFF; /* corrupt the CRC low byte to test bootloader strictness */
  printf("  -> GET_INFO  "); hexdump("frame", pkt + 1, 8);
  if (hid_write(h, pkt, 65) < 0) {
    printf("  hid_write failed: %ls\n", hid_error(h));
    hid_close(h); hid_exit();
    return 1;
  }
  uint8_t rsp[64];
  int n = hid_read_timeout(h, rsp, sizeof(rsp), 1500);
  if (n <= 0) {
    printf("  <- NO REPLY (n=%d). transport/protocol does NOT match - do NOT flash.\n", n);
    hid_close(h); hid_exit();
    return 2;
  }
  hexdump("  <- reply", rsp, n);
  /* response frame: [addr=DF][fn][seq][len][data...][chk]; additive checksum sums to 0 mod 256 */
  if (n >= 6) {
    uint8_t dlen = rsp[3];
    if (4 + dlen + 1 <= n) {
      int ok = frame_ok(rsp, n);
      printf("  parsed: addr=%02X fn=%02X seq=%02X len=%u data0=%02X chk=%02X  %s\n",
             rsp[0], rsp[1], rsp[2], dlen, dlen ? rsp[4] : 0, rsp[4 + dlen],
             ok ? "(checksum OK)" : "(checksum BAD)");
      if (ok && rsp[0] == SRC_ADDR && rsp[1] == FN_GET_INFO) {
        printf("  PROTOCOL CONFIRMED: valid GET_INFO reply (additive checksum). flash is safe to attempt.\n");
        hid_close(h); hid_exit();
        return 0;
      }
    }
  }
  printf("  reply did not validate as a GET_INFO response - inspect the dump above.\n");
  hid_close(h); hid_exit();
  return 3;
}

/* print a one-line summary of a reply frame: first 8 bytes + parsed addr/fn/seq + checksum */
static void show_reply(const char *tag, const uint8_t *r, int n) {
  printf("    <- %-12s n=%2d  [", tag, n);
  for (int i = 0; i < n && i < 8; i++) printf("%02X ", r[i]);
  printf("]  addr=%02X fn=%02X seq=%02X data0=%02X  %s",
         n > 0 ? r[0] : 0, n > 1 ? r[1] : 0, n > 2 ? r[2] : 0, n > 4 ? r[4] : 0,
         frame_ok(r, n) ? "chk-OK" : "chk-BAD");
  printf("\n");
}

/* sweep: send a read-only GET_INFO to every possible target address 0x00..0xFF and report any
 * response whose data differs from the majority - this finds the address the bootloader accepts. */
static int cmd_sweep(int argc, char **argv) {
  unsigned short vid = ATK_VID, pid = ATK_PID;
  uint8_t fn = FN_GET_INFO;
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--vid") && i + 1 < argc) vid = (unsigned short)strtol(argv[++i], NULL, 0);
    if (!strcmp(argv[i], "--pid") && i + 1 < argc) pid = (unsigned short)strtol(argv[++i], NULL, 0);
    if (!strcmp(argv[i], "--fn") && i + 1 < argc) fn = (uint8_t)strtol(argv[++i], NULL, 0);
  }
  hid_device *h = open_dev(vid, pid);
  if (!h) return 1;
  printf("sweeping target address 0x00..0xFF with fn 0x%02X (read-only)\n", fn);
  int shown = 0;
  for (int a = 0; a <= 0xFF; a++) {
    g_dev_addr = (uint8_t)a;
    uint8_t pkt[65];
    build_packet(pkt, fn, 0, NULL, 0);
    if (hid_write(h, pkt, 65) < 0) continue;
    uint8_t rsp[64];
    int n = hid_read_timeout(h, rsp, sizeof(rsp), 300);
    if (n <= 0) continue;
    /* the uniform/baseline reply is addr=DF len=1 data=05; print anything that is NOT that */
    int baseline = (n >= 6 && rsp[0] == SRC_ADDR && rsp[3] == 0x01 && rsp[4] == 0x05);
    if (!baseline) {
      printf("  addr 0x%02X -> [", a);
      for (int i = 0; i < n && i < 10; i++) printf("%02X ", rsp[i]);
      printf("]  %s\n", frame_ok(rsp, n) ? "chk-OK" : "chk?");
      shown++;
    }
  }
  if (!shown) printf("  all addresses returned the same baseline reply (addr=DF len=1 data=05).\n");
  hid_close(h); hid_exit();
  return 0;
}

/* parse a hex string like "08020A01" or "08 02 0a 01" into bytes; returns count */
static int parse_hex(const char *s, uint8_t *out, int cap) {
  int n = 0;
  while (*s && n < cap) {
    while (*s == ' ' || *s == ',') s++;
    if (!*s) break;
    char b[3] = {0, 0, 0};
    b[0] = *s++;
    if (*s && *s != ' ' && *s != ',') b[1] = *s++;
    out[n++] = (uint8_t)strtol(b, NULL, 16);
  }
  return n;
}

/* fnsweep: send a single frame for each function code in [--lo,--hi] to addr 0xAF (default)
 * with an optional --data payload, and print every reply. maps which command codes the live
 * bootloader actually answers. zero payload by default (least likely to trigger a flash write).
 * USE WITH CARE: unknown codes could mutate device state; only run when already non-booting. */
static int cmd_fnsweep(int argc, char **argv) {
  unsigned short vid = ATK_VID, pid = ATK_PID;
  int lo = 0x10, hi = 0x2F;
  uint8_t seq = 0;
  uint8_t data[60];
  int dlen = 0;
  for (int i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--vid") && i + 1 < argc) vid = (unsigned short)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--pid") && i + 1 < argc) pid = (unsigned short)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--addr") && i + 1 < argc) g_dev_addr = (uint8_t)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--lo") && i + 1 < argc) lo = (int)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--hi") && i + 1 < argc) hi = (int)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--seq") && i + 1 < argc) seq = (uint8_t)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--data") && i + 1 < argc) dlen = parse_hex(argv[++i], data, (int)sizeof(data));
  }
  hid_device *h = open_dev(vid, pid);
  if (!h) return 1;
  printf("fnsweep addr=0x%02X fn=0x%02X..0x%02X seq=0x%02X dlen=%d (reply within 350ms)\n",
         g_dev_addr, lo, hi, seq, dlen);
  for (int fn = lo; fn <= hi; fn++) {
    uint8_t pkt[65];
    build_packet(pkt, (uint8_t)fn, seq, dlen ? data : NULL, (uint8_t)dlen);
    if (hid_write(h, pkt, 65) < 0) { printf("  fn 0x%02X -> write error\n", fn); continue; }
    uint8_t rsp[64];
    int n = hid_read_timeout(h, rsp, sizeof(rsp), 350);
    if (n <= 0) { printf("  fn 0x%02X -> (no reply)\n", fn); continue; }
    printf("  fn 0x%02X -> [", fn);
    for (int i = 0; i < n && i < 12; i++) printf("%02X ", rsp[i]);
    printf("]  addr=%02X fn=%02X len=%u data0=%02X  %s\n",
           rsp[0], n > 1 ? rsp[1] : 0, n > 3 ? rsp[3] : 0, n > 4 ? rsp[4] : 0,
           frame_ok(rsp, n) ? "chk-OK" : "chk?");
  }
  hid_close(h); hid_exit();
  return 0;
}

static int cmd_flash(int argc, char **argv) {
  if (argc < 1) {
    fprintf(stderr, "usage: atktool flash <in.atk> [--vid V] [--pid P] [--addr A] [--force] [--dry-run] [--quiet]\n");
    return 2;
  }
  unsigned short vid = ATK_VID, pid = ATK_PID;
  int force = 0, dry = 0, verbose = 1, dexor = 0, no_commit = 0;
  /* commit = SetBoot (0x15): it FINALIZES the freshly-flashed image (sets the app-valid
   * flag) and reboots. SetStartAPP (0x16) only JUMPS to an already-valid resident app
   * WITHOUT finalizing, so a fresh flash committed with 0x16 never boots. Confirmed live. */
  int commit_code = FN_SET_BOOT;
  int start_delay = 0, chunk_delay = 0, end_delay = 0; /* flash-timing delays in ms */
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "--force")) force = 1;
    else if (!strcmp(argv[i], "--dry-run")) dry = 1;
    else if (!strcmp(argv[i], "--quiet")) verbose = 0;
    else if (!strcmp(argv[i], "--dexor")) dexor = 1; /* send payload de-obfuscated (XOR 0xFF) */
    else if (!strcmp(argv[i], "--no-commit")) no_commit = 1; /* skip the post-END commit command */
    else if (!strcmp(argv[i], "--commit") && i + 1 < argc) commit_code = (int)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--start-delay") && i + 1 < argc) start_delay = (int)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--chunk-delay") && i + 1 < argc) chunk_delay = (int)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--end-delay") && i + 1 < argc) end_delay = (int)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--vid") && i + 1 < argc) vid = (unsigned short)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--pid") && i + 1 < argc) pid = (unsigned short)strtol(argv[++i], NULL, 0);
    else if (!strcmp(argv[i], "--addr") && i + 1 < argc) g_dev_addr = (uint8_t)strtol(argv[++i], NULL, 0);
  }
  if (!force) {
    fprintf(stderr,
            "REFUSING: flash is EXPERIMENTAL for the T90 (update-mode protocol partially confirmed:\n"
            "outbound framing accepted by the bootloader, but request CRC is NOT validated and the\n"
            "write/ACK semantics are inferred). Keep the factory t90.atk for recovery, then --force.\n");
    return 1;
  }
  long len = 0;
  uint8_t *atk = read_file(argv[0], &len);
  if (!atk) return 1;
  if (atk_check_magic(atk, len)) { free(atk); return 1; }

  hid_device *h = open_dev(vid, pid);
  if (!h) { free(atk); return 1; }

  /* T90 HID bootloader update sequence (reverse-engineered from the official ATK_T90_UPGRAGE.exe
   * struct/method metadata + live device probing):
   *   GetDevInfo(0x10) -> SetFwInfo(0x11, the 13-byte FirmInfo = .atk header)
   *   -> SetStartSendFW(0x12) -> SetSendData(0x13) x N -> SetSendDataEnd(0x14)
   *   -> SetStartAPP(0x15)  [commit + jump to the new app; device reboots]
   * frame target address 0xAF; 1-byte additive checksum; result byte 0x00 = OK.
   * the chunk size is FirmInfo.dataLenght (.atk header byte 5); the bootloader places each chunk
   * at offset seq*dataLenght, so the chunk size MUST match it. payload is streamed XOR'd as stored
   * in the .atk; the bootloader de-obfuscates using FirmInfo.fileType. */
  uint8_t r[64];
  int n;

  n = xfer(h, FN_GET_INFO, 0, NULL, 0, 1, r, sizeof(r));
  if (n < 0) goto fail;
  if (verbose) show_reply("GET_INFO", r, n);

  n = xfer(h, FN_SET_FW_INFO, 0, atk, ATK_HEADER_SIZE, 1, r, sizeof(r));
  if (n < 0) goto fail;
  if (verbose) show_reply("SET_FW_INFO", r, n);
  if (n < 6 || r[4] != RESULT_OK) { fprintf(stderr, "SET_FW_INFO rejected (result=0x%02X); dev_type/header mismatch?\n", n >= 5 ? r[4] : 0xFF); goto fail; }

  n = xfer(h, FN_START, 0, NULL, 0, 1, r, sizeof(r));
  if (n < 0) goto fail;
  if (verbose) show_reply("START", r, n);
  if (n < 6 || r[4] != RESULT_OK) { fprintf(stderr, "START rejected (result=0x%02X)\n", n >= 5 ? r[4] : 0xFF); goto fail; }
  if (start_delay) { if (verbose) printf("  waiting %d ms after START (flash erase)...\n", start_delay); usleep((useconds_t)start_delay * 1000); }

  if (dry) {
    printf("  --dry-run: stopped after START (no payload written). observe the replies above.\n");
    hid_close(h); hid_exit(); free(atk);
    return 0;
  }

  /* chunk size = FirmInfo.dataLenght (.atk header byte 5) */
  uint8_t chunk = atk[5];
  if (chunk == 0 || chunk > CHUNK) chunk = CHUNK;
  if (verbose) printf("  chunk size = %u bytes (FirmInfo.dataLenght)\n", chunk);
  long off = ATK_HEADER_SIZE;
  uint8_t seq = 0;
  long total = len - ATK_HEADER_SIZE;
  int chunk_idx = 0;
  long last_off = len - chunk;
  if (verbose) printf("  payload: %s\n", dexor ? "DE-XOR'd (raw app bytes)" : "as-stored (XOR 0xFF)");
  uint8_t cbuf[64];
  while (off < len) {
    uint8_t dlen = (uint8_t)((len - off) >= chunk ? chunk : (len - off));
    const uint8_t *src = atk + off;
    if (dexor) { for (int i = 0; i < dlen; i++) cbuf[i] = (uint8_t)(atk[off + i] ^ 0xFF); src = cbuf; }
    int tries = 0;
    do {
      n = xfer(h, FN_DATA, seq, src, dlen, 1, r, sizeof(r));
      if (n < 0) { fprintf(stderr, "\nflash FAILED at chunk %d (offset %ld): no reply\n", chunk_idx, off - ATK_HEADER_SIZE); goto fail; }
    } while ((n < 6 || r[4] != RESULT_OK) && ++tries < 3);
    if (n < 6 || r[4] != RESULT_OK) { fprintf(stderr, "\nflash FAILED: chunk %d rejected (result=0x%02X)\n", chunk_idx, n >= 5 ? r[4] : 0xFF); goto fail; }
    if (verbose && (chunk_idx < 3 || off >= last_off)) { printf("\n    [chunk %d, %ld B] ", chunk_idx, off - ATK_HEADER_SIZE); show_reply("DATA", r, n); }
    off += dlen;
    seq++;
    chunk_idx++;
    if (chunk_delay) usleep((useconds_t)chunk_delay * 1000);
    if ((off % 8192) < chunk) { printf("\r  flashing %ld/%ld bytes...", off - ATK_HEADER_SIZE, total); fflush(stdout); }
  }

  /* END: finish the data stream */
  n = xfer(h, FN_END, 0, NULL, 0, 1, r, sizeof(r));
  if (n > 0 && verbose) show_reply("END", r, n);
  if (n >= 6 && r[4] != RESULT_OK) fprintf(stderr, "\nwarning: END result=0x%02X (continuing to START_APP)\n", r[4]);
  if (end_delay) { if (verbose) printf("  waiting %d ms after END (flash finalize)...\n", end_delay); usleep((useconds_t)end_delay * 1000); }

  /* commit: send the commit/jump command (default SET_START_APP) - the device should reboot */
  if (!no_commit) {
    printf("\n  committing (fn 0x%02X) - the iron should reboot into the new firmware...\n", commit_code);
    n = xfer(h, (uint8_t)commit_code, 0, NULL, 0, 1, r, sizeof(r));
    if (n > 0 && verbose) show_reply("COMMIT", r, n);
    else if (n <= 0) printf("    <- COMMIT: no reply (device reset to boot the new app - expected)\n");
  } else {
    printf("\n  --no-commit: data+END sent, commit skipped.\n");
  }

  printf("\n  flashed %ld bytes in %d chunks.\n", total, chunk_idx);
  printf("flash complete. the iron should now boot the new firmware; if not, re-flash the factory t90.atk.\n");
  hid_close(h); hid_exit(); free(atk);
  return 0;

fail:
  hid_close(h); hid_exit(); free(atk);
  return 1;
}
#else
static int cmd_probe(int argc, char **argv) {
  (void)argc; (void)argv;
  fprintf(stderr, "probe/flash need hidapi: install it (brew install hidapi / apt install libhidapi-dev) and rebuild.\n");
  return 1;
}
static int cmd_flash(int argc, char **argv) { return cmd_probe(argc, argv); }
static int cmd_sweep(int argc, char **argv) { return cmd_probe(argc, argv); }
static int cmd_fnsweep(int argc, char **argv) { return cmd_probe(argc, argv); }
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
  if (!strcmp(cmd, "sweep")) return cmd_sweep(argc - 2, argv + 2);
  if (!strcmp(cmd, "fnsweep")) return cmd_fnsweep(argc - 2, argv + 2);
  if (!strcmp(cmd, "flash")) return cmd_flash(argc - 2, argv + 2);
  return usage();
}
