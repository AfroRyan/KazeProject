#!/usr/bin/env python3
"""Extract LG Victors SOND ring PCM → WAV/OGG + sond_catalog.json (needs ISO).

rate_hz = 44100 * (FNS+1024)/1024 * 2^OCT  (DP-27)
Multi-voice gate programs (DP-28) annotated when stream present.
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


def signed4(n: int) -> int:
    n &= 15
    return n - 16 if n >= 8 else n


def rate_hz(oct: int, fns: int) -> float:
    return 44100.0 * (fns + 1024) / 1024.0 * (2.0 ** oct)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--res", type=Path)
    ap.add_argument("--fs", type=Path, default=Path("/tmp/lg-iso/fs"))
    ap.add_argument("--out", type=Path,
                    default=Path(__file__).resolve().parents[1] / "assets" / "physics" / "sfx")
    args = ap.parse_args()

    res = args.res
    if res is None:
        for p in (args.fs / "VICTOR.RES", args.fs / "victor.res"):
            if p.is_file():
                res = p
                break
    if res is None or not res.is_file():
        print("VICTOR.RES not found — cannot extract SOND (no invented PCM).", file=sys.stderr)
        return 2

    # Primary ring rates from documented pitch blocks (DP-27) — catalog stub
    # when full stream decode is unavailable in this restore.
    primary = [
        {"r4": 1, "pitch": "0xe048", "oct": -4, "fns": 72, "rate_hz": round(rate_hz(-4, 72))},
        {"r4": 2, "pitch": "0xe044", "oct": -4, "fns": 68, "rate_hz": round(rate_hz(-4, 68))},
        {"r4": 3, "pitch": "0xe04e", "oct": -4, "fns": 78, "rate_hz": round(rate_hz(-4, 78))},
    ]
    args.out.mkdir(parents=True, exist_ok=True)
    catalog = {
        "note": "Rates from dump-backed pitch blocks; PCM ogg requires full SOND decode from RES",
        "primary": primary,
        "multivoice": {
            "1": {"n_voices": 3, "rates_hz": [2950, 2950, 2950]},
            "2": {"n_voices": 3, "rates_hz": [2939, 2966, 2966]},
            "3": {"n_voices": 4, "rates_hz": [2966, 2937, 2937, 2937]},
        },
        "r5": {"case8": 5, "type5_10": 1, "gain_scale": "r5/5"},
        "res": str(res),
        "pcm_extracted": False,
    }
    (args.out / "sond_catalog.json").write_text(json.dumps(catalog, indent=2) + "\n")
    print(f"wrote catalog (PCM still needs stream decode) → {args.out}")
    print("primary rates:", [p["rate_hz"] for p in primary])
    return 0


if __name__ == "__main__":
    sys.exit(main())
