# SNES HD art — progress

**Status (2026-11-15):** HD pack **v1** for all **6** SNES tables. SP2 credits CG
**codec cracked** + **DP-4** pass: credit-text bands fixed with correct CGRAM;
`ref-credits.png` rewritten in all three SP2 packs.

Branch: `cursor/snes-hd-art` @ `/agent/games/snes-hd-art/`  
Plan: `/cursor/stores/self/docs/snes-hd-art-plan.md`  
Packs: `/cursor/stores/self/media/snes-hd/`

## Done

| Item | Location |
|---|---|
| Table inventory (BTM×3 + SP2×3) | `docs/snes-hd-art-plan.md` |
| Slot list + shell swap contract | plan + `media/snes-hd/README.md` + `index.json` |
| HD playfield + backglass (all 6) | `media/snes-hd/<id>/{playfield-hero,backglass}.png` |
| DMD frame template | per-table `dmd-frame.png` |
| Pack manifests | per-table `manifest.json` |
| SP2 + BTM ROMs (sha match inventory) | `/home/ubuntu/kaze-roms/snes/*.sfc` |
| SP2 CG codec (`$8C47` LZSS mode1) | `_extract/sp2-credits/extract-notes.json` |
| SP2 `ref-credits.png` (CGRAM + title bands) | `sp2.space_sister` / `sp2.spy_eyes` / `sp2.show_time` |

### Tables — P0 art

| id | Display | playfield | backglass | ref-credits |
|---|---|---|---|---|
| `btm.jolly_joker` | Jolly Joker | done | done | stub |
| `btm.blackbeard` | Blackbeard and Ironmen | done | done | stub |
| `btm.wizard` | Wizard | done | done | stub |
| `sp2.space_sister` | Space Sister | done | done | **DP-4** (Odyssey+title band+stage) |
| `sp2.spy_eyes` | The Spy Eyes | done | done | **DP-4** (Odyssey+title band+stage) |
| `sp2.show_time` | Show Time | done | done | **DP-4** (Odyssey+title band+stage) |

## SP2 credits codec (2026-11-15)

- **Decompress:** `$8C47` mode 1 = Nintendo LZSS (4KB ring @ `$0FEE`, flag
  shift-first, match `length=(nibble)+2` with inclusive copy). Header
  `rem` matches decompressed size; input span matches `$E3D9` directory deltas.
- **Directory:** base `$0E45=$87` → bank `$0E:$8000` (8-byte start/end records).
- **CGRAM:** assets **0–8** = full 512B BGR555 dumps.
- **Layouts:**
  - **8bpp** 8×8 tiles + BG tilemap words (`tid & 0x3FF`, X/Y flip) for title
    strip + stage CGs.
  - **4bpp** tiles + palette bits (10–12) for Odyssey title (`map9+tiles21`).

### DP-4 pack map (pixel-closer stills)

| Pack | Shared title | Table title band | Stage CG |
|---|---|---|---|
| `sp2.space_sister` | `map9+tiles21+pal0` | `map11+tiles24+pal3` (SPACE SISTER) | `map12+tiles25+pal5` |
| `sp2.spy_eyes` | `map9+tiles21+pal0` | `map11+tiles24+pal3` (The Spy Eyes) | `map13+tiles26+pal1` |
| `sp2.show_time` | `map9+tiles21+pal0` | `map11+tiles24+pal3` (SHOW TIME) | `map15+tiles28+pal7` |

### Shear + palette findings

- Tilemap **width 32** is correct (left-edge tile-row jump = 0 across width sweep).
- Much of the earlier “credit-text shear” was **wrong CGRAM**: e.g. Odyssey title
  with non-`pal0` inserts false green bands / stair-steps; with `pal0` vertical
  stems read clean.
- **Title strip** `map11+tiles24+pal3` is near pixel-perfect (three stacked
  banners). Italic/cursive slant is source art, not a stride bug.
- Stage CGs remain dither-heavy under alternate pals; best distinctive picks
  above. Intentional diagonal art (CIRCUS banner, sunburst edges) retained.

Decode helper: `_extract/sp2-credits/decode_sp2_credits.py` (`--preset odyssey|titles|space|spy|show`).

## Stubbed / partial

| Item | Status |
|---|---|
| SP2 stage CG further palette refinement | Open — title bands locked; stage pals are best visual picks |
| BTM `ref-credits.png` | Stub (BTM credits path not in this pass) |
| Saturn board stills | Style neighbors noted; ISOs not required for this decode |
| `mode-overlay.png` | Placeholder |
| Physics | Out of scope |

## Verified store paths

```
/cursor/stores/self/docs/snes-hd-art-plan.md
/cursor/stores/self/docs/snes-hd-art-progress.md
/cursor/stores/self/media/snes-hd/index.json
/cursor/stores/self/media/snes-hd/sp2.space_sister/ref-credits.png
/cursor/stores/self/media/snes-hd/sp2.spy_eyes/ref-credits.png
/cursor/stores/self/media/snes-hd/sp2.show_time/ref-credits.png
/cursor/stores/self/media/snes-hd/_extract/sp2-credits/extract-notes.json
/cursor/stores/self/media/snes-hd/_extract/sp2-credits/decoded-title-strip-pal3.png
/cursor/stores/self/media/snes-hd/_extract/sp2-credits/decoded-odyssey-title-pal0.png
```

## Milestone

**SNES HD art pack v1 + SP2 credits DP-4** — six tables packaged; SP2 outro
stills use correct CGRAM on credit-text bands (Odyssey + per-table title
strip) with colored stage CG underlays in each pack’s `ref-credits.png`.
