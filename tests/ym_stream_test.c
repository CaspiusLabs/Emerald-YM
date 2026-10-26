/* Streaming engine tests; no hardware, MIDI or USB access. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../firmware/src/ym/ym_engine.h"
static int32_t audio[512];
static void check(int ok,const char*what){printf("%s: %s\n",what,ok?"PASS":"FAIL");if(!ok)exit(2);}
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
 uint32_t bar=ym_bar();ym_control(132,100,35,0,0);for(int n=0;n<200;n++)ym_render(audio,256,4096);
 int32_t peak=0;for(int n=0;n<512;n++){int32_t v=audio[n]<0?-audio[n]:audio[n];if(v>peak)peak=v;}
 check(bar==ym_bar(),"pause stops sequencer");check(peak<100,"pause fades to silence");
 ym_control(180,100,100,12,1);for(int n=0;n<1000;n++)ym_render(audio,256,4096);
 ym_control(80,0,0,-12,1);for(int n=0;n<1000;n++)ym_render(audio,256,4096);
 check(ym_bar()>bar,"tempo and transpose extremes keep running");
 uint32_t before=ym_arena_bytes();ym_reseed(1234567);ym_render(audio,256,4096);
 check(ym_seed()==1234567&&ym_arena_bytes()==before,"new seed without allocating");
 printf("PASS: 180+ seconds streamed, %.3f s desktop CPU time (not a target benchmark).\n",(double)(clock()-start)/CLOCKS_PER_SEC);return 0;
}
