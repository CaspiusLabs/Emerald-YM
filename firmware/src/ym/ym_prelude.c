/* SPDX-License-Identifier: GPL-3.0-only
 * Small application on the unchanged DUB-1/Felucca hardware and update platform. */
#include "ym_engine.h"
#include "ym_clock.h"
#include "ym_song.h"
static struct ym_clock yclock={0,0,0,0,0,132,0,0,0,0};
#ifndef EN_VERSION
#define EN_VERSION "DEV"
#endif
/* Crossing XIP -> low RAM requires the platform's indirect far-call helper. */
#define ym_control(...) FL_FAR(ym_control)(__VA_ARGS__)
#define ym_external_clock(...) FL_FAR(ym_external_clock)(__VA_ARGS__)
#define ym_midi_snapshot(...) FL_FAR(ym_midi_snapshot)(__VA_ARGS__)
#define ym_render(...) FL_FAR(ym_render)(__VA_ARGS__)
#define ym_init(...) FL_FAR(ym_init)(__VA_ARGS__)
#define ym_bar(...) FL_FAR(ym_bar)(__VA_ARGS__)
static void ys_song_reset(void);
#define ym_reseed(s) (ys_song_reset(),FL_FAR(ym_reseed)(s))
#define ym_seed(...) FL_FAR(ym_seed)(__VA_ARGS__)
#define ym_step(...) FL_FAR(ym_step)(__VA_ARGS__)
#define ym_song_section(...) FL_FAR(ym_song_section)(__VA_ARGS__)
#define ym_live_keys(...) FL_FAR(ym_live_keys)(__VA_ARGS__)
#define ym_psg_enable(...) FL_FAR(ym_psg_enable)(__VA_ARGS__)
#define ym_psg_level(...) FL_FAR(ym_psg_level)(__VA_ARGS__)
#define ym_instrument_level(...) FL_FAR(ym_instrument_level)(__VA_ARGS__)
#define ym_effect(...) FL_FAR(ym_effect)(__VA_ARGS__)
#define ym_drum_level(...) FL_FAR(ym_drum_level)(__VA_ARGS__)
#define ym_mix_levels(...) FL_FAR(ym_mix_levels)(__VA_ARGS__)
#define ym_live_active_keys(...) FL_FAR(ym_live_active_keys)(__VA_ARGS__)
#define ym_sequence_keys(...) FL_FAR(ym_sequence_keys)(__VA_ARGS__)
static volatile uint32_t audio_cpu_pct,audio_xruns,audio_cpu_peak,audio_cpu_lifetime_peak;
static volatile uint32_t ym_overload;
static volatile uint32_t yusb_only; /* GLO: volatile session state, not persisted */
static uint32_t ymaster_adc=512; /* filtered physical position, UI only; never persisted */
static uint32_t master_q12=256; /* start quietly, master pot updates after boot */
static uint32_t ytempo=132,ydensity=100,ymutation=35,yplaying=0;
static int32_t ytranspose;
static uint32_t ypsg=1,ymidi_keys,ymidi_port_keys[2];
static volatile uint32_t ymidi_out,ymidi_revision,ymidi_physical,ymidi_clock_fault;
static uint32_t ymidi_discard;
static uint32_t yinstrument,ylevels[12]={100,100,100,100,100,100,100,100,100,100,100,100};
static uint32_t yfx_kind[12],yfx_values[12][3];
int plat_store_load(uint32_t obj,void*dst,uint32_t max){return flash_ok?st_load(obj,dst,max):-1;}
int plat_store_save(uint32_t obj,const void*src,uint32_t len){return flash_ok?st_save(obj,src,len):-9;}
#include "app/panel.c"
#include "ym_settings.c"
static void ypost(void);
#include "ym_midi_mode.h"
/* Called with the IRQ briefly masked; heavy work happens in the audio ISR. */
static void ypost(void){uint32_t f=irq_save();if(yclock.mode)yclock.running=yplaying;ym_control(ytempo,ydensity*ysong.density/100u,ymutation,ytranspose,yplaying);ym_external_clock(yclock.mode,yclock.events,yclock.bpm);irq_restore(f);}
static void ys_song_apply(void){
 uint32_t f=irq_save();ys_song_output_mask=ysong.enabled?ysong.mask:0xfffu;ym_song_section(ysong.section);
 for(uint32_t i=0;i<12u;i++)ym_instrument_level(i,ys_song_level(&ysong,i,ylevels[i]));
 ypost();irq_restore(f);
}
static void ys_song_service(void){
 if(ysong_reset_pending){if(ym_bar()>1u)return;ysong_reset_pending=0;}
 struct ym_song_form next=ysong;ys_song_profile(&next,ym_bar(),ym_step());
 if(next.done&&yplaying){yplaying=0;ypost();}
 if(next.section!=ysong.section||next.mask!=ysong.mask||next.gain!=ysong.gain||next.density!=ysong.density){ysong=next;ys_song_apply();}
 else ysong=next;
}
static void ys_song_reset(void){
 ys_song_profile(&ysong,1,0);ysong_reset_pending=1;ys_song_apply();
}
#include "ym_ui.c"
#include "ym_leds.c"
