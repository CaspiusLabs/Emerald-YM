/* SPDX-License-Identifier: MIT
 * Control-rate arrangement. No state or work in the oscillator sample loops. */
struct ym_song_form {uint32_t enabled,done,phase,local,length,section,density,mask,gain,fill;};
static struct ym_song_form ysong={1,0,0,1,8,0,55,1,100,0};
static uint32_t ysong_reset_pending;
static volatile uint32_t ys_song_output_mask=0xfffu;
static const char*const ys_song_names[]={"INTRO","VERSE","CHORUS","BREAK","OUTRO","END"};
static void ys_song_profile(struct ym_song_form*s,uint32_t bar,uint32_t step){
 static const uint8_t lengths[]={8,16,16,8,16,16,8};
 static const uint8_t phases[]={0,1,2,3,1,2,4};
 uint32_t b=bar?bar-1u:0u,part=0;
 s->gain=100;s->mask=0xfffu;s->density=100;s->section=255;s->fill=0;
 if(!s->enabled){s->done=0;s->phase=0;s->local=bar;s->length=0;return;}
 while(part<7u&&b>=lengths[part]){b-=lengths[part];part++;}
 s->done=part==7u;
 if(s->done){s->phase=5;s->local=0;s->length=0;s->mask=1u<<10;return;}
 s->phase=phases[part];s->local=b+1u;s->length=lengths[part];
 s->fill=b+1u==s->length&&step>=12u;
 switch(s->phase){
 case 0:s->section=b<4u?0u:b<7u?2u:3u;s->density=55;
  s->mask=(1u<<0)|(1u<<10);if(b>=2)s->mask|=(1u<<5)|(1u<<7);
  if(b>=4)s->mask|=(7u<<2)|(1u<<9);break;
 case 1:s->section=4;s->density=65;s->mask=0xfffu&~(1u<<8);break;
 case 2:s->section=24;s->density=100;break;
 case 3:s->section=16;s->density=45;s->mask=(1u<<0)|(7u<<2)|(1u<<9)|(1u<<10)|(1u<<11);break;
 default:s->section=b<4u?4u:2u;s->density=50;
  s->gain=(8u-b)*100u/8u;break;
 }
 if(s->fill)s->mask|=(1u<<1)|(1u<<6)|(1u<<8)|(1u<<11);
}
static uint32_t ys_song_level(const struct ym_song_form*s,uint32_t source,uint32_t level){
 if(source==10u)return level; /* Physical/live MIDI keys never fade with the song. */
 return s->enabled?((s->mask&(1u<<source))?level*s->gain/100u:0u):level;
}
