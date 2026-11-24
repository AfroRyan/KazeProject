# SNES multiball collision (compilation feature)

**Branch:** `cursor/snes-multiball-collision`  
**Workspace:** `/home/ubuntu/kaze-decomp/` (symlink `/agent/kaze-decomp`)

## What it is

Ball–ball collision for BTM + SP2 (and LG via the same pack API).  
SNES originals pass balls through each other; Saturn LG/Necro already run
multi-ball play loops. This feature brings Saturn-style equal-mass circle
response to SNES tables under the universal physics pack API.

## Defaults

| Option | Default | Purist |
|---|---|---|
| `multiball_collision` | **ON** | Set **OFF** for legacy SNES pass-through |
| `physics_pack` | per-table (LG → `lg-97`) | Any of `btm` / `sp2` / `lg-usa` / `lg-97` / `necro` |

Region/rev ROM pickers are **unchanged** and orthogonal to `physics_pack`.

## How to toggle

### Compilation shell

```bash
cd /home/ubuntu/kaze-decomp/games/compilation
./serve.sh   # http://127.0.0.1:8765/
```

Open **Options** → **Ball–ball collision** checkbox (ON by default).  
**Physics pack** dropdown selects the pack ID (see `physics-profiles.md`).

Persisted in `localStorage` key `kaze.compileOptions`:

```json
{ "multiball_collision": true, "physics_pack": "btm" }
```

### Harness / C API

```c
KazeCompileOptions opt;
kaze_options_init_defaults(&opt);          /* multiball ON */
kaze_options_set_multiball(&opt, 0);       /* purist OFF */
kaze_options_set_pack(&opt, KAZE_PACK_LG_97);
```

BTM/SP2 worlds: `btm_multi_set_multiball` / `sp2_multi_set_multiball`.

## Smoke

```bash
make -C /home/ubuntu/kaze-decomp/games/last-gladiators/harness multiball
# ON: velocities/positions change on overlap
# OFF: pair step is a no-op (legacy pass-through)
node /home/ubuntu/kaze-decomp/games/compilation/tools/verify.mjs
```

## Code map

| Piece | Path |
|---|---|
| Packs | `games/_shared/physics_pack.{h,c}` |
| Ball–ball | `games/_shared/ball_ball_collide.{h,c}` |
| Options | `games/_shared/compile_options.{h,c}` |
| BTM wrap | `games/behind-the-mask/src/btm_multiball.*` |
| SP2 wrap | `games/super-pinball-ii/src/sp2_multiball.*` |
| LG wrap | `games/last-gladiators/src/lg_multiball.*` (default pack 9.7) |
| Shell UI | `games/compilation/` Options tab |

## Saturn reference (evidence)

LG `phys_play_frame` already ticks **multiple balls** then places (pass 28);
inter-ball collide was a documented harness hole. This module fills that hole
portably and exposes it to SNES tables via Options — not a claim of a recovered
SNES ROM routine (static SNES has no ball–ball writer).
