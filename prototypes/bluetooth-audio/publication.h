/* SPDX-License-Identifier: GPL-3.0-only */
#ifndef EYM_BT_PUBLICATION_H
#define EYM_BT_PUBLICATION_H
#if defined(EYM_BT_PI32)
/* Same core ISR/task only; no cross-core or DMA access to these rings.
 * SDK asm/cpu.h uses csync for publication. Add a compiler memory clobber.
 * Avoid this toolchain's __atomic loads: they call __sync runtime helpers. */
#define FENCE() __asm__ volatile("csync;" ::: "memory")
#else
/* Desktop functional tests are deliberately single-threaded. */
#define FENCE() ((void)0)
#endif
static unsigned int load_pos(const unsigned int *p)
{
    unsigned int value = *(const volatile unsigned int *)p;
    FENCE();
    return value;
}
static void store_pos(unsigned int *p, unsigned int value)
{
    FENCE();
    *(volatile unsigned int *)p = value;
}
#endif
