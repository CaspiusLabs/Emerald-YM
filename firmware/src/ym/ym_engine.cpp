/* SPDX-License-Identifier: MIT
 * Emerald YM: bounded-memory YM2612 generative engine. No full-song buffers,
 * no event vectors, no audio-thread allocation, no floating-point libm. */
#include "ym_engine.h"
#include "ymfm/ymfm_opn.h"
#include "ym_tables.h"
/* Main-loop-only leaf controls run in XIP; the renderer never calls them.
 * Keep the small audio RAM region for synthesis, without changing its map. */
#if defined(__clang__)
#define YM_CONTROL_CODE __attribute__((section(".ym_control")))
#else
#define YM_CONTROL_CODE
#endif
uint32_t emerald_fm_gain_q8=256;
static volatile uint32_t req_fm_gain_q8=256,req_drum_gain_q8=256;
static uint32_t drum_gain_q8=256;

alignas(8) static uint8_t arena[12288];
static uint32_t arena_used;
extern "C" void ym_unavailable(void){for(;;){} } // watchdog returns device to safe mode
void *operator new(size_t n){n=(n+7)&~size_t(7);if(n>sizeof(arena)-arena_used)ym_unavailable();void*p=arena+arena_used;arena_used+=uint32_t(n);return p;}
void *operator new(size_t,void*p) noexcept{return p;}
void operator delete(void*) noexcept{}
void operator delete(void*,size_t) noexcept{}
extern "C" void __cxa_pure_virtual(){ym_unavailable();}
alignas(8) static uint8_t interface_mem[sizeof(ymfm::ymfm_interface)];
alignas(8) static uint8_t chip_mem[sizeof(ymfm::ym2612)];
static ymfm::ym2612 *chip;
static uint32_t state,initial_seed,steps,step_phase,resample_phase,native_count,notes;
static uint32_t hold[5];
static int8_t motifs[4][8];
static int32_t prev_l,prev_r,next_l,next_r;
static int32_t last_dac;
static float dc_l,dc_r,gain,sequence_gain;
static int16_t fm_note[5];
static uint32_t tempo=132,density=100,mutation=35,playing=1;
static int32_t transpose;
static volatile uint32_t req_tempo=132,req_density=100,req_mutation=35,req_play=1,req_seed;
static volatile int32_t req_transpose;
#include "ym_psg.h"
static uint32_t random32(){state^=state<<13;state^=state>>17;state^=state<<5;return state;}
static uint32_t pick(uint32_t n){return random32()%n;}
static void wr(int r,int v){chip->write(r>=256?2:0,uint8_t(r));chip->write(r>=256?3:1,uint8_t(v));}
static void cw(int c,int r,int v){wr((c>=3?256:0)+r+c%3,v);}
static int key(int c){return c%3+(c>=3?4:0);}
static void off(int c){wr(0x28,key(c));hold[c]=0;fm_note[c]=-1;}
struct Patch {uint8_t fb,mul[4],tl[4],ar[4],dr[4],sr[4],sl[4],rr[4];};
static const Patch bass={5,{1,1,1,1},{29,7,37,12},{31,31,31,31},{17,13,19,14},{5,6,5,6},{5,5,5,5},{10,10,10,10}};
static const Patch lead={3,{2,1,1,1},{34,9,42,15},{31,30,31,30},{13,9,16,10},{3,2,4,2},{5,3,6,3},{10,10,10,10}};
static const Patch keys={4,{1,1,3,1},{32,19,43,25},{31,31,31,31},{19,15,22,17},{7,6,8,7},{8,7,8,7},{11,11,11,11}};
static void patch(int c,const Patch&p,int pan){
 static const uint8_t slots[]={0,8,4,12};
 for(int i=0;i<4;i++){int s=slots[i];cw(c,0x30+s,p.mul[i]);cw(c,0x40+s,p.tl[i]);cw(c,0x50+s,p.ar[i]);cw(c,0x60+s,p.dr[i]);cw(c,0x70+s,p.sr[i]);cw(c,0x80+s,(p.sl[i]<<4)|p.rr[i]);cw(c,0x90+s,0);}
 cw(c,0xb0,(p.fb<<3)|4);cw(c,0xb4,pan);
}
static void note(int c,int n,uint32_t gate){
 n+=transpose;if(n<12)n=12;if(n>115)n=115;int f=ym_pitch[n];off(c);
 cw(c,0xa4,f>>8);cw(c,0xa0,f&255);wr(0x28,0xf0|key(c));hold[c]=gate;fm_note[c]=(int16_t)n;notes++;
}
struct Drum {const int8_t*p;uint32_t len,pos,level;};
static Drum drums[4];
static void hit(int d,uint32_t level){drums[d].pos=0;drums[d].level=level;}
static void make_motifs(){
 static const int8_t base[4][8]={{2,4,5,4,2,1,0,1},{1,2,4,3,2,1,2,4},{4,5,6,5,4,2,1,0},{3,2,1,0,1,2,4,3}};
 uint32_t rot=pick(4);for(int m=0;m<4;m++)for(int k=0;k<8;k++)motifs[m][k]=base[(m+rot)%4][k];
 for(int m=0;m<4;m++){int k=1+pick(6),n=motifs[m][k]+(pick(2)?1:-1);motifs[m][k]=n<0?0:n>6?6:n;}
}
static void restart(uint32_t seed){
 state=initial_seed=seed?seed:1;steps=step_phase=notes=0;psg_reset();
 chip->reset();wr(0x22,0);wr(0x27,0);wr(0x2a,128);wr(0x2b,128);last_dac=128;
 for(int c=0;c<5;c++)off(c);
 patch(0,bass,0xc0);patch(1,lead,0xc0);patch(2,keys,0x80);patch(3,keys,0xc0);patch(4,keys,0x40);cw(5,0xb4,0xc0);
 drums[0]={ym_kick,sizeof(ym_kick),sizeof(ym_kick),100};drums[1]={ym_snare,sizeof(ym_snare),sizeof(ym_snare),100};
 drums[2]={ym_hat,sizeof(ym_hat),sizeof(ym_hat),100};drums[3]={ym_open,sizeof(ym_open),sizeof(ym_open),100};make_motifs();
}
static void tick(){
 uint32_t b=steps/16,pos=steps%16,section=b%32;bool intro=section<4,bridge=section>=16&&section<24;
 static const int8_t roots[]={40,37,45,47,44,37,42,47},ci[]={1,1,2,3,1,4,6,7};
 static const int8_t chords[8][3]={{64,68,71},{64,68,71},{61,64,68},{63,66,69},{63,68,71},{64,68,71},{61,64,69},{63,66,69}};
 static const int8_t scale[]={64,66,68,69,71,73,75};
 uint32_t idx=bridge?ci[b%8]:b%8,unit=(7670454u/144u)*15u/tempo;
 if(pos==0&&b&&b%8==0&&pick(100)<mutation){int m=pick(4),k=1+pick(6),n=motifs[m][k]+(pick(2)?1:-1);motifs[m][k]=n<0?0:n>6?6:n;}
 static const uint8_t bp[]={0,3,4,7,10,12,14,15};
 for(int k=0;k<8;k++)if(pos==bp[k]&&!(intro&&k==7)&&!(bridge&&k==3)){
  int n=roots[idx]+(k==1||k==5?12:k==3||k==6?7:0);
  if(k==7)n=roots[(idx+1)%8]+(pick(2)?-1:1);
  note(0,n,unit*(k==0?7:4)/5);
 }
 if(!intro||section>=2)if(pos==0||pos==3||(!bridge&&(pos==6||pos==11)))for(int v=0;v<3;v++)note(v+2,chords[idx][v],unit*4/5);
 static const uint8_t ra[]={0,2,5,6,8,11,12,14},rb[]={0,3,4,6,9,10,12,14};const uint8_t*rh=b%2?rb:ra;
 if(!intro)for(int k=0;k<8;k++)if(pos==rh[k]&&!(bridge&&(k==1||k==5))&&(k==0||pick(100)<density)){
  int degree=motifs[b%4][k];if(section>=24&&k==5&&degree<6)degree++;
  int n=(k==0||k==4?chords[idx][(b/4+k)%3]:scale[degree])+(bridge?0:12);
  int next=k<7?rh[k+1]:16;note(1,n,(next-rh[k])*unit*4/5);
 }
 if(section==3&&pos>=8&&!(pos&1))note(1,scale[3+(pos-8)/2]+12,unit*4/5);
 if(pos==0||pos==10||( !bridge&&(pos==6||pos==8||pos==14)))hit(0,intro?85:100);
 if(bridge?pos==8:(pos==4||pos==12))hit(1,100);
 if(!(pos&1))hit(pos==14?3:2,pos%4?65:90);
 if(b%8==7&&pos>=12)hit(1,35+(pos-12)*15);
 if(req_psg){
  /* Stable up/down chord arp, one octave below the original short bleeps. */
  static const uint8_t arp[]={0,1,2,1,0,1,2,1};
  if((pos&1u)==0u){psg_note=(uint8_t)(chords[idx][arp[pos/2]]+transpose);psg_gate=unit*7/4;}
  if((pos&3u)==2u){psg_lfsr=0x8000;psg_noise_hold=2205;}
 }
 psg_targets();
 steps++;
}
static void native(){
 if(playing){
  if(psg_gate&&!--psg_gate)psg_note=255;
  for(int c=0;c<5;c++)if(hold[c]&&!--hold[c])off(c);
  if(steps==0)tick();step_phase+=tempo*4u*144u;if(step_phase>=7670454u*60u){step_phase-=7670454u*60u;tick();}
 }
 if(!(native_count++&3)){
  int v=0;for(int d=0;d<4;d++)if(drums[d].pos<drums[d].len){v+=int(drums[d].p[drums[d].pos++])*int(drums[d].level)/100;}
  if(!playing)v=0;if(v>124)v=124;if(v< -124)v= -124;
  v=v*(int32_t)drum_gain_q8/256;
  if(128+v!=last_dac){last_dac=128+v;wr(0x2a,last_dac);}
 }
 ymfm::ym2612::output_data o;chip->generate(&o);next_l=o.data[0];next_r=o.data[1];
}
extern "C" void ym_init(uint32_t seed){
 // Cold initialization only, before the device enables its audio IRQ. Chip reset
 // intentionally leaves some free-running emulation counters unchanged; reconstruct
 // here so separate cold starts are sample-exact without changing upstream emulation.
 arena_used=0;auto*iface=new(interface_mem)ymfm::ymfm_interface;chip=new(chip_mem)ymfm::ym2612(*iface);
 req_seed=req_keys=0;req_psg=1;req_psg_level=300;req_fm_gain_q8=req_drum_gain_q8=emerald_fm_gain_q8=drum_gain_q8=256;drum_gain=1.0f;old_keys=live_mask=sequence_mask=key_clock=0;old_psg=1;
 for(int k=0;k<27;k++)key_age[k]=0;sequence_gain=0;
 playing=req_play=1;tempo=req_tempo=132;density=req_density=100;mutation=req_mutation=35;transpose=req_transpose=0;
 resample_phase=native_count=0;prev_l=prev_r=next_l=next_r=0;dc_l=dc_r=gain=0;restart(seed);
}
extern "C" YM_CONTROL_CODE void ym_control(uint32_t t,uint32_t d,uint32_t m,int32_t tr,uint32_t p){req_tempo=t<80?80:t>180?180:t;req_density=d>100?100:d;req_mutation=m>100?100:m;req_transpose=tr< -12?-12:tr>12?12:tr;req_play=p!=0;}
extern "C" YM_CONTROL_CODE void ym_reseed(uint32_t s){req_seed=s?s:1;}
extern "C" YM_CONTROL_CODE void ym_live_keys(uint32_t keys){req_keys=keys&0x07ffffffu;}
extern "C" YM_CONTROL_CODE void ym_psg_enable(uint32_t enabled){req_psg=enabled!=0;}
extern "C" YM_CONTROL_CODE void ym_psg_level(uint32_t percent){req_psg_level=percent>400?400:percent;}
extern "C" YM_CONTROL_CODE void ym_mix_levels(uint32_t melody,uint32_t drums){
 req_fm_gain_q8=((melody>100?100:melody)*256u+50u)/100u;
 req_drum_gain_q8=((drums>100?100:drums)*256u+50u)/100u;
}
extern "C" YM_CONTROL_CODE uint32_t ym_live_active_keys(){return live_mask;}
extern "C" YM_CONTROL_CODE uint32_t ym_sequence_keys(){return sequence_mask;}
extern "C" YM_CONTROL_CODE uint32_t ym_bar(){return steps?((steps-1)/16)+1:1;}
extern "C" YM_CONTROL_CODE uint32_t ym_step(){return steps?(steps-1)%16u:0u;}
extern "C" YM_CONTROL_CODE uint32_t ym_seed(){return initial_seed;}
extern "C" uint32_t ym_arena_bytes(){return arena_used;}
extern "C" uint32_t ym_note_count(){return notes;}
extern "C" void ym_render(int32_t*out,uint32_t frames,uint32_t master){
 tempo=req_tempo;density=req_density;mutation=req_mutation;transpose=req_transpose;
 uint32_t newseed=req_seed;if(newseed){req_seed=0;restart(newseed);}
 if(playing&&!req_play){for(int c=0;c<5;c++)off(c);}playing=req_play;
 emerald_fm_gain_q8=req_fm_gain_q8;drum_gain_q8=req_drum_gain_q8;drum_gain=float(drum_gain_q8)*(1.0f/256.0f);
 live_update();psg_targets();
 float target=float(master)/4096.0f,psg_level=float(req_psg_level)*0.01f;
 for(uint32_t i=0;i<frames;i++){
  resample_phase+=7670454u;while(resample_phase>=44100u*144u){prev_l=next_l;prev_r=next_r;native();resample_phase-=44100u*144u;}
  if(psg_gate==0&&psg_tone[2].target&&live_key[2]==255)psg_tone[2].target=0;
  float alpha=float(resample_phase)*(1.0f/6350400.0f);
  float l=float(prev_l)+float(next_l-prev_l)*alpha,r=float(prev_r)+float(next_r-prev_r)*alpha;
  dc_l+=(l-dc_l)*0.00256f;dc_r+=(r-dc_r)*0.00256f;gain+=(target-gain)*0.001f;
  sequence_gain+=((playing?1.0f:0.0f)-sequence_gain)*0.001f;
  float psg=psg_sample()*psg_level;
  l=((l-dc_l)*sequence_gain*0.85f+psg)*gain*512.0f;r=((r-dc_r)*sequence_gain*0.85f+psg)*gain*512.0f;
  out[2*i]=int32_t(l>8300000.0f?8300000.0f:l< -8300000.0f?-8300000.0f:l);
  out[2*i+1]=int32_t(r>8300000.0f?8300000.0f:r< -8300000.0f?-8300000.0f:r);
 }
 uint32_t mask=0;
 if(playing){for(int c=0;c<5;c++)if(hold[c]&&fm_note[c]>=53&&fm_note[c]<=79)mask|=1u<<(fm_note[c]-53);
 if(req_psg&&psg_gate&&live_key[2]==255&&psg_note>=53&&psg_note<=79)mask|=1u<<(psg_note-53);}
 sequence_mask=mask;
}
