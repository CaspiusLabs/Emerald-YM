"""Project production UAC DSP/descriptors into a desktop-only translation unit.
No HAL/USB registers are executed. Hardware scheduling is outside this test.
"""
from pathlib import Path
s=Path('firmware/src/usb.c').read_text()
state=s[s.index('/* USB audio input.'):s.index('\n#endif',s.index('/* USB audio input.'))]
desc=s[s.index('/* Name the audio function explicitly'):s.index('static int get_desc')]
core=s[s.index('static __attribute__((noinline)) void uac_render_start'):s.index('/* TIMER5, 2 kHz, also nested')]
header='#define __attribute__(x)\n#define FELUCCA_CDC 0\n#define EN_YM2612 1\nstatic struct { unsigned config; } usb;\n'
Path('build/host/uac').mkdir(parents=True,exist_ok=True)
Path('build/host/uac/usb.c').write_text(header+state+'\n'+desc+'\n'+core,encoding='utf-8')
