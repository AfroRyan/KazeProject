# KazeProject

Digital Pinball decomp / Saturn-style **compilation frontend** + LG Victors physics harness.

Restored 2026-11-24 from the Project Agent Store after the prior `/agent/games/compilation` disk was lost. **No game ROMs/ISOs/BIOS are vendored.**

## Layout

| Path | Role |
|---|---|
| `games/compilation/` | Menu shell, Ultimate Victors play/credits, JS/WASM harness loader |
| `games/last-gladiators/src/` | Portable `physics.c` / `physics.h` (DP-13…DP-30) |
| `games/last-gladiators/harness/` | Native headless smoke (`make run`) |
| `games/_shared/physics_pack.h` | Universal pack IDs (`lg-97` default for LG) |
| `docs/` | Spec, playlist, physics profiles, progress |
| `media/` | Dog DMD/credits + DP-29 freeze evidence |

## Quick verify (no ISO)

```bash
make -C games/last-gladiators/harness run
cd games/compilation && node tools/smoke-lg-harness.mjs && node tools/verify.mjs
```

Serve the shell locally:

```bash
cd games/compilation && python3 -m http.server 8765 --bind 127.0.0.1
# open http://127.0.0.1:8765/
```

## DP-30 — MASK `contact_flags` binder

Live collide flags come from MASK **bytecode** opcodes (`phys_mask_contact_flags_from_cmd`), not RES seg word4. Seg-table loads default `0x0100` (type-1 → soft 2). Bounce reads `PhysMaskSeg.contact_flags`.

## Still missing (Drive / ISO)

- LG USA full Redump + `VICTOR.RES` → `victor_MASK_{segs,poly}.bin`, SOND oggs
- Saturn BIOS (`mpr-17933.bin`, `sega_101.bin`) under Drive `system/`
- Prior `lg_phys.wasm` (rebuild with wasi-sdk when ready)
- SNES/Necro table art & FMV/CDDA named in the playlist

## Credits

Ultimate Victors caption: **Viber Coder: Ryan Fisher**  
Flipper pivots: shell approx `0x1400` / `0x2c00` × `0x6800` (DP-29 unrecovered)
