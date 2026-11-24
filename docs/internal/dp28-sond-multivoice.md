---
cursor:
  subagentId: "bc-80f5490c-b461-5cbd-81a5-28ca11530dcc"
---

# DP-28 — SOND multi-voice + r5 priority

## Ask
Dump-back SOND multi-voice layering and r5 priority (were host-approximate).
Wire into collision SFX path; update notes + progress. Dog assets locked.

## Multi-voice (VICTOR.RES SOND region[3])

Gated programs:

```
0200  |(gate_hi << 8) | 0x7F
then gate_hi+1 × (vol id addr 0 len 001f001f007f)
```

Each slot has its own preceding `layer:u16 + (OCT<<11|FNS)` pitch block.
Often the same PCM with slight FNS detune (SCSP unison stack).

| r4 | gate_hi | n_voices | rates (Hz) |
|---|---|---|---|
| 1 | 2 | 3 | 2950 / 2950 / 2950 |
| 2 | 2 | 3 | **2939 / 2966 / 2966** |
| 3 | 3 | 4 | 2966 / 2937 / 2937 / 2937 |

## r5 priority

RingSfx `@0x060116E0` → play `@0x06064412` queues cmd `0x0100` with
**r5 at byte +2** (sound id at +4). Collision call sites: case-8 r5=5;
type5/type10 r5=1.

Host: `gain_scale = r5 / 5`; per-voice `0.55 × scale / √n`.
No full SCSP steal allocator on host.

## Verify

`python3 tools/extract-lg-sond-sfx.py`  
`node tools/smoke-lg-harness.mjs` → multi-voice + r5 OK
