/* SPDX-License-Identifier: GPL-3.0-only */
#include "sbc_queue.h"
#include "publication.h"
#define SLOTS 8u
#define FRAME_BYTES 512u
static struct {
    unsigned int write_pos, read_pos, drops;
    struct { unsigned int size; unsigned char data[FRAME_BYTES]; } frames[SLOTS];
} queue;

void eym_bt_sbc_reset(void)
{
    queue.write_pos = queue.read_pos = queue.drops = 0;
}

unsigned int eym_bt_sbc_drops(void) { return queue.drops; }

int eym_bt_a2dp_offer_frame(const void *frame, unsigned int bytes)
{
    const unsigned char *data = (const unsigned char *)frame;
    unsigned int w = load_pos(&queue.write_pos), i;
    unsigned int r = load_pos(&queue.read_pos);
    if (!bytes || bytes > FRAME_BYTES || w - r == SLOTS) {
        ++queue.drops;
        return 0;
    }
    for (i = 0; i < bytes; ++i) queue.frames[w & (SLOTS - 1)].data[i] = data[i];
    queue.frames[w & (SLOTS - 1)].size = bytes;
    store_pos(&queue.write_pos, w + 1);
    return 1;
}

int a2dp_sbc_encoder_get_data(unsigned char *packet, unsigned short capacity,
                            int *frame_size)
{
    unsigned int r = load_pos(&queue.read_pos), w = load_pos(&queue.write_pos);
    unsigned int bytes = 0, size, first_size = 0, i;
    *frame_size = 0;
    while (r != w) {
        size = queue.frames[r & (SLOTS - 1)].size;
        if (size > (unsigned int)capacity - bytes || (first_size && size != first_size)) break;
        first_size = size;
        for (i = 0; i < size; ++i) packet[bytes + i] = queue.frames[r & (SLOTS - 1)].data[i];
        bytes += size;
        ++r;
    }
    if (bytes) *frame_size = (int)first_size;
    store_pos(&queue.read_pos, r);
    return (int)bytes;
}
