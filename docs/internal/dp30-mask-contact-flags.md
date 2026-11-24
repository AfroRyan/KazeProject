---
cursor:
  subagentId: "bc-80f5490c-b461-5cbd-81a5-28ca11530dcc"
---

# DP-30 — MASK RES → live `contact_flags` binder

## Ask

Trace how RES MASK word4/word5 / `contact_flags` become live collide flags in
Victors (SH2 + harness). Wire a real binder (or document a hard limit). Do not
reopen flipper XY; leave dog assets untouched; no origin push.

## Trace (A.SH2 base `0x06010000`)

| VA | Role |
|---|---|
| `0x06012CF4` | Collide-response: `r3=*(obj+88)`; `flags=*(u16*)(r3+8)`; type=`(flags>>8)&15` |
| `0x06013ABA` | Bind contact record ptr → `obj+88` (nearest hit) |
| `0x0603AD08` | Contact table base; record stride **20**; flags at **+8** |
| `0x06015444` | MASK **bytecode** dispatcher (`r4`=stream); opcode=`cmd&0x0F00`; end=`0xFFFF` |
| `0x060155B6` | Opcode **0x0100**: lo-nibble → base `{2:0x1000,3:0x3000,4:0x2000}` then `+0x0100` or `+0x8100` if `cmd&0x8000` |
| `0x06015742` | Opcode **0x0700**: `flags = 0x6000 \| cmd` → binder `0x06016A98` |
| `0x06016A98` / `0x060163CA` | Write `contact_flags` to record `+8` (batch arg / stream) |

RES 6×u16 seg `word4` is **not** copied into `*(contact+8)`. Live flags come from
bytecode opcode dispatch when contact records are built.

## Host wire

- `PhysMaskSeg.contact_flags` — live collide field
- `phys_mask_contact_flags_from_cmd(cmd)` — opcode 0x0100 / 0x0700 paths
- `phys_mask_load_be` binds default **`0x0100`** (type-1 fallthrough → soft 2)
- `phys_mask_bounce_type` reads **`contact_flags`**, not RES word4

## Hard limit

`victor_MASK_segs.bin` is the expanded geometry table only — **no** opcode
stream. Per-seg live flag differentiation is unrecovered from RES segs alone.
Default `0x0100` matches Victors live WRAM contact majority (type-1 → host soft 2).

## Verify (prior session, 2026-11-16)

`make -C games/last-gladiators/harness run` → **RESULT PASS**; WASM rebuilt;
`node tools/smoke-lg-harness.mjs` OK. Changes were **uncommitted** when the
usage-limit pause recycled the VM.

## Resume blocker (2026-11-24)

This worker has **empty `/agent`** and **no linked Cursor environment / repo**.
Harness sources and `cursor/uv-mask-contact-flags-0dcc` are not on disk.
Need Project environment remount of `/agent/games/compilation` (or a git remote
with that tree) to re-apply the binder patch and re-smoke.

## Left alone

Flipper shell pivots (DP-29); dog DMD/credits.
