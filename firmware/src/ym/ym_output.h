/* SPDX-License-Identifier: GPL-3.0-only
 * Fork the master mix to USB before fading only the physical DAC/I2S buffer.
 * No allocation, codec reset or stopped DMA: USB/MIDI/diagnostics keep running. */
static uint32_t ylocal_gain=256u;
static void youtput_controls(uint32_t pressed){
 if(pressed&(1u<<B_GLO))yusb_only^=1u;
}
static void youtput_block(int32_t*out,uint32_t frames,uint32_t master,uint32_t usb_only){
 uint32_t target=usb_only?0u:256u;
 if(ym_overload<8u)ym_render(out,frames,master);
 else memset(out,0,frames*2u*sizeof(int32_t));
#if FELUCCA_UAC
 uac_tap(out,frames); /* Copy the unmuted mix before touching the local output. */
#endif
 if(ylocal_gain==256u&&target==256u)return;
 if(ylocal_gain==0u&&target==0u){memset(out,0,frames*2u*sizeof(int32_t));return;}
 /* One 256-frame block (~5.8ms) ramps between zero and unity. Keep all math
    within signed 32 bits for the engine's bounded 24-bit output. */
 for(uint32_t i=0;i<frames;i++){
  if(ylocal_gain<target)ylocal_gain++;else if(ylocal_gain>target)ylocal_gain--;
  if(ylocal_gain<256u){
   out[2u*i]=(out[2u*i]/256)*(int32_t)ylocal_gain;
   out[2u*i+1u]=(out[2u*i+1u]/256)*(int32_t)ylocal_gain;
  }
 }
}
