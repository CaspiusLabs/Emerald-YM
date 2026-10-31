/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef EMERALD_BT_PCM_QUEUE_H
#define EMERALD_BT_PCM_QUEUE_H

/* One producer (audio), one consumer (encoder), on the SAME CORE.
 * No reset while either runs. Target publication uses aligned u32 + csync.
 * The target probe checks that publication has no atomic runtime calls.
 * This is an isolated prototype, not included in the production firmware. */
#define EYM_BT_PCM_FRAMES 2048u
struct eym_bt_pcm_queue {
    unsigned int write_pos, read_pos;
    unsigned int dropped_frames, silent_frames;
    short samples[EYM_BT_PCM_FRAMES * 2];
};
void eym_bt_pcm_reset(struct eym_bt_pcm_queue *q);
unsigned int eym_bt_pcm_push(struct eym_bt_pcm_queue *q,
                           const int *stereo24, unsigned int frames);
unsigned int eym_bt_pcm_read(struct eym_bt_pcm_queue *q,
                           short *stereo16, unsigned int frames);
#endif
