/* SPDX-License-Identifier: MIT
 * Sega PSG musical profile: 3 ten-bit tone dividers, 2 dB attenuation,
 * 16-bit noise LFSR (taps 0 and 3), NTSC 3579545 Hz clock.
 * Integrates square/noise edges over each output sample. No oversampling
 * loop at the chip clock and no libm. This is not a general VGM/register API.
 * Noise uses the fixed clock/512 white-noise mode. Musical envelopes and
 * voice allocation are software controls, as on a console sound driver.
 */
#pragma once
static const uint16_t psg_attenuation[16]={640,508,404,321,255,202,161,128,101,81,64,51,40,32,25,0};
struct PSGTone { int32_t remaining; uint32_t period; int32_t level,target; uint8_t high; };
static PSGTone psg_tone[3];
static uint32_t psg_noise_remaining,psg_noise_hold;
static uint16_t psg_lfsr;
static uint8_t psg_note=255;
static uint32_t psg_gate;
static int32_t psg_noise_level;
/* Load the positive clock step from RAM: this old compiler otherwise encodes
 * subtract-immediate as a ROM-window literal, rejected by our strict guard. */
static volatile int32_t psg_clock_step=3579545;
static volatile uint32_t req_keys,req_psg=1,req_psg_level=300;
static uint32_t old_keys,old_psg,key_age[27],key_clock,live_mask,sequence_mask;
static uint8_t live_key[3];
/* Generated offline: round(3579545/(32*440*2^((n-69)/12))), clamped
 * to the hardware's ten-bit range. F3..G5 keyboard + transposition. */
static const uint16_t psg_divider[128]={
1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1017,960,906,855,807,762,719,679,641,605,571,539,508,480,453,428,404,381,360,339,320,302,285,269,254,240,226,214,202,190,180,170,160,151,143,135,127,120,113,107,101,95,90,85,80,76,71,67,64,60,57,53,50,48,45,42,40,38,36,34,32,30,28,27,25,24,22,21,20,19,18,17,16,15,14,13,13,12,11,11,10,9,9
};
static void psg_reset(){
 old_psg=2; /* Rebuild live assignments after reseeding even if keys did not change. */
 for(int c=0;c<3;c++){psg_tone[c]={705600,705600,0,0,1};live_key[c]=255;}
 psg_noise_remaining=512u*44100u;psg_noise_hold=0;psg_lfsr=0x8000;
 psg_note=255;psg_gate=0;psg_noise_level=0;
}
static void psg_pitch(int c,int n,int attenuation){
 if(n<0)n=0;if(n>127)n=127;
 uint32_t period=psg_divider[n]*705600u;
 psg_tone[c].period=period;
 if(psg_tone[c].remaining>(int32_t)period)psg_tone[c].remaining=(int32_t)period;
 psg_tone[c].target=psg_attenuation[attenuation];
}
static void live_update(){
 uint32_t keys=req_keys&0x07ffffffu,added=keys&~old_keys;
 if(keys==old_keys&&old_psg==req_psg)return;old_psg=req_psg;
 for(int k=0;k<27;k++)if(added&(1u<<k))key_age[k]=++key_clock;
 old_keys=keys;live_mask=0;
 for(int c=0;c<3;c++){
  int best=-1;
  for(int k=0;k<27;k++)if((keys&~live_mask)&(1u<<k))
   if(best<0||(int32_t)(key_age[k]-key_age[best])>0)best=k;
  live_key[c]=best<0?255:(uint8_t)best;
  if(best>=0)live_mask|=1u<<best;
 }
 if(!req_psg)live_mask=0;
}
static void psg_targets(){
 for(int c=0;c<3;c++){
  if(req_psg&&live_key[c]!=255)psg_pitch(c,53+live_key[c]+transpose,2);
  else if(req_psg&&playing&&c==2&&psg_gate)psg_pitch(c,psg_note,4);
  else psg_tone[c].target=0;
 }
}
static inline float psg_sample(){
 int32_t clock_step=psg_clock_step;
 float mixed=0;
 for(int c=0;c<3;c++){
  PSGTone &t=psg_tone[c];
  int32_t diff=t.target-t.level;
  if(diff)t.level+=diff>0?(diff+15)/16:(diff-15)/16;
  if(!t.level)continue;
  /* Every supported MIDI pitch has divider >=9, so there can be
   * at most one edge per 44.1 kHz sample. Most samples have none. */
  t.remaining-=clock_step;
  float value=float(t.high?t.level:-t.level);
  if(t.remaining<=0){
   value*=1.0f+float(t.remaining)*(2.0f/3579545.0f);
   t.remaining+=(int32_t)t.period;t.high^=1;
  }
  mixed+=value;
 }
 if(psg_noise_hold)psg_noise_hold--;
 int32_t target=req_psg&&playing&&psg_noise_hold?64:0;
 int32_t diff=target-psg_noise_level;
 if(diff)psg_noise_level+=diff>0?(diff+15)/16:(diff-15)/16;
 if(psg_noise_level){
  int32_t remaining=(int32_t)psg_noise_remaining-clock_step;
  uint32_t bit=psg_lfsr&1u;
  float value=float(bit?psg_noise_level:-psg_noise_level);
  if(remaining<=0){
   psg_lfsr=(uint16_t)((psg_lfsr>>1)|(((psg_lfsr^(psg_lfsr>>3))&1u)<<15));
   if(bit!=(psg_lfsr&1u))value*=1.0f+float(remaining)*(2.0f/3579545.0f);
   remaining+=512u*44100u;
  }
  psg_noise_remaining=(uint32_t)remaining;mixed+=value;
 }
 return mixed;
}
