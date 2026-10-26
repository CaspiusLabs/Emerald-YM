"""Export source, licenses, tested binary and logs; never touches a device."""
from pathlib import Path
import hashlib,subprocess,zipfile,json,sys
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT.parents[1]/'outputs'
VERSION=sys.argv[1] if len(sys.argv)>1 else '0.1.4'
PREFIX=f'Emerald-YM-{VERSION}'
tracked=subprocess.check_output(['git','ls-files'],cwd=ROOT,text=True).splitlines()
files={ROOT/p:p for p in tracked if (ROOT/p).is_file()}
for p in (ROOT/'firmware/src/ym').rglob('*'):
 if p.is_file() and '__pycache__' not in p.parts:files[p]=p.relative_to(ROOT).as_posix()
for rel in ['EMERALD-YM-PL.md','EMERALD-YM-0.1.3-PL.md',f'EMERALD-YM-{VERSION}-PL.md','YM-USB-PROTOCOL.md','LICENSE-emerald.txt','JieLi-SDK-LICENSE.txt','build-ym-windows.ps1',
 'tests/ym_stream_test.c','tests/ym_ui_test.c','tools/test-ui-windows.cmd','tools/prepare_ym.py','tools/test-ym-windows.cmd','tools/verify_ym_package.py','tools/export_ym.py','tools/ym_hardware_test.py',
 'build/emerald-ym-0.1.0.fwsc',f'build/emerald-ym-{VERSION}.fwsc','build/amb.bin','build/amb.elf','build/loader/ota.bin',
 'build/build-ym.log','build/host-tests-0.1.2.log',f'build/ui-tests-{VERSION}.log','build/installer-tests.log',f'build/ota-final-{VERSION}.log',
 'build/loader-different-image-tests.log','build/firmware-verification.json',f'build/hardware-{VERSION}-test.json',f'build/hardware-{VERSION}-test.log',f'build/hardware-install-{VERSION}.log','build/host/ui-playing.png']:
 files[ROOT/rel]=rel
sdk=ROOT.parent/'tooling/ac79-sdk/cpu/wl82/tools'
for rel in ['uboot.boot','cfg_tool.bin','cfg/eq_cfg_hw.bin']:
 files[sdk/rel]='vendor-sdk/cpu/wl82/tools/'+rel
zip_path=OUT/(PREFIX+'-sources.zip')
with zipfile.ZipFile(zip_path,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
 for source,rel in sorted(files.items(),key=lambda x:x[1]):
  # The new README is the entry point; preserve the upstream README separately.
  if rel=='README.md':rel='DUB-1-upstream-README.md'
  z.write(source,PREFIX+'/'+rel)
 z.write(ROOT/f'EMERALD-YM-{VERSION}-PL.md',PREFIX+'/README.md')
with zipfile.ZipFile(zip_path) as z:
 assert z.testzip() is None
 fw=z.read(PREFIX+f'/build/emerald-ym-{VERSION}.fwsc')
 assert fw==(OUT/f'emerald-ym-{VERSION}.fwsc').read_bytes()
info=json.loads((OUT/(PREFIX+'-verification.json')).read_text())
assert hashlib.sha256(fw).hexdigest()==info['sha256']
lines=[]
for name in [f'emerald-ym-{VERSION}.fwsc',PREFIX+'-sources.zip',PREFIX+'-instrukcja.md',PREFIX+'-verification.json',PREFIX+'-hardware-test.json']:
 p=OUT/name;lines.append(hashlib.sha256(p.read_bytes()).hexdigest()+'  '+name)
(OUT/(PREFIX+'-SHA256.txt')).write_text('\n'.join(lines)+'\n')
print(f'Exported {len(files)} source/artifact files; ZIP verified; firmware SHA-256 matches metadata.')
print(zip_path)
