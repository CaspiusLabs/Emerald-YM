#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""A video of the simulated FM-1: the host simulator's film (amb_host's `film`) and its WAV, drawn as the
FM-1's panel (the web emulator's drawing, web/emu/index.html) with the screen, the lights and the buttons and
keys as they are pressed, encoded with ffmpeg.

  tools/film.py FILM WAV OUT.mp4 [--layout split|panel|screen] [--fps 30]

Layouts: split (1920x1080: the panel on the left, the screen large on the right), panel (1920x1080: the
panel alone), screen (1080x1080: the screen alone, 4x). Needs Pillow, numpy and ffmpeg."""
import argparse
import os
import struct
import subprocess
import sys

import numpy as np
from PIL import Image, ImageDraw, ImageFont

NE = 7
HDR = 6 + NE
FRAME = HDR * 4 + 240 * 240 * 2

BTN = {"FX": 0, "SEL": 1, "ENV": 2, "LFO": 3, "EDIT": 4, "GLO": 5, "HOME": 6, "SAVE": 7, "ARP": 8, "SEQ": 9,
       "PLAY": 10, "REC": 11, "OCT-": 12, "OCT+": 13}
ENC = {"SELECT": 0, "ALGORITHM": 1, "PRESETS": 2, "KNOB1": 3, "KNOB2": 4, "KNOB3": 5, "KNOB4": 6}
WHITE_K = [0, 2, 4, 6, 7, 9, 11, 12, 14, 16, 18, 19, 21, 23, 24, 26]
BLACK_K = [1, 3, 5, 8, 10, 13, 15, 17, 20, 22, 25]
BLACK_AFTER = [0, 1, 2, 4, 5, 7, 8, 9, 11, 12, 14]
BODY, TRAY, KEY_W, KEY_B, KNOB, BTNC, LABEL = "#3a3d42", "#26282c", "#d9dadc", "#c9cacd", "#1f2124", "#2e3135", "#e8e8ea"
LIT_BTN, LIT_TXT, LIT_KW, LIT_KB, HELD = "#f28a5a", "#2a140a", "#f6b48e", "#4cc3b6", "#f28a5a"
GROUND = (13, 16, 20)
FONT = os.path.join(os.path.dirname(__file__), "..", "assets", "fonts", "BarlowSemiCondensed-SemiBold.ttf")
SS = 2                                         # supersampling for the static drawing and the sprites


def font(px):
    return ImageFont.truetype(FONT, max(6, int(round(px))))


class Panel:
    """The panel in panel units (1000 x 640), drawn at s pixels a unit."""

    def __init__(self, s):
        self.s = s
        self.knobs = []                        # (x, y, r, role)
        self.buttons = []                      # (x, y, w, h, label, two)
        self.keys = {}                         # key index -> (x, y, w, h, white)

    def layout(self):
        def knob(x, y, r, label, role):
            self.knobs.append((x, y, r, label, role))
        knob(105, 100, 24, "MASTER", "master")
        knob(215, 100, 24, "SELECT", ENC["SELECT"])
        knob(105, 205, 24, "PRESETS", ENC["PRESETS"])
        knob(215, 205, 24, "ALGORITHM", ENC["ALGORITHM"])
        for i in range(4):
            knob(610 + i * 104, 100, 22, f"KNOB{i + 1}", ENC[f"KNOB{i + 1}"])
        self.buttons += [(72, 268, 82, 38, "OCT-", None), (170, 268, 82, 38, "OCT+", None)]
        row1, row2 = ["FX", "SEL", "ENV", "LFO", "EDIT", "GLO"], ["HOME", "SAVE", "ARP", "SEQ", "PLAY", "REC"]
        for i in range(6):
            self.buttons.append((582 + i * 63, 174, 54, 48, row1[i], None))
            self.buttons.append((582 + i * 63, 236, 54, 48, row2[i], "STOP" if row2[i] == "PLAY" else None))
        x0, y0, w, h = 36, 340, 928, 270
        pitch = (w - 24) / 16
        kw = pitch * 0.78
        for i in range(16):
            self.keys[WHITE_K[i]] = (x0 + 12 + i * pitch + (pitch - kw) / 2, y0 + h * 0.43, kw, h * 0.52, True)
        for k, a in enumerate(BLACK_AFTER):
            self.keys[BLACK_K[k]] = (x0 + 12 + (a + 1) * pitch - kw / 2, y0 + h * 0.06, kw, h * 0.33, False)

    def base(self):
        s = self.s * SS
        im = Image.new("RGB", (int(1000 * s), int(640 * s)), GROUND)
        d = ImageDraw.Draw(im)
        R = lambda x, y, w, h, r, **k: d.rounded_rectangle((x * s, y * s, (x + w) * s, (y + h) * s), r * s, **k)
        R(4, 4, 992, 632, 44, fill=BODY)
        R(58, 258, 210, 58, 14, fill=TRAY)
        R(300, 44, 236, 236, 26, fill="#111214")
        R(568, 160, 400, 140, 18, fill=TRAY)
        R(36, 340, 928, 270, 18, fill=TRAY)
        for x, y, r, label, _ in self.knobs:
            d.ellipse(((x - r - 5) * s, (y - r - 5) * s, (x + r + 5) * s, (y + r + 5) * s), fill=TRAY)
            d.ellipse(((x - r) * s, (y - r) * s, (x + r) * s, (y + r) * s), fill=KNOB, outline="#44474c", width=int(2 * s))
            d.text((x * s, (y - r - 12) * s), label, font=font(17 * s), fill=LABEL, anchor="ms")
        im = im.resize((int(1000 * self.s), int(640 * self.s)), Image.LANCZOS)
        for b in self.buttons:
            self.paste(im, *self.button_sprite(b, False, False))
        for k in self.keys:
            self.paste(im, *self.key_sprite(k, False, False))
        return im

    @staticmethod
    def paste(im, sp, xy):
        im.paste(sp, xy, sp)

    def sprite(self, x, y, w, h, draw):
        """A control drawn on its own, supersampled: (RGBA image, top-left on the panel image)."""
        s, pad = self.s, 4
        X, Y = int(x * s) - pad, int(y * s) - pad
        W, H = int((x + w) * s) + pad - X + 1, int((y + h) * s) + pad - Y + 1
        im = Image.new("RGBA", (W * SS, H * SS), (0, 0, 0, 0))
        draw(ImageDraw.Draw(im), lambda u, v: ((u * s - X) * SS, (v * s - Y) * SS), s * SS)
        return im.resize((W, H), Image.LANCZOS), (X, Y)

    def button_sprite(self, b, lit, held):
        x, y, w, h, label, two = b

        def draw(d, P, s):
            stroke, sw = (HELD, 4) if held else ("#4a4d52", 1.5)
            d.rounded_rectangle((*P(x, y), *P(x + w, y + h)), 9 * s, fill=LIT_BTN if lit else BTNC,
                                outline=stroke, width=max(1, int(sw * s)))
            c = LIT_TXT if lit else LABEL
            if two:
                d.text(P(x + w / 2, y + h / 2 - 1), label, font=font(12 * s), fill=c, anchor="ms")
                d.text(P(x + w / 2, y + h / 2 + 12), two, font=font(12 * s), fill=c, anchor="ms")
            else:
                d.text(P(x + w / 2, y + h / 2 + 5), label, font=font(15 * s), fill=c, anchor="ms")
        return self.sprite(x, y, w, h, draw)

    def key_sprite(self, k, lit, held):
        x, y, w, h, white = self.keys[k]

        def draw(d, P, s):
            fill = (LIT_KW if white else LIT_KB) if lit else (KEY_W if white else KEY_B)
            if held:                           # pressed: the outline, and a little darker
                fill = mix(fill, "#000000", 0.18)
            d.rounded_rectangle((*P(x, y), *P(x + w, y + h)), w / 2 * s, fill=fill,
                                outline=HELD if held else None, width=int(4 * s) if held else 0)
        return self.sprite(x, y, w, h, draw)


def mix(a, b, t):
    a, b = [tuple(int(c[i:i + 2], 16) for i in (1, 3, 5)) for c in (a, b)]
    return "#%02x%02x%02x" % tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(3))


def screen_rgb(raw):
    v = np.frombuffer(raw, dtype=">u2").reshape(240, 240).astype(np.uint32)
    rgb = np.stack([(v >> 11 & 31) * 255 // 31, (v >> 5 & 63) * 255 // 63, (v & 31) * 255 // 31], -1)
    return Image.fromarray(rgb.astype(np.uint8))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("film")
    ap.add_argument("wav")
    ap.add_argument("out")
    ap.add_argument("--layout", default="split", choices=["split", "panel", "screen"])
    ap.add_argument("--fps", type=int, default=30)
    a = ap.parse_args()

    if a.layout == "screen":
        W, H, ps, scr, off = 1080, 1080, None, (60, 60, 960), None
    elif a.layout == "panel":
        W, H, ps = 1920, 1080, 1.8
        off = ((W - int(1000 * ps)) // 2, (H - int(640 * ps)) // 2)
        scr = (off[0] + int(318 * ps), off[1] + int(62 * ps), int(200 * ps))
    else:
        W, H, ps = 1920, 1080, 0.85
        off = (30, (H - int(640 * ps)) // 2)
        scr = (W - 960 - 40, 60, 960)          # 4x, on the right
    canvas = Image.new("RGB", (W, H), GROUND)
    if ps:
        P = Panel(ps)
        P.layout()
        base = P.base()
        canvas.paste(base, off)
        bsprites, ksprites = {}, {}
    ff = subprocess.Popen(["ffmpeg", "-loglevel", "error", "-y", "-f", "rawvideo", "-pix_fmt", "rgb24",
                           "-s", f"{W}x{H}", "-r", str(a.fps), "-i", "-", "-i", a.wav,
                           "-c:v", "libx264", "-preset", "medium", "-crf", "14", "-pix_fmt", "yuv420p",
                           "-c:a", "aac", "-b:a", "320k", "-shortest", "-movflags", "+faststart", a.out],
                          stdin=subprocess.PIPE)
    angle = {e: 0.0 for e in range(NE)}
    n = 0
    with open(a.film, "rb") as f:
        while True:
            rec = f.read(FRAME)
            if len(rec) < FRAME:
                break
            h = struct.unpack(f"<{HDR}I", rec[:HDR * 4])
            _, held_b, held_k, lit_b, lit_k, master = h[:6]
            turns = struct.unpack(f"<{NE}i", rec[24:HDR * 4])
            for e in range(NE):
                angle[e] += 18.0 * turns[e]
            scrn = screen_rgb(rec[HDR * 4:])
            im = canvas.copy()
            if ps:
                for b in P.buttons:
                    bit = BTN[b[4]]
                    st = (bool(lit_b >> bit & 1), bool(held_b >> bit & 1))
                    if st != (False, False):
                        key = (b[4], st)
                        if key not in bsprites:
                            bsprites[key] = P.button_sprite(b, *st)
                        sp, (x, y) = bsprites[key]
                        im.paste(sp, (x + off[0], y + off[1]), sp)
                for k in P.keys:
                    st = (bool(lit_k >> k & 1), bool(held_k >> k & 1))
                    if st != (False, False):
                        key = (k, st)
                        if key not in ksprites:
                            ksprites[key] = P.key_sprite(k, *st)
                        sp, (x, y) = ksprites[key]
                        im.paste(sp, (x + off[0], y + off[1]), sp)
                d = ImageDraw.Draw(im)
                for x, y, r, label, role in P.knobs:
                    ang = -135 + 270 * master / 4096 if role == "master" else angle[role]
                    t = np.radians(ang)
                    cx, cy = off[0] + x * ps, off[1] + y * ps
                    p0 = (cx + np.sin(t) * (r - 5) * ps, cy - np.cos(t) * (r - 5) * ps)
                    p1 = (cx + np.sin(t) * (r - 15) * ps, cy - np.cos(t) * (r - 15) * ps)
                    wd = max(2, int(3 * ps))
                    d.line((p0, p1), fill=LABEL, width=wd)
                    for q in (p0, p1):
                        d.ellipse((q[0] - wd / 2, q[1] - wd / 2, q[0] + wd / 2, q[1] + wd / 2), fill=LABEL)
                if a.layout == "split":        # the panel's own screen, small
                    sz = int(200 * ps)
                    im.paste(scrn.resize((sz, sz), Image.LANCZOS), (off[0] + int(318 * ps), off[1] + int(62 * ps)))
            x, y, sz = scr
            im.paste(scrn.resize((sz, sz), Image.NEAREST if sz % 240 == 0 else Image.LANCZOS), (x, y))
            ff.stdin.write(im.tobytes())
            n += 1
    ff.stdin.close()
    ff.wait()
    print(f"film: {a.out}  {n} frames, {n / a.fps:.1f} s, {W}x{H}")
    return ff.returncode


if __name__ == "__main__":
    sys.exit(main())
