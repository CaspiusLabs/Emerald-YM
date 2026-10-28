/* SPDX-License-Identifier: GPL-3.0-only
 * Explicit SDK audit policy, not FM-1 startup or an interrupt port.
 * Sentinel is the official demo_edr default for auto-assigned interrupts. */
#include "system/includes.h"
const struct irq_info irq_info_table[] = {{-1, -1, -1}};
/* No SDK OTA path in the audio prototype; bitmask 0 selects no update modes. */
const int config_update_mode = 0;
/* Sizes/priorities from demo_edr/app_main.c, retaining only service/encoder/BT
 * tasks. This is an allocation policy, not a scheduler replacement. */
const struct task_info task_info_table[] = {
    {"app_core", 15, 1024, 256}, {"sys_event", 29, 512, 0},
    {"systimer", 14, 256, 0}, {"sys_timer", 9, 512, 64},
    {"audio_server", 16, 512, 64}, {"audio_encoder", 12, 384, 64},
    {"vir_dev_task", 13, 256, 0}, {"sbc_encoder", 13, 512, 0},
    {"#C0btencry", 16, 512, 128}, {"#C0btctrler", 19, 512, 384},
    {"#C0btstack", 18, 1024, 384}, {0, 0}
};
