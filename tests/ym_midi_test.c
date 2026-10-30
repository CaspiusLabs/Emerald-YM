/* Production queue + UART parser + Emerald dispatcher, inert hardware. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define __attribute__(x)
#define FM1_TICKS_PER_US 24u
static uint32_t now;
#define MIDI_INPUT_TIME() now
#define MIDI_UART_SOURCE 0x10u
#define RING_PUBLISH() ((void)0)
#include "../firmware/src/midi_input.h"
#include "midi_uart.c"
#include "../firmware/src/ym/ym_clock.h"
static struct ym_clock yclock={0,0,0,0,0,132,0,0,0,0};
static uint32_t yplaying,ym_overload,ymidi_keys,ymidi_port_keys[2],fm1_ms;
static uint32_t ymidi_out,ymidi_revision,ymidi_discard,ymidi_clock_fault;
enum {B_EDIT=4};
static uint32_t yfx_values[12][3],effect_calls,dirty_calls,reseed_calls;
static uint32_t irq_save(void){return 0;}
static void irq_restore(uint32_t f){(void)f;}
static uint32_t fm1_ticks(void){return now;}
static void ypost(void){if(yclock.mode)yclock.running=yplaying;}
static uint32_t ym_seed(void){return 42;}
static void ym_reseed(uint32_t seed){assert(seed==42);reseed_calls++;}
static void ym_effect(uint32_t s,uint32_t e,uint32_t v){assert(s<12&&e<3&&v<=100);effect_calls++;}
static void ys_changed(uint32_t ms){(void)ms;dirty_calls++;}
/* One ISR-published mailbox; inject completion at a selected get boundary. */
static uint8_t frame[640],reply[128];
static uint32_t frame_len,frame_ready,arrival_after,frame_gets,frame_releases,reply_len,replies;
static int ota_frame_get(const uint8_t**p,uint32_t*n){
 frame_gets++;if(arrival_after&&!--arrival_after)frame_ready=1;
 if(!frame_ready)return 0;*p=frame;*n=frame_len;return 1;
}
static void ota_frame_done(void){assert(frame_ready);frame_ready=0;frame_releases++;}
static int ota_wire_send(const uint8_t*p,uint32_t n){assert(n<=sizeof reply);memcpy(reply,p,n);reply_len=n;replies++;return 0;}
#define OTA_FRAME_MAX 640u
#define FELUCCA_ID "FM-1_68001101"
#include "ota_handshake.h"
static uint32_t audio_cpu_pct,audio_cpu_lifetime_peak,audio_xruns;
static uint32_t ym_bar(void){return 1;}
static struct {uint32_t halves,max_us;} en_dbg;
static struct {uint32_t failed;} bootguard;
#include "../firmware/src/ym/ym_midi.c"
#include "ym_dispatch_iteration.h"
#include "../firmware/src/ym/ym_midi_mode.h"
static void usb(uint32_t s,uint32_t a,uint32_t b){assert(midi_input_put((s<<8)|(a<<16)|(b<<24)|(s>=0xf8?15:s>>4)));}
static void trs(uint32_t b){um_byte(b);}
static void status_frame(void){
 static const uint8_t query[]={0x7d,0x45,0x4d,0x59,1};
 memcpy(frame,query,sizeof query);frame_len=sizeof query;
}
static void diagnostic_race(void){
 /* Reproduce 0.11.0: ISR publishes after yusb_service's empty read;
  * the second consumer ota_service then releases it without replying. */
 status_frame();arrival_after=2;yusb_service();assert(!frame_ready&&!replies);
 ota_service();assert(!frame_ready&&!replies&&frame_releases==1);
 /* Actual production iteration: late arrival survives until next iteration.
  * Both IN/OUT, every status field and handshake still work. */
 for(uint32_t mode=0;mode<2;mode++)for(uint32_t phase=1;phase<=3;phase++){
  ymidi_out=mode;status_frame();arrival_after=phase;
  uint32_t before=replies,gets=frame_gets,releases=frame_releases;
  yplaying=1;audio_cpu_pct=46;audio_cpu_lifetime_peak=72;audio_xruns=0;
  fm1_ms=100255;en_dbg.halves=12345;en_dbg.max_us=4188;bootguard.failed=0;
  for(uint32_t k=0;k<phase;k++)ym_dispatch_iteration();
  assert(replies==before+1&&frame_gets==gets+phase&&frame_releases==releases+1&&!frame_ready);
  assert(reply_len==62&&reply[0]==0xf0&&reply[5]==0x41&&reply[61]==0xf7);
  uint32_t expected[]={1,46,72,0,0,1,42,100255,12345,4188,0};
  for(uint32_t i=0;i<11;i++){
   uint32_t v=0;for(uint32_t j=0;j<5;j++){assert(reply[6+i*5+j]<128);v|=(uint32_t)reply[6+i*5+j]<<(7*j);}
   assert(v==expected[i]);
  }
 }
 /* Recognized OTA handshake and malformed/unknown frames release once;
  * neither can poison the following status request. No flash hooks exist. */
 uint8_t handshake[]={0,0x59,0x11,0,0,0,0xff};
 frame_len=ota_pack7(handshake,sizeof handshake,frame);frame_ready=1;
 uint32_t before=replies;ym_dispatch_iteration();assert(!frame_ready&&replies==before+1);
 uint8_t decoded[64];uint32_t n=ota_unpack7(reply+1,reply_len-2,decoded,sizeof decoded);
 assert(n==34&&decoded[2]==0x11&&!memcmp(decoded+6,FELUCCA_ID,strlen(FELUCCA_ID)));
 for(uint32_t i=0;i<2;i++){
  frame[0]=0x7d;frame_len=i?5:1;frame[1]=0;frame_ready=1;before=replies;
  ym_dispatch_iteration();assert(!frame_ready&&replies==before);
  status_frame();frame_ready=1;ym_dispatch_iteration();assert(replies==before+1);
 }
 ymidi_out=yplaying=0;fm1_ms=0;
 puts("PASS: reproduced competing-consumer status loss; production single-owner dispatch preserves late requests in IN/OUT, all 11 fields, OTA identity and unknown-frame recovery");
}
int main(void){
 diagnostic_race();
 /* Timestamp captured at arrival, not at the delayed UI/main-loop consumer. */
 now=100;usb(0xf8,0,0);assert(midi_in_at[0]==100);now=1000;yusb_service();assert(yclock.last==100);
 usb(0x90,60,100);trs(0x90);trs(60);trs(100);yusb_service();assert(ymidi_keys==(1u<<7));
 usb(0x80,60,0);yusb_service();assert(ymidi_keys==(1u<<7));
 trs(0xb0);trs(123);trs(0);yusb_service();assert(!ymidi_keys);
 usb(0x90,61,100);trs(0x90);trs(62);trs(100);yusb_service();assert(ymidi_port_keys[0]&&ymidi_port_keys[1]);
 trs(0x80);trs(62);trs(0);yusb_service();assert(ymidi_keys==(1u<<8));
 usb(0xb0,120,0);yusb_service();assert(!ymidi_keys);
 trs(0xb2);trs(22);trs(127);yusb_service();assert(yfx_values[2][2]==100&&effect_calls==1&&dirty_calls==1);
 usb(0xfa,0,0);yusb_service();assert(yplaying&&yclock.mode&&reseed_calls==1&&!yclock.events);
 for(int i=0;i<12;i++){now+=500000;usb(0xf8,0,0);yusb_service();}
 assert(yclock.events==2);
 trs(0xfc);trs(0xf8);trs(0xfa);yusb_service();assert(yplaying&&yclock.events==2&&reseed_calls==1);
 usb(0xfc,0,0);yusb_service();assert(!yplaying);
 for(int i=0;i<10;i++){now+=500000;usb(0xf8,0,0);yusb_service();}assert(yclock.events==2);
 usb(0xfb,0,0);yusb_service();assert(yplaying&&reseed_calls==1);
 now+=YC_TIMEOUT_US*24u+1;yusb_service();assert(!yplaying&&yclock.lost);
 usb(0xf8,0,0);yusb_service();assert(!yplaying); /* no surprise restart */
 usb(0xfa,0,0);yusb_service();assert(yplaying&&reseed_calls==2&&!yclock.events);
 for(unsigned i=0;i<MQ;i++)usb(0xf8,0,0);
 assert(!midi_input_put(0xf8ff));yusb_service();assert(!yplaying&&yclock.lost&&!ymidi_keys&&mi_r==mi_w);
 ym_overload=8;usb(0xfa,0,0);yusb_service();assert(!yplaying&&reseed_calls==2);
 ym_overload=0;yc_internal(&yclock);yplaying=1;
 trs(0x90);trs(64);trs(0xf8);trs(100);trs(65);trs(90);yusb_service();
 assert(ymidi_keys==((1u<<11)|(1u<<12))); /* running status + realtime */
 uint32_t before=mi_w;trs(0xf0);trs(0x7d);trs(1);trs(0xf7);assert(mi_w==before);
 uint32_t lost=midi_in_lost;
 for(unsigned i=0;i<UM_RING;i++)um_ring[i]=0x30;
 uart_midi_take(UM_RING+8);assert(midi_in_lost==lost+1);yusb_service();assert(!ymidi_keys);
 trs(0x90);trs(60);trs(100);yusb_service();assert(ymidi_keys==(1u<<7));
 mi_r=mi_w=0xfffffffeu;usb(0x80,60,0);usb(0x90,61,100);usb(0x80,61,0);yusb_service();
 assert(mi_r==mi_w&&ymidi_keys==(1u<<7)); /* the independent TRS note stays held */
 usb(0x90,64,100);ymidi_controls(1u<<B_EDIT);assert(ymidi_out&&!ymidi_keys);yusb_service();assert(!ymidi_keys);
 uint32_t edits=dirty_calls;
 trs(0x90);trs(60);trs(100);usb(0x90,64,100);usb(0xb0,20,127);yusb_service();assert(!ymidi_keys&&dirty_calls==edits);
 uint32_t master_reseeds=reseed_calls;
 usb(0xfa,0,0);usb(0xf8,0,0);usb(0xfb,0,0);usb(0xfc,0,0);yusb_service();
 assert(!yclock.mode&&yplaying&&reseed_calls==master_reseeds); /* Never follow our own echo. */
 ymidi_play();assert(!yplaying);ymidi_play();assert(yplaying&&reseed_calls==master_reseeds+1&&!yclock.mode);
 usb(0x90,64,100);ymidi_controls(1u<<B_EDIT);yusb_service();assert(!ymidi_out&&!ymidi_keys); /* no stale held key */
 usb(0x90,64,100);yusb_service();assert(ymidi_keys==(1u<<11));
 assert(ymidi_revision==2&&dirty_calls==edits);
 usb(0xfa,0,0);yusb_service();assert(yclock.mode&&yplaying); /* Follower retained in IN. */
 usb(0xfc,0,0);yusb_service();assert(!yplaying);
 ymidi_play();assert(yplaying&&yclock.running);ymidi_play();assert(!yplaying&&!yclock.running);
 puts("PASS: production timestamps, USB/TRS isolation, running status, CC, USB transport/clock, timeout, overflow and overload guard; TRS SysEx ignored");
 return 0;
}
