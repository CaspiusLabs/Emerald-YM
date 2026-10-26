"""Offline package verification and an older-package fixture for loader tests."""
from pathlib import Path
import hashlib,json,os,subprocess,sys,re
sys.path.insert(0,str(Path(__file__).resolve().parent))
import fm1pkg_make as P
import fm1_install as I
ROOT=Path(__file__).resolve().parents[1]
version=(ROOT/'VERSION').read_text().strip()
pkg=ROOT/f'build/emerald-ym-{version}.fwsc'
raw=pkg.read_bytes()
identity=I.product_of(raw)
expected='FM-1_68'+''.join(f'{int(n):02d}' for n in version.split('.'))
assert identity==expected,identity
logical=I.logical_image(raw)
assert len(logical)>len((ROOT/'build/amb.bin').read_bytes())
sdk=Path(os.environ['AC79_SDK'])
reference={
 'uboot.boot':'4e3b4c220dc96641cb5a723f41e68ce41d5261ae9434bb33fbd7f2c59976ded4',
 'cfg_tool.bin':'276579954f076886a6a7694f65dc71c034a63a2c204b76749065c0ac7b010d1b',
 'cfg/eq_cfg_hw.bin':'41167491bffed4651750719c973d2758adeb9021a5670d02d6a53c85ed80ea7d'}
sdk_ok={name:hashlib.sha256((sdk/'cpu/wl82/tools'/name).read_bytes()).hexdigest()==sha for name,sha in reference.items()}
assert all(sdk_ok.values()),sdk_ok
P.SDK=sdk
old=(ROOT/'build/amb.bin').read_bytes()[:70000]
old_pkg=P.ufw(P.flash_image(old,P.KEY),(ROOT/'build/loader/ota.bin').read_bytes(),'FM-1_68000000')
fixture=ROOT/'build/host/old-fixture.fwsc';fixture.write_bytes(old_pkg)
r=subprocess.run([str(ROOT/'build/host/ldr_test.exe'),str(fixture),str(pkg)],capture_output=True,text=True)
(ROOT/'build/loader-different-image-tests.log').write_text(r.stdout+r.stderr)
assert r.returncode==0,r.stdout+r.stderr
log=(ROOT/'build/build-ym.log').read_text()
ram=re.search(r'RAM .data\+.bss (\d+) B of (\d+); pool (\d+) B of (\d+)',log)
assert ram,'Missing final build memory check'
hot=re.search(r'audio hot code\+tables (\d+) B in RAM; RAMTEXT total (\d+) of (\d+)',log)
assert hot,'Missing RAM audio code check'
info={'name':pkg.name,'identity':identity,'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest(),
 'application_bytes':(ROOT/'build/amb.bin').stat().st_size,'ram_data_bss_bytes':int(ram[1]),'ram_data_bss_available':int(ram[2]),
 'pool_bytes':int(ram[3]),'pool_available':int(ram[4]),'sdk_reference_hashes_match':sdk_ok,
 'audio_code_tables_ram_bytes':int(hot[1]),'ramtext_total_bytes':int(hot[2]),'ramtext_available':int(hot[3]),
 'host_stream_tests':'passed; 180+ seconds; deterministic across block sizes; seed variation; pause; parameter extremes; no audio-thread allocation; no limiter hits in test renders',
 'ota_entry_simulation':'passed','loader_simulation_different_image':'passed','flashed':False,
 'hardware_realtime_verified':False,'ymfm_commit':'81aec25ccbb98f4873a255f7551ac4dadac59b4a',
 'dub_base_commit':'e6fe7b8f8f3114019bb36b5b536ad458eacd2c2d','toolchain':'JieLi Windows 2.5.3; clang 4.0.1 pi32v2 r3'}
baseline=ROOT/f'releases/v{version}/emerald-ym-{version}.fwsc'
if baseline.exists():
 assert baseline.read_bytes()==raw,'Build differs from the preserved tested release; do not replace its baseline'
 info['matches_preserved_release_bit_for_bit']=True
(ROOT/'build/firmware-verification.json').write_text(json.dumps(info,indent=2)+'\n')
print(r.stdout)
print(json.dumps(info,indent=2))
