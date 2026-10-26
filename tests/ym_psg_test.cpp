/* Independent clock/frequency checks of the Sega PSG musical profile. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
static int32_t transpose;
static uint32_t playing;
#include "../firmware/src/ym/ym_psg.h"
static void check(int v,const char*s){printf("%s: %s\n",s,v?"PASS":"FAIL");if(!v)exit(2);}
int main(){
 int valid=1;for(int n=0;n<128;n++)if(psg_divider[n]<9||psg_divider[n]>1023)valid=0;
 check(valid,"supported MIDI range guarantees at most one edge per sample");
 const int pitches[]={0,36,53,69,79,96,115,127};
 int accurate=1;
 for(int p=0;p<8;p++){
  psg_reset();psg_pitch(0,pitches[p],0);
  for(int i=0;i<4410;i++){
   int32_t remaining=psg_tone[0].remaining,span=3579545,area=0;unsigned high=psg_tone[0].high;
   while(span){int32_t part=remaining<span?remaining:span;area+=high?part:-part;span-=part;remaining-=part;if(!remaining){remaining=(int32_t)psg_tone[0].period;high^=1;}}
   float value=psg_sample(),expected=float(area)*(1.0f/3579545.0f)*float(psg_tone[0].level);
   if(value-expected>0.001f||expected-value>0.001f)accurate=0;
  }
 }
 check(accurate,"fast edge integration matches independent reference across pitch range");
 psg_reset();psg_pitch(0,69,0);int crossings=0,previous=0;
 for(int i=0;i<44100;i++){float sample=psg_sample();int sign=sample>0;if(i&&sign!=previous)crossings++;previous=sign;}
 check(crossings>=879&&crossings<=882,"A4: hardware divider 254 gives about 440.4 Hz");
 check(psg_tone[0].period==254u*705600u,"NTSC tone clock quantized to ten-bit divider");
 psg_reset();psg_noise_hold=44100;playing=1;
 int energy=0;for(int i=0;i<1000;i++)if(psg_sample()!=0)energy++;
 check(energy>800&&psg_lfsr!=0,"white noise is audible and LFSR never locks at zero");
 psg_reset();uint16_t first=psg_lfsr;unsigned shifts=0;
 do{unsigned feedback=(psg_lfsr^(psg_lfsr>>3))&1u;psg_lfsr=(uint16_t)((psg_lfsr>>1)|(feedback<<15));shifts++;}while(psg_lfsr!=first&&shifts<65536);
 check(shifts==57337,"Sega taps 0/3: known 57337-step white-noise cycle");
 return 0;
}
