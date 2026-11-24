# Kaze pinball decomp progress

**Status (2026-11-10):** Compilation feature — SNES multiball collision + universal
physics packs. Dump-free physics ceilings from pass 60 still hold for ROM
recovery; this workstream adds **cross-game pack API** and optional ball–ball.

Workspace: `/home/ubuntu/kaze-decomp/` · branch `cursor/snes-multiball-collision`

## Scores (readiness)

| Game | Score | Notes |
|---|---|---|
| **LG** | **9.9** | Pack default `lg-97`; multiball via shared step |
| **Necro** | **9.75** | Pack `necro`; same ball–ball API |
| **BTM** | **8.5** | Multiball ON by default behind pack API |
| **SP2** | **8.4** | Multiball twin + pack selector |

## SNES section — multiball / packs

| Item | Status |
|---|---|
| Ball–ball collide | **done** — default ON; Options OFF = legacy pass-through |
| Physics packs | **done** — `btm` `sp2` `lg-usa` `lg-97` `necro` |
| LG table default | **`lg-97`** |
| Region/rev choice | unchanged (orthogonal) |
| Docs | `snes-multiball-collision.md`, `physics-profiles.md` |

## Remaining gaps (ROM recovery)

| Gap | Status |
|---|---|
| Tilt bit writer / multi concurrency in BTM ROM | sibling WRAM |
| `$85C4` orphan | sibling |
| SPC firmware / SCSP | ceiling |

## Milestone

**Multiball collision + universal physics pack API; Options toggle; smokes green.**
