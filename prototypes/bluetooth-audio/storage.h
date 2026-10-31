/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef EYM_BT_STORAGE_H
#define EYM_BT_STORAGE_H
/* Non-executable queues only. Existing production .pool* collection/zeroing
 * covers these bytes; no new region, guard change or executable pool storage. */
#if defined(EYM_BT_USE_POOL) && defined(EYM_BT_PI32)
#define EYM_BT_STORAGE __attribute__((section(".pool.bt"), aligned(4)))
#else
#define EYM_BT_STORAGE
#endif
#endif
