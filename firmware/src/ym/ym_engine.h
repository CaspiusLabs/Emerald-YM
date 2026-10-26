/* SPDX-License-Identifier: MIT */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void ym_init(uint32_t seed);
void ym_render(int32_t *out,uint32_t frames,uint32_t gain_q12);
void ym_control(uint32_t tempo,uint32_t density,uint32_t mutation,int32_t transpose,uint32_t playing);
void ym_reseed(uint32_t seed);
uint32_t ym_bar(void);
uint32_t ym_seed(void);
uint32_t ym_arena_bytes(void);
uint32_t ym_note_count(void);
uint32_t ym_step(void);
#ifdef __cplusplus
}
#endif
