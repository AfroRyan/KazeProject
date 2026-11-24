# SNES HD art plan — Behind the Mask / Super Pinball II

Ryan Fisher — Kaze compilation · optional higher-res table artwork  
Branch target: `cursor/snes-hd-art`  
Store packs: `/cursor/stores/self/media/snes-hd/`  
Workspace mirrors (when present): `games/*/assets/hd/`

## Goal

Optional HD art packs for **every SNES table**, using:

- Saturn Digital Pinball visual language (Last Gladiators / Necronomicon) as style reference
- Manuals / box / in-game identity for each board
- **Super Pinball II credits outro stills** (higher-res CG) as table-aspect refs

Do **not** invent physics. Keep each table’s identity; coordinate look with Saturn polish.

## SNES table inventory (all need HD)

### Behind the Mask (Super Pinball) — 3 tables

Conquest / `$0275` order from playlist research:

| id | Display | Theme cues | Saturn style neighbors |
|---|---|---|---|
| `btm.jolly_joker` | Jolly Joker | Carnival / jester / comedy masks | LG showmanship (Glad/Rose energy) |
| `btm.blackbeard` | Blackbeard and Ironmen | Pirate / iron / sea | LG War / Drago grit |
| `btm.wizard` | Wizard | Magic / staff / arcane | Necro / Warlock mood |

Also referenced in ROM strings (modes / doors / features, not separate tables):
Forbidden Door, Mystery, Treasure House, Limbo, THE MASK, NECROMANCER, ENIGMA,
BLACK MAGIC — treat as **overlay / DMD / mode art** slots under the parent table.

### Super Pinball II: The Amazing Odyssey — 3 tables

| id | Display | Theme cues | Saturn style neighbors |
|---|---|---|---|
| `sp2.space_sister` | Space Sister | Sci-fi / heroine / space | LG Glad neon + clean CG |
| `sp2.spy_eyes` | The Spy Eyes | Espionage / eyes / night city | Necro Arkham intrigue |
| `sp2.show_time` | Show Time | Stage / lights / performance | LG Rose / show |

## Asset slots per table

Every table pack under `media/snes-hd/<table_id>/` should eventually include:

| File | Role | Priority |
|---|---|---|
| `playfield-hero.png` | Full-bleed table art (compilation / select) | **P0** |
| `backglass.png` | Cabinet / title glass | **P0** |
| `dmd-frame.png` | Dot-matrix bezel / frame (128×32 content area marked) | P1 |
| `mode-overlay.png` | Conquest door / mode splash | P1 |
| `ref-credits.png` | Still extracted or derived from SP2 credits CG | P0 for SP2; optional BTM |
| `ref-saturn.png` | Saturn board still used as style guide | P1 |
| `manifest.json` | Slot metadata + swap key | **P0** |

Shared pack root files:

| File | Role |
|---|---|
| `media/snes-hd/index.json` | All tables + pack paths + shell toggle contract |
| `media/snes-hd/README.md` | How compilation swaps HD vs native |

## Reference sources

1. **SP2 ROM credits / outro CG** — higher-res pictures Ryan flagged; raw bank
   dumps live at `media/snes-hd/_extract/sp2-credits/` (banks `$0F–$16`,
   `$21–$24`). Decoded stills await CG codec.
2. **BTM USA Rev 1 ROM** — acquired (`65aff140…`); ending/staff extract deferred.
3. **Saturn** — LG `GLAD/ROSE/DRAGO/WAR` + Necro `ARKHAM/BLOODY/DLANDS` as
   palette/material neighbors (not 1:1 copies); noted in per-table manifests.
4. **Manuals / box** — theme confirmation only.

## Compilation shell swap (QoL)

Optional toggle later (not required for first art milestone):

```text
settings.artPack = "native" | "hd"
resolve(tableId, slot) →
  hd   ? media/snes-hd/<tableId>/<slot>.png
  :      games/<game>/assets/native/<tableId>/<slot>.png (or ROM decode)
```

`index.json` exposes `swapKey` per slot so `games/compilation` (or future shell)
can bind without hardcoding paths. Physics / collide / WRAM paths stay native.

## Out of scope

- Inventing collide / scoring / tilt / multi-ball
- Replacing Saturn table art (SNES-only workstream)
- Full Mode 7 playfield mesh rebuild
