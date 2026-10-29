/* Real engine + production USB queue + master and note publishers. No devices. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../firmware/src/ym/ym_engine.h"
#define MQ 64u
#define RING_PUBLISH() ((void)0)
static struct {uint32_t config,suspended;} usb={1,0};
static uint32_t midi_out_q[MQ],mo_r,mo_w;
#include "midi_out_queue.h"
#define YMO_SEND(p) midi_out_try(p)
#include "../firmware/src/ym/ym_midi_out.h"
#define YMS_SEND(p) midi_out_try(p)
#define YMS_TICKET() mo_w
#define YMS_PENDING(ticket) ((int32_t)(mo_r-(ticket))<0)
#define YMS_DROP() (mo_r=mo_w)
#include "../firmware/src/ym/ym_midi_sync.h"
static struct ym_midi_sync sync_state;
static struct ym_midi_output notes;
static struct ym_midi_state v;
static uint32_t start_count,stop_count,clock_count,note_count,running;
static uint32_t held[16][128];
static int32_t pcm[512];
static void receive(){
 while(mo_r!=mo_w){
  uint32_t p=midi_out_q[mo_r++%MQ],status=(p>>8)&255,c=status&15,n=(p>>16)&127;
  assert((p&0xf0)==0);
  if(status>=0xf8){
   assert((p&255)==15&&(p>>16)==0);
   if(status==0xfa){start_count++;running=1;}
   else if(status==0xfc){stop_count++;running=0;}
   else {assert(status==0xf8&&running);clock_count++;}
  }else if((status&0xf0)==0x90){assert(!held[c][n]);held[c][n]=1;note_count++;if(c!=0)assert(running);}
  else if((status&0xf0)==0x80)held[c][n]=0;
  else {assert((status&0xf0)==0xb0&&n==123);memset(held[c],0,sizeof held[c]);}
 }
}
static int update(uint32_t enable=1,uint32_t ready=1,uint32_t session=0,uint32_t rev=0){return yms_update(&sync_state,&v,enable,ready,session,rev);}
static void reset(){
 memset(&sync_state,0,sizeof sync_state);memset(&notes,0,sizeof notes);memset(&v,0,sizeof v);memset(held,0,sizeof held);
 for(int s=0;s<12;s++)v.note[s]=-1;
 mo_r=mo_w=start_count=stop_count=clock_count=note_count=running=0;
}
static void block(uint32_t enabled=1,uint32_t keys=0){
 ym_render(pcm,256,4096);ym_midi_snapshot(&v);
 int ok=update(enabled);
 struct ym_midi_state routed=v;
 if(!ok){routed.playing=0;for(int s=0;s<12;s++)routed.note[s]=-1;}
 if(sync_state.recover)notes.panic=9;
 ymo_update(&notes,&routed,enabled,1,0,0,keys,v.transpose);receive();
}
int main(){
 reset();update(0);receive();assert(!start_count&&!stop_count&&!clock_count);
 v.playing=1;v.epoch=1;v.clocks=1;assert(update());
 assert((midi_out_q[mo_r%MQ]&0xffff)==0xfa0f);receive();assert(start_count==1&&clock_count==1);
 for(uint32_t k=2;k<=97;k++){v.clocks=k;assert(update());receive();}
 assert(clock_count==97&&start_count==1);v.playing=0;update();receive();assert(stop_count==1&&!running);
 for(int i=0;i<100;i++)update();receive();assert(stop_count==1&&clock_count==97);
 v.epoch++;v.playing=1;v.clocks=1;update();receive();assert(start_count==2&&clock_count==98);
 update(0,1,0,1);receive();assert(stop_count==2); /* EDIT OFF sends Stop once. */
 puts("PASS: single-byte USB CIN, Start before Clock, 24 PPQ/six per step, no idle Clock, Stop once, restart and EDIT off");

 reset();v.playing=1;v.epoch=1;v.clocks=1;assert(update()); /* leave first F8 queued */
 v.clocks=2;assert(!update()&&sync_state.blocked&&sync_state.recover);receive();assert(stop_count==1&&!clock_count);
 for(int i=0;i<20;i++){v.clocks++;update();receive();}assert(!start_count&&!clock_count); /* no catch-up/restart */
 v.epoch++;v.clocks=1;assert(update());receive();assert(start_count==1&&clock_count==1);
 for(int i=0;i<64;i++)assert(midi_out_try(0x007bb00bu));
 v.clocks=2;assert(!update()&&sync_state.blocked);receive();assert(stop_count==2);
 v.epoch++;v.clocks=1;update();receive();assert(start_count==2);
 update(1,0);assert(sync_state.blocked);mo_r=mo_w;update(1,1,1);receive();assert(stop_count==3&&start_count==2);
 v.epoch++;v.clocks=1;update(1,1,1);receive();assert(start_count==3);
 v.clocks=4;assert(!update(1,1,1)&&sync_state.blocked);receive();assert(stop_count==4); /* no multi-pulse burst */
 puts("PASS: queued Clock/full queue fail closed, pending Stop/recovery, no automatic restart after suspend, no burst catch-up");

 reset();v.playing=1;v.clocks=1;v.epoch=1;
 for(int i=0;i<64;i++)assert(midi_out_try(0x007bb00bu));
 assert(!update()&&sync_state.blocked);receive();assert(!start_count&&!clock_count); /* no partial Start */
 v.epoch++;update();receive();assert(start_count==1&&clock_count==1);
 for(int i=0;i<64;i++)assert(midi_out_try(0x007bb00bu));
 v.playing=0;assert(!update()&&sync_state.pending_stop);receive();assert(!stop_count);
 update();receive();assert(stop_count==1&&!sync_state.pending_stop&&!running);
 puts("PASS: full queue at Start cannot leak clocks; a rejected Stop retries before new traffic");

 /* Cursor and pulse counter wrap without signed scheduling or division. */
 reset();mo_r=mo_w=0xfffffffeu;v.playing=1;v.clocks=1;v.epoch=1;update();receive();
 sync_state.last_clock=0xffffffffu;v.clocks=0;update();receive();assert(clock_count==2&&!sync_state.blocked);
 v.playing=0;update();receive();assert(stop_count==1);

 for(uint32_t bpm=80;bpm<=180;bpm+=(bpm==80?52:48)){
  reset();ym_init(42);ym_control(bpm,100,35,0,1);
  const uint32_t blocks=44100u*60u/256u;
  for(uint32_t i=0;i<blocks;i++)block(1,(i%200)<100?7u:0u);
  uint32_t expected=1u+(blocks*256u*bpm/ (44100u*60u))*24u;
  /* More precise floor, using host integer double-width only in the test. */
  expected=1u+(uint32_t)((uint64_t)blocks*256u*bpm*24u/(44100u*60u));
  assert(clock_count>=expected-1&&clock_count<=expected+1&&!sync_state.blocked&&start_count==1);
  assert(note_count>100&&v.clocks==clock_count);
  ym_control(bpm,100,35,0,0);block(1,7u);assert(stop_count==1&&!running);
  block(0);for(int c=0;c<16;c++)for(int n=0;n<128;n++)assert(!held[c][n]);
 }
 puts("PASS: actual engine 60s each at 80/132/180 BPM; MIDI note ordering, chord output, Stop cleanup, phase-derived Clock without drift");

 reset();ym_init(42);ym_control(80,100,35,0,1);
 for(uint32_t i=0;i<5000;i++){
  if(i==1000)ym_control(180,100,35,0,1);
  if(i==3000)ym_control(132,100,35,0,1);
  block();assert(!sync_state.blocked&&start_count==1);
 }
 ym_reseed(42);block();assert(start_count==2&&stop_count==1&&sync_state.last_clock==1);
 /* Disabled route models overload/update: Stop followed by note cleanup. */
 block(0);assert(stop_count==2&&!running);
 puts("PASS: live tempo changes preserve pulse phase, same-seed reset restarts remote transport, overload/update disable stops");
 return 0;
}
