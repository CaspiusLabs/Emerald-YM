/* Actual engine + production control-rate form + USB publishers, no hardware.
 * Keep assertions active even when the engine is built with /DNDEBUG. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "../firmware/src/ym/ym_engine.h"
#include "../firmware/src/ym/ym_song.h"
static uint32_t yplaying,ytempo=132,ydensity=100,ymutation=35;
static int32_t ytranspose;
static uint32_t ylevels[12]={100,80,70,60,50,90,85,75,65,55,45,35};
static uint32_t irq_save(){return 0;}static void irq_restore(uint32_t){}
static void ypost(){ym_control(ytempo,ydensity*ysong.density/100u,ymutation,ytranspose,yplaying);}
static void ys_song_reset();
#include "ym_song_control.h"
static uint32_t clocks,starts,stops,ticket;
static uint8_t held[16][128];
static int send(uint32_t p){
 uint32_t s=(p>>8)&255,n=(p>>16)&127,c=s&15;
 if(s==0xfa)starts++;if(s==0xfc)stops++;if(s==0xf8)clocks++;
 if((s&240)==0x90)held[c][n]=1;
 if((s&240)==0x80)held[c][n]=0;
 ticket++;return 1;
}
#define YMO_SOURCE_ENABLED(s) (ys_song_output_mask&(1u<<(s)))
#define YMO_SEND(p) send(p)
#include "../firmware/src/ym/ym_midi_out.h"
#define YMS_SEND(p) send(p)
#define YMS_TICKET() ticket
#define YMS_PENDING(t) 0
#define YMS_DROP() ((void)0)
#include "../firmware/src/ym/ym_midi_sync.h"
static void profile_tests(){
 ym_song_form s={};s.enabled=1;
 const uint32_t bars[]={1,9,25,41,49,65,81,89};
 const uint32_t phases[]={0,1,2,3,1,2,4,5};
 for(uint32_t i=0;i<8;i++){ys_song_profile(&s,bars[i],0);assert(s.phase==phases[i]&&s.done==(i==7));}
 ys_song_profile(&s,8,11);assert(!s.fill);ys_song_profile(&s,8,12);assert(s.fill&&(s.mask&(1u<<6)));
 ys_song_profile(&s,88,0);assert(s.gain==12&&ys_song_level(&s,0,100)==12&&ys_song_level(&s,10,45)==45);
 /* Two verses and two choruses must evolve, not replay one static mask.
  * Keep the original phase boundaries, outro, and saved mixer semantics. */
 ys_song_profile(&s,9,0);assert(s.density==55&&!(s.mask&(1u<<1))&&!(s.mask&(1u<<9))&&(s.mask&(1u<<10)));
 ys_song_profile(&s,13,0);assert(s.density==65&&(s.mask&(1u<<1))&&!(s.mask&(1u<<9)));
 ys_song_profile(&s,17,0);assert(s.density==70&&(s.mask&(1u<<9)));
 ys_song_profile(&s,24,0);assert(s.density==80&&(s.mask&(1u<<8)));
 ys_song_profile(&s,25,0);assert(s.density==90&&(s.mask&(1u<<1)));
 ys_song_profile(&s,33,0);assert(s.density==100);
 ys_song_profile(&s,41,0);assert(s.density==45&&!(s.mask&(1u<<1)));
 ys_song_profile(&s,45,0);assert(s.density==65&&(s.mask&(1u<<1)));
 ys_song_profile(&s,49,0);assert(s.density==70&&(s.mask&(1u<<1))&&!(s.mask&(1u<<9)));
 ys_song_profile(&s,51,0);assert(!(s.mask&(1u<<1)));
 ys_song_profile(&s,57,0);assert(s.density==86&&(s.mask&(1u<<9)));
 ys_song_profile(&s,61,0);assert(s.density==94);
 ys_song_profile(&s,65,0);assert(s.density==100&&(s.mask&(1u<<9)));
 ys_song_profile(&s,69,0);assert(!(s.mask&(1u<<9)));
 ys_song_profile(&s,71,0);assert(!(s.mask&(1u<<1)));
 ys_song_profile(&s,72,0);assert(s.mask&(1u<<1));
 s.enabled=0;ys_song_profile(&s,0xffffffffu,15);assert(!s.done&&s.section==255&&s.density==100&&ys_song_level(&s,0,73)==73);
 puts("PASS: 88-bar form boundaries, fill, outro attenuation, live exemption and endless bypass");
}
/* Actual rendered PCM must repeat for the same seed and change for a
 * different seed in song mode. The engine-only endless golden stays intact. */
