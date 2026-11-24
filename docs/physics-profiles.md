# Kaze physics packs (universal cross-game)

Any table can run under any pack. **LG tables default to Ver.9.7.**  
BTM region/rev ROM selection stays a separate Options control.

## Pack IDs

| ID | `short_id` | Source feel | Notes |
|---|---|---|---|
| 0 | `btm` | Behind the Mask | Gravity −`$000C` / tick |
| 1 | `sp2` | Super Pinball II | Gravity −`$0009` / tick |
| 2 | `lg-usa` | Last Gladiators USA | Arena 1.46 family |
| 3 | `lg-97` | Last Gladiators Ver.9.7 | **Default for all LG tables** |
| 4 | `necro` | Necronomicon | Saturn family twin of 9.7 |

Portable API: `games/_shared/physics_pack.h`  
(`kaze_physics_pack`, `kaze_physics_pack_by_key`, `kaze_physics_pack_default_for_table`).

## Defaults by game family

| Family | Default pack |
|---|---|
| `btm` | `btm` |
| `sp2` | `sp2` |
| `lg` | **`lg-97`** |
| `necro` | `necro` |

## Multiball

See `snes-multiball-collision.md` — `multiball_collision` is an independent flag (default ON).

## Compilation shell (Ultimate Victors)

In-shell UV play **prefers the dump-free LG C harness** (WASM), default pack
**`lg-97`**.

| Layer | Path |
|---|---|
| C source | `games/last-gladiators/src/physics.c` |
| Native smoke | `make -C games/last-gladiators/harness run` |
| WASM | `games/compilation/assets/physics/lg_phys.wasm` |
| Loader | `games/compilation/js/lg-physics-harness.js` |
| JS fallback | `games/compilation/js/lg-physics.js` |

Contracts: `physics-profiles.json` `lg_usa` / `lg_ver97` + decomp passes.
Victors MASK (447 segs), Q14 `AngleSinCos`, case-7/8 + SFX ring, drain-aux,
FLIP-stream rest/len, and MASK bounce-type map included in C/WASM (DP-20;
DP-25 dump-backed A.SH2 switch — see `victor_mask_bounce_map.json`).
See `games/compilation/data/lg-physics-notes.json`.
