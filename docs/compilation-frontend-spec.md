# Kaze Pinball Compilation Frontend Spec

Ryan Fisher — Digital Pinball Decomp/Compilation Project

## Goal

Saturn-menu-inspired compilation frontend covering every table from every Kaze pinball title in this archive, plus **Ultimate Conquest** and **Ultimate Victors**.

Source dog photo for DMD credit art: `/cursor/stores/self/media/afro-ryan-dog.jpg` (focus on dog only; convert to Last Gladiators–style dot-matrix).

## Menu shell

- Resemble Saturn Digital Pinball menus (Last Gladiators / Necronomicon style): layout, typography cues, flow — expanded to list every table.
- Tables listed in **chronological original release order** of their parent games.
- Within each SNES game, order tables as in that game’s **Conquest** mode.
- Pressing **Right** while hovering **Victors** selects unlocked alternate **Ultimate Victors** (same slot, right to switch).
- Early unlock: on initial title screen, **Konami Code** `↑ ↑ ↓ ↓ ← → ← → B A Start`. On success, play Necronomicon clip **“It’s Dark Knowledge, beware.”** and unlock Ultimate Victors without beating Ultimate Conquest.

## Ultimate Conquest

Play every table back-to-back in release order. Use each original game’s Conquest / Realms rules to proceed to the next table.

### Last Gladiators segment

LG had no Conquest mode. Invent Conquest-style progression for LG tables by mirroring **Necronomicon Realms** general rules (shared engine family). When the LG set is fully cleared, play the **Last Gladiators opening FMV** as reward, then continue into Necronomicon.

### Necronomicon segment

Use original **Realms** progression. Between stages, insert Necronomicon’s unique **FMV sequences** as in Realms. After Necronomicon outro, skip its credits sequence here (credits belong on Ultimate Victors).

### Super Pinball / Super Pinball II segments

After each SNES game’s Conquest section completes, show that game’s **original ending**, **skip credits**. Then continue the Ultimate Conquest chain.

### After all tables + Necronomicon outro

Load **Victors** from Last Gladiators as **Ultimate Victors**.

## Ultimate Victors

Base: original Victors table design.

### Presentation

- Backglass text: **“Ultimate Victors”** (not just Victors).
- Dot-matrix display: **blank** during play (except credit art moments specified below).
- Extend playtime to match the longer multi-game credits sequence.

### Credits sequence (in order)

Play each game’s **original credits music** for its section. Table theming and board text update with fades as sections change. Non-LG name styling matches Last Gladiators credit styling; add **new DMD art** sampled from that game’s random/event DMD art.

1. **Super Pinball (Behind the Mask)**  
   - Theme: black/white (title-screen shades).  
   - Board text: **BEHIND THE MASK** above **STAFF**.  
   - Music: SP/BTM credits.

2. Fade to **Super Pinball II**  
   - Theme: blues (SP2 title font colors).  
   - Text: **THE AMAZING ODYSSEY** (fade from BEHIND THE MASK).  
   - Music: SP2 credits.

3. Transition to **Last Gladiators** original Victors theming  
   - Text: **LAST GLADIATORS**.  
   - Music: LG credits.

4. Transition to **Necronomicon**  
   - Text: **NECRONOMICON**.  
   - Theme: red / silver / black (title + box art).  
   - Music: Necronomicon credits.

5. New credit: **Viber Coder: Ryan Fisher**  
   - Credits dog portrait in Victors amber DMD style (same BG/off-LED palette as shell DMD).  
   - Assets: `media/ultimate-victors-dog-credits.png` + `-crop.png` → `assets/credits/`.  
   - Play DMD `ultimate-victors-dog-dmd.png` (128×64) matches credits amber portrait.

6. Outro thanks to original team stylized **KAZe**.

7. Thank the players → fade out of table → **initials entry** for Ultimate Victors.  
   - Save: score for this extended Ultimate Victors run **and** clear time for Ultimate Conquest.  
   - Unlock Ultimate Victors as alternate Victors menu entry (Right while hovering Victors).

## Options — universal physics packs

Players may apply **any** game’s physics pack to **any** table (QoL/fun; not expected to be balanced).

Catalog: `/cursor/stores/self/docs/physics-profiles.md` + `.json`  
Shell IDs: `btm` · `sp2` · `lg-usa` · `lg-97` · `necro`

| Family | Default pack |
|---|---|
| Behind the Mask (`btm`) | `btm` |
| Super Pinball II (`sp2`) | `sp2` |
| Last Gladiators (`lg`) | **`lg-97`** (USA/`lg-usa` remains selectable) |
| Necronomicon (`necro`) | `necro` |

- **Global override** and **per-table override** live in Options (`physics_pack_global`, `physics_pack_by_table`).
- BTM **region/rev** ROM profile (`btm_region`) is a separate control from pack swap.
- `multiball_collision` (default ON), `hd_art`, and `portrait_resolution` remain independent hooks.
- Packs may be stubs backed by decomp harnesses until portable packs are fully wired.

## Options — Portrait resolution

Universal display toggle (SNES + Saturn/Necro + Ultimate Victors play):

| Setting | Behavior |
|---|---|
| **OFF** (default) | Original shell layout — landscape-friendly stage, unchanged. |
| **ON** | Full **portrait** resolution for a more natural pinball view. |

When portrait is ON:

- Playfield fills the portrait plane (free-play, Conquest tables, Victors play).
- Score, messages, DMD, and conquest/event chrome sit in a **top-of-screen HUD band** — they must **not** cover the playfield.
- Persist with other Options (`portrait_resolution` in `kaze.compilation.options.v1`).

## Implementation notes

- Prefer building atop existing `/home/ubuntu/kaze-decomp` harness/assets where present; otherwise scaffold `games/compilation/` (or similar) with a runnable frontend prototype.
- Inventory exact table lists + Conquest/Realms order from ROMs/ISOs/docs already in the workspace; document the ordered playlist in the Project store.
- Prototype may stub FMVs/audio with placeholders if codecs aren’t ready, but wire the full flow and name every asset slot.
- Write user-facing progress to `/cursor/stores/self/docs/compilation-progress.md`.
- Put playlist/order research in `/cursor/stores/self/docs/ultimate-conquest-playlist.md`.
- Save generated DMD dog art under `/cursor/stores/self/media/ultimate-victors-dog-dmd.*` and verify files exist.

## Out of scope for first milestone

- Full bit-perfect physics ports of every table inside the shell (reuse stubs/harnesses where useful).
- Shipping a packaged installer.
