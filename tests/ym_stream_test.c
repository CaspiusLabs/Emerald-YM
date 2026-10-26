/* Streaming engine tests; no hardware, MIDI or USB access. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../firmware/src/ym/ym_engine.h"
static int32_t audio[512];
static void check(int ok,const char*what){printf("%s: %s\n",what,ok?"PASS":"FAIL");if(!ok)exit(2);}
static void stem(uint32_t melody,uint32_t drums,int32_t *out){
 ym_init(20261026);ym_psg_enable(0);ym_mix_levels(melody,drums);
 for(int n=0;n<1400;n++)ym_render(audio,256,4096);
 memcpy(out,audio,sizeof audio);
}
static uint32_t render(uint32_t seed,int block,const char*file){
 FILE*f=0;uint32_t hash=2166136261u,total=44100u*60u,done=0;int32_t peak=0;uint32_t limited=0,arena;
 if(file){unsigned char head[44]={0};uint32_t val;memcpy(head,"RIFF",4);val=36+total*4;memcpy(head+4,&val,4);memcpy(head+8,"WAVEfmt ",8);head[16]=16;head[20]=1;head[22]=2;val=44100;memcpy(head+24,&val,4);val=44100*4;memcpy(head+28,&val,4);head[32]=4;head[34]=16;memcpy(head+36,"data",4);val=total*4;memcpy(head+40,&val,4);f=fopen(file,"wb");check(f!=0,"open WAV");fwrite(head,1,44,f);}
 ym_init(seed);ym_control(132,100,35,0,1);arena=ym_arena_bytes();
 while(done<total){uint32_t n=(uint32_t)block;if(n>total-done)n=total-done;ym_render(audio,n,4096);
  for(uint32_t i=0;i<n*2;i++){
   int32_t v=audio[i],a=v<0?-v:v;if(a>peak)peak=a;if(a==8300000)limited++;
   hash^=(uint32_t)v;hash*=16777619u;
   if(f){int16_t sample=(int16_t)(v/256);fwrite(&sample,2,1,f);}
  }done+=n;
 }
 if(f)fclose(f);
 check(ym_arena_bytes()==arena,"no allocations while rendering");check(peak>50000&&peak<=8300000,"non-silent bounded audio");check(limited==0,"no hard limiter hits");
 printf("seed=%u block=%d hash=%08x arena=%u peak=%d bar=%u notes=%u\n",seed,block,hash,arena,peak,ym_bar(),ym_note_count());return hash;
}
int main(int argc,char**argv){
 clock_t start=clock();
 uint32_t a=render(20261026,256,argc>1?argv[1]:0),b=render(20261026,63,0),c=render(20261027,256,0);
 check(a==b,"block-size independent and deterministic");check(a!=c,"seed changes composition");
 check(a==0x7ce44fbeu,"unity mixer preserves accepted 0.2.2 audio exactly");
 uint32_t bar=ym_bar();ym_control(132,100,35,0,0);for(int n=0;n<200;n++)ym_render(audio,256,4096);
 int32_t peak=0;for(int n=0;n<512;n++){int32_t v=audio[n]<0?-audio[n]:audio[n];if(v>peak)peak=v;}
 check(bar==ym_bar(),"pause stops sequencer");check(peak<100,"pause fades to silence");
 ym_control(180,100,100,12,1);for(int n=0;n<1000;n++)ym_render(audio,256,4096);
 ym_control(80,0,0,-12,1);for(int n=0;n<1000;n++)ym_render(audio,256,4096);
 check(ym_bar()>bar,"tempo and transpose extremes keep running");
 uint32_t before=ym_arena_bytes();ym_reseed(1234567);ym_render(audio,256,4096);
 check(ym_seed()==1234567&&ym_arena_bytes()==before,"new seed without allocating");
 ym_init(20261026);ym_control(132,100,35,0,0);
 ym_live_keys(1u);for(int n=0;n<40;n++)ym_render(audio,256,4096);
 check(ym_live_active_keys()==1u&&ym_bar()==1,"live keyboard plays while sequencer paused");
 peak=0;for(int n=0;n<512;n++){int32_t v=audio[n]<0?-audio[n]:audio[n];if(v>peak)peak=v;}
 check(peak>50000,"paused live voice is audible");
 ym_live_keys(3u);ym_render(audio,256,4096);check(ym_live_active_keys()==3u,"two live voices");
 ym_live_keys(15u);ym_render(audio,256,4096);check(ym_live_active_keys()==14u,"three voices: newest keys take priority");
 ym_live_keys(7u);ym_render(audio,256,4096);check(ym_live_active_keys()==7u,"released voice restores held stolen key");
 ym_psg_enable(0);for(int n=0;n<40;n++)ym_render(audio,256,4096);
 check(ym_live_active_keys()==0,"PSG off clears live indication");
 ym_psg_enable(1);ym_render(audio,256,4096);check(ym_live_active_keys()==7u,"PSG on restores held keys");
 ym_reseed(9876);ym_render(audio,256,4096);check(ym_live_active_keys()==7u,"reseed preserves held live keys");
 ym_live_keys(0);for(int n=0;n<100;n++)ym_render(audio,256,4096);
 peak=0;for(int n=0;n<512;n++){int32_t v=audio[n]<0?-audio[n]:audio[n];if(v>peak)peak=v;}
 check(ym_live_active_keys()==0&&peak<100,"all live keys released: no stuck sound");
 ym_control(180,100,100,12,1);ym_live_keys(0x07ffffffu);ym_psg_level(400);
 before=ym_arena_bytes();int bounded=1;
 for(int n=0;n<2000;n++){ym_render(audio,256,4096);for(int i=0;i<512;i++)if(audio[i]>=8300000||audio[i]<=-8300000)bounded=0;}
 check(bounded,"three PSG voices plus FM/DAC: no limiter hits");
 check(ym_arena_bytes()==before,"full held keyboard and sequencer: no allocations");
 {static int32_t all[512],fm[512],drum[512],muted[512],half[512];
  stem(100,100,all);stem(100,0,fm);stem(0,100,drum);stem(0,0,muted);stem(50,0,half);
  int isolated=1,quiet=1;int64_t fm_energy=0,drum_energy=0,half_energy=0;
  for(int i=0;i<512;i++){
   int32_t delta=all[i]-fm[i]-drum[i];if(delta>2048||delta< -2048)isolated=0;
   if(muted[i]>100||muted[i]< -100)quiet=0;
   fm_energy+=fm[i]<0?-fm[i]:fm[i];drum_energy+=drum[i]<0?-drum[i]:drum[i];half_energy+=half[i]<0?-half[i]:half[i];
  }
  check(fm_energy>100000&&drum_energy>100000,"FM and drums produce independent audible stems");
  check(isolated,"FM plus drum stems reproduce combined backing within quantization");
  check(quiet,"SELECT=0 PRESETS=0 PSG=off mutes backing");
  check(half_energy>fm_energy*45/100&&half_energy<fm_energy*55/100,"SELECT 50 percent scales FM amplitude without changing drums");
 }
 ym_init(20261026);ym_control(132,100,35,0,0);ym_mix_levels(0,0);ym_live_keys(1);
 for(int n=0;n<40;n++)ym_render(audio,256,4096);
 peak=0;for(int i=0;i<512;i++){int32_t v=audio[i]<0?-audio[i]:audio[i];if(v>peak)peak=v;}
 check(peak>50000,"muted FM and drums preserve independent PSG live voice");
 {int every_voice=1;
  for(uint32_t voice=0;voice<12;voice++){
   ym_init(20261026);for(uint32_t k=0;k<12;k++)ym_instrument_level(k,k==voice?100:0);
   if(voice==10)ym_live_keys(1);
   int64_t energy=0;for(int block=0;block<1800;block++){
    ym_render(audio,256,4096);if(block>172)for(int i=0;i<512;i++)energy+=audio[i]<0?-audio[i]:audio[i];
   }
   if(energy<1000000)every_voice=0;
  }
  check(every_voice,"each of twelve instruments is independently audible with the other eleven muted");
 }
 ym_init(20261026);ym_control(132,100,35,0,0);ym_live_keys(1);ym_instrument_level(10,0);
 for(int n=0;n<100;n++)ym_render(audio,256,4096);
 peak=0;for(int i=0;i<512;i++){int32_t v=audio[i]<0?-audio[i]:audio[i];if(v>peak)peak=v;}
 check(peak<100,"individual PSG live level zero mutes held notes");
 ym_instrument_level(10,100);for(int n=0;n<40;n++)ym_render(audio,256,4096);
 peak=0;for(int i=0;i<512;i++){int32_t v=audio[i]<0?-audio[i]:audio[i];if(v>peak)peak=v;}
 check(peak>50000,"individual level restores held PSG voice without retrigger");
 printf("PASS: 180+ seconds streamed, %.3f s desktop CPU time (not a target benchmark).\n",(double)(clock()-start)/CLOCKS_PER_SEC);return 0;
}
