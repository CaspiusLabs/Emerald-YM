#include "ym_midi.c"
static void ymidi_quiesce(void){
 ymidi_out=0;ymidi_revision++;
 /* Bounded opportunity for the audio owner to enqueue Offs and USB to drain.
  * An unplugged/non-reading host still needs its own panic on disconnect. */
 fm1_delay_ms(20);
}
static void fm1_main(void){
 uint32_t last_frame=0,seed=20261026,boot_until;int32_t knob=512*16;
 {uint32_t f=irq_save();flash_ok=FL_FAR(fl_jedec_ram)()==0x856014u;irq_restore(f);if(flash_ok)ota_boot_cleanup();}
 lcd_init();palette_set(0);lcd_fill(0,0,240,240,C_BLACK);
 if(en_dbg.magic!=DBG_MAGIC){memset(&en_dbg,0,sizeof en_dbg);en_dbg.magic=DBG_MAGIC;}
 if(safe_mode)safe_main();
 en_dbg.boots++;en_dbg.max_us=0;en_dbg.halves=0;en_dbg.cpu_q8=0;fm1_input_init();fm1_adc_init();panel_init();yled_init();
 yui_boot();ym_init(seed);ys_load();yclock.bpm=ytempo;ym_psg_enable(ypsg);ypost();audio_init();usb_start();
#if EN_TRS
 uart_midi_init();
#endif
 fm1_timer5_start(isr_timer5,4);fm1_guard_lock_top();fm1_irq_enable_all();
 fm1_delay_ms(30);
 if((fm1_in.buttons&3u)==3u){panel_setup();boot_until=0;}else boot_until=fm1_ms+1100u;
 for(;;){
  fm1_wdt_feed();usb_retry(fm1_ms);
  if(fm1_ms>30000u&&bootguard.pending){bootguard.pending=0;bootguard.failed=0;}
  {int32_t a=fm1_adc_read(FM1_ADC_MASTER);if(a>=0){uint32_t k;knob+=(a*16-knob)/8;k=(uint32_t)(knob/16);ymaster_adc=k;master_q12=(k*k)>>8;}}
  {static uint32_t held;uint32_t both=(1u<<panel.btn[B_OCTDN])|(1u<<panel.btn[B_OCTUP]);
   if((fm1_in.buttons&both)!=both)held=fm1_ms;else if(fm1_ms-held>5000u){ymidi_quiesce();enter_uboot("UBOOT");}}
  yusb_service();
  ymidi_physical=fm1_in.notes;
  {static uint32_t previous=0xffffffffu;uint32_t held=ym_overload>=8u?0:fm1_in.notes|ymidi_keys;
   if(held!=previous){ym_live_keys(held);previous=held;}}
  if(usb.ota_req){usb.ota_req=0;yplaying=0;ypost();ymidi_quiesce();if(flash_ok)ota_session();ydisplay_invalidate();ydisplay();}
  if(usb.uboot_req){ymidi_quiesce();enter_uboot("UBOOT (USB)");}
  /* A sustained high load stops the renderer itself: preserve USB/panel responsiveness. */
  if((ym_overload>=8||ymidi_clock_fault)&&yplaying){yplaying=0;ypost();}
  if(fm1_ms-last_frame>=20u){
   uint32_t pressed=0,raw_pressed=fm1_input_edges(0);int32_t d;
   uint32_t oldtempo=ytempo,olddensity=ydensity,oldmutation=ymutation;int32_t oldtranspose=ytranspose;
   last_frame=fm1_ms;en_dbg.ui_frames++;
   for(uint32_t i=0;i<NB;i++)if(raw_pressed&(1u<<panel.btn[i]))pressed|=1u<<i;
   youtput_controls(pressed);
   ymidi_controls(pressed);
   if(pressed&(1u<<B_PLAY))ymidi_play();
   if(pressed&(1u<<B_REC)){uint32_t f=irq_save();seed^=fm1_ms+0x9e3779b9u;ym_reseed(seed?seed:1);yclock.events=yclock.phase=0;irq_restore(f);ypost();yrec_until=fm1_ms+180u;}
   if(pressed&(1u<<B_SEQ)){ymutation=ymutation?0:35;ypost();}
   if(pressed&(1u<<B_SAVE)){if(ys_dirty){yplaying=0;ypost();ys_quiet_at=fm1_ms;}ys_retry(fm1_ms);}
   if(pressed&(1u<<B_FX))yfx_kind[yinstrument]=(yfx_kind[yinstrument]+1u)%3u;
   if(pressed&(1u<<B_ARP)){ypsg^=1u;ym_psg_enable(ypsg);}
   d=fm1_enc_take(panel.enc[EN_SELECT])*panel.dir[EN_SELECT];
   if(d){int32_t next=((int32_t)yinstrument+d)%12;if(next<0)next+=12;yinstrument=(uint32_t)next;}
   d=fm1_enc_take(panel.enc[EN_PRESET])*panel.dir[EN_PRESET];
   if(d){uint32_t value=(uint32_t)clamp((int32_t)ylevels[yinstrument]+d*5,0,100);if(value!=ylevels[yinstrument]){ylevels[yinstrument]=value;ym_instrument_level(yinstrument,value);ys_changed(fm1_ms);}}
   d=fm1_enc_take(panel.enc[EN_ALGO])*panel.dir[EN_ALGO];
   if(d){uint32_t effect=yfx_kind[yinstrument];uint32_t value=(uint32_t)clamp((int32_t)yfx_values[yinstrument][effect]+d*5,0,100);if(value!=yfx_values[yinstrument][effect]){yfx_values[yinstrument][effect]=value;ym_effect(yinstrument,effect,value);ys_changed(fm1_ms);}}
   d=fm1_enc_take(panel.enc[EN_K1])*panel.dir[EN_K1];ytempo=(uint32_t)clamp((int32_t)ytempo+d,80,180);
   if(d){yc_internal(&yclock);ypost();}
   d=fm1_enc_take(panel.enc[EN_K2])*panel.dir[EN_K2];ydensity=(uint32_t)clamp((int32_t)ydensity+d*2,0,100);
   d=fm1_enc_take(panel.enc[EN_K3])*panel.dir[EN_K3];ymutation=(uint32_t)clamp((int32_t)ymutation+d*2,0,100);
   d=fm1_enc_take(panel.enc[EN_K4])*panel.dir[EN_K4];ytranspose=clamp(ytranspose+d,-12,12);
   if(oldtempo!=ytempo||olddensity!=ydensity||oldmutation!=ymutation||oldtranspose!=ytranspose){ypost();ys_changed(fm1_ms);}
   if(ys_due(fm1_ms,yplaying||fm1_in.notes||ymidi_keys)){ys_state=YS_WRITING;ydisplay();lcd_sync();ys_commit(fm1_ms);}
   yled_update();
   if(boot_until&&((int32_t)(boot_until-fm1_ms)>0)&&!pressed&&!yplaying&&!fm1_in.notes&&!ymidi_keys){}
   else {boot_until=0;ydisplay();}
  }
 }
}
