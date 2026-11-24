/* Headless LG physics smoke — DP-30 contact_flags binder + shell pivots. */
#include "../src/physics.h"

#include <stdio.h>
#include <string.h>

static int fails;

#define CHECK(cond, msg)                                                       \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "FAIL: %s\n", msg);                                      \
      fails++;                                                                 \
    }                                                                          \
  } while (0)

static void put_be16(uint8_t *p, uint16_t v) {
  p[0] = (uint8_t)(v >> 8);
  p[1] = (uint8_t)(v & 0xff);
}

/* One synthetic seg: midpoint (0x2000,0x3000), radius 16. */
static void make_seg(uint8_t out[12], uint16_t x0, uint16_t y0, uint16_t x1,
                     uint16_t y1, uint16_t w4, uint16_t w5) {
  put_be16(out + 0, x0);
  put_be16(out + 2, y0);
  put_be16(out + 4, x1);
  put_be16(out + 6, y1);
  put_be16(out + 8, w4);
  put_be16(out + 10, w5);
}

static void test_dp30_binder(void) {
  /* Opcode 0x0100, lo=1 → base 0 + 0x0100 */
  CHECK(phys_mask_contact_flags_from_cmd(0x0101) == 0x0100,
        "cmd 0x0101 → flags 0x0100");
  CHECK(phys_mask_bounce_type(0x0100) == 2, "0x0100 → bounce type soft 2");

  /* lo=2 → base 0x1000 + 0x0100 = 0x1100 → (>>8)&15 = 1 → coerce soft 2 */
  CHECK(phys_mask_contact_flags_from_cmd(0x0102) == 0x1100,
        "cmd 0x0102 → 0x1100");
  CHECK(phys_mask_bounce_type(0x1100) == 2, "0x1100 type1 → soft 2");

  /* lo=3 → 0x3100 → type-1 nibble → soft 2 (bases are non-type bits) */
  CHECK(phys_mask_contact_flags_from_cmd(0x0103) == 0x3100,
        "cmd 0x0103 → 0x3100");
  CHECK(phys_mask_bounce_type(0x3100) == 2, "0x3100 type1 → soft 2");

  /* lo=4 → 0x2100 → type 1 → soft 2 */
  CHECK(phys_mask_contact_flags_from_cmd(0x0104) == 0x2100,
        "cmd 0x0104 → 0x2100");
  CHECK(phys_mask_bounce_type(0x2100) == 2, "0x2100 type1 → soft 2");

  /* bit15 set → +0x8100 */
  CHECK(phys_mask_contact_flags_from_cmd(0x8102) == 0x9100,
        "cmd 0x8102 → 0x9100");

  /* Opcode 0x0700: 0x6000|cmd → type ((>>8)&15)=7 */
  CHECK(phys_mask_contact_flags_from_cmd(0x0705) == 0x6705,
        "cmd 0x0705 → 0x6705");
  CHECK(phys_mask_bounce_type(0x6705) == 7, "0x6705 → type 7");

  /* Default / unknown */
  CHECK(phys_mask_contact_flags_from_cmd(0x0000) == PHYS_MASK_CONTACT_FLAGS_DEFAULT,
        "unknown → default 0x0100");

  /* Switch membership passthrough */
  CHECK(phys_mask_bounce_type(0x0800) == 8, "type 8 passthrough");
  CHECK(phys_mask_bounce_type(0x0A00) == 10, "type 10 passthrough");
  CHECK(phys_mask_bounce_type(0x0400) == 2, "type 4 coerce soft 2");
}

static void test_mask_load_default_flags(void) {
  PhysWorld w;
  phys_world_init_shell(&w);
  uint8_t blob[24];
  /* Seg0: mid ~0x2000/0x3000, word4 deliberately 0x2200 (would be type2 if
   * bounce read word4 — binder must ignore it). */
  make_seg(blob, 0x1F00, 0x2F00, 0x2100, 0x3100, 0x2200, 0x0010);
  make_seg(blob + 12, 0, 0, 0, 0, 0, 0); /* zero skipped */
  int n = phys_mask_load_be(&w, blob, 24);
  CHECK(n == 1, "load 1 nonzero seg");
  CHECK(w.segs[0].contact_flags == 0x0100,
        "DP-30 default contact_flags 0x0100 (not word4)");
  CHECK(w.segs[0].w4 == 0x2200, "word4 retained as residual");
  CHECK(phys_mask_bounce_type(w.segs[0].contact_flags) == 2,
        "default flags → soft 2");

  /* Probe hit at midpoint */
  int seg = -1;
  CHECK(phys_mask_probe(&w, 0x2000, 0x3000, &seg) == 1, "probe hit mid");
  CHECK(seg == 0, "probe seg0");
  CHECK(phys_mask_probe(&w, 0x0100, 0x0100, &seg) == 0, "probe miss far");

  /* Collide must use contact_flags, not word4 */
  w.ball.x = 0x2000;
  w.ball.y = 0x3000;
  w.ball.vel_x = 0;
  w.ball.vel_y = -0x400;
  w.ball.basis_c = 16384;
  w.ball.basis_s = 0;
  phys_live_collide_step(&w);
  CHECK(w.ball.hit == 1, "collide hit");
  CHECK(w.ball.contact_flags == 0x0100, "collide used binder flags");
}

