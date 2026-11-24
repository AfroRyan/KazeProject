# Compilation frontend progress

**Status (2026-11-24):** Drive assets imported locally. DP-30 binder still green.
MASK poly/segs + SOND oggs + `lg_phys.wasm` restored into the repo from LG USA
Redump extract. Flipper shell kept. Caption **Viber Coder: Ryan Fisher**.

Workspace: `games/compilation/` · repo `github.com/AfroRyan/KazeProject`

## Drive import (2026-11-24)

| Item | Location | In git? |
|---|---|---|
| LG USA full Redump (86 tracks + cue) | `/workspace/roms/lg_usa/` | **no** (gitignored) |
| Saturn BIOS `sega_101.bin` | `/workspace/bios/saturn/` MD5 `85ec9ca4…` | **no** |
| Saturn BIOS `sega_100.bin` / `saturn_bios.bin` | same · MD5 `af5828fd…` | **no** |
| `vsaturn.bin` | same · MD5 `ac4e4b65…` | **no** |
| **`mpr-17933.bin`** | **missing from Drive `system/`** | — |
| Mednafen firmware symlinks | `~/.mednafen/firmware/` | local |
| Ymir IPL symlinks | `~/.local/share/StrikerX3/Ymir/roms/ipl/` | local |
| Derived MASK poly 1404B / segs **659** | `assets/physics/victor_MASK_*` | **yes** |
| Derived SOND oggs + catalog | `assets/physics/sfx/` | **yes** (PCM heuristic) |
| `lg_phys.wasm` | `assets/physics/lg_phys.wasm` | **yes** |

Drive folder: https://drive.google.com/drive/folders/1KkH3hNpEG6isq-qKTFMRkrlvUHe16PTj  
(`system/` id `1L-YVcw7le0qm0HUrCbtwyEMfH8kiI23t`; USA zip id `1SAcw4QnW2iKjzcViGeq7fST1jrDyUzMw`)

## Delivered

| Item | Location |
|---|---|
| LG C harness | `games/last-gladiators/src/physics.{c,h}` · `harness/` |
| MASK contact_flags | `phys_mask_contact_flags_from_cmd` + seg default `0x0100` (DP-30) |
| Bounce | reads `contact_flags`, **not** RES word4 |
| MASK segs/poly | TOC MASK→BACK · poly 1404B · **659** segs (notes formerly said 653) |
| WASM | `assets/physics/lg_phys.wasm` (clang wasm32) |
| SOND | `sfx/lg_ring_r4_{1,2,3}.ogg` + catalog rates 2950/2939/2966 |
| UV / menu shell | `games/compilation/` |
| Dog DMD/credits | caption **Viber Coder: Ryan Fisher** |

## Still missing / approximate

| Item | Notes |
|---|---|
| `mpr-17933.bin` (US/EU IPL) | Not in Drive `system/` (searched top-level + 19 subfolders) |
| SOND PCM binder | Ogg slices are **heuristic** high-variance runs; rates dump-backed |
| SNES/Necro FMV·CDDA | Named playlist slots only |
| Bit-identical prior `physics.c` | Reconstructed from store contracts |

## DP-30 — MASK `contact_flags` binder

Host binder re-applied; bounce reads `contact_flags`. Seg load defaults `0x0100`.
Verify: `make -C games/last-gladiators/harness run` → **RESULT PASS**.

## DP-29 — Flipper XY

Shell pivots **`0x1400/0x2c00 × 0x6800`** unchanged.

## Verify

```bash
make -C games/last-gladiators/harness run
cd games/compilation && node tools/smoke-lg-harness.mjs && node tools/verify.mjs
python3 games/compilation/tools/extract-lg-mask-segs.py --res /tmp/lg-iso/fs/VICTOR.RES
python3 games/compilation/tools/extract-lg-sond-sfx.py --res /tmp/lg-iso/fs/VICTOR.RES
```
