/* Documented in YM-USB-PROTOCOL.md. Status only; no flash/settings commands. */
static void yusb_service(void){
 const uint8_t*p;uint32_t n,post=0,old_events=yclock.events,old_bpm=yclock.bpm;
 static uint32_t seen_lost;
 if(seen_lost!=midi_in_lost){
  uint32_t f=irq_save();seen_lost=midi_in_lost;mi_r=mi_w;irq_restore(f);
  ymidi_port_keys[0]=ymidi_port_keys[1]=ymidi_keys=ymidi_discard=0;yc_fail(&yclock);
  if(yclock.mode){yplaying=0;post=1;}
 }
 if(yc_expire(&yclock,fm1_ticks(),FM1_TICKS_PER_US)){yplaying=0;post=1;}
 for(uint32_t read=0;read<MQ&&mi_r!=mi_w;read++){
  uint32_t packet=midi_in_q[mi_r%MQ],at=midi_in_at[mi_r%MQ],status=(packet>>8)&255u,port=(packet>>4)&15u,discard=ymidi_discard!=0;
  if(discard)ymidi_discard--;
  RING_PUBLISH();mi_r++;
  if(port>1u)continue;
  if(!port&&(status==0xf8u||status==0xfau||status==0xfbu||status==0xfcu)){
   if(ymidi_out||discard)continue; /* master must never follow its own DAW echo */
   if((status==0xfau||status==0xfbu)&&ym_overload>=8u)continue;
   uint32_t action=yc_usb(&yclock,status,at,FM1_TICKS_PER_US);
   if(action){yplaying=yclock.running;post=1;if(action==2u)ym_reseed(ym_seed());}
  }
  else if(ymidi_out||discard)continue;
  else if((status&0xf0u)==0x90u||(status&0xf0u)==0x80u){
   uint32_t note=(packet>>16)&127u,velocity=(packet>>24)&127u;
   if(note>=53u&&note<=79u){uint32_t bit=1u<<(note-53u);
    if((status&0xf0u)==0x90u&&velocity)ymidi_port_keys[port]|=bit;else ymidi_port_keys[port]&=~bit;
    ymidi_keys=ymidi_port_keys[0]|ymidi_port_keys[1];}
  }else if((status&0xf0u)==0xb0u&&((packet>>16)&127u)>=20u&&((packet>>16)&127u)<=22u){
   uint32_t source=status&15u,effect=((packet>>16)&127u)-20u;
   if(source<12u){uint32_t value=(((packet>>24)&127u)*100u+63u)/127u;if(yfx_values[source][effect]!=value){yfx_values[source][effect]=value;ym_effect(source,effect,value);ys_changed(fm1_ms);}}
  }else if((status&0xf0u)==0xb0u&&(((packet>>16)&127u)==120u||((packet>>16)&127u)==123u)){ymidi_port_keys[port]=0;ymidi_keys=ymidi_port_keys[0]|ymidi_port_keys[1];}
 }
 if(yc_expire(&yclock,fm1_ticks(),FM1_TICKS_PER_US)){yplaying=0;post=1;}
 if(post||old_events!=yclock.events||old_bpm!=yclock.bpm)ypost();
 if(ota_frame_get(&p,&n)&&n==5u&&p[0]==0x7du&&p[1]==0x45u&&p[2]==0x4du&&p[3]==0x59u&&p[4]==1u){
  uint8_t reply[62]={0xf0,0x7d,0x45,0x4d,0x59,0x41};uint32_t v[11],f=irq_save();
  v[0]=yplaying;v[1]=audio_cpu_pct;v[2]=audio_cpu_lifetime_peak;v[3]=audio_xruns;v[4]=ym_overload;
  v[5]=ym_bar();v[6]=ym_seed();v[7]=fm1_ms;v[8]=en_dbg.halves;v[9]=en_dbg.max_us;v[10]=bootguard.failed;
  irq_restore(f);ota_frame_done();
  for(uint32_t i=0;i<11u;i++)for(uint32_t k=0;k<5u;k++)reply[6u+i*5u+k]=(uint8_t)((v[i]>>(7u*k))&127u);
  reply[61]=0xf7;ota_wire_send(reply,sizeof reply);
 }
}
