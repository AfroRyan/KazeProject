# SNES collide probe → mode/UI/render

Branch: `cursor/snes-collide-probe-eda5`  
Workspace: `/home/ubuntu/kaze-decomp/`

Physics evidence closed earlier. Current focus: **dump-free mode/UI/render**.

## Mode FSM `$0275`

Modes 0..2. Select `$AA03`, cycle `$A3D3`, install `$A388`→`$025A=$A3EE[{2,0,4}]`.
Also drives soft `$CD74`, score `$CF70`, map `$B0E5`, table UI `$F0xx`.

## Scoring + HUD digits

`$CEC0` pack add → `$DA10` ASCII (`$1AB2`→`$0230`).
`$E3D4`: `$0230 |= $3030` → nibble pack → `$B96E`→`$1A38` digit bank
(not OAM). SP2 digit-add twin `$C2ED`→`$1932`; no `ORA #$3030` twin.

## Ball OAM + `$A2DA` projection

`$19E4/$19E8` → `$AC8C` temps → **`$A2DA` screen** → `$ACA0` → `$0300` →
NMI DMA `$E624`. Portable: `kaze_btm_ball_project_screen` + `$8359` div.
SP2 `$836C` = div twin only (no camera project).

Notes: `games/behind-the-mask/notes/mode-ui-render.md`.

## Prior (still green)

Frame-tick (flipper/`$B00E`/soft), tilt `$0200←$30`, `$1B8C` extra-ball only.
`make snes` PASS.

## Scores

BTM **8.5** · SP2 **8.2**

## Next milestone

`$ACA0` sprite pack → `$0300` OAM shadow (hi bits `$051E`), or `$DA70`/`$1A38`
display path beyond digit add; SP2 ball-draw WRAM `$1870`→sprite if twins clear.
