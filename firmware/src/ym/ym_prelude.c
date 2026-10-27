/* SPDX-License-Identifier: GPL-3.0-only
 * Small application on the unchanged DUB-1/Felucca hardware and update platform. */
#include "ym_engine.h"
#ifndef EN_VERSION
#define EN_VERSION "DEV"
#endif
/* Crossing XIP -> low RAM requires the platform's indirect far-call helper. */
#define ym_control(...) FL_FAR(ym_control)(__VA_ARGS__)
#define ym_render(...) FL_FAR(ym_render)(__VA_ARGS__)
#define ym_init(...) FL_FAR(ym_init)(__VA_ARGS__)
#define ym_bar(...) FL_FAR(ym_bar)(__VA_ARGS__)
#define ym_reseed(...) FL_FAR(ym_reseed)(__VA_ARGS__)
#define ym_seed(...) FL_FAR(ym_seed)(__VA_ARGS__)
#define ym_step(...) FL_FAR(ym_step)(__VA_ARGS__)
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
static uint32_t master_q12=256; /* start quietly, master pot updates after boot */
static uint32_t ytempo=132,ydensity=100,ymutation=35,yplaying=0;
static int32_t ytranspose;
static uint32_t ypsg=1,ymidi_keys;
static uint32_t yinstrument,ylevels[12]={100,100,100,100,100,100,100,100,100,100,100,100};
static uint32_t yfx_kind[12],yfx_values[12][3];
int plat_store_load(uint32_t obj,void*dst,uint32_t max){return flash_ok?st_load(obj,dst,max):-1;}
int plat_store_save(uint32_t obj,const void*src,uint32_t len){return flash_ok?st_save(obj,src,len):-9;}
#include "app/panel.c"
#include "ym_settings.c"
/* Called with the IRQ briefly masked; heavy work happens in the audio ISR. */
static void ypost(void){uint32_t f=irq_save();ym_control(ytempo,ydensity,ymutation,ytranspose,yplaying);irq_restore(f);}
#include "ym_ui.c"
#include "ym_leds.c"
