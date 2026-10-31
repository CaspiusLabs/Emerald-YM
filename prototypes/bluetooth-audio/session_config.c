/* SPDX-License-Identifier: GPL-3.0-only
 * Explicit session-only SDK syscfg backend, never a persistent-store claim.
 * Requires initialized SDK RTOS and task context; never called from audio IRQ.
 * Startup is not wired in this offline prototype. No fabricated FM-1 identity. */
#include "system/includes.h"
#include "syscfg/syscfg_id.h"
#include "session_store.h"
#include "storage.h"
static struct eym_session_store store EYM_BT_STORAGE;
static OS_MUTEX mutex;
static unsigned char mac[6];
static int ready;
int eym_session_config_init(const unsigned char *identity)
{
    unsigned int i, nonzero = 0;
    if (ready || !identity || (identity[0] & 1)) return -1;
    for (i = 0; i < 6; ++i) nonzero |= identity[i];
    if (!nonzero || os_mutex_create(&mutex)) return -1;
    for (i = 0; i < 6; ++i) mac[i] = identity[i];
    if (eym_session_write(&store, CFG_BT_MAC_ADDR, mac, 6) != 6) return -1;
    ready = 1;
    return 0;
}
const unsigned char *eym_session_mac(void) { return ready ? mac : 0; }
int syscfg_read(u16 id, void *dst, u16 capacity)
{
    int result;
    if (!ready || os_mutex_pend(&mutex, 0)) return -1;
    result = eym_session_read(&store, id, dst, capacity);
    os_mutex_post(&mutex);
    return result;
}
int syscfg_write(u16 id, void *src, u16 length)
{
    int result;
    if (!ready || id == CFG_BT_MAC_ADDR || os_mutex_pend(&mutex, 0)) return -1;
    result = eym_session_write(&store, id, src, length);
    os_mutex_post(&mutex);
    return result;
}
int syscfg_dma_write(u16 id, void *src, u16 length)
{
    if ((unsigned int)src & 3u) return -1;
    return syscfg_write(id, src, length);
}
int syscfg_read_string(u16 id, void *dst, u16 capacity, u8 version)
{
    return version ? -1 : syscfg_read(id, dst, capacity);
}
