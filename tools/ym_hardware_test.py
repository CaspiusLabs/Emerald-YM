"""Measure the real FM-1 through the documented YM-USB-PROTOCOL.md interface.
Explicitly matches only Emerald YM FM-1. Reads status, sends MIDI Start/Stop.
No updater, firmware, preset, or persistent writes are performed here.
"""
import sys,time,json,argparse
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
import mido
import fm1_install as I

FIELDS=['playing','cpu_avg_pct','cpu_max_pct','xruns','overload_blocks','bar','seed','uptime_ms','audio_halves','max_block_us','failed_boots']
QUERY=[0x7d,0x45,0x4d,0x59,1]
def decode(data):
 if len(data)!=60 or list(data[:5])!=[0x7d,0x45,0x4d,0x59,0x41]:return None
 if any(v>127 for v in data):return None
 vals=[]
 for i in range(11):
  part=data[5+i*5:10+i*5]
  if part[4]>15:return None
  vals.append(sum(v<<(7*k) for k,v in enumerate(part)))
 return dict(zip(FIELDS,vals))
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--seconds',type=int,default=30);ap.add_argument('--output',default='build/hardware-0.1.4-test.json');ap.add_argument('--status-only',action='store_true');a=ap.parse_args()
 # Verify the known identity before sending this firmware's private status query.
 backend=I.MidoBackend();up=I.Updater(backend,'Emerald YM FM-1');dev=up.find()
 if dev is None:raise RuntimeError('Explicit Emerald YM MIDI port did not answer identity query')
 identity=dev.id.text;dev.link.close()
 if identity not in ('FM-1_68000102','FM-1_68000103','FM-1_68000104'):raise RuntimeError('Unsupported identity '+identity)
 ins=[n for n in mido.get_input_names() if 'emerald ym fm-1' in n.lower()]
 outs=[n for n in mido.get_output_names() if 'emerald ym fm-1' in n.lower()]
 if len(ins)!=1 or len(outs)!=1:raise RuntimeError('Ambiguous Emerald YM port pair')
 samples=[]
 with mido.open_input(ins[0]) as inp,mido.open_output(outs[0]) as out:
  def status():
   for _ in inp.iter_pending():pass
   out.send(mido.Message('sysex',data=QUERY));end=time.monotonic()+2
   while time.monotonic()<end:
    msg=inp.poll()
    if msg is not None and msg.type=='sysex':
     d=decode(msg.data)
     if d is not None:
      samples.append(d);print(json.dumps(d),flush=True);return d
    time.sleep(.005)
   raise RuntimeError('No status response')
  baseline=status()
  try:
   if not a.status_only:
    if baseline['overload_blocks']>=8:raise RuntimeError('Overload already latched; not starting')
    out.send(mido.Message('start'))
    for i in range(max(1,min(120,a.seconds))):
     time.sleep(1);s=status()
     if s['xruns']>baseline['xruns'] or s['overload_blocks']>=8:break
  finally:
   if not a.status_only:out.send(mido.Message('stop'));time.sleep(.2);status()
 report={'identity':identity,'seconds_requested':a.seconds,'samples':samples,
 'passed':None if a.status_only else len(samples)>2 and max(s['xruns'] for s in samples)==baseline['xruns'] and max(s['overload_blocks'] for s in samples)<8 and any(s['playing'] for s in samples) and samples[-1]['bar']>baseline['bar']}
 Path(a.output).write_text(json.dumps(report,indent=2)+'\n');print('RESULT '+json.dumps({k:v for k,v in report.items() if k!='samples'}))
 return 0 if a.status_only or report['passed'] else 2
if __name__=='__main__':sys.exit(main())
