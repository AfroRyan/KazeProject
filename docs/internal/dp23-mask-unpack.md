---
cursor:
  subagentId: "bc-80f5490c-b461-5cbd-81a5-28ca11530dcc"
---

# DP-23 — Victors MASK 6×u16 unpack

Branch: `cursor/uv-mask-unpack-0dcc`

## Fix

Victors MASK TOC is `o1` only (no `o2`; `BACK` follows). Prior extract was a
windowed 447-seg guess. New path skips poly/palette in `o1..0xc50d8` and takes
the longest BE 6×u16 run @ `0xc31ec` → **653** nonzero segs.

Also: probe radius = `word5 & 0xff` capped at 32; `PHYS_MASK_CAP=1024`; harness
hit/miss probe assert.

## Artifacts

- `tools/extract-lg-mask-segs.py`
- `assets/physics/victor_MASK_segs.{bin,json}` + `victor_MASK_extract.json`
- Notes + progress updated

## Still approximate

word4/word5 semantics (flags/radius vs coordinate-range values); bounce-type
hi-nibble heuristic; poly header not consumed for collide.

## Dog DMD

Parked.
