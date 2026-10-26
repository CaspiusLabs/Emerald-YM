/* SPDX-License-Identifier: GPL-3.0-only
 * Publish complete LED rows; the scan never sees a clear-and-relight interval. */
static uint8_t yled_pos[14];
static uint32_t yrec_until;
static void yled_init(void){
 for(uint32_t id=0;id<14u;id++){
  yled_pos[id]=255;
  for(uint32_t c=0;c<FM1_NCOL;c++)for(uint32_t r=1;r<5;r++)if(FM1_KEYMAP[r][c]==(int8_t)id)yled_pos[id]=(uint8_t)((c<<3)|r);
 }
}
static void yled_add(uint8_t*rows,uint32_t role,int green){
 uint8_t pos=green?(uint8_t)((8u<<3)|1u):yled_pos[panel.btn[role]];
 if(pos!=255)rows[pos>>3]|=(uint8_t)(1u<<(pos&7u));
}
static void yled_update(void){
 uint8_t bright[FM1_NCOL]={0},dim[FM1_NCOL]={0};
 if(ym_overload>=8)yled_add(bright,B_PLAY,0);
 else yled_add(yplaying?bright:dim,B_PLAY,1);
 yled_add((int32_t)(yrec_until-fm1_ms)>0?bright:dim,B_REC,0);
 yled_add(ymutation==0?bright:dim,B_SEQ,0);
 {uint32_t both=(1u<<panel.btn[B_OCTDN])|(1u<<panel.btn[B_OCTUP]);
  if((fm1_in.buttons&both)==both){yled_add(bright,B_OCTDN,0);yled_add(bright,B_OCTUP,0);}}
 for(uint32_t c=0;c<FM1_NCOL;c++){fm1_led[c]=bright[c];fm1_led_dim[c]=dim[c];}
 fm1_led_dim_div=4;
}
