# DUB-1 for the M-VAVE FM-1

DUB-1 (a working name) turns the M-VAVE FM-1 into a generative dub techno machine. It plays itself
from the moment it's switched on: chord stabs through a resonant low-pass into a tape echo, short
FM pings on loops of unequal length, a sub bass on one of five grooves, a four-on-the-floor kick
that ducks the rest, a shaker and an open hat, and a bed of vinyl crackle. An eight-bar arrangement
takes the kick out for a break, thins the layers and brings them back with a fill, and now and then
a stab is thrown into the echo. You don't play it, you steer it: pick a piece and a scale, take
notes out, open the filter, push the feedback, hold the echo.

DUB-1 is a fork of [AMB-1](https://github.com/charlesvestal/fm1-amb), Charles Vestal's generative
ambient firmware for the FM-1, and keeps its platform, its panel and much of its sound engine.

![DUB-1's main screen: the piece Basic, F Dorian, 122; FULL top left, the stabs' ripples round the chord Fm7, the lanes below](docs/img/screen-main.png) ![PERFORM, THROW held: every stab a double ring](docs/img/screen-perform.png) ![Dub hold: the rings and "hold" in red](docs/img/screen-hold.png)

- **Read the manual**: [docs/MANUAL.md](docs/MANUAL.md)
- **Try it, install it:** a hosted emulator, web installer and firmware download aren't published
  for DUB-1 yet. For now, build it from this source and install it from the command line (below).
  To hear and see it on a computer without an FM-1, see [EMULATOR.md](EMULATOR.md).

Installing it is at your own risk, though it's hard to get stuck: M-VAVE's own updater can put the
stock firmware back, and an FM-1 that fails to start twice comes up in a safe mode that the
installer can reach.

## In short

| | |
|---|---|
| Keys | The scale: each key adds its note or takes it out |
| PLAY · REC · EDIT | Play / rest · dub hold · the next chord, on the next bar |
| SEL | PERFORM: the knobs a mixer; the keys mute the layers and, held, throw, open, shut, drop the kick, push the echo |
| HOME · ENV · ARP · LFO · SEQ · FX · GLO | The pages: Dub, Chord, Texture, Bass, Drums, Space, Setup; KNOB 1–4 set their four values |
| SELECT | The previous or next page; with HOME held, the tempo |
| ALGORITHM | On a layer's page, its voice (7 chord, 5 ping, 4 bass voices, 5 kits); elsewhere, the mode |
| PRESETS | The piece (eight of them) |
| OCT− / OCT+ | The pings' octave |
| SAVE | Saves; held, puts the piece back to its recipe |

Hold any button (but PLAY) for a second and a card says what it does.

DUB-1 also tells the [FM-1 Screen Reader](https://charlesvestal.github.io/fm1-reader/) what its
screen shows, over USB: the page and its knobs, a knob's value as it turns, the piece, the chord as
it moves, and what each button did.

The [manual](docs/MANUAL.md) has the rest. [PLAN.md](PLAN.md) has the design, what's done and
what's still to do.

## Building and testing

The toolchain is the same as X0X's (see [BUILDING.md](BUILDING.md)):

```
tests/run_tests.sh      # the instrument, the platform pieces, the app in a simulator, the web builds
./build.sh              # build/amb.fwsc
python3 tools/fm1_install.py build/amb.fwsc     # install it over USB
```

`build/host/amb_host SCRIPT OUTDIR` runs the whole app on a computer from a script
(`tests/scenarios/*.amb`), with audio and screenshots coming out. `web/emu/build.sh` builds the
same code for the browser (the emulator); `web/app/build.sh` builds the engine alone into a web
app for phone and desktop. [EMULATOR.md](EMULATOR.md) says how to set these up on a Mac and how to
listen to a change.

## Credits

DUB-1 is a fork of [AMB-1](https://github.com/charlesvestal/fm1-amb) by Charles Vestal
(GPL-3.0-only): the engine's loops, echo chain, plate and pieces system, the UI and the platform
all come from there. The idea comes from AMB-1 and from dub techno in general: chords dissolving
in a tape echo over a steady kick. The reverb is Jon Dattorro's plate ("Effect Design, Part 1",
JAES 1997), as in [FoMni-1](https://github.com/charlesvestal/fm1-fomni); the granular cloud follows the
design of CLOUD in [JIANT](https://github.com/juanjiant-bit/JIANT) (GPL-3.0); the Euclidean rhythms
are Godfried Toussaint's reading of Bjorklund's algorithm. The platform (hardware layer, USB,
update loader, storage, installer) is [Felucca](https://github.com/hugelton/Felucca)'s by Leo
Kuroshita (Hügelton Instruments), by way of [X0X](https://github.com/charlesvestal/fm1-x0x),
FoMni-1 and AMB-1. The idea of running FM-1 firmware in the browser comes from
[Groove OS](https://www.groove-os.com/). GPL-3.0-only; see [LICENSING.md](LICENSING.md).

M-VAVE and FM-1 are trademarks of their owners. DUB-1 is not affiliated with M-VAVE or Hügelton
Instruments.
