/* SPDX-License-Identifier: GPL-3.0-only */
#include "session_store.h"
int eym_session_read(struct eym_session_store *s, unsigned short id, void *dst, unsigned short capacity)
{
    unsigned int i, j;
    unsigned char *out = (unsigned char *)dst;
    if (!dst || !capacity) return -1;
    for (i = 0; i < EYM_SESSION_SLOTS; ++i) {
        struct eym_session_entry *e = &s->entry[i];
        if (e->length && e->id == id) {
            if (capacity < e->length) return -1;
            for (j = 0; j < e->length; ++j) out[j] = e->bytes[j];
            return e->length;
        }
    }
    return -1;
}
int eym_session_write(struct eym_session_store *s, unsigned short id, const void *src, unsigned short length)
{
    unsigned int i, j, slot = EYM_SESSION_SLOTS;
    const unsigned char *in = (const unsigned char *)src;
    if (!src || !length || length > EYM_SESSION_BYTES) return -1;
    for (i = 0; i < EYM_SESSION_SLOTS; ++i) {
        if (s->entry[i].length && s->entry[i].id == id) { slot = i; break; }
        if (!s->entry[i].length && slot == EYM_SESSION_SLOTS) slot = i;
    }
    if (slot == EYM_SESSION_SLOTS) return -1;
    for (j = 0; j < length; ++j) s->entry[slot].bytes[j] = in[j];
    s->entry[slot].id = id;
    s->entry[slot].length = length;
    return length;
}
