"""Compile the real UART parser with inert HAL functions; never access a device."""
from pathlib import Path
s=Path('firmware/src/midi_uart.c').read_text(encoding='utf-8')
include='#include "../hal/fm1_uart.h"'
assert s.count(include)==1
s=s.replace(include,'static void fm1_uart1_midi_init(volatile uint8_t*p,uint32_t n){(void)p;(void)n;}\nstatic uint32_t fm1_uart1_rx_take(void){return 0;}')
Path('build/host/midi').mkdir(parents=True,exist_ok=True)
Path('build/host/midi/midi_uart.c').write_text(s,encoding='utf-8')
u=Path('firmware/src/usb.c').read_text(encoding='utf-8')
start=u.index('static int midi_out_try(')
end=u.index('/* ------------------------------------------------------- descriptors',start)
Path('build/host/midi/midi_out_queue.h').write_text(u[start:end],encoding='utf-8')
# Exercise the real handshake decoder, not a stub that silently ignores frames.
o=Path('firmware/src/ota.c').read_text(encoding='utf-8')
start=o.index('static uint32_t ota_rd16(')
end=o.index('/* one read request',start)
Path('build/host/midi/ota_handshake.h').write_text(o[start:end],encoding='utf-8')
# Preserve the actual main-loop dispatch order in the race regression test.
loop=Path('firmware/src/ym/ym_loop.c').read_text(encoding='utf-8')
calls=[line.strip() for line in loop.splitlines() if line.strip().startswith('yusb_service();')]
assert len(calls)==1
Path('build/host/midi/ym_dispatch_iteration.h').write_text(
    'static void ym_dispatch_iteration(void){'+calls[0]+'}\n',encoding='utf-8')
control=Path('firmware/src/ym/ym_prelude.c').read_text(encoding='utf-8')
start=control.index('static void ys_song_apply(void){')
end=control.index('#include "ym_ui.c"',start)
Path('build/host/midi/ym_song_control.h').write_text(control[start:end],encoding='utf-8')
