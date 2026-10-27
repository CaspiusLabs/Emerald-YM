/* SPDX-License-Identifier: GPL-3.0-only
 * Compile against the actual pinned AC79 SDK headers, not replacement structs.
 * Control/encoder task only. No radio startup and no renderer integration. */
#include "server/server_core.h"
#include "server/audio_server.h"
#include "pcm_queue.h"
#include "sbc_queue.h"

static struct eym_bt_pcm_queue pcm_queue;
static struct server *encoder;

static int pcm_read(u8 *buf, u32 bytes)
{
    if (bytes & 3u) return -1;
    eym_bt_pcm_read(&pcm_queue, (short *)buf, bytes / 4);
    return bytes;
}

static int sbc_write(void *file, void *data, u32 bytes)
{
    (void)file;
    /* Full queue drops this frame. Never wait in the SDK output callback.
     * Transport must count drops and preserve SBC frame boundaries. */
    eym_bt_a2dp_offer_frame(data, bytes);
    return bytes;
}

static const struct audio_vfs_ops output_ops = { .fwrite = sbc_write };

int eym_bt_encoder_open(void)
{
    union audio_req req = {.enc = {0}};
    int result;
    if (encoder) return -1;
    eym_bt_pcm_reset(&pcm_queue);
    eym_bt_sbc_reset();
    encoder = server_open("audio_server", "enc");
    if (!encoder) return -1;
    req.enc.cmd = AUDIO_ENC_OPEN;
    req.enc.channel = 2;
    req.enc.volume = 100;
    req.enc.sample_rate = 44100;
    req.enc.format = "sbc";
    req.enc.sample_source = "virtual";
    req.enc.read_input = pcm_read;
    req.enc.vfs_ops = &output_ops;
    req.enc.frame_size = 512; /* 128 stereo PCM16 frames, SDK emitter profile. */
    req.enc.output_buf_len = 2048;
    /* SDK default codec settings only: negotiation must be wired before radio
     * use. This probe does not advertise a codec configuration to headphones. */
    result = server_request(encoder, AUDIO_REQ_ENC, &req);
    if (result) { server_close(encoder); encoder = 0; }
    return result;
}

void eym_bt_encoder_close(void)
{
    union audio_req req = {.enc = {0}};
    if (!encoder) return;
    req.enc.cmd = AUDIO_ENC_CLOSE;
    server_request(encoder, AUDIO_REQ_ENC, &req);
    server_close(encoder);
    encoder = 0;
}

/* Future tap: master mix BEFORE the physical USB ONLY fade, like uac_tap.
 * Caller must quiesce producer and SDK reader before open/close/reset. */
unsigned int eym_bt_encoder_push(const int *stereo24, unsigned int frames)
{
    return eym_bt_pcm_push(&pcm_queue, stereo24, frames);
}
