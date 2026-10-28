/* Real clock state and renderer; no USB or device access. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "../firmware/src/ym/ym_clock.h"
#include "../firmware/src/ym/ym_engine.h"
static int32_t pcm[512];
static void block(){ym_render(pcm,256,4096);}
static void publish(uint32_t events,uint32_t playing){ym_control(132,100,35,0,playing);ym_external_clock(1,events,120);}
int main(){
 struct ym_clock c={};c.bpm=132;
 uint32_t at=0xfff00000u;
 for(unsigned i=0;i<100;i++){yc_usb(&c,0xf8,at,24);at+=500000;}
 assert(c.bpm==120&&!c.mode&&!c.events);
 assert(yc_usb(&c,0xfa,at,24)==2&&c.running&&c.mode);
 for(unsigned i=0;i<7200;i++){
  /* Five minutes, wrapping TIMER4 repeatedly, with nonaccumulating jitter. */
  uint32_t jitter=(i%3)*2400;
  yc_usb(&c,0xf8,at+jitter,24);at+=500000;
 }
 assert(c.events==1200&&c.phase==0&&c.bpm>=119&&c.bpm<=121);
 uint32_t events=c.events;yc_usb(&c,0xfc,at,24);
 for(int i=0;i<24;i++){yc_usb(&c,0xf8,at,24);at+=500000;}
 assert(c.events==events&&!c.running);
 yc_usb(&c,0xfb,at,24);yc_usb(&c,0xf8,at,24);assert(c.events==events+1);
 at+=YC_TIMEOUT_US*24u+1;assert(yc_expire(&c,at,24)&&!c.running&&c.lost);
 yc_usb(&c,0xf8,at,24);assert(!c.running);yc_usb(&c,0xfa,at,24);assert(!c.events&&!c.phase&&!c.lost);
 /* Batched arrival stamps still count every pulse; zero duration never divides. */
 for(int i=0;i<96;i++)yc_usb(&c,0xf8,at,24);
 assert(c.events==16&&c.bpm>=30&&c.bpm<=300);
 yc_internal(&c);assert(!c.mode&&!c.events);
 for(unsigned bpm=30;bpm<=300;bpm+=30){
  c={};c.bpm=132;at=123;
  for(unsigned i=0;i<97;i++){yc_usb(&c,0xf8,at,24);at+=60000000u/bpm;}
  assert(c.bpm==bpm);
 }
 puts("PASS: 24 PPQ, 6 clocks/step, 5-minute drift/jitter/wrap, transport, loss, batches, 30-300 BPM");

 ym_init(42);publish(0,1);
 for(int i=0;i<200;i++)block();assert(!ym_note_count());
 publish(1,1);block();assert(ym_note_count()&&ym_step()==0);
 uint32_t notes=ym_note_count();for(int i=0;i<200;i++)block();assert(ym_note_count()==notes&&ym_step()==0);
 publish(2,1);block();assert(ym_step()==1);
 publish(2,0);for(int i=0;i<20;i++)block();assert(ym_step()==1);
 publish(2,1);block();assert(ym_step()==1);
 publish(3,1);block();assert(ym_step()==2);
 /* A malformed/batched source cannot cause unbounded ticks in one block. */
 publish(30,1);block();assert(ym_step()==3);
 ym_reseed(42);publish(0,1);block();assert(!ym_note_count());
 publish(1,1);block();assert(ym_step()==0&&ym_note_count());
 ym_control(132,100,35,0,0);ym_live_keys(1);block();assert(ym_live_active_keys());
 ym_external_clock(0,0,132);ym_control(132,100,35,0,1);
 for(int i=0;i<100;i++)block();assert(ym_step()!=0);
 puts("PASS: actual renderer waits for clock, has no free-run drift, resumes without reset, bounds catch-up, resets on Start, keeps live keys and returns to internal");
 return 0;
}
