/* SPDX-License-Identifier: MIT
 * Main-loop button edges only; the mode is session-only and never saved.
 * OUT is clock master; discard all pre-switch performance/real-time input. */
static void ymidi_controls(uint32_t pressed){
 if(pressed&(1u<<B_EDIT)){
  uint32_t f=irq_save();
  ymidi_out^=1u;ymidi_revision++;ymidi_discard=mi_w-mi_r;
  ymidi_keys=ymidi_port_keys[0]=ymidi_port_keys[1]=0;
  ymidi_clock_fault=0;
  if(ymidi_out){yc_internal(&yclock);if(yplaying)ym_reseed(ym_seed());ypost();}
  irq_restore(f);
 }
}
static void ymidi_play(void){
 if(ym_overload>=8u)return;
 uint32_t f=irq_save();
 yplaying=!yplaying;
 if(ymidi_out){
  ymidi_clock_fault=0;
  if(yplaying){yc_internal(&yclock);ym_reseed(ym_seed());}
 }else if(yplaying&&yclock.mode)yc_arm(&yclock,fm1_ticks());
 ypost();irq_restore(f);
}
