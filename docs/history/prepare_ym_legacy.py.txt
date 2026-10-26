"""Prepare the freestanding YM2612 fork. No device access."""
from pathlib import Path
import shutil, re, math
ROOT=Path(__file__).resolve().parents[1]
YM=ROOT/'firmware/src/ym'
YM.mkdir(parents=True,exist_ok=True)
source=ROOT.parent/'genesis/ymfm'
if source.exists() and not (YM/'ymfm').exists():
 shutil.copytree(source/'src',YM/'ymfm',dirs_exist_ok=True)
 shutil.copyfile(source/'LICENSE',YM/'ymfm/LICENSE')
p=YM/'ymfm/ymfm.h'
s=p.read_text()
if '#ifdef YMFM_FREESTANDING' not in s:
 start=s.index('#include <cassert>');end=s.index('\nnamespace ymfm',start)
 s=s[:start]+'#ifdef YMFM_FREESTANDING\n#include "../ym_compat.h"\n#else\n'+s[start:end]+'\n#endif\n'+s[end:]
p.write_text(s)
# Static, original percussion samples and a fixed-point tuning table.
out=['/* Original synthetic percussion and NTSC tuning; generated offline. */','#pragma once']
state=0x38f79d15
def noise():
 global state
 state^=(state<<13)&0xffffffff;state^=state>>17;state^=(state<<5)&0xffffffff;state&=0xffffffff
 return state/4294967296*2-1
rate=7670454/144/4
for name,dur in [('kick',.25),('snare',.18),('hat',.055),('open',.18)]:
 vals=[];phase=0;prev=0
 for i in range(int(dur*rate)):
  x=i/rate;n=noise();hp=n-prev;prev=n
  if name=='kick':
   phase+=2*math.pi*(48+145*math.exp(-x*45))/rate
   v=.58*math.sin(phase)*math.exp(-x*22)+.025*hp*math.exp(-x*280)
  elif name=='snare':v=.22*hp*math.exp(-x*27)+.20*math.sin(2*math.pi*190*x)*math.exp(-x*38)
  else:v=.08*hp*math.exp(-x*(23 if name=='open' else 95))
  vals.append(str(round(max(-.98,min(.98,v))*127)))
 out.append('static const int8_t ym_'+name+'[] = {'+','.join(vals)+'};')
pitch=[]
for n in range(128):
 fn=440*2**((n-69)/12)*144*2097152/7670454;block=0
 while fn>1400 and block<7:fn/=2;block+=1
 pitch.append(str((block<<11)|min(2047,round(fn))))
out.append('static const uint16_t ym_pitch[128] = {'+','.join(pitch)+'};')
(YM/'ym_tables.h').write_text('\n'.join(out)+'\n')
# Keep the original platform/OTA implementation; choose a separate app at build time.
p=ROOT/'firmware/src/main.c';s=p.read_text()
if '#ifdef EN_YM2612' not in s:
 marker='#include "app/app.h"';s=s.replace(marker,'#ifdef EN_YM2612\n#include "ym/main.c"\n#else\n'+marker)+'\n#endif /* EN_YM2612 */\n';p.write_text(s)
p=ROOT/'tools/build.py';s=p.read_text()
s=s.replace('"dsp/amb.c"','"ym/ym_engine.cpp", "ym/ymfm/ymfm_opn.cpp", "ym/ymfm/ymfm_adpcm.cpp", "ym/ymfm/ymfm_ssg.cpp"')
s=s.replace('not (Path(tc) / "pi32v2" / "bin" / "clang").exists()','not ((Path(tc) / "pi32v2" / "bin" / "clang").exists() or (Path(tc) / "pi32" / "bin" / "clang.exe").exists())')
s=s.replace('native = platform.system() == "Linux" and platform.machine() in ("x86_64", "AMD64")','native = platform.system() in ("Linux", "Windows") and platform.machine() in ("x86_64", "AMD64")')
marker='    if use_docker():\n        cmd = ["docker", "exec"'
if 'windows_tools =' not in s:
 s=s.replace(marker,'    if platform.system() == "Windows":\n        windows_tools = {"pi32v2/bin/clang": "pi32/bin/clang.exe", "pi32v2/bin/ld": "pi32/bin/pi32v2-ld.exe", "common/bin/objcopy": "pi32/bin/llvm-objcopy.exe", "common/bin/objdump": "pi32/bin/llvm-objdump.exe"}\n        tool = windows_tools.get(tool, tool)\n'+marker)
s=s.replace('flags = [*CFLAGS, *FPU, "-Ifirmware/hal"','flags = [*CFLAGS, *FPU, "-DEN_YM2612=1", "-DNDEBUG", "-DYMFM_FREESTANDING=1", "-Ifirmware/hal"')
s=s.replace('o = OUT / "o2" / (u.replace("/", "_")[:-2] + ".o")','o = OUT / "o2" / (u.replace("/", "_").rsplit(".",1)[0] + ".o")')
s=s.replace('units.append(("cc", *o2, "-c", FW / "src" / u, "-o", o))','units.append(("cc", *o2, *(["-std=c++14", "-fno-exceptions", "-fno-rtti", "-fno-threadsafe-statics", "-fno-use-cxa-atexit"] if u.endswith(".cpp") else []), "-c", FW / "src" / u, "-o", o))')
s=s.replace('name = f"dub-{a.release}.fwsc"','name = f"emerald-ym-{a.release}.fwsc"')
p.write_text(s)
# Reuse audio interrupts, crash diagnostics, safe mode, and boot initialization verbatim,
# except the renderer and labels. A dedicated, small UI replaces DUB's parameter pages.
s=(ROOT/'firmware/src/app/main_fm1.c').read_text()
start=s.index('static void splash(');end=s.index('void fm1_cstart(',start)
s=s[:start]+'#include "ym_loop.c"\n\n'+s[end:]
s=s.replace('en_render(o + 2u * k, REND_FRAMES, master_q12);','if (ym_overload < 8u) ym_render(o + 2u * k, REND_FRAMES, master_q12); else memset(o + 2u * k, 0, REND_FRAMES * 2u * sizeof(int32_t));')
s=re.sub(r'        en_load = .*?;[^\n]*','        if (pct > 85u && en_dbg.halves > 64u) ym_overload++;',s)
s=s.replace('uint32_t pct = us * 100u / budget;','uint32_t pct = us * 100u / budget;\n        if (pct > audio_cpu_lifetime_peak) audio_cpu_lifetime_peak = pct;')
s=s.replace('DUB-1','EMERALD YM')
s=s.replace('extern uint32_t _pool_start[], _pool_end[], _rt_start[], _rt_end[], _rt_load[];',
 'extern uint32_t _pool_start[], _pool_end[], _rt_start[], _rt_end[], _rt_load[];\nextern uint32_t _audio_start[], _audio_end[], _audio_load[];')
s=s.replace('    fm1_mailbox_clear();','    for (s = _audio_load, d = _audio_start; d < _audio_end; s++, d++)\n        *d = *s;\n    fm1_mailbox_clear();')
(YM/'main.c').write_text('#include "ym_prelude.c"\n'+s)
print('Prepared YM engine sources, native Windows build support, samples and tuning.')
