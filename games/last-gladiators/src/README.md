# Last Gladiators — portable physics

Reconstructed from Agent Store decomp contracts (passes 8–60, DP-18…DP-30).

- `physics.h` / `physics.c` — integrate, bounce cases, MASK load/probe/poly,
  DP-30 `phys_mask_contact_flags_from_cmd`, shell flipper pivots.
- Harness: `../harness` (`make run`).

Default pack for LG tables in the compilation shell: **`lg-97`**.
