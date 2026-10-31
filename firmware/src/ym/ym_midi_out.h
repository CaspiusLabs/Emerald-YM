/* SPDX-License-Identifier: MIT
 * Audio-owner USB note publisher. Sink returns 1 only after enqueueing a packet.
 * No wait, allocation, MIDI Thru or UART pin changes. Releases retry under
 * backpressure; new notes wait until all obsolete notes have been released. */
#pragma once
#include "ym_engine.h"
#ifndef YMO_SOURCE_ENABLED
#define YMO_SOURCE_ENABLED(s) 1u
#endif
struct ym_midi_output {
 uint32_t initialized,session,revision,epoch,enabled,used,panic,serial[12];
 int32_t note[12],live_note[27];
};
static uint32_t ymo_channel(uint32_t s){return s<5u?s+1u:s<9u?9u:s==9u?6u:7u;}
static int ymo_note(uint32_t channel,int32_t note,uint32_t on){
 uint32_t status=(on?0x90u:0x80u)|channel;
 return YMO_SEND((status>>4)|(status<<8)|((uint32_t)note<<16)|((on?90u:0u)<<24));
}
static void ymo_reset(struct ym_midi_output*m){
 for(uint32_t s=0;s<12u;s++)m->note[s]=-1;
 for(uint32_t k=0;k<27u;k++)m->live_note[k]=-1;
 m->initialized=1;m->enabled=0;
}
static void ymo_update(struct ym_midi_output*m,const struct ym_midi_state*v,
 uint32_t enabled,uint32_t ready,uint32_t session,uint32_t revision,
 uint32_t keys,int32_t transpose){
 static const int32_t percussion[4]={36,38,42,46};
 int32_t wanted[12];uint32_t release_all,releases=1;
 if(!m->initialized)ymo_reset(m);
 if(!ready){if(m->used)m->panic=9u;return;}
 if(m->session!=session){if(m->used)m->panic=9u;m->session=session;}
 /* After suspend/reset, queued packets may have been discarded by USB.
  * All Notes Off on our nine channels repairs uncertainty before any On. */
 while(m->panic){
  uint32_t channel=m->panic==9u?9u:m->panic-1u;
  if(!YMO_SEND(0xbu|((0xb0u|channel)<<8)|(123u<<16)))return;
  if(!--m->panic)ymo_reset(m);
 }
 if(enabled)m->used=1;
 release_all=!enabled||m->revision!=revision||m->epoch!=v->epoch;
 for(uint32_t s=0;s<12u;s++){
  wanted[s]=enabled&&YMO_SOURCE_ENABLED(s)?v->note[s]:-1;
  /* A very short drum may finish within the block containing its trigger.
   * Export its pulse once, then release on the next block. Never replay a
   * stale drum on enable/reconnect/reseed. */
  if(enabled&&YMO_SOURCE_ENABLED(s)&&v->playing&&m->enabled&&!release_all&&s>=5u&&s<9u&&v->serial[s]!=m->serial[s])wanted[s]=percussion[s-5u];
  if(m->note[s]>=0&&(release_all||m->note[s]!=wanted[s]||v->serial[s]!=m->serial[s])){
   if(ymo_note(ymo_channel(s),m->note[s],0))m->note[s]=-1;else releases=0;
  }
 }
 for(uint32_t k=0;k<27u;k++)if(m->live_note[k]>=0){
  if(release_all||!(keys&(1u<<k))||m->live_note[k]!=(int32_t)(53u+k)+transpose){
   if(ymo_note(0,m->live_note[k],0))m->live_note[k]=-1;else releases=0;
  }
 }
 if(!releases)return;
 m->revision=revision;m->epoch=v->epoch;
 if(enabled){
  for(uint32_t s=0;s<12u;s++){
   if(wanted[s]<0||m->note[s]==wanted[s])m->serial[s]=v->serial[s];
   else if(ymo_note(ymo_channel(s),wanted[s],1)){m->note[s]=wanted[s];m->serial[s]=v->serial[s];}
  }
  for(uint32_t k=0;k<27u;k++)if((keys&(1u<<k))&&m->live_note[k]<0){
   int32_t n=(int32_t)(53u+k)+transpose;
   if(ymo_note(0,n,1))m->live_note[k]=n;
  }
 }
 m->enabled=enabled;
}
