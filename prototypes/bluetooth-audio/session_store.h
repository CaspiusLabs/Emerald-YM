/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef EYM_SESSION_STORE_H
#define EYM_SESSION_STORE_H
/* Task-owned store; caller serializes access. No allocation or flash backend. */
#define EYM_SESSION_SLOTS 16u
#define EYM_SESSION_BYTES 256u
struct eym_session_entry {
    unsigned short id, length;
    unsigned char bytes[EYM_SESSION_BYTES];
};
struct eym_session_store { struct eym_session_entry entry[EYM_SESSION_SLOTS]; };
int eym_session_read(struct eym_session_store *, unsigned short, void *, unsigned short);
int eym_session_write(struct eym_session_store *, unsigned short, const void *, unsigned short);
#endif
