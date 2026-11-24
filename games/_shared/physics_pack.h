/* Universal physics pack IDs for compilation Options (DP-6). */
#ifndef KAZE_PHYSICS_PACK_H
#define KAZE_PHYSICS_PACK_H

typedef enum KazePhysicsPackId {
  KAZE_PACK_BTM = 0,
  KAZE_PACK_SP2 = 1,
  KAZE_PACK_LG_USA = 2,
  KAZE_PACK_LG_97 = 3,
  KAZE_PACK_NECRO = 4
} KazePhysicsPackId;

static inline const char *kaze_physics_pack_key(KazePhysicsPackId id) {
  switch (id) {
  case KAZE_PACK_BTM: return "btm";
  case KAZE_PACK_SP2: return "sp2";
  case KAZE_PACK_LG_USA: return "lg-usa";
  case KAZE_PACK_LG_97: return "lg-97";
  case KAZE_PACK_NECRO: return "necro";
  default: return "lg-97";
  }
}

static inline KazePhysicsPackId kaze_physics_pack_by_key(const char *key) {
  if (!key) return KAZE_PACK_LG_97;
  if (key[0]=='b' && key[1]=='t') return KAZE_PACK_BTM;
  if (key[0]=='s' && key[1]=='p') return KAZE_PACK_SP2;
  if (key[0]=='n') return KAZE_PACK_NECRO;
  if (key[0]=='l' && key[3]=='u') return KAZE_PACK_LG_USA;
  return KAZE_PACK_LG_97;
}

/* Family defaults: LG → lg-97. */
static inline KazePhysicsPackId kaze_physics_pack_default_for_table(const char *family) {
  if (!family) return KAZE_PACK_LG_97;
  if (family[0]=='b') return KAZE_PACK_BTM;
  if (family[0]=='s') return KAZE_PACK_SP2;
  if (family[0]=='n') return KAZE_PACK_NECRO;
  return KAZE_PACK_LG_97;
}

#endif
