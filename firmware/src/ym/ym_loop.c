/* Documented in YM-USB-PROTOCOL.md. Status only; no flash/settings commands. */
static void yusb_service(void){
 const uint8_t*p;uint32_t n;
 while(mi_r!=mi_w){
  uint32_t packet=midi_in_q[mi_r%MQ],status=(packet>>8)&255u;mi_r++;
  if(status==0xFCu){yplaying=0;ypost();}
  else if((status==0xFAu||status==0xFBu)&&ym_overload<8u){yplaying=1;ypost();}
  else if((status&0xf0u)==0x90u||(status&0xf0u)==0x80u){
   uint32_t note=(packet>>16)&127u,velocity=(packet>>24)&127u;
   if(note>=53u&&note<=79u){uint32_t bit=1u<<(note-53u);
    if((status&0xf0u)==0x90u&&velocity)ymidi_keys|=bit;else ymidi_keys&=~bit;}
  }else if((status&0xf0u)==0xb0u&&((packet>>16)&127u)>=20u&&((packet>>16)&127u)<=22u){
   uint32_t source=status&15u,effect=((packet>>16)&127u)-20u;
   if(source<12u){uint32_t value=(((packet>>24)&127u)*100u+63u)/127u;yfx_values[source][effect]=value;ym_effect(source,effect,value);}
  }else if((status&0xf0u)==0xb0u&&(((packet>>16)&127u)==120u||((packet>>16)&127u)==123u))ymidi_keys=0;
 }
 if(ota_frame_get(&p,&n)&&n==5u&&p[0]==0x7du&&p[1]==0x45u&&p[2]==0x4du&&p[3]==0x59u&&p[4]==1u){
  uint8_t reply[62]={0xf0,0x7d,0x45,0x4d,0x59,0x41};uint32_t v[11],f=irq_save();
  v[0]=yplaying;v[1]=audio_cpu_pct;v[2]=audio_cpu_lifetime_peak;v[3]=audio_xruns;v[4]=ym_overload;
  v[5]=ym_bar();v[6]=ym_seed();v[7]=fm1_ms;v[8]=en_dbg.halves;v[9]=en_dbg.max_us;v[10]=bootguard.failed;
  irq_restore(f);ota_frame_done();
  for(uint32_t i=0;i<11u;i++)for(uint32_t k=0;k<5u;k++)reply[6u+i*5u+k]=(uint8_t)((v[i]>>(7u*k))&127u);
  reply[61]=0xf7;ota_wire_send(reply,sizeof reply);
 }
}
static void fm1_main(void){
 uint32_t last_frame=0,seed=20261026;int32_t knob=512*16;
 {uint32_t f=irq_save();flash_ok=FL_FAR(fl_jedec_ram)()==0x856014u;irq_restore(f);if(flash_ok)ota_boot_cleanup();}
 lcd_init();palette_set(0);lcd_fill(0,0,240,240,C_BLACK);
 if(en_dbg.magic!=DBG_MAGIC){memset(&en_dbg,0,sizeof en_dbg);en_dbg.magic=DBG_MAGIC;}
 if(safe_mode)safe_main();
 en_dbg.boots++;en_dbg.max_us=0;en_dbg.halves=0;en_dbg.cpu_q8=0;fm1_input_init();fm1_adc_init();panel_init();yled_init();
 ym_init(seed);ym_psg_enable(ypsg);ypost();audio_init();usb_start();
#if EN_TRS
 uart_midi_init();
#endif
 fm1_timer5_start(isr_timer5,4);fm1_guard_lock_top();fm1_irq_enable_all();
 fm1_delay_ms(30);
 if((fm1_in.buttons&3u)==3u)panel_setup();
 ydisplay();
 for(;;){
  fm1_wdt_feed();usb_retry(fm1_ms);
  if(fm1_ms>30000u&&bootguard.pending){bootguard.pending=0;bootguard.failed=0;}
  {int32_t a=fm1_adc_read(FM1_ADC_MASTER);if(a>=0){uint32_t k;knob+=(a*16-knob)/8;k=(uint32_t)(knob/16);master_q12=(k*k)>>8;}}
  {static uint32_t held;uint32_t both=(1u<<panel.btn[B_OCTDN])|(1u<<panel.btn[B_OCTUP]);
   if((fm1_in.buttons&both)!=both)held=fm1_ms;else if(fm1_ms-held>5000u)enter_uboot("UBOOT");}
  yusb_service();ota_service();
  {static uint32_t previous=0xffffffffu;uint32_t held=ym_overload>=8u?0:fm1_in.notes|ymidi_keys;
   if(held!=previous){ym_live_keys(held);previous=held;}}
  if(usb.ota_req){usb.ota_req=0;yplaying=0;ypost();if(flash_ok)ota_session();ydisplay_invalidate();ydisplay();}
  if(usb.uboot_req)enter_uboot("UBOOT (USB)");
  /* A sustained high load stops the renderer itself: preserve USB/panel responsiveness. */
  if(ym_overload>=8&&yplaying){yplaying=0;ypost();}
  if(fm1_ms-last_frame>=20u){
   uint32_t pressed=0,raw_pressed=fm1_input_edges(0);int32_t d;
   uint32_t oldtempo=ytempo,olddensity=ydensity,oldmutation=ymutation;int32_t oldtranspose=ytranspose;
   last_frame=fm1_ms;en_dbg.ui_frames++;
   for(uint32_t i=0;i<NB;i++)if(raw_pressed&(1u<<panel.btn[i]))pressed|=1u<<i;
   if(pressed&(1u<<B_PLAY)){if(ym_overload<8){yplaying=!yplaying;ypost();}}
   if(pressed&(1u<<B_REC)){uint32_t f=irq_save();seed^=fm1_ms+0x9e3779b9u;ym_reseed(seed?seed:1);irq_restore(f);yrec_until=fm1_ms+180u;}
   if(pressed&(1u<<B_SEQ)){ymutation=ymutation?0:35;ypost();}
   if(pressed&(1u<<B_FX))yfx_kind[yinstrument]=(yfx_kind[yinstrument]+1u)%3u;
   if(pressed&(1u<<B_ARP)){ypsg^=1u;ym_psg_enable(ypsg);}
   d=fm1_enc_take(panel.enc[EN_SELECT])*panel.dir[EN_SELECT];
   if(d){int32_t next=((int32_t)yinstrument+d)%12;if(next<0)next+=12;yinstrument=(uint32_t)next;}
   d=fm1_enc_take(panel.enc[EN_PRESET])*panel.dir[EN_PRESET];
   if(d){ylevels[yinstrument]=(uint32_t)clamp((int32_t)ylevels[yinstrument]+d*5,0,100);ym_instrument_level(yinstrument,ylevels[yinstrument]);}
   d=fm1_enc_take(panel.enc[EN_ALGO])*panel.dir[EN_ALGO];
   if(d){uint32_t effect=yfx_kind[yinstrument];yfx_values[yinstrument][effect]=(uint32_t)clamp((int32_t)yfx_values[yinstrument][effect]+d*5,0,100);ym_effect(yinstrument,effect,yfx_values[yinstrument][effect]);}
   d=fm1_enc_take(panel.enc[EN_K1])*panel.dir[EN_K1];ytempo=(uint32_t)clamp((int32_t)ytempo+d,80,180);
   d=fm1_enc_take(panel.enc[EN_K2])*panel.dir[EN_K2];ydensity=(uint32_t)clamp((int32_t)ydensity+d*2,0,100);
   d=fm1_enc_take(panel.enc[EN_K3])*panel.dir[EN_K3];ymutation=(uint32_t)clamp((int32_t)ymutation+d*2,0,100);
   d=fm1_enc_take(panel.enc[EN_K4])*panel.dir[EN_K4];ytranspose=clamp(ytranspose+d,-12,12);
   if(oldtempo!=ytempo||olddensity!=ydensity||oldmutation!=ymutation||oldtranspose!=ytranspose)ypost();
   yled_update();ydisplay();
  }
 }
}
