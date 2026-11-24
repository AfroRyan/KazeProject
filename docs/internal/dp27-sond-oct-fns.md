---
cursor:
  subagentId: "bc-80f5490c-b461-5cbd-81a5-28ca11530dcc"
---

# DP-27 — SOND OCT/FNS playback rates

## Ask
Decode LG SOND OCT/FNS so collision SFX (`lg_ring_r4_*`) play at correct
rates instead of fixed 22050 Hz. Update extract + playback; notes + progress.
Dog assets locked.

## Decode

Before each sample descriptor in VICTOR.RES SOND region[3]:

`layer:u16` (1..16) + `pitch:u16` (`OCT<<11 | FNS`, e0xx/fxxx tone-bank)

```
OCT = signed4((pitch >> 11) & 15)
FNS = pitch & 0x3FF
rate_hz = 44100 * (FNS+1024)/1024 * 2^OCT
```

## Primary rings

| r4 | Sample | pitch | OCT | FNS | rate |
|---|---|---|---|---|---|
| 1 | `0x4a86` | `0xe048` | −4 | 72 | **2950 Hz** |
| 2 | `0x6b1c` | `0xe044` | −4 | 68 | **2939 Hz** |
| 3 | `0xe936` | `0xe04e` | −4 | 78 | **2966 Hz** |

Fallback when no pitch block: OCT=−1, FNS=0 → 22050 Hz.

## Host

- `tools/extract-lg-sond-sfx.py` — per-sample WAV/OGG rate + catalog oct/fns
- `js/lg-physics-harness.js` — loads `sond_catalog.json`; OGG header drives Web Audio
- Smoke asserts primary rates 2950/2939/2966

## Verify

`python3 tools/extract-lg-sond-sfx.py`  
`node tools/smoke-lg-harness.mjs` → SOND OCT/FNS rates OK
