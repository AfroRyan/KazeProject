# Compilation frontend progress

**Status (2026-11-24):** **Restored to `AfroRyan/KazeProject`**. DP-30 binder
re-applied in reconstructed `physics.c`; native harness **RESULT PASS**. Prior
`/agent/games/compilation` disk + uncommitted `cursor/uv-mask-contact-flags-0dcc`
work were lost; rebuilt from Agent Store docs/media (no invented ROMs).
Flipper XY shell kept. Dog caption **Viber Coder: Ryan Fisher**.

Workspace: `games/compilation/` · repo root `/workspace` → `github.com/AfroRyan/KazeProject`

## Delivered (this restore)

| Item | Location |
|---|---|
| LG C harness | `games/last-gladiators/src/physics.{c,h}` · `harness/` |
| MASK contact_flags | `phys_mask_contact_flags_from_cmd` + seg default `0x0100` (DP-30) |
| Bounce | reads `contact_flags`, **not** RES word4 |
| UV / menu shell | `games/compilation/index.html` + `js/` + `css/shell.css` |
| JS physics | `js/lg-physics.js` + `js/lg-physics-harness.js` |
| Pack API | `games/_shared/physics_pack.h` · `docs/physics-profiles.*` |
| FLIP pivots JSON | `assets/physics/victor_FLIP_pivots.json` (shell approx) |
| MASK bounce map | `assets/physics/victor_mask_bounce_map.json` |
| Dog DMD (live) | `assets/dmd/ultimate-victors-dog-dmd.png` (**128×64**) |
| Dog credits | `assets/credits/ultimate-victors-dog-credits{,-crop}.png` |
| Extract tools | `tools/extract-lg-mask-segs.py`, `extract-lg-sond-sfx.py` (need ISO) |
| Spec / playlist | `docs/compilation-frontend-spec.md`, `ultimate-conquest-playlist.md` |

## Still missing (need Drive / ISO — not invented)

| Asset | Notes |
|---|---|
| `victor_MASK_segs.bin` (653) | Extract from `VICTOR.RES` via tool |
| `victor_MASK_poly.bin` (1404B) | Same |
| `lg_phys.wasm` | Rebuild with wasi-sdk from `physics.c` |
| `sfx/lg_ring_r4_{1,2,3}.ogg` + `sond_catalog.json` | SOND extract needs RES |
| Saturn BIOS | Drive `system/` → `mpr-17933.bin`, `sega_101.bin` |
| LG USA full Redump | 86-track; Track-01-only is wrong image |
| SNES/Necro FMV·CDDA | Named slots in playlist only |

## DP-30 — MASK RES → live `contact_flags` binder (re-applied)

| Item | Result |
|---|---|
| Collide | `*(obj+88)+8` → `(u16>>8)&15` @ `0x06012CF4` |
| Bind path | Dispatcher `0x06015444` → contact table `0x0603AD08` stride 20 → `obj+88` |
| Opcode 0x0100 | lo-nibble → base `{2:0x1000,3:0x3000,4:0x2000}` + `(bit15?0x8100:0x0100)` |
| Opcode 0x0700 | `flags = 0x6000 \| cmd` |
| Host | `phys_mask_contact_flags_from_cmd` + `s->contact_flags`; bounce reads binder |
| Seg-table | No bytecode in RES segs → default **`0x0100`** (type-1 → soft 2) |
| Verify | `make -C games/last-gladiators/harness run` → **RESULT PASS** |

## DP-29 — Flipper absolute world XY (**unrecovered**)

Shell pivots **`0x1400/0x2c00 × 0x6800`** unchanged. Evidence:
`media/dp29-victors-live-freeze.webp` · `docs/internal/dp29-flipper-xy-wram.md`.

## Dog credits / DMD

Caption **Viber Coder: Ryan Fisher**. DMD 128×64 amber; credits crop 720×842.

## Verify

```bash
make -C games/last-gladiators/harness run
cd games/compilation && node tools/smoke-lg-harness.mjs && node tools/verify.mjs
python3 games/compilation/tools/extract-lg-mask-segs.py   # needs /tmp/lg-iso/fs
```
