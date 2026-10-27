"""Measure the real FM-1 through the documented YM-USB-PROTOCOL.md interface.
Explicitly matches only Emerald YM FM-1. Reads status, sends MIDI Start/Stop. --effects-stress also sends documented
CC20/21/22 and restores the twelve sources to neutral afterward.
No updater, firmware, preset, or persistent writes are performed here.
"""
import sys,time,json,argparse
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
import mido
import fm1_install as I

FIELDS=['playing','cpu_avg_pct','cpu_max_pct','xruns','overload_blocks','bar','seed','uptime_ms','audio_halves','max_block_us','failed_boots']
QUERY=[0x7d,0x45,0x4d,0x59,1]
LIVE_TEST_NOTES=(64,71,76) # E4/B4/E5: test voices stay in the tune's pitch set
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
 ap=argparse.ArgumentParser();ap.add_argument('--seconds',type=int,default=30);ap.add_argument('--output',default='build/hardware-test.json');ap.add_argument('--status-only',action='store_true');ap.add_argument('--live-stress',action='store_true');ap.add_argument('--require-continuous',action='store_true');ap.add_argument('--effects-stress',action='store_true');ap.add_argument('--effect-decay',type=int,choices=range(101),default=25);a=ap.parse_args()
 if a.status_only and (a.live_stress or a.effects_stress):ap.error('Live stress requires a playback test')
 # Verify the known identity before sending this firmware's private status query.
 backend=I.MidoBackend();up=I.Updater(backend,'Emerald YM FM-1');dev=up.find()
 if dev is None:raise RuntimeError('Explicit Emerald YM MIDI port did not answer identity query')
 identity=dev.id.text;dev.link.close()
 if identity not in ('FM-1_68000102','FM-1_68000103','FM-1_68000104','FM-1_68000200','FM-1_68000201','FM-1_68000202','FM-1_68000203','FM-1_68000204','FM-1_68000300','FM-1_68000301'):raise RuntimeError('Unsupported identity '+identity)
 if a.live_stress and identity not in ('FM-1_68000200','FM-1_68000201','FM-1_68000202','FM-1_68000203','FM-1_68000204','FM-1_68000300','FM-1_68000301'):raise RuntimeError('Live stress requires 0.2.x')
 if a.effects_stress and identity not in ('FM-1_68000300','FM-1_68000301'):raise RuntimeError('Effects stress requires 0.3.x')
 ins=[n for n in mido.get_input_names() if 'emerald ym fm-1' in n.lower()]
 outs=[n for n in mido.get_output_names() if 'emerald ym fm-1' in n.lower()]
 if len(ins)!=1 or len(outs)!=1:raise RuntimeError('Ambiguous Emerald YM port pair')
 samples=[];failure=None
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
    if a.effects_stress:
     for source in range(12):
      for control,value in ((20,(a.effect_decay*127+50)//100),(21,127),(22,127)):
       out.send(mido.Message('control_change',channel=source,control=control,value=value));time.sleep(.003)
    out.send(mido.Message('start'))
    for i in range(max(1,min(120,a.seconds))):
     if a.live_stress:
      # Standard MIDI events: two live voices + generated PSG, then three
      # live voices replacing the PSG arp. Never probes another device.
      for note in LIVE_TEST_NOTES:out.send(mido.Message('note_off',note=note))
      for note in (LIVE_TEST_NOTES[:2] if i%4<2 else LIVE_TEST_NOTES):out.send(mido.Message('note_on',note=note,velocity=90))
     time.sleep(1);s=status()
     if s['xruns']>baseline['xruns'] or s['overload_blocks']>baseline['overload_blocks']:break
  except Exception as error:
   failure=str(error)
  finally:
   if not a.status_only:
    try:
     if a.live_stress:
      for note in LIVE_TEST_NOTES:out.send(mido.Message('note_off',note=note))
     if a.effects_stress:
      for source in range(12):
       for control in (20,21,22):
        out.send(mido.Message('control_change',channel=source,control=control,value=0));time.sleep(.003)
     out.send(mido.Message('stop'));time.sleep(.2);status()
    except Exception as error:failure=failure or str(error)
 report={'identity':identity,'seconds_requested':a.seconds,'live_stress':a.live_stress,'effects_stress':a.effects_stress,'effect_values':{'decay':a.effect_decay,'color':100,'echo':100} if a.effects_stress else None,'live_test_notes':list(LIVE_TEST_NOTES) if a.live_stress else [],'failure':failure,'continuous_playback_observed':all(s['playing'] for s in samples[1:-1]) if len(samples)>2 else False,'samples':samples,
 'passed':None if a.status_only else len(samples)>=max(1,min(120,a.seconds))+2 and max(s['xruns'] for s in samples)==baseline['xruns'] and max(s['overload_blocks'] for s in samples)==baseline['overload_blocks'] and any(s['playing'] for s in samples) and samples[-1]['bar']>baseline['bar']}
 if failure:report['passed']=False
 if a.require_continuous and not a.status_only:report['passed']=report['passed'] and report['continuous_playback_observed']
 Path(a.output).write_text(json.dumps(report,indent=2)+'\n');print('RESULT '+json.dumps({k:v for k,v in report.items() if k!='samples'}))
 return 0 if a.status_only or report['passed'] else 2
if __name__=='__main__':sys.exit(main())
