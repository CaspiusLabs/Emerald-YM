/* SPDX-License-Identifier: MIT
 * Fixed-memory per-source controls and a shared stereo, half-rate echo bus.
 * All functions run from copied RAM. No allocation or libm in audio.
 */
#pragma once
#if defined(__clang__)
#define FX_CODE __attribute__((section(".audio_hot.fx"),noinline))
#define FX_POOL __attribute__((section(".pool"),aligned(8)))
#else
#define FX_CODE
#define FX_POOL
#endif
static volatile uint32_t fx_requested[12][3],fx_dirty;
static uint32_t fx_values[12][3],fx_gate[12],fx_color_mask[4];
static float fx_ring_depth[2];
static uint32_t fx_noise_period=512u*44100u,fx_live_frames;
static uint32_t fx_clock,fx_keyon[27],fx_echo_any,fx_tail;
static int16_t fx_delay[4096][2] FX_POOL;
static uint32_t fx_tags[4096] FX_POOL;
static uint32_t fx_epoch=1,fx_pos,fx_half;
static float fx_input_l,fx_input_r,fx_return_l,fx_return_r;
static float fx_pg_send,fx_pg_gain[3];
int32_t emerald_echo_gain[5],emerald_echo_left,emerald_echo_right;
static int32_t fx_drum_send,fx_drum_echo[4];
static int32_t fx_prev_l,fx_prev_r,fx_next_l,fx_next_r;

static FX_CODE void fx_reset_audio(){
 if(++fx_epoch==0)fx_epoch=1;
 fx_pos=fx_half=fx_tail=0;fx_input_l=fx_input_r=fx_return_l=fx_return_r=0;
 fx_drum_send=fx_prev_l=fx_prev_r=fx_next_l=fx_next_r=0;
 emerald_echo_left=emerald_echo_right=0;
}
static FX_CODE void fx_init(){
 for(int i=0;i<4096;i++)fx_tags[i]=0;
 for(int s=0;s<12;s++){for(int e=0;e<3;e++)fx_requested[s][e]=fx_values[s][e]=0;fx_gate[s]=256;}
 for(int i=0;i<5;i++)emerald_echo_gain[i]=0;
 for(int i=0;i<4;i++){fx_color_mask[i]=255;fx_drum_echo[i]=0;}
 for(int i=0;i<3;i++)fx_pg_gain[i]=0;
 for(int i=0;i<27;i++)fx_keyon[i]=0;
 fx_ring_depth[0]=fx_ring_depth[1]=0;fx_noise_period=512u*44100u;
 fx_dirty=fx_echo_any=fx_live_frames=fx_clock=0;fx_epoch=1;fx_reset_audio();
}
/* Called once per block. Return changed source mask for hardware FM writes. */
static FX_CODE uint32_t fx_sync(){
 uint32_t changed=fx_dirty;if(!changed)return 0;fx_dirty=0;fx_echo_any=0;
 for(int s=0;s<12;s++){
  if(changed&(1u<<s)){
   for(int e=0;e<3;e++)fx_values[s][e]=fx_requested[s][e];
   fx_gate[s]=256u-fx_values[s][0]*192u/100u;
   if(s<5)emerald_echo_gain[s]=(int32_t)(fx_values[s][2]*64u/100u);
   else if(s<9){fx_color_mask[s-5]=(255u<<(fx_values[s][1]/20u))&255u;fx_drum_echo[s-5]=(int32_t)(fx_values[s][2]*64u/100u);}
   else fx_pg_gain[s-9]=float(fx_values[s][2])*0.0025f;
  }
  fx_echo_any|=fx_values[s][2];
 }
 fx_ring_depth[0]=float(fx_values[9][1])*0.01f;fx_ring_depth[1]=float(fx_values[10][1])*0.01f;
 fx_noise_period=(512u*44100u)<<(fx_values[11][1]/34u);
 fx_live_frames=fx_values[10][0]?(102u-fx_values[10][0])*441u:0;
 return changed;
}
static FX_CODE int32_t fx_quantize(int32_t value){
 return value>28000?28000:value< -28000?-28000:value;
}
/* Input/output units match pre-master YM/PSG. 4096 half-rate frames ~=186 ms.
 * Shared return, independent sends. Feedback 5/16; bounded 2.2 s release. */
static FX_CODE void fx_echo(float send_l,float send_r,float *left,float *right){
 if(fx_echo_any)fx_tail=97020;
 if(!fx_tail)return;
 --fx_tail;
 if(fx_half){
  int32_t dl=0,dr=0;if(fx_tags[fx_pos]==fx_epoch){dl=fx_delay[fx_pos][0];dr=fx_delay[fx_pos][1];}
  fx_return_l=float(dl);fx_return_r=float(dr);
  fx_delay[fx_pos][0]=(int16_t)fx_quantize(int32_t((send_l+fx_input_l)*0.5f+float(dl)*0.3125f));
  fx_delay[fx_pos][1]=(int16_t)fx_quantize(int32_t((send_r+fx_input_r)*0.5f+float(dr)*0.3125f));
  fx_tags[fx_pos]=fx_epoch;fx_pos=(fx_pos+1u)&4095u;
 }else{fx_input_l=send_l;fx_input_r=send_r;}
 fx_half^=1u;*left+=fx_return_l*0.75f;*right+=fx_return_r*0.75f;
}
