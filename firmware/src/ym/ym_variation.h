/* SPDX-License-Identifier: MIT
 * Pure, bounded 16-step composition decisions. Song-only caller: one tick per
 * sixteenth note, never per output sample. Tables belong to pitch-data/XIP.
 * Keep this header independent of hardware for deterministic desktop tests. */
#pragma once
#include <stdint.h>
#ifndef YM_VARIATION_TABLE
#define YM_VARIATION_TABLE
#endif

struct ym_var_drums { uint16_t kick,snare,hat,open; };
static const struct ym_var_drums ym_var_grooves[4] YM_VARIATION_TABLE={
 {0x4541u,0x1010u,0x5555u,0x4000u}, /* syncopated */
 {0x4909u,0x9010u,0x5d5du,0x8080u}, /* broken */
 {0x1111u,0x1010u,0x5555u,0x4000u}, /* straight four */
 {0x9481u,0x1810u,0xdd6du,0x4040u}  /* offbeat */
};
static const uint8_t ym_var_progressions[4][8] YM_VARIATION_TABLE={
 {0,1,2,3,4,5,6,7},
 {0,4,2,5,6,1,3,7},
 {2,5,0,4,3,6,1,7},
 {4,1,6,2,5,0,3,7}
};
static const uint8_t ym_var_motifs[8][8] YM_VARIATION_TABLE={
 {2,4,5,4,2,1,0,1}, {1,2,4,3,2,1,2,4},
 {4,5,6,5,4,2,1,0}, {3,2,1,0,1,2,4,3},
 {0,2,4,5,4,2,1,0}, {2,2,5,4,3,1,3,2},
 {4,3,1,2,4,6,5,2}, {1,4,2,5,3,6,4,1}
};
static const uint8_t ym_var_rhythms[4][8] YM_VARIATION_TABLE={
 {0,2,5,6,8,11,12,14},
 {0,3,4,6,9,10,12,14},
 {0,2,4,7,8,10,13,15},
 {0,1,4,6,8,11,13,14}
};
/* 2^32 wrap and Xorshift only: no RNG draw/audio allocation. */
static inline uint32_t ym_var_style(uint32_t seed){
 uint32_t h=(seed?seed:1u)^0x9e3779b9u;
 h^=h<<13;h^=h>>17;h^=h<<5;
 return (h&3u)|(((h>>3)&3u)<<2)|(((h>>8)&7u)<<4)|(((h>>13)&3u)<<7);
}
/* 0..3 harmonic palette; transpositions are by existing K4 control only. */
static inline uint32_t ym_var_chord(uint32_t style,uint32_t bar,uint32_t section){
 uint32_t offset=(section>=16u&&section<24u)?4u:0u;
 return ym_var_progressions[style&3u][(bar+offset)&7u];
}
static inline uint32_t ym_var_motif(uint32_t style,uint32_t bar,uint32_t event){
 uint32_t family=(((style>>4)&7u)+3u*((bar>>2)&1u))&7u;
 return ym_var_motifs[family][event&7u];
}
static inline const uint8_t *ym_var_lead_rhythm(uint32_t style,uint32_t bar){
 return ym_var_rhythms[(((style>>2)&3u)+((bar>>2)&1u))&3u];
}
static inline const struct ym_var_drums *ym_var_drum_pattern(uint32_t style,uint32_t bar,uint32_t section){
 uint32_t index=(((style>>2)&3u)+((bar>>3)&1u)+(section>=24u?1u:0u))&3u;
 return &ym_var_grooves[index];
}
/* Alternating question/answer; a phrase rest cannot remove beat-one. */
static inline int ym_var_lead_event(uint32_t bar,uint32_t event){
 return !(((bar>>2)&1u)!=0u&&(event==3u||event==7u));
}
static inline int ym_var_bass_event(uint32_t style,uint32_t bar,uint32_t event){
 uint32_t contour=(style>>7)&3u;
 return !((contour==1u&&event==7u&&(bar&1u))||
          (contour==2u&&event==3u&&(bar&3u)==2u)||
          (contour==3u&&event==5u&&(bar&3u)==1u));
}
