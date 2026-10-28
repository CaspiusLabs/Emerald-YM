/* SPDX-License-Identifier: MIT */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void ym_init(uint32_t seed);
void ym_render(int32_t *out,uint32_t frames,uint32_t gain_q12);
void ym_control(uint32_t tempo,uint32_t density,uint32_t mutation,int32_t transpose,uint32_t playing);
/* Main-loop publication: cumulative six-clock boundaries, consumed once/block.
 * Start must also reseed; external mode never advances from its BPM estimate. */
void ym_external_clock(uint32_t enabled,uint32_t events,uint32_t bpm);
/* Held FM-1 key bitmap F3..G5; audio thread owns voice allocation. */
void ym_live_keys(uint32_t keys);
void ym_psg_enable(uint32_t enabled);
void ym_psg_level(uint32_t percent);
/* FM melodic backing and all percussion, 0..100%, unity by default. */
/* 0..4 FM voices, 5..8 DAC drums, 9 arp, 10 live, 11 PSG noise. */
void ym_instrument_level(uint32_t instrument,uint32_t percent);
/* DECAY=0, COLOR=1, ECHO=2; independent 0..100 values per source. */
void ym_effect(uint32_t source,uint32_t effect,uint32_t value);
void ym_drum_level(uint32_t percent);
void ym_mix_levels(uint32_t melody_percent,uint32_t drums_percent);
uint32_t ym_live_active_keys(void);
uint32_t ym_sequence_keys(void);
void ym_reseed(uint32_t seed);
uint32_t ym_bar(void);
uint32_t ym_seed(void);
uint32_t ym_arena_bytes(void);
uint32_t ym_note_count(void);
uint32_t ym_step(void);
#ifdef __cplusplus
}
#endif
