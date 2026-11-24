---
cursor:
  subagentId: "bc-80f5490c-b461-5cbd-81a5-28ca11530dcc"
---

# DP-25 — Victors MASK bounce type map

## Ask
Improve MASK word4/word5 → bounce-type mapping (was heuristic hi-nibble). Prefer
dump-backed evidence from VICTOR.RES / A.SH2. Dog assets locked.

## A.SH2 evidence (USA, load base `0x06010000`)

| VA | Role |
|---|---|
| `0x06012CF4` | Collide-response entry |
| `0x06012d16` | `mov.w @(8,r3),r0` — contact u16 from `*(obj+88)+8` |
| `0x06012e02..16` | `type = (u16 >> 8) & 15` then `bra 0x06013520` |
| `0x06013520` | Switch: 2/3/5/6/7/8/10 |
| `0x060133d8` | Type **2** loads `e=0x012C` → bounce tail |
| `0x06012e3a` | Type **8** loads `0xFD62` thresh + RingSfx(2,5) |
| `0x06013554` | Common bounce tail (`n<0` → `-(n*e)>>10`) |

## Host change

`phys_mask_bounce_type`: passthrough for switch set `{2,3,5,6,7,8,10}`; else
coerce to soft **2**. **Removed** DP-20 `hi==2 → type 8` (contradicts dump).

Victors RES word4 hi-nibble ∈ `{0,1,2}` → all **653** segs map to type **2**.

## Still open

RES word4 remains a stand-in for live `contact_flags`; the binder that copies
MASK trailer → `*(obj+88)+8` is not fully traced. word5 probe radius still
lo-byte ≤32 (DP-23).

## Verify

`make -C games/last-gladiators/harness run` → **RESULT PASS**
