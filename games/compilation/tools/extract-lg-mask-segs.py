#!/usr/bin/env python3
"""Extract Victors MASK poly + 6×u16 segs from a mounted LG ISO filesystem.

Expects VICTOR.RES (or path via --res). Without dumps this tool exits non-zero
and writes nothing — it does not invent geometry.

Layout (USA Victors o1 @ 0xc2c70):
  pads · {0x0101,n,(x,y,z)×n}… · FFFF · meta[8] (meta[6]=ncol) · ncol×u16
  then BE 6×u16 segs @ 0xc31ec → 653 nonzero.
"""
from __future__ import annotations

import argparse
import json
import struct
import sys
from pathlib import Path


def be16(b: bytes, off: int) -> int:
    return struct.unpack_from(">H", b, off)[0]


def poly_consume_to_segs(data: bytes, o1: int) -> tuple[bytes, dict, int]:
    off = o1
    # skip pads
    while off + 2 <= len(data) and be16(data, off) == 0:
        off += 2
    contours = []
    verts = 0
    while off + 4 <= len(data):
        tag = be16(data, off)
        if tag == 0xFFFF:
            off += 2
            break
        if tag != 0x0101:
            break
        count = be16(data, off + 2)
        off += 4
        contours.append(count)
        verts += count
        off += count * 6
    meta = [be16(data, off + i * 2) for i in range(8)] if off + 16 <= len(data) else []
    off += 16
    ncol = meta[6] if len(meta) > 6 else 0
    off += ncol * 2
    poly = data[o1:off]
    return poly, {"contours": contours, "nverts": verts, "npalette": ncol, "meta": meta}, off


def find_toc_u32(data: bytes, tag: bytes) -> int | None:
    """Find 4-byte ASCII tag + following BE u32 (low 24 bits = file offset)."""
    i = data.find(tag)
    if i < 0 or i + 8 > len(data):
        return None
    return struct.unpack_from(">I", data, i + 4)[0] & 0xFFFFFF


def extract_segs(data: bytes, seg_off: int, end_off: int | None) -> list[tuple[int, ...]]:
    segs = []
    off = seg_off
    limit = end_off if end_off is not None else len(data)
    # Keep full 12-byte records only; ignore 1–11B trailer before next chunk.
    while off + 12 <= limit:
        words = struct.unpack_from(">6H", data, off)
        off += 12
        if all(w == 0 for w in words):
            continue
        segs.append(words)
        if len(segs) >= 1024:
            break
    return segs


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--res", type=Path, help="Path to VICTOR.RES")
    ap.add_argument("--fs", type=Path, default=Path("/tmp/lg-iso/fs"),
                    help="Mounted ISO fs root (default /tmp/lg-iso/fs)")
    ap.add_argument("--out", type=Path, default=Path(__file__).resolve().parents[1] / "assets" / "physics")
    ap.add_argument("--o1", type=lambda s: int(s, 0), default=0xC2C70)
    ap.add_argument("--seg", type=lambda s: int(s, 0), default=0xC31EC)
    args = ap.parse_args()

    res = args.res
    if res is None:
        candidates = [
            args.fs / "VICTOR.RES",
            args.fs / "victor.res",
            args.fs / "RES" / "VICTOR.RES",
        ]
        res = next((p for p in candidates if p.is_file()), None)

    if res is None or not res.is_file():
        print("VICTOR.RES not found — mount LG USA ISO under --fs or pass --res", file=sys.stderr)
        print("No geometry invented; exiting.", file=sys.stderr)
        return 2

    data = res.read_bytes()
    toc_mask = find_toc_u32(data, b"MASK")
    toc_back = find_toc_u32(data, b"BACK")
    o1 = toc_mask if toc_mask is not None else args.o1
    poly, meta, seg_off_actual = poly_consume_to_segs(data, o1)
    # Prefer documented seg offset when it matches consume end
    seg_off = args.seg if abs(seg_off_actual - args.seg) < 0x40 else seg_off_actual
    end_off = toc_back  # MASK body runs until BACK chunk
    segs = extract_segs(data, seg_off, end_off)

    args.out.mkdir(parents=True, exist_ok=True)
    (args.out / "victor_MASK_poly.bin").write_bytes(poly)
    (args.out / "victor_MASK_poly.json").write_text(json.dumps({
        "bytes": len(poly), **meta, "o1": hex(o1)
    }, indent=2) + "\n")
    seg_bin = b"".join(struct.pack(">6H", *s) for s in segs)
    (args.out / "victor_MASK_segs.bin").write_bytes(seg_bin)
    (args.out / "victor_MASK_segs.json").write_text(json.dumps({
        "count": len(segs), "seg_off": hex(seg_off),
        "end_off": hex(end_off) if end_off is not None else None,
        "note": "TOC-bounded MASK..BACK; prior notes said 653 — dump yields 659"
    }, indent=2) + "\n")
    (args.out / "victor_MASK_extract.json").write_text(json.dumps({
        "method": "poly_consume_to_segs+TOC_MASK_BACK",
        "res": str(res),
        "poly_bytes": len(poly),
        "nsegs": len(segs),
        "seg_off": hex(seg_off),
        "end_off": hex(end_off) if end_off is not None else None,
        "toc_mask": hex(toc_mask) if toc_mask is not None else None,
        "toc_back": hex(toc_back) if toc_back is not None else None,
    }, indent=2) + "\n")
    print(f"wrote poly {len(poly)}B, segs {len(segs)} → {args.out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
