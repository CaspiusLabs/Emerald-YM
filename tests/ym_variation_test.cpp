/* Verify pure song-planning decisions without FM-1 hardware. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../firmware/src/ym/ym_variation.h"

static uint32_t signature(uint32_t seed){
 uint32_t s=ym_var_style(seed),h=2166136261u;
 for(uint32_t bar=0;bar<88u;bar++){
  const uint32_t section=bar<8u?0u:bar<24u?4u:bar<40u?24u:
                         bar<48u?16u:bar<64u?4u:bar<80u?24u:4u;
  const ym_var_drums*g=ym_var_drum_pattern(s,bar,section);
  uint32_t values[]={ym_var_chord(s,bar,section),
   ym_var_motif(s,bar,bar&7u),g->kick,g->snare,g->hat,g->open,
   (uint32_t)ym_var_lead_event(bar,3u),
   (uint32_t)ym_var_bass_event(s,bar,7u)};
  for(unsigned i=0;i<sizeof(values)/sizeof(values[0]);i++){h^=values[i];h*=16777619u;}
 }
 return h;
}
int main(){
 const uint32_t seeds[]={20261026u,20261027u,1u,2u,3u,0xffffffffu};
 uint32_t seen=0,h[6]={};
 for(unsigned i=0;i<6u;i++){
  const uint32_t style=ym_var_style(seeds[i]);
  assert((style&~511u)==0u);
  for(unsigned bar=0;bar<88u;bar++){
   uint32_t section=bar>=40u&&bar<48u?16u:bar>=24u&&bar<40u?24u:4u;
   const ym_var_drums*g=ym_var_drum_pattern(style,bar,section);
   const uint8_t*rh=ym_var_lead_rhythm(style,bar);
   assert(ym_var_chord(style,bar,section)<8u);
   assert(ym_var_chord(style,bar,section)==ym_var_chord(style,bar,section));
   assert((g->kick&1u)!=0u&&(g->snare&0x1010u)!=0u);
   for(unsigned k=0;k<8u;k++){
    assert(ym_var_motif(style,bar,k)<7u);
    assert(rh[k]<16u);
    if(k)assert(rh[k]>rh[k-1]);
   }
   assert(ym_var_lead_event(bar,0u));
   assert(ym_var_bass_event(style,bar,0u));
  }
  h[i]=signature(seeds[i]);assert(h[i]==signature(seeds[i]));
  seen|=1u<<(style&3u);
 }
 unsigned different=0;
 for(unsigned i=1;i<6u;i++)if(h[i]!=h[0])different++;
 assert(different>=4u&&seen!=0u);
 assert(signature(0u)==signature(1u)); /* zero seed normalized consistently */
 assert(!ym_var_lead_event(4u,3u)&&ym_var_lead_event(0u,3u));
 for(unsigned k=0;k<4u;k++){
  for(unsigned i=0;i<8u;i++)assert(ym_var_progressions[k][i]<8u);
  assert((ym_var_grooves[k].kick&1u)!=0u);
 }
 puts("PASS: deterministic seeds, distinct 88-bar harmonic/motif/groove signatures, phrase rests and guarded note patterns");
 return 0;
}
