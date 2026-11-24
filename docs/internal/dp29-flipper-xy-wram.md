---
cursor:
  subagentId: "bc-80f5490c-b461-5cbd-81a5-28ca11530dcc"
---

# DP-29 — Victors flipper WRAM XY capture

## Ask
Recover absolute Victors flipper world XY from live WRAM (object +4/+8) via
mednafen/ymir on LG USA Victors. Dump pivots, wire harness, update notes.
If blocked, document exact capture attempt. Dog assets locked.

## Result: **unrecovered**

Shell pivots kept: **L (0x1400, 0x6800) / R (0x2c00, 0x6800)**. Not wired.

## BIOS / disc (done earlier)

- Drive `system/` → `mpr-17933.bin` / `sega_101.bin` (retrobios MD5s)
- Full 86-track USA Redump required (Track-01-only → Venus SOND debugger)

## Victors live dump (2026-11-16 03:10)

| Item | Detail |
|---|---|
| Table | Teal / `COMPLETE ALL ROUNDS FOR LAST JUDGEMENT` / `ETERNAL WARRIOR` (VICTOR.RES; apron “WARLOCK” is staff credit text) |
| Play | Live — score **5,464,580**, FREEZE overlay, flippers visible |
| Savestate | `~/.mednafen/mcs/Last Gladiators - Digital Pinball (USA) (4S).*.mc0` (F12) |
| Screenshot | `media/dp29-victors-live-freeze.webp` |
| WorkRAMH | 1 MiB extracted (`WorkRAMH` BE size `0x100000`) |

## Pool confirmation

| Item | Value |
|---|---|
| PhysFlipSlot base | **`0x06043BAC`** (A.SH2 literal `@0x06016E0C`; flip-pad load `@0x06011F38`) |
| Stride | **80** (`index * 80`) |
| Pivot write | `+4/+8` s32 ← ResObj **s16 `@+10/+12`** (`0x06016D4A`) |
| Flip-pad object | also uses **`0x06043CEC`** (= pool slot 4) |

## Why pivots unrecovered

1. Pool slots 0/1 live `+4/+8` are **not** shell-band world XY  
   (e.g. slot0 `+4=0xff7bebff`, `+8=0xff5dae00`).
2. **Zero** aligned s32 L/R pairs in bands  
   Lx∈[0x0C00,0x1C00], Rx∈[0x2400,0x3400], Y∈[0x5000,0x7800].
3. **Zero** permissive stride-80 ordered pairs with playfield-like Y across all WorkRAMH.
4. ResObj s16 path is confirmed in SH2; live s16 band hits exist as noise, but they are **not** present as aligned s32 pool pivots in this dump.

Stopping here per brief: no further BIOS iteration. Further gains need a deeper scroll/world-space SH2 trace or another live moment — not emulator setup.

## Artifacts

- `games/compilation/tools/capture-victors-flipper-xy.sh`
- `assets/physics/victor_flipper_xy_search.json` → `dp29_victors_live_dump`
- `assets/physics/victor_FLIP_pivots.json` (`world_xy.status: unrecovered`)
- `docs/compilation-progress.md`
