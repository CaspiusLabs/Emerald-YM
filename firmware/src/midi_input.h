/* SPDX-License-Identifier: GPL-3.0-only
 * Single ISR producer context (USB then UART), main-loop consumer. */
#pragma once
#include <stdint.h>
#define MQ 64u
#ifndef MIDI_INPUT_TIME
#define MIDI_INPUT_TIME() 0u
#endif
#ifndef MIDI_USB_INPUT_PACKET
#define MIDI_USB_INPUT_PACKET(pkt) (pkt)
#endif
static uint32_t midi_in_q[MQ],midi_in_at[MQ];
static volatile uint32_t mi_w,mi_r,midi_in_lost;
static int midi_input_put(uint32_t packet){
 if(mi_w-mi_r>=MQ){midi_in_lost++;return 0;}
 midi_in_q[mi_w%MQ]=packet;midi_in_at[mi_w%MQ]=MIDI_INPUT_TIME();
 RING_PUBLISH();mi_w++;return 1;
}
