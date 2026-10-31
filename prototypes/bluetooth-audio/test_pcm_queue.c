/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include "pcm_queue.h"
#include "sbc_queue.h"

static struct eym_bt_pcm_queue q;
static int input[4096];
static short output[4096];

int main(void)
{
    unsigned int i, iteration;
    eym_bt_pcm_reset(&q);
    assert(eym_bt_pcm_read(&q, output, 128) == 0);
    for (i = 0; i < 256; ++i) assert(output[i] == 0);
    {
        unsigned char frame[512], packet[1024];
        int size;
        for (i = 0; i < sizeof(frame); ++i) frame[i] = (unsigned char)i;
        eym_bt_sbc_reset();
        assert(!a2dp_sbc_encoder_get_data(packet, sizeof(packet), &size) && !size);
        assert(!eym_bt_a2dp_offer_frame(frame, 0));
        assert(!eym_bt_a2dp_offer_frame(frame, 513));
        for (i = 0; i < 8; ++i) assert(eym_bt_a2dp_offer_frame(frame, 118));
        assert(!eym_bt_a2dp_offer_frame(frame, 118));
        assert(eym_bt_sbc_drops() == 3);
        assert(!a2dp_sbc_encoder_get_data(packet, 117, &size) && !size);
        assert(a2dp_sbc_encoder_get_data(packet, 237, &size) == 236 && size == 118);
        for (i = 0; i < 236; ++i) assert(packet[i] == frame[i % 118]);
        assert(a2dp_sbc_encoder_get_data(packet, sizeof(packet), &size) == 708);
        assert(eym_bt_a2dp_offer_frame(frame, 118));
        assert(eym_bt_a2dp_offer_frame(frame, 120));
        assert(a2dp_sbc_encoder_get_data(packet, sizeof(packet), &size) == 118 && size == 118);
        assert(a2dp_sbc_encoder_get_data(packet, sizeof(packet), &size) == 120 && size == 120);
    }
    assert(q.silent_frames == 128);
    input[0] = INT_MAX; input[1] = INT_MIN;
    input[2] = 8388352; input[3] = -8388608;
    assert(eym_bt_pcm_push(&q, input, 2) == 2);
    assert(eym_bt_pcm_read(&q, output, 2) == 2);
    assert(output[0] == 32767 && output[1] == -32768);
    assert(output[2] == 32767 && output[3] == -32768);

    /* Exercise both physical ring and unsigned cursor wrap, preserving stereo
     * order across production-sized blocks and SBC-sized reads. */
    eym_bt_pcm_reset(&q);
    q.write_pos = q.read_pos = UINT_MAX - 127;
    for (iteration = 0; iteration < 10000; ++iteration) {
        for (i = 0; i < 512; ++i) input[i] = ((int)i - 256) * 256;
        assert(eym_bt_pcm_push(&q, input, 256) == 256);
        assert(eym_bt_pcm_read(&q, output, 128) == 128);
        for (i = 0; i < 256; ++i) assert(output[i] == (int)i - 256);
        assert(eym_bt_pcm_read(&q, output, 128) == 128);
        for (i = 0; i < 256; ++i) assert(output[i] == (int)i);
    }
    assert(!q.dropped_frames && !q.silent_frames);

    eym_bt_pcm_reset(&q);
    for (i = 0; i < 4096; ++i) input[i] = (int)i * 256;
    assert(eym_bt_pcm_push(&q, input, 2048) == 2048);
    assert(eym_bt_pcm_push(&q, input, 256) == 0);
    assert(q.dropped_frames == 256);
    assert(eym_bt_pcm_read(&q, output, 2048) == 2048);
    for (i = 0; i < 4096; ++i) assert(output[i] == (int)i);
    assert(eym_bt_pcm_read(&q, output, 128) == 0);
    for (i = 0; i < 256; ++i) assert(output[i] == 0);
    puts("PASS: PCM stereo/clamp/silence/overflow/wrap, 10000 blocks; SBC whole frames/capacity/drop/size change");
    return 0;
}
