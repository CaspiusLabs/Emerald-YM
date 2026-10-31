/* SPDX-License-Identifier: GPL-3.0-only */
#include "pcm_queue.h"
#include "publication.h"

void eym_bt_pcm_reset(struct eym_bt_pcm_queue *q)
{
    q->write_pos = q->read_pos = 0;
    q->dropped_frames = q->silent_frames = 0;
}

static short to_pcm16(int value)
{
    /* Match the production USB tap's 24-bit to 16-bit scaling. */
    int s = value >> 8;
    if (s > 32767) s = 32767;
    if (s < -32768) s = -32768;
    return (short)s;
}

unsigned int eym_bt_pcm_push(struct eym_bt_pcm_queue *q,
                           const int *pcm, unsigned int frames)
{
    unsigned int w = load_pos(&q->write_pos);
    unsigned int r = load_pos(&q->read_pos);
    unsigned int count = EYM_BT_PCM_FRAMES - (w - r), i;
    if (count > frames) count = frames;
    for (i = 0; i < count; ++i) {
        unsigned int pos = ((w + i) & (EYM_BT_PCM_FRAMES - 1)) * 2;
        q->samples[pos] = to_pcm16(pcm[i * 2]);
        q->samples[pos + 1] = to_pcm16(pcm[i * 2 + 1]);
    }
    store_pos(&q->write_pos, w + count);
    q->dropped_frames += frames - count;
    return count;
}

unsigned int eym_bt_pcm_read(struct eym_bt_pcm_queue *q,
                           short *pcm, unsigned int frames)
{
    unsigned int r = load_pos(&q->read_pos);
    unsigned int w = load_pos(&q->write_pos);
    unsigned int count = w - r, i;
    if (count > frames) count = frames;
    for (i = 0; i < count; ++i) {
        unsigned int pos = ((r + i) & (EYM_BT_PCM_FRAMES - 1)) * 2;
        pcm[i * 2] = q->samples[pos];
        pcm[i * 2 + 1] = q->samples[pos + 1];
    }
    for (; i < frames; ++i) pcm[i * 2] = pcm[i * 2 + 1] = 0;
    store_pos(&q->read_pos, r + count);
    q->silent_frames += frames - count;
    return count;
}
