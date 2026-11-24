---
cursor:
  subagentId: "bc-80f5490c-b461-5cbd-81a5-28ca11530dcc"
---

# DP-26 — Victors MASK poly header consume

## Ask
Fully consume the Victors MASK poly header that collide previously skipped.
Wire into harness/WASM; update notes + progress. Dog assets locked.

## Layout (VICTOR.RES o1 `0xc2c70`)

| Field | Encoding |
|---|---|
| Pads | leading `0x0000` u16 words |
| Contours | repeat `0x0101` + `count:u16` + `count×(x,y,z)` BE u16 |
| Palette mark | `0xFFFF` |
| Meta | `8×u16`; **`meta[6]` = color count** |
| Colors | `ncol×u16` LUT |
| Next | BE 6×u16 seg table @ `0xc31ec` |

Victors: **1404B** → contours **[4,4,4,50,16]** (78 verts) · palette **448** ·
`meta[5]=640`, `meta[6]=448`.

## Host

- Extractor method `poly_consume_to_segs` (replaces longest-run skip for Victors)
- `phys_mask_poly_parse` + `phys_world_bind_poly`
- WASM: `phys_wasm_bind_poly`, `phys_wasm_poly_{ncontours,nverts,npalette,bytes}`
- Artifacts: `victor_MASK_poly.bin` / `.json`; fixture `MASK_poly.bin`

Collide path unchanged: poly verts are large-world draw/VDP geometry; probe
still uses the following 653 nonzero 6×u16 segs.

## Verify

`make -C games/last-gladiators/harness run` → **RESULT PASS**  
`node games/compilation/tools/smoke-lg-harness.mjs` → poly **1404B / 5 contours**
