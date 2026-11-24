/* LG Victors portable physics — reconstructed from Agent Store contracts.
 * DP-30: MASK bytecode → live contact_flags binder; bounce reads contact_flags. */
#include "physics.h"

#include <string.h>

/* ---- Q14 AngleSinCos (coarse host table; dump AngleSinCos when ISO present) ---- */
static int32_t q14_sin(int a) {
  /* a in 0..0xFFFF full circle; coarse 16-step */
  static const int16_t tab[16] = {
      0, 6270, 11585, 15137, 16384, 15137, 11585, 6270,
      0, -6270, -11585, -15137, -16384, -15137, -11585, -6270};
  unsigned i = ((unsigned)a >> 12) & 15u;
  return tab[i];
}

void phys_angle_sincos(int angle_q, int32_t *out_c, int32_t *out_s) {
  if (out_s) *out_s = q14_sin(angle_q);
  if (out_c) *out_c = q14_sin(angle_q + 0x4000);
}

int32_t phys_damp_mag_from_speed(int32_t speed) {
  if (speed < 0) speed = -speed;
  /* LG attenuate path — soft host stand-in */
  return speed > 0x51 ? speed - 0x51 : 0;
}

void phys_contact_project(const PhysLiveObj *o, int32_t *n, int32_t *t) {
  int32_t c = o->basis_c, s = o->basis_s;
  int32_t vx = o->vel_x, vy = o->vel_y;
  /* project vel into contact frame */
  *n = (vx * c + vy * s) >> 14;
  *t = (-vx * s + vy * c) >> 14;
}

void phys_bounce_response(int32_t *n, int32_t *t, int e, int32_t bounce_boost,
                          int32_t spin_boost) {
  int32_t nn = *n;
  if (nn < 0) nn = -((nn * e) >> 10);
  nn += bounce_boost;
  *n = nn;
  *t = *t + spin_boost;
}

void phys_contact_attenuate_eighth(int32_t *n) { *n -= (*n >> 3); }

/* ============================================================================
 * DP-30 — MASK RES opcode stream → live contact_flags
 *
 * Dispatcher 0x06015444: opcode = cmd & 0x0F00
 *   0x0100 @ 0x060155B6: lo-nibble → base {2:0x1000,3:0x3000,4:0x2000}
 *                         then +0x0100 or +0x8100 if cmd&0x8000
 *   0x0700 @ 0x06015742: flags = 0x6000 | cmd
 * Collide reads *(obj+88)+8 → type = (flags>>8)&15
 * ============================================================================ */
uint16_t phys_mask_contact_flags_from_cmd(uint16_t cmd) {
  uint16_t op = (uint16_t)(cmd & 0x0F00u);
  if (op == 0x0100u) {
    uint16_t lo = (uint16_t)(cmd & 0x000Fu);
    uint16_t base = 0;
    if (lo == 2) base = 0x1000;
    else if (lo == 3) base = 0x3000;
    else if (lo == 4) base = 0x2000;
    /* type-1 / other lo → base 0; fallthrough soft via +0x0100 */
    uint16_t hi = (cmd & 0x8000u) ? 0x8100u : 0x0100u;
    return (uint16_t)(base + hi);
  }
  if (op == 0x0700u) {
    return (uint16_t)(0x6000u | cmd);
  }
  /* Unknown opcode / absent bytecode → type-1 fallthrough soft 2 */
  return PHYS_MASK_CONTACT_FLAGS_DEFAULT;
}

int phys_mask_bounce_type(uint16_t contact_flags) {
  int t = (int)((contact_flags >> 8) & 15u);
  switch (t) {
  case 2:
  case 3:
  case 5:
  case 6:
  case 7:
  case 8:
  case 10:
    return t;
  default:
    /* DP-25/30: coerce unknown to soft 2 (incl. type-1 from default 0x0100) */
    return 2;
  }
}

