#!/usr/bin/env python3
"""Scambia due indici di palette in una BMP indicizzata."""
import argparse
from PIL import Image


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("input", help="BMP di input (indicizzata)")
    ap.add_argument("output", help="BMP di output")
    ap.add_argument("-a", type=int, default=0, help="primo indice (default 0)")
    ap.add_argument("-b", type=int, default=7, help="secondo indice (default 6)")
    ap.add_argument("--mode", choices=["palette", "pixels", "both"],
                    default="palette")
    args = ap.parse_args()

    im = Image.open(args.input)
    if im.mode != "P":
        raise SystemExit(f"L'immagine non è indicizzata (modo: {im.mode})")

    a, b = args.a, args.b
    if not (0 <= a < 256 and 0 <= b < 256):
        raise SystemExit("Gli indici devono essere tra 0 e 255")

    # Palette piatta [r,g,b,r,g,b,...], la porto a 256 voci
    pal = list(im.getpalette() or [])
    pal += [0] * (768 - len(pal))

    if args.mode in ("pixels", "both"):
        lut = list(range(256))
        lut[a], lut[b] = b, a
        im = im.point(lut)

    if args.mode in ("palette", "both"):
        pal[a*3:a*3+3], pal[b*3:b*3+3] = pal[b*3:b*3+3], pal[a*3:a*3+3]

    im.putpalette(pal)
    im.save(args.output, format="BMP")
    print(f"Fatto: indici {a} <-> {b} ({args.mode}) -> {args.output}")


if __name__ == "__main__":
    main()
