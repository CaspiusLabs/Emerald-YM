"""Compile the real UART parser with inert HAL functions; never access a device."""
from pathlib import Path
s=Path('firmware/src/midi_uart.c').read_text()
include='#include "../hal/fm1_uart.h"'
assert s.count(include)==1
s=s.replace(include,'static void fm1_uart1_midi_init(volatile uint8_t*p,uint32_t n){(void)p;(void)n;}\nstatic uint32_t fm1_uart1_rx_take(void){return 0;}')
Path('build/host/midi').mkdir(parents=True,exist_ok=True)
Path('build/host/midi/midi_uart.c').write_text(s,encoding='utf-8')
u=Path('firmware/src/usb.c').read_text()
start=u.index('static int midi_out_try(')
end=u.index('/* ------------------------------------------------------- descriptors',start)
Path('build/host/midi/midi_out_queue.h').write_text(u[start:end],encoding='utf-8')
