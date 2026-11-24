#!/usr/bin/env python3
"""Extract LG Victors SOND ring PCM → WAV/OGG + sond_catalog.json (needs ISO).

rate_hz = 44100 * (FNS+1024)/1024 * 2^OCT  (DP-27)
Multi-voice gate programs (DP-28) annotated when stream present.

PCM slicing is heuristic over high-variance 8-bit runs in the SOND body when
a full SCSP descriptor→wave binder is not yet wired. Rates/pitch/sid remain
dump-backed.
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
import wave
from array import array
from pathlib import Path


def signed4(n: int) -> int:
    n &= 15
    return n - 16 if n >= 8 else n


def rate_hz(oct: int, fns: int) -> float:
    return 44100.0 * (fns + 1024) / 1024.0 * (2.0 ** oct)


def find_pcm_regions(data: bytes, start: int, end: int) -> list[tuple[int, int, float]]:
    window = 2048
    candidates: list[tuple[int, int, float]] = []
    i = start
    while i + window < end:
        chunk = data[i : i + window]
        mad = sum(abs(b - 128) for b in chunk) / window
        zeros = chunk.count(0)
        if mad > 20 and zeros < window * 0.05:
            j = i + window
            while j < end and abs(data[j] - 128) > 2:
                j += 1
                if j - i > 20000:
                    break
            candidates.append((i, j - i, mad))
            i = j
        else:
            i += 256
    candidates.sort(key=lambda x: -x[1])
    return candidates


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--res", type=Path)
    ap.add_argument("--fs", type=Path, default=Path("/tmp/lg-iso/fs"))
    ap.add_argument(
        "--out",
        type=Path,
        default=Path(__file__).resolve().parents[1] / "assets" / "physics" / "sfx",
    )
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

    data = res.read_bytes()
    primary_meta = [
        {"r4": 1, "sid": "0x4a86", "pitch": "0xe048", "oct": -4, "fns": 72},
        {"r4": 2, "sid": "0x6b1c", "pitch": "0xe044", "oct": -4, "fns": 68},
        {"r4": 3, "sid": "0xe936", "pitch": "0xe04e", "oct": -4, "fns": 78},
    ]
    for p in primary_meta:
        p["rate_hz"] = round(rate_hz(p["oct"], p["fns"]))

    args.out.mkdir(parents=True, exist_ok=True)
    regions = find_pcm_regions(data, 0x575D8, 0xC0884)
    picked: list[tuple[int, int, float]] = []
    for c in regions:
        if c[1] < 1500:
            break
        if any(abs(c[0] - p[0]) < 500 for p in picked):
            continue
        picked.append(c)
        if len(picked) >= 3:
            break

    pcm_ok = False
    for idx, p in enumerate(primary_meta):
        if idx >= len(picked):
            continue
        a, length, _ = picked[idx]
        pcm = data[a : a + min(length, 12000)]
        samples = array("h", ((b - 128) * 256 for b in pcm))
        wav_path = args.out / f"lg_ring_r4_{p['r4']}.wav"
        with wave.open(str(wav_path), "w") as w:
            w.setnchannels(1)
            w.setsampwidth(2)
            w.setframerate(p["rate_hz"])
            w.writeframes(samples.tobytes())
        p["pcm_off"] = hex(a)
        p["pcm_bytes"] = len(pcm)
        p["wav"] = wav_path.name
        ogg_path = args.out / f"lg_ring_r4_{p['r4']}.ogg"
        r = subprocess.run(
            ["ffmpeg", "-y", "-i", str(wav_path), "-c:a", "libvorbis", "-q:a", "4", str(ogg_path)],
            capture_output=True,
        )
        if r.returncode == 0:
            p["ogg"] = ogg_path.name
            pcm_ok = True

    catalog = {
        "note": "Rates from dump-backed pitch blocks; PCM heuristically sliced from SOND body",
        "primary": primary_meta,
        "multivoice": {
            "1": {"n_voices": 3, "rates_hz": [2950, 2950, 2950]},
            "2": {"n_voices": 3, "rates_hz": [2939, 2966, 2966]},
            "3": {"n_voices": 4, "rates_hz": [2966, 2937, 2937, 2937]},
        },
        "r5": {"case8": 5, "type5_10": 1, "gain_scale": "r5/5"},
        "res": str(res),
        "pcm_extracted": pcm_ok,
        "pcm_method": "high-variance-8bit-runs",
    }
    (args.out / "sond_catalog.json").write_text(json.dumps(catalog, indent=2) + "\n")
    print(f"wrote catalog (pcm_extracted={pcm_ok}) → {args.out}")
    print("primary rates:", [p["rate_hz"] for p in primary_meta])
    return 0


if __name__ == "__main__":
    sys.exit(main())
