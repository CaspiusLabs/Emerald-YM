/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef EYM_BT_SBC_QUEUE_H
#define EYM_BT_SBC_QUEUE_H
#include "storage.h"
#ifndef EYM_BT_SBC_SLOTS
#define EYM_BT_SBC_SLOTS 8u
#endif
#if EYM_BT_SBC_SLOTS < 2 || (EYM_BT_SBC_SLOTS & (EYM_BT_SBC_SLOTS - 1))
#error "SBC queue slots must be a power of two, at least two"
#endif
/* One encoder producer, one stack consumer, same core. Reset only quiescent. */
void eym_bt_sbc_reset(void);
int eym_bt_a2dp_offer_frame(const void *frame, unsigned int bytes);
unsigned int eym_bt_sbc_drops(void); /* read only after quiescing producer */
/* Matches the SDK emitter's pull callback. No radio or role startup here. */
int a2dp_sbc_encoder_get_data(unsigned char *packet, unsigned short capacity,
                            int *frame_size);
#endif
