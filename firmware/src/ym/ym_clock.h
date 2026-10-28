/* SPDX-License-Identifier: GPL-3.0-only
 * Main-loop USB MIDI clock follower. Unsigned TIMER4 timestamps, no allocation.
 * Scheduling counts pulses; BPM is only a gate-length/display estimate. */
#ifndef EMERALD_YM_CLOCK_H
#define EMERALD_YM_CLOCK_H
#include <stdint.h>
#define YC_TIMEOUT_US 500000u
struct ym_clock {uint32_t mode,running,lost,phase,events,bpm,seen,last,base,count;};
static void yc_internal(struct ym_clock*c){c->mode=c->running=c->lost=c->phase=c->events=0;}
static void yc_arm(struct ym_clock*c,uint32_t now){c->mode=1;c->running=1;c->lost=0;c->last=now;c->seen=c->count=0;}
static void yc_fail(struct ym_clock*c){if(c->mode){c->running=0;c->lost=1;}c->seen=c->count=0;}
static int yc_expire(struct ym_clock*c,uint32_t now,uint32_t ticks_per_us){
 if(c->mode&&c->running&&(uint32_t)(now-c->last)>YC_TIMEOUT_US*ticks_per_us){yc_fail(c);return 1;}return 0;
}
/* Returns 2 for Start/reset, 1 for another transport change, 0 for clock/data. */
static uint32_t yc_usb(struct ym_clock*c,uint32_t status,uint32_t at,uint32_t ticks_per_us){
 if(status==0xfau){yc_arm(c,at);c->phase=c->events=0;return 2;}
 if(status==0xfbu){yc_arm(c,at);return 1;}
 if(status==0xfcu){c->running=0;return 1;}
 if(status!=0xf8u)return 0;
 if(!c->seen||(uint32_t)(at-c->last)>YC_TIMEOUT_US*ticks_per_us){c->base=at;c->count=0;c->seen=1;}
 else if(++c->count==24u){
  uint32_t us=(uint32_t)(at-c->base)/ticks_per_us;
  if(us>=200000u&&us<=2000000u)c->bpm=(60000000u+us/2u)/us;
  c->base=at;c->count=0;
 }
 c->last=at;
 if(c->mode&&c->running){if(c->phase==0)c->events++;if(++c->phase==6u)c->phase=0;}
 return 0;
}
#endif