static void test_poly_consume(void) {
  /* Minimal poly: one contour of 4 verts, FFFF, meta[6]=2, 2 palette entries */
  uint8_t buf[128];
  size_t o = 0;
  put_be16(buf + o, 0);
  o += 2; /* pad */
  put_be16(buf + o, 0x0101);
  o += 2;
  put_be16(buf + o, 4);
  o += 2;
  for (int i = 0; i < 4; i++) {
    put_be16(buf + o, (uint16_t)(i * 10));
    o += 2;
    put_be16(buf + o, (uint16_t)(i * 20));
    o += 2;
    put_be16(buf + o, 0);
    o += 2;
  }
  put_be16(buf + o, 0xFFFF);
  o += 2;
  for (int i = 0; i < 8; i++) {
    put_be16(buf + o, (i == 6) ? 2 : (i == 5 ? 640 : 0));
    o += 2;
  }
  put_be16(buf + o, 0x1234);
  o += 2;
  put_be16(buf + o, 0x5678);
  o += 2;

  PhysMaskPoly p;
  int bytes = phys_mask_poly_parse(&p, buf, o);
  CHECK(bytes > 0, "poly parse bytes");
  CHECK(p.ncontours == 1, "poly 1 contour");
  CHECK(p.nverts == 4, "poly 4 verts");
  CHECK(p.npalette == 2, "poly palette 2");
}

static void test_flipper_shell(void) {
  PhysWorld w;
  phys_world_init_shell(&w);
  phys_flip_pad_tick(&w, 1, 1);
  CHECK(w.flip_lx == PHYS_FLIP_L_X && w.flip_rx == PHYS_FLIP_R_X,
        "flipper X shell 0x1400/0x2c00");
  CHECK(w.flip_ly == PHYS_FLIP_Y && w.flip_ry == PHYS_FLIP_Y,
        "flipper Y shell 0x6800");
}

static void test_integrate_bounce(void) {
  PhysWorld w;
  phys_world_init_shell(&w);
  w.ball.x = 0x1000;
  w.ball.y = 0x1000;
  w.ball.vel_x = 0x80;
  w.ball.vel_y = 0;
  int32_t x0 = w.ball.x;
  phys_live_integrate(&w.ball);
  CHECK(w.ball.x > x0, "integrate advances x");

  /* Soft bounce e path */
  int32_t n = -0x100, t = 0;
  phys_bounce_response(&n, &t, PHYS_E_SOFT, 0, 0);
  CHECK(n > 0, "soft bounce flips n");
}

static void test_cases_3_5_8(void) {
  PhysWorld w;
  phys_world_init_shell(&w);
  uint8_t blob[12];
  make_seg(blob, 0x1F00, 0x2F00, 0x2100, 0x3100, 0, 0x0010);
  phys_mask_load_be(&w, blob, 12);

  /* Force type-8 via binder override (simulates bytecode cmd producing 0x0800) */
  w.segs[0].contact_flags = 0x0800;
  w.ball.x = 0x2000;
  w.ball.y = 0x3000;
  w.ball.vel_y = -0x200;
  w.ball.basis_c = 16384;
  phys_live_collide_step(&w);
  CHECK(w.case8 == 1, "case8 flag");
  CHECK(w.sfx_ring_r4 == 2 && w.sfx_ring_r5 == 5, "RingSfx(2,5)");

  w.segs[0].contact_flags = 0x0300;
  w.case8 = 0;
  phys_live_collide_step(&w);
  CHECK(w.ball.hit == 1, "type3 hit");
}

int main(void) {
  fails = 0;
  test_dp30_binder();
  test_mask_load_default_flags();
  test_poly_consume();
  test_flipper_shell();
  test_integrate_bounce();
  test_cases_3_5_8();
  if (fails) {
    printf("RESULT FAIL (%d)\n", fails);
    return 1;
  }
  printf("RESULT PASS\n");
  return 0;
}
