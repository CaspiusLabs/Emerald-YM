/* SPDX-License-Identifier: MIT
 * Main-loop button edges only; the mode is session-only and never saved.
 * Preserve clock packets in the input queue, discard pre-switch note/CCs. */
static void ymidi_controls(uint32_t pressed){
 if(pressed&(1u<<B_EDIT)){
  uint32_t f=irq_save();
  ymidi_out^=1u;ymidi_revision++;ymidi_discard=mi_w-mi_r;
  ymidi_keys=ymidi_port_keys[0]=ymidi_port_keys[1]=0;
  irq_restore(f);
 }
}
