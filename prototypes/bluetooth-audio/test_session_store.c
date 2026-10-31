/* SPDX-License-Identifier: GPL-3.0-only */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "session_store.h"
static struct eym_session_store store;
int main(void)
{
    unsigned char input[EYM_SESSION_BYTES + 1], output[EYM_SESSION_BYTES + 1];
    unsigned int i;
    memset(input, 0x45, sizeof input);
    memset(output, 0x91, sizeof output);
    assert(eym_session_read(&store, 1, output, sizeof output) < 0);
    assert(output[0] == 0x91);
    assert(eym_session_write(&store, 1, 0, 4) < 0);
    assert(eym_session_write(&store, 1, input, 0) < 0);
    assert(eym_session_write(&store, 1, input, sizeof input) < 0);
    for (i = 0; i < EYM_SESSION_SLOTS; ++i) {
        input[0] = (unsigned char)i;
        assert(eym_session_write(&store, (unsigned short)i, input, EYM_SESSION_BYTES) == EYM_SESSION_BYTES);
    }
    assert(eym_session_write(&store, 1234, input, 1) < 0);
    for (i = 0; i < EYM_SESSION_SLOTS; ++i) {
        assert(eym_session_read(&store, (unsigned short)i, output, EYM_SESSION_BYTES) == EYM_SESSION_BYTES);
        assert(output[0] == i && output[EYM_SESSION_BYTES-1] == 0x45);
    }
    memset(output, 0x91, sizeof output);
    assert(eym_session_read(&store, 0, output, EYM_SESSION_BYTES-1) < 0);
    assert(output[0] == 0x91 && output[EYM_SESSION_BYTES-2] == 0x91);
    input[0] = 0xab;
    assert(eym_session_write(&store, 0, input, 1) == 1);
    assert(eym_session_read(&store, 0, output, sizeof output) == 1);
    assert(output[0] == 0xab && output[1] == 0x91);
    assert(eym_session_read(&store, 0, 0, 1) < 0);
    puts("PASS: session store missing/full/oversize/null/short-read/replacement/isolation; no persistence backend");
    return 0;
}
