/* SPDX-License-Identifier: MIT
 * Audio-owner, block-quantized master. Clocks follow actual native sequencer
 * phase, never an independent millisecond timer. At most one Clock per block;
 * missed/still-queued pulses fail closed instead of bursting catch-up clocks.
 * The queue consumer never runs during this audio-owner critical context. */
#pragma once
struct ym_midi_sync {
 uint32_t initialized,used,session,revision,epoch,running,pending_stop;
 uint32_t blocked,last_clock,ticket,has_ticket,recover;
};
static int yms_real(uint32_t status){return YMS_SEND(0x0fu|(status<<8));}
static void yms_block(struct ym_midi_sync*c){
 YMS_DROP(); /* discard unsent Clock/On; publisher will repair note state */
 c->blocked=1;c->running=0;c->has_ticket=0;c->pending_stop=c->used;c->recover=1;
}
/* Returns 1 only when sequencer note output is synchronized. Live keys are
 * independent and may still be exported while the backing is stopped. */
static int yms_update(struct ym_midi_sync*c,const struct ym_midi_state*v,
 uint32_t enabled,uint32_t ready,uint32_t session,uint32_t revision){
 uint32_t restart=!c->initialized||c->revision!=revision||c->epoch!=v->epoch;
 c->recover=0;
 if(restart){
  if(c->running)c->pending_stop=1;
  c->running=0;c->has_ticket=0;c->blocked=0;
 }
 c->initialized=1;c->revision=revision;c->epoch=v->epoch;
 if(c->session!=session||!ready){
  if(c->used)c->pending_stop=1;
  c->running=0;c->has_ticket=0;
  if(enabled&&v->playing)c->blocked=1;
 }
 c->session=session;
 if((!enabled||!v->playing)&&c->running){c->pending_stop=1;c->running=0;c->has_ticket=0;}
 if(!ready)return 0;
 if(c->pending_stop){if(!yms_real(0xfcu))return 0;c->pending_stop=0;}
 if(!enabled||!v->playing||c->blocked)return 0;
 if(!c->running){
  if(v->clocks!=1u){yms_block(c);goto stop;}
  if(!yms_real(0xfau)){yms_block(c);goto stop;}
  c->running=c->used=1;c->last_clock=0;c->has_ticket=0;
 }
 {
  uint32_t delta=v->clocks-c->last_clock;
  if(delta){
   if(delta!=1u||(c->has_ticket&&YMS_PENDING(c->ticket))||!yms_real(0xf8u)){
    yms_block(c);goto stop;
   }
   c->last_clock=v->clocks;c->ticket=YMS_TICKET();c->has_ticket=1;
  }
 }
 return 1;
stop:
 if(c->pending_stop&&yms_real(0xfcu))c->pending_stop=0;
 return 0;
}