static uint32_t song_audio_signature(uint32_t seed){
 ym_init(seed);ysong.enabled=1;yplaying=1;ys_song_reset();
 int32_t pcm[512];uint32_t frames=0,next_control=0,hash=2166136261u;
 while(frames<44100u*4u){
  if(frames>=next_control){ys_song_service();next_control+=882u;}
  ym_render(pcm,256,256);
  for(unsigned i=0;i<512u;i++){hash^=(uint32_t)pcm[i];hash*=16777619u;}
  frames+=256u;
 }
 return hash;
}
int main(){
 profile_tests();
 uint32_t first=song_audio_signature(20261026u);
 assert(first==song_audio_signature(20261026u));
 assert(first!=song_audio_signature(20261027u));
 puts("PASS: FM/PSG/DAC song audio is same-seed deterministic and changes across seeds");
 ym_init(20261026);ysong.enabled=1;yplaying=1;ys_song_reset();
 ym_midi_output output={};ym_midi_sync sync={};int32_t pcm[512];uint32_t seen=0,next_control=0,frames=0;
 while(frames<44100u*180u){
  if(frames>=next_control){ys_song_service();next_control+=882u;seen|=1u<<ysong.phase;}
  ym_render(pcm,256,256);ym_midi_state v;ym_midi_snapshot(&v);
  yms_update(&sync,&v,1,1,0,0);assert(!sync.blocked);ymo_update(&output,&v,1,1,0,0,0,0);
  frames+=256;
  if(stops)break;
 }
 assert(ysong.done&&!yplaying&&seen==63&&starts==1&&stops==1&&clocks>8000);
 for(int c=0;c<16;c++)for(int n=0;n<128;n++)assert(!held[c][n]);
 assert(ylevels[1]==80&&ydensity==100); /* transient arrangement never edits saved mixer/knobs */
 uint32_t before=clocks;for(int i=0;i<20;i++){ym_render(pcm,256,256);ym_midi_state v;ym_midi_snapshot(&v);yms_update(&sync,&v,1,1,0,0);ymo_update(&output,&v,1,1,0,0,0,0);}assert(before==clocks);
 /* Repeated boot/REC/external Start reset publication must not stop before
  * the audio owner has consumed a pending reseed from the completed song. */
 ys_song_reset();ym_reseed(20261026);yplaying=1;ypost();ys_song_service();assert(yplaying&&ysong_reset_pending);
 ym_render(pcm,256,256);ys_song_service();assert(yplaying&&!ysong.done&&!ysong_reset_pending&&ysong.phase==0);
 /* Even sub-block drum serials must not leak from a muted arrangement stem. */
 ym_midi_state v={};for(int i=0;i<12;i++)v.note[i]=-1;v.playing=1;v.epoch=99;
 ymo_update(&output,&v,1,1,0,0,0,0);ysong.mask=1u<<10;ys_song_apply();v.serial[5]++;
 ymo_update(&output,&v,1,1,0,0,0,0);assert(!held[9][36]);
 printf("PASS: actual 88-bar render ends after %.2fs, Start/Clock/Stop cleanup, no idle clock, reseed after END, saved values untouched and muted drum pulses suppressed\n",frames/44100.0);
 return 0;
}
