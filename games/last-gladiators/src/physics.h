/* LG Victors portable physics — reconstructed from decomp contracts (DP-13..DP-30).
 * Dump-free host; MASK segs / poly / SOND load from extracted assets when present. */
#ifndef KAZE_LG_PHYSICS_H
#define KAZE_LG_PHYSICS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PHYS_MASK_CAP 1024
#define PHYS_POLY_CONTOUR_CAP 32
#define PHYS_POLY_VERT_CAP 256

/* Shell-approx flipper pivots (DP-29 unrecovered world XY). */
#define PHYS_FLIP_L_X 0x1400
#define PHYS_FLIP_R_X 0x2c00
#define PHYS_FLIP_Y   0x6800

/* Bounce e values (A.SH2). */
#define PHYS_E_SOFT 0x012C
#define PHYS_E_FLIP 0x01D6
#define PHYS_CASE3_BOOST 0x3A20
#define PHYS_CASE8_THRESH 0xFD62
#define PHYS_FLIP_MAG 0x01AE

/* DP-30: default contact_flags when MASK bytecode absent from RES segs. */
#define PHYS_MASK_CONTACT_FLAGS_DEFAULT 0x0100

typedef struct PhysVec2 {
  int32_t x, y;
} PhysVec2;

typedef struct PhysLiveObj {
  int32_t x, y;           /* +4/+8 world (shell space) */
  int32_t vel_x, vel_y;   /* +24/+28 */
  int32_t work_x, work_y; /* +36/+40 */
  int32_t basis_c, basis_s; /* +0x40/+0x44 Q14 */
  int32_t grav_y;         /* scalar pull (−Y) */
  uint16_t contact_flags; /* live collide flags (binder output) */
  int32_t contact_nx, contact_ny;
  int hit;
} PhysLiveObj;

/* RES MASK 6×u16 segment (big-endian on disc). */
typedef struct PhysMaskSeg {
  uint16_t w0, w1, w2, w3, w4, w5;
  uint16_t contact_flags; /* DP-30 live field — NOT a copy of w4 */
} PhysMaskSeg;

typedef struct PhysMaskPoly {
  int ncontours;
  int nverts;
  int npalette;
  int bytes;
  uint16_t contour_counts[PHYS_POLY_CONTOUR_CAP];
  int16_t verts_x[PHYS_POLY_VERT_CAP];
  int16_t verts_y[PHYS_POLY_VERT_CAP];
  int16_t verts_z[PHYS_POLY_VERT_CAP];
  uint16_t palette[512];
} PhysMaskPoly;

typedef struct PhysWorld {
  PhysMaskSeg segs[PHYS_MASK_CAP];
  int nsegs;
  PhysMaskPoly poly;
  int poly_bound;
  PhysLiveObj ball;
  int32_t flip_lx, flip_ly, flip_rx, flip_ry;
  int32_t flip_rest_l, flip_rest_r;
  int32_t flip_len_l, flip_len_r;
  int case8;
  int sfx_ring_r4;
  int sfx_ring_r5;
} PhysWorld;

/* --- angle / bounce helpers --- */
void phys_angle_sincos(int angle_q, int32_t *out_c, int32_t *out_s);
int32_t phys_damp_mag_from_speed(int32_t speed);
void phys_contact_project(const PhysLiveObj *o, int32_t *n, int32_t *t);
void phys_bounce_response(int32_t *n, int32_t *t, int e, int32_t bounce_boost, int32_t spin_boost);
void phys_contact_attenuate_eighth(int32_t *n);

/* --- DP-30 MASK contact_flags binder --- */
uint16_t phys_mask_contact_flags_from_cmd(uint16_t cmd);
int phys_mask_bounce_type(uint16_t contact_flags);

/* --- MASK load / probe --- */
int phys_mask_load_be(PhysWorld *w, const uint8_t *data, size_t len);
int phys_mask_poly_parse(PhysMaskPoly *p, const uint8_t *data, size_t len);
void phys_world_bind_poly(PhysWorld *w, const PhysMaskPoly *p);
int phys_mask_probe(const PhysWorld *w, int32_t x, int32_t y, int *out_seg);

/* --- live tick --- */
void phys_live_apply_gravity(PhysLiveObj *o);
void phys_live_integrate(PhysLiveObj *o);
void phys_live_collide_step(PhysWorld *w);
void phys_flip_pad_tick(PhysWorld *w, int left_down, int right_down);
void phys_live_tick_frame(PhysWorld *w, int left_down, int right_down);
void phys_play_frame(PhysWorld *w, int left_down, int right_down);

void phys_world_init_shell(PhysWorld *w);

/* WASM / host exports (stable names used by lg-physics-harness.js). */
int phys_wasm_bind_mask(const uint8_t *data, size_t len);
int phys_wasm_bind_poly(const uint8_t *data, size_t len);
int phys_wasm_nsegs(void);
int phys_wasm_poly_ncontours(void);
int phys_wasm_poly_nverts(void);
int phys_wasm_poly_npalette(void);
int phys_wasm_poly_bytes(void);
uint16_t phys_wasm_seg_contact_flags(int i);
int phys_wasm_seg_bounce_type(int i);

#ifdef __cplusplus
}
#endif

#endif /* KAZE_LG_PHYSICS_H */