static uint16_t be16(const uint8_t *p) {
  return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

int phys_mask_load_be(PhysWorld *w, const uint8_t *data, size_t len) {
  if (!w || !data || len < 2) return -1;
  size_t n = len / 12;
  if (n > PHYS_MASK_CAP) n = PHYS_MASK_CAP;
  w->nsegs = 0;
  for (size_t i = 0; i < n; i++) {
    const uint8_t *p = data + i * 12;
    PhysMaskSeg *s = &w->segs[w->nsegs];
    s->w0 = be16(p + 0);
    s->w1 = be16(p + 2);
    s->w2 = be16(p + 4);
    s->w3 = be16(p + 6);
    s->w4 = be16(p + 8);
    s->w5 = be16(p + 10);
    if (s->w0 == 0 && s->w1 == 0 && s->w2 == 0 && s->w3 == 0 && s->w4 == 0 &&
        s->w5 == 0)
      continue;
    /* DP-30: seg table has no opcode stream → default contact_flags */
    s->contact_flags = phys_mask_contact_flags_from_cmd(0x0100);
    w->nsegs++;
  }
  return w->nsegs;
}

int phys_mask_poly_parse(PhysMaskPoly *p, const uint8_t *data, size_t len) {
  if (!p || !data) return -1;
  memset(p, 0, sizeof(*p));
  size_t off = 0;
  /* skip leading 0 pads */
  while (off + 2 <= len && be16(data + off) == 0) off += 2;
  while (off + 4 <= len) {
    uint16_t tag = be16(data + off);
    if (tag == 0xFFFF) {
      off += 2;
      break;
    }
    if (tag != 0x0101) {
      /* not a contour — stop consume */
      break;
    }
    uint16_t count = be16(data + off + 2);
    off += 4;
    if (p->ncontours < PHYS_POLY_CONTOUR_CAP)
      p->contour_counts[p->ncontours] = count;
    p->ncontours++;
    for (uint16_t i = 0; i < count && off + 6 <= len; i++) {
      int16_t x = (int16_t)be16(data + off);
      int16_t y = (int16_t)be16(data + off + 2);
      int16_t z = (int16_t)be16(data + off + 4);
      off += 6;
      if (p->nverts < PHYS_POLY_VERT_CAP) {
        p->verts_x[p->nverts] = x;
        p->verts_y[p->nverts] = y;
        p->verts_z[p->nverts] = z;
        p->nverts++;
      }
    }
  }
  if (off + 16 <= len) {
    uint16_t meta[8];
    for (int i = 0; i < 8; i++) meta[i] = be16(data + off + (size_t)i * 2);
    off += 16;
    int ncol = meta[6];
    if (ncol < 0) ncol = 0;
    if (ncol > 512) ncol = 512;
    p->npalette = ncol;
    for (int i = 0; i < ncol && off + 2 <= len; i++) {
      p->palette[i] = be16(data + off);
      off += 2;
    }
  }
  p->bytes = (int)off;
  return p->bytes;
}

void phys_world_bind_poly(PhysWorld *w, const PhysMaskPoly *p) {
  if (!w || !p) return;
  w->poly = *p;
  w->poly_bound = 1;
}

/* Axis-aligned probe using seg mid + radius (word5 lo-byte ≤32). */
int phys_mask_probe(const PhysWorld *w, int32_t x, int32_t y, int *out_seg) {
  if (!w) return 0;
  for (int i = 0; i < w->nsegs; i++) {
    const PhysMaskSeg *s = &w->segs[i];
    int32_t mx = ((int32_t)s->w0 + (int32_t)s->w2) / 2;
    int32_t my = ((int32_t)s->w1 + (int32_t)s->w3) / 2;
    int r = (int)(s->w5 & 0xff);
    if (r > 32) r = 32;
    if (r < 1) r = 8;
    int32_t dx = x - mx, dy = y - my;
    if (dx * dx + dy * dy <= (int32_t)r * (int32_t)r) {
      if (out_seg) *out_seg = i;
      return 1;
    }
  }
  return 0;
}

void phys_live_apply_gravity(PhysLiveObj *o) {
  if (!o) return;
  int32_t g = o->grav_y ? o->grav_y : 0xF0;
  o->vel_y -= g;
}

void phys_live_integrate(PhysLiveObj *o) {
  if (!o) return;
  o->x += o->vel_x >> 3;
  o->y += o->vel_y >> 3;
}

static void commit_bounce(PhysLiveObj *o, int32_t n, int32_t t) {
  int32_t c = o->basis_c, s = o->basis_s;
  o->vel_x = (n * c - t * s) >> 14;
  o->vel_y = (n * s + t * c) >> 14;
  o->work_x = o->vel_x;
  o->work_y = o->vel_y;
}

void phys_live_collide_step(PhysWorld *w) {
  if (!w) return;
  PhysLiveObj *o = &w->ball;
  int seg = -1;
  if (!phys_mask_probe(w, o->x, o->y, &seg)) {
    o->hit = 0;
    return;
  }
  o->hit = 1;
  /* DP-30: bounce type from live contact_flags, NOT RES word4 */
  uint16_t flags = w->segs[seg].contact_flags;
  o->contact_flags = flags;
  int typ = phys_mask_bounce_type(flags);

  int32_t n, t;
  if (o->basis_c == 0 && o->basis_s == 0) {
    o->basis_c = 16384;
    o->basis_s = 0;
  }
  phys_contact_project(o, &n, &t);

  w->case8 = 0;
  w->sfx_ring_r4 = 0;
  w->sfx_ring_r5 = 0;

  switch (typ) {
  case 3:
    phys_bounce_response(&n, &t, 0, PHYS_CASE3_BOOST, 0);
    break;
  case 5:
    phys_bounce_response(&n, &t, 0, 0x10, 0);
    break;
  case 6:
    phys_bounce_response(&n, &t, PHYS_E_FLIP, 0, 0);
    break;
  case 7:
    phys_contact_attenuate_eighth(&n);
    phys_bounce_response(&n, &t, PHYS_E_SOFT, 0, 0);
    break;
  case 8:
    phys_bounce_response(&n, &t, 0, 0, 0);
    w->case8 = 1;
    w->sfx_ring_r4 = 2;
    w->sfx_ring_r5 = 5; /* RingSfx(2,5) */
    break;
  case 10:
    w->sfx_ring_r4 = 2;
    w->sfx_ring_r5 = 1;
    phys_bounce_response(&n, &t, PHYS_E_SOFT, 0, 0);
    break;
  case 2:
  default:
    phys_bounce_response(&n, &t, PHYS_E_SOFT, 0, 0);
    break;
  }
  commit_bounce(o, n, t);
}

void phys_flip_pad_tick(PhysWorld *w, int left_down, int right_down) {
  if (!w) return;
  /* Shell pivots fixed (DP-29 unrecovered). rest/len host approx. */
  (void)left_down;
  (void)right_down;
  w->flip_lx = PHYS_FLIP_L_X;
  w->flip_ly = PHYS_FLIP_Y;
  w->flip_rx = PHYS_FLIP_R_X;
  w->flip_ry = PHYS_FLIP_Y;
  if (!w->flip_rest_l) w->flip_rest_l = 0x20;
  if (!w->flip_rest_r) w->flip_rest_r = 0x20;
  if (!w->flip_len_l) w->flip_len_l = 0x180;
  if (!w->flip_len_r) w->flip_len_r = 0x180;
}

void phys_live_tick_frame(PhysWorld *w, int left_down, int right_down) {
  phys_flip_pad_tick(w, left_down, right_down);
  phys_live_apply_gravity(&w->ball);
  phys_live_integrate(&w->ball);
  phys_live_collide_step(w);
}

void phys_play_frame(PhysWorld *w, int left_down, int right_down) {
  phys_live_tick_frame(w, left_down, right_down);
}

void phys_world_init_shell(PhysWorld *w) {
  memset(w, 0, sizeof(*w));
  w->ball.grav_y = 0xF0;
  w->ball.basis_c = 16384;
  w->ball.basis_s = 0;
  w->flip_lx = PHYS_FLIP_L_X;
  w->flip_ly = PHYS_FLIP_Y;
  w->flip_rx = PHYS_FLIP_R_X;
  w->flip_ry = PHYS_FLIP_Y;
  w->flip_rest_l = w->flip_rest_r = 0x20;
  w->flip_len_l = w->flip_len_r = 0x180;
}

/* ---- WASM singleton world ---- */
static PhysWorld g_wasm;

int phys_wasm_bind_mask(const uint8_t *data, size_t len) {
  return phys_mask_load_be(&g_wasm, data, len);
}

int phys_wasm_bind_poly(const uint8_t *data, size_t len) {
  PhysMaskPoly p;
  int n = phys_mask_poly_parse(&p, data, len);
  if (n < 0) return n;
  phys_world_bind_poly(&g_wasm, &p);
  return n;
}

int phys_wasm_nsegs(void) { return g_wasm.nsegs; }
int phys_wasm_poly_ncontours(void) { return g_wasm.poly.ncontours; }
int phys_wasm_poly_nverts(void) { return g_wasm.poly.nverts; }
int phys_wasm_poly_npalette(void) { return g_wasm.poly.npalette; }
int phys_wasm_poly_bytes(void) { return g_wasm.poly.bytes; }

uint16_t phys_wasm_seg_contact_flags(int i) {
  if (i < 0 || i >= g_wasm.nsegs) return 0;
  return g_wasm.segs[i].contact_flags;
}

int phys_wasm_seg_bounce_type(int i) {
  if (i < 0 || i >= g_wasm.nsegs) return 2;
  return phys_mask_bounce_type(g_wasm.segs[i].contact_flags);
}
