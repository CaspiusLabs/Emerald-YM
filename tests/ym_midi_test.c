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
static uint32_t yfx_values[12][3],effect_calls,dirty_calls,reseed_calls;
static uint32_t irq_save(void){return 0;}
static void irq_restore(uint32_t f){(void)f;}
static uint32_t fm1_ticks(void){return now;}
static void ypost(void){if(yclock.mode)yclock.running=yplaying;}
static uint32_t ym_seed(void){return 42;}
static void ym_reseed(uint32_t seed){assert(seed==42);reseed_calls++;}
static void ym_effect(uint32_t s,uint32_t e,uint32_t v){assert(s<12&&e<3&&v<=100);effect_calls++;}
static void ys_changed(uint32_t ms){(void)ms;dirty_calls++;}
static int ota_frame_get(const uint8_t**p,uint32_t*n){(void)p;(void)n;return 0;}
static void ota_frame_done(void){}
static void ota_wire_send(const uint8_t*p,uint32_t n){(void)p;(void)n;assert(0);}
static uint32_t audio_cpu_pct,audio_cpu_lifetime_peak,audio_xruns;
static uint32_t ym_bar(void){return 1;}
static struct {uint32_t halves,max_us;} en_dbg;
static struct {uint32_t failed;} bootguard;
#include "../firmware/src/ym/ym_midi.c"
static void usb(uint32_t s,uint32_t a,uint32_t b){assert(midi_input_put((s<<8)|(a<<16)|(b<<24)|(s>=0xf8?15:s>>4)));}
static void trs(uint32_t b){um_byte(b);}
int main(void){
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
 puts("PASS: production timestamps, USB/TRS isolation, running status, CC, USB transport/clock, timeout, overflow and overload guard; TRS SysEx ignored");
 return 0;
}
