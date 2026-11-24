# Ultimate Conquest — ordered playlist

Evidence: ROM/ISO table IDs from the kaze-decomp archive (LG `res_loader`
GLAD/ROSE/DRAGO/WAR/VICTOR; Necro ARKHAM/BLOODY/DLANDS RES boards; BTM/SP2
`$0275` three-table Conquest) cross-checked against manuals / HG101 /
MobyGames / TV Tropes. Chronological by **parent game release**.

## Parent release order

| # | Game | Platform | Year |
|---|---|---|---|
| 1 | Super Pinball: Behind the Mask | SNES | 1993–94 |
| 2 | Super Pinball II: The Amazing Odyssey | SNES | 1995 |
| 3 | Digital Pinball: Last Gladiators | Saturn | 1995 |
| 4 | Digital Pinball: Necronomicon | Saturn | 1996 |

## Full Ultimate Conquest chain

### A. Behind the Mask — original Conquest order

| Step | Table id | Display name | Clear rule (original) | Asset slots |
|---|---|---|---|---|
| 1 | `btm.jolly_joker` | Jolly Joker | 60M → Forbidden Door → Mystery (30s) | `fmv/none`, `audio/btm_table_jj.*` |
| 2 | `btm.blackbeard` | Blackbeard and Ironmen | 120M → Door → Treasure House (30s) | `audio/btm_table_bb.*` |
| 3 | `btm.wizard` | Wizard | 180M → Door → Limbo (30s) | `audio/btm_table_wz.*` |

After step 3: play **BTM ending** (`fmv/btm_ending.*`), **skip credits**, continue.

### B. Super Pinball II — original Conquest order

| Step | Table id | Display name | Clear rule | Asset slots |
|---|---|---|---|---|
| 4 | `sp2.space_sister` | Space Sister | 100M + timed clear shot | `audio/sp2_table_ss.*` |
| 5 | `sp2.spy_eyes` | The Spy Eyes | 100M + timed clear shot | `audio/sp2_table_se.*` |
| 6 | `sp2.show_time` | Show Time | 100M + timed clear shot | `audio/sp2_table_st.*` |

After step 6: play **SP2 ending** (`fmv/sp2_ending.*`), **skip credits**, continue.

### C. Last Gladiators — invented Conquest (mirrors Necro Realms)

LG had no Conquest. Progression mirrors Necromomicon Realms: complete the
table’s round set → open a Conquest lane → shoot to advance. Menu / PRZ order
from USA Ver.9.7 board list.

| Step | Table id | Display name (cabinet) | PRZ/RES | Clear rule (invented) | Asset slots |
|---|---|---|---|---|---|
| 7 | `lg.glad` | Gladiators | `GLAD` | Finish rounds → Conquest lane | `audio/lg_table_glad.*` |
| 8 | `lg.rose` | Knight of the Roses | `ROSE` | same | `audio/lg_table_rose.*` |
| 9 | `lg.drago` | Dragon Showdown | `DRAGO` | same | `audio/lg_table_drago.*` |
| 10 | `lg.war` | Warlock | `WAR` | same | `audio/lg_table_war.*` |

**Victors** (`lg.victors` / `VICTOR`) is **not** a mid-chain Conquest table.
It loads only as **Ultimate Victors** after the full chain.

After step 10: play **Last Gladiators opening FMV** as reward
(`fmv/lg_opening.*`), then continue into Necronomicon.

### D. Necronomicon — original Realms order

| Step | Table id | Display name | RES board | Between-stage FMV | Asset slots |
|---|---|---|---|---|---|
| 11 | `necro.arkham` | Arkham | `ARKHAM` | Realms scene after clear | `fmv/necro_realms_1.*`, `audio/necro_arkham.*` |
| 12 | `necro.bloody` | Cult of the Bloody Tongue | `BLOODY` | Realms scene | `fmv/necro_realms_2.*`, `audio/necro_bloody.*` |
| 13 | `necro.dreamlands` | Dreamlands | `DLANDS` | Necro **outro** (not staff roll) | `fmv/necro_outro.*`, `audio/necro_dreamlands.*` |

After Necro outro: **skip Necro credits** → load **Ultimate Victors**.

### E. Ultimate Victors

| Step | Table id | Notes |
|---|---|---|
| 14 | `lg.ultimate_victors` | Base Victors board; backglass **Ultimate Victors**; blank DMD in play; extended credits (see spec) |

Credits sections (music asset slots):

1. `audio/credits_btm.*` — theme B/W — text BEHIND THE MASK / STAFF  
2. `audio/credits_sp2.*` — blues — THE AMAZING ODYSSEY  
3. `audio/credits_lg.*` — LAST GLADIATORS  
4. `audio/credits_necro.*` — red/silver/black — NECRONOMICON  
5. **Viber Coder: Ryan Fisher** — credits portrait `media/ultimate-victors-dog-credits-crop.png` (720×842; full plate `ultimate-victors-dog-credits.png`)  
6. Thanks KAZe  
7. Thanks players → initials entry (save UV score + UC clear time)

## Menu listing (chronological parents)

Same tables as above for free play, plus:

- **Ultimate Conquest** (mode entry)
- **Victors** — Right (when unlocked) switches to **Ultimate Victors**
- Early unlock: title-screen Konami Code → clip
  `audio/necro_clip_dark_knowledge.*` (“It’s Dark Knowledge, beware.”)

## Stub policy

Every slot is named in `games/compilation/assets/manifest.json`. All
priority FMV/audio slots including **table beds** are **real** (Saturn CDDA
+ SNES SPC + ending stills). See `docs/compilation-progress.md`.
