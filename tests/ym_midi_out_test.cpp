/* Actual engine, production USB note queue and publisher. No hardware APIs. */
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
static struct ym_midi_output output;
static struct ym_midi_state state;
static uint32_t ons[16],offs[16],held[16][128],panics,total;
static int32_t pcm[512];
static void drain(){
 while(mo_r!=mo_w){
  uint32_t p=midi_out_q[mo_r++%MQ],status=(p>>8)&255,c=status&15,n=(p>>16)&127;
  assert((p&0xf0u)==0);assert((p&15u)==status>>4);total++;
  if((status&0xf0)==0x90){assert((p>>24)==90);assert(!held[c][n]);held[c][n]=1;ons[c]++;}
  else if((status&0xf0)==0x80){held[c][n]=0;offs[c]++;}
  else {assert((status&0xf0)==0xb0&&n==123);memset(held[c],0,sizeof held[c]);panics++;}
 }
}
static uint32_t held_count(){uint32_t n=0;for(int c=0;c<16;c++)for(int k=0;k<128;k++)n+=held[c][k];return n;}
static void update(uint32_t enabled=1,uint32_t ready=1,uint32_t session=0,uint32_t revision=0,uint32_t keys=0,int32_t tr=0){
 ymo_update(&output,&state,enabled,ready,session,revision,keys,tr);
}
static void block(uint32_t enabled=1,uint32_t keys=0,int32_t tr=0){
 ym_render(pcm,256,4096);ym_midi_snapshot(&state);update(enabled,1,0,0,keys,tr);drain();
}
int main(){
 for(int s=0;s<12;s++)state.note[s]=-1;
 update(0);drain();assert(!total);
 update(1,1,0,1,(1u<<0)|(1u<<4)|(1u<<7));drain();assert(ons[0]==3&&held_count()==3);
 uint32_t count=total;update(1,1,0,1,(1u<<0)|(1u<<4)|(1u<<7));drain();assert(total==count);
 update(1,1,0,1,1u,12);drain();assert(held_count()==1&&held[0][65]);
 update(0,1,0,2,1u,12);drain();assert(!held_count());
 state.playing=1;state.note[0]=40;state.serial[0]=1;
 update();drain();assert(held[1][40]);
 state.serial[0]++;update();drain();assert(held[1][40]&&ons[1]==2&&offs[1]==1);
 state.serial[0]=0xffffffffu;update();drain();state.serial[0]=0;update();drain();assert(held[1][40]);
 state.serial[5]++;update();drain();assert(held[9][36]);update();drain();assert(!held[9][36]);
 puts("PASS: physical-key polyphony, transpose/releases, repeat suppression, FM retrigger, sub-block drum pulse and GM channel 10");
 /* A full production queue must not make the publisher forget an Off. */
 for(unsigned i=0;i<MQ;i++)assert(midi_out_try(0x007bb00bu));
 assert(!midi_out_try(0x007bb00bu));update(0);assert(output.note[0]==40);
 drain();update(0);drain();assert(!held_count()&&output.note[0]<0);
 state.note[0]=41;update();drain();assert(held[1][41]);
 update(1,0);assert(output.panic==9);mo_r=mo_w; /* real wrapper discards pending packets while unavailable */
 update();drain();assert(panics>=9&&held[1][41]);
 update(1,1,1);drain();assert(held[1][41]);
 update(0,1,1);drain();assert(!held_count());
 state.note[0]=-1;state.serial[5]++;state.playing=0;update(1,1,1);drain();assert(!held_count());
 mo_r=mo_w=0xfffffffeu;assert(midi_out_try(0x007bb00bu));assert(midi_out_try(0x007bb00bu));drain();assert(mo_r==mo_w);
 puts("PASS: full queue retry, disable cleanup, suspend/reset recovery, no stale drum after Stop and unsigned queue wrap");
 memset(&output,0,sizeof output);memset(held,0,sizeof held);memset(ons,0,sizeof ons);memset(offs,0,sizeof offs);
 ym_init(42);ym_control(180,100,100,-12,1);
 for(int i=0;i<11000;i++)block(1,(i%100<50)?((1u<<0)|(1u<<4)|(1u<<7)):0,-12);
 for(int c=0;c<8;c++)assert(ons[c]>0);assert(ons[9]>0&&ons[8]==0);
 ym_control(180,100,100,-12,0);block(1,1u,-12);assert(held_count()==1&&held[0][41]);
 block(0);assert(!held_count());
 /* External 300 BPM, maximum DECAY, same-seed reset and overload/disable. */
 for(int s=0;s<12;s++)ym_effect(s,0,100);
 ym_external_clock(1,0,300);ym_control(132,100,35,12,1);
 uint32_t event=0;
 for(int i=0;i<1800;i++){if(i%9==0)event++;ym_external_clock(1,event,300);block();}
 ym_reseed(42);block();ym_control(132,100,35,12,0);block(0);assert(!held_count());
 printf("PASS: actual engine 63s internal + external clock/short gates/reseed, all nine MIDI channels, Stop keeps keyboard, disable clears all (%u packets)\n",total);
 return 0;
}
