/* SPDX-License-Identifier: GPL-3.0-only
 * Atomic offscreen tiles: never clear the visible screen during normal refresh.
 * Uses the platform's existing canvas and LCD DMA; no new framebuffer. */
#define Y_BG RGB(5,9,20)
#define Y_CARD RGB(13,23,42)
#define Y_EDGE RGB(28,48,77)
#define Y_BLUE RGB(50,197,255)
#define Y_GREEN RGB(96,241,169)
#define Y_WHITE RGB(224,239,255)
#define Y_DIM RGB(121,149,181)
#define Y_AMBER RGB(255,176,64)
#define Y_RED RGB(255,35,40)
static uint8_t yui_ready;
static struct {uint32_t tempo,density,mutation,bar,playing,overload,step,psg,selected,level,fxsource,fxkind,fxvalue,save,master,usb_only;int32_t transpose;} yui_old;
/* Original console silhouette: circular cartridge deck, vents and red power LED.
 * Drawn in the existing tile canvas, no bitmap asset or extra framebuffer. */
static void yui_ring(int cx,int cy,int r,uint16_t c){
 int x=r,y=0,err=1-r;
 while(x>=y){
  cv_pset(cx+x,cy+y,c);cv_pset(cx+y,cy+x,c);cv_pset(cx-y,cy+x,c);cv_pset(cx-x,cy+y,c);
  cv_pset(cx-x,cy-y,c);cv_pset(cx-y,cy-x,c);cv_pset(cx+y,cy-x,c);cv_pset(cx+x,cy-y,c);
  y++;if(err<0)err+=2*y+1;else{x--;err+=2*(y-x)+1;}
 }
}
static void yui_boot(void){
 lcd_fill(0,0,240,240,Y_BG);cv_begin(224,128,Y_BG);
 cv_rect(6,17,212,102,RGB(39,42,49));cv_rect(3,22,218,90,RGB(26,29,36));
 cv_rect(9,24,206,82,RGB(17,20,28));
 for(int r=39;r>=35;r--)yui_ring(91,63,r,r==37?RGB(162,147,102):Y_EDGE);
 yui_ring(91,63,31,RGB(49,55,66));cv_rect(60,43,62,11,C_BLACK);
 cv_rect(62,44,58,2,Y_DIM);cv_text(72,64,&FONT_XS,"16-BIT",Y_WHITE);
 for(int k=0;k<10;k++)cv_rect(148,32+k*5,49,2,RGB(57,63,75));
 cv_rect(18,93,5,5,RGB(255,55,66));cv_text(27,92,&FONT_XS,"POWER",Y_DIM);
 cv_rect(63,109,34,6,C_BLACK);cv_rect(131,109,34,6,C_BLACK);
 cv_blit(8,25);
 cv_begin(224,66,Y_BG);
 cv_text((224-text_w(&FONT_M,"EMERALD YM"))/2,0,&FONT_M,"EMERALD YM",Y_WHITE);
 cv_text((224-text_w(&FONT_XS,"16-BIT SOUND SYSTEM"))/2,28,&FONT_XS,"16-BIT SOUND SYSTEM",Y_BLUE);
 cv_text((224-text_w(&FONT_XS,EN_VERSION))/2,45,&FONT_XS,EN_VERSION,Y_DIM);cv_blit(8,164);
 yui_ready=0;
}
static void yui_header(void){
 cv_begin(240,44,Y_BG);
 /* Original faceted jewel + striped 16-bit badge, drawn natively. */
 for(int i=0;i<3;i++){
  int r=15-i*4;uint16_t c=i==1?Y_WHITE:Y_BLUE;
  cv_line(22,5+i*4,22+r,20,c);cv_line(22+r,20,22,35-i*4,c);
  cv_line(22,35-i*4,22-r,20,c);cv_line(22-r,20,22,5+i*4,c);
 }
 cv_line(7,20,37,20,Y_GREEN);cv_line(22,5,22,35,Y_GREEN);
 cv_text(41,2,&FONT_M,"EMERALD YM",RGB(14,68,122));
 cv_text(40,1,&FONT_M,"EMERALD YM",Y_WHITE);
 cv_text(45,28,&FONT_XS,"YM + PSG / LIVE",Y_BLUE);
 cv_rect(185,4,48,21,Y_EDGE);cv_rect(187,6,44,17,Y_CARD);
 cv_text(191,6,&FONT_XS,"16-BIT",Y_WHITE);
 cv_rect(191,19,36,1,Y_BLUE);cv_rect(191,21,36,1,Y_BLUE);
 cv_text(185+(48-text_w(&FONT_XS,EN_VERSION))/2,28,&FONT_XS,EN_VERSION,Y_DIM);
 cv_rect(8,42,224,1,Y_EDGE);cv_blit(0,0);
}
/* Reference-inspired layout on a 240x240 LCD: 40px volume rail and 180px cards. */
static void yui_frame(int w,int h){
 cv_rect(0,0,w,1,Y_EDGE);cv_rect(0,h-1,w,1,Y_EDGE);
 cv_rect(0,0,1,h,Y_EDGE);cv_rect(w-1,0,1,h,Y_EDGE);
 cv_rect(1,1,w-2,1,RGB(19,62,89));
}
/* Position uses the linear pot reading, not squared audio gain or a VU meter.
 * Hysteresis is visual only: about two ADC counts beyond a half-pixel boundary. */
static uint32_t yui_master_position(uint32_t raw,uint32_t previous){
 if(raw>1023u)raw=1023u;
 if(previous<=90u){
  int32_t delta=(int32_t)(raw*90u)-(int32_t)(previous*1023u);
  if(delta>=-700&&delta<=700)return previous;
 }
 return (raw*90u+511u)/1023u;
}
static void yui_volume_base(void){
 /* Split the tall rail into two tiles, each below the canvas's 144-row limit. */
 cv_begin(40,136,Y_CARD);cv_rect(0,0,40,1,Y_EDGE);cv_rect(1,1,38,1,Y_BLUE);
 cv_rect(0,0,1,136,Y_EDGE);cv_rect(39,0,1,136,Y_EDGE);
 cv_text(2,5,&FONT_XS,"VOLUME",Y_WHITE);
 for(int k=0;k<=10;k++)cv_rect(24,30+k*9,2,1,Y_DIM);
 cv_text(26,24,&FONT_XS,"10",Y_DIM);cv_text(26,69,&FONT_XS,"5",Y_DIM);cv_text(26,114,&FONT_XS,"0",Y_DIM);
 cv_blit(8,48);
}
static void yui_output_mode(void){
 cv_begin(40,32,Y_CARD);cv_rect(0,0,1,32,Y_EDGE);cv_rect(39,0,1,32,Y_EDGE);
 cv_rect(0,31,40,1,Y_EDGE);cv_rect(1,30,38,1,Y_BLUE);
 if(yusb_only){
  cv_text(11,3,&FONT_XS,"USB",Y_AMBER);cv_text(8,16,&FONT_XS,"ONLY",Y_AMBER);
 }else{
 /* Native headphone symbol, no bitmap or copied logo. */
 cv_line(12,17,12,10,Y_WHITE);cv_line(12,10,16,5,Y_WHITE);
 cv_line(16,5,23,5,Y_WHITE);cv_line(23,5,27,10,Y_WHITE);cv_line(27,10,27,17,Y_WHITE);
 cv_rect(11,16,5,10,Y_DIM);cv_rect(12,16,2,10,Y_WHITE);
 cv_rect(24,16,5,10,Y_DIM);cv_rect(25,16,2,10,Y_WHITE);}
 cv_blit(8,184);
}
static void yui_volume(uint32_t position){
 int y=2+(int)(90u-position);
 cv_begin(22,108,Y_CARD);
 cv_rect(3,1,14,106,Y_EDGE);cv_rect(4,2,12,104,Y_BG);
 for(int k=0;k<11;k++)cv_rect(6,6+k*9,8,1,Y_EDGE);
 cv_rect(0,y,22,14,Y_BG);cv_rect(1,y+1,20,12,Y_EDGE);
 cv_rect(2,y+2,18,10,RGB(15,50,75));cv_rect(3,y+2,16,1,Y_DIM);
 cv_rect(2,y+6,18,3,Y_RED);cv_rect(3,y+12,16,1,RGB(9,27,42));
 cv_blit(10,70);
}
static void yui_transport(uint32_t bar){
 char b[16];uint16_t accent=ym_overload>=8?Y_AMBER:yplaying?Y_GREEN:Y_DIM;
 cv_begin(180,24,Y_CARD);yui_frame(180,24);
 if(yplaying){for(int x=0;x<9;x++)cv_rect(8+x,5+x/2,1,13-x,accent);}
 else {cv_rect(8,6,3,12,accent);cv_rect(14,6,3,12,accent);}
 cv_text(25,4,&FONT_S,ym_overload>=8?"OVERLOAD":yplaying?"PLAYING":"PAUSED",accent);
 if(ys_state!=YS_CLEAN){
  static const char*const save[]={"","SAVE *","SAVING","SAVED","SAVE ERR"};
  cv_text(126,6,&FONT_XS,save[ys_state],ys_state==YS_ERROR?Y_AMBER:ys_state==YS_SAVED?Y_GREEN:Y_DIM);
 }else {cv_text(111,6,&FONT_XS,"BAR",Y_DIM);fmt_int(b,(int32_t)(bar%100000u));cv_text(136,4,&FONT_S,b,Y_WHITE);}
 cv_blit(52,48);
}
static void yui_card(uint32_t index,int32_t value){
 static const char*const label[]={"K1 TEMPO","K2 DENSITY","K3 MUTATION","K4 TRANSPOSE"};
 char b[16];uint16_t accent=index==0||index==3?Y_BLUE:Y_GREEN;
 cv_begin(86,44,Y_CARD);yui_frame(86,44);
 cv_text(5,2,&FONT_XS,label[index],Y_DIM);
 if(index==3&&value>0){b[0]='+';fmt_int(b+1,value);}else fmt_int(b,value);
 cv_text(5,14,&FONT_M,b,Y_WHITE);
 cv_text(62,24,&FONT_XS,index==0?"BPM":index==3?"ST":"%",accent);
 cv_rect(5,39,76,3,Y_EDGE);
 if(index==3){
  int x=43+value*3;
  cv_rect(value<0?x:43,39,value<0?43-x:x-43,3,accent);
  cv_rect(42,38,1,5,Y_DIM);cv_rect(x-1,38,2,5,Y_WHITE);
 }else{
  uint32_t fill=index==0?(uint32_t)(value-80)*76u/100u:(uint32_t)value*76u/100u;
  if(fill>76u)fill=76u;cv_rect(5,39,(int32_t)fill,3,accent);
 }
 cv_blit(index&1u?146u:52u,index>=2u?128u:78u);
}
static void yui_voice(void){
 static const char*const names[]={"FM BASS","FM LEAD","FM CHORD 1","FM CHORD 2","FM CHORD 3","KICK","SNARE","CLOSED HAT","OPEN HAT","PSG ARP","PSG LIVE","PSG NOISE"};
 char b[32];uint32_t value=ylevels[yinstrument];int enabled=yinstrument<9u||ypsg;
 cv_begin(116,38,Y_CARD);yui_frame(116,38);
 str_cpy(b,"SEL ",sizeof b);str_cpy(b+4,names[yinstrument],sizeof b-4);
 cv_text(4,3,&FONT_XS,b,Y_DIM);
 if(enabled){str_cpy(b,"PRE ",sizeof b);fmt_int(b+4,(int32_t)value);str_cpy(b+str_len(b),"%",sizeof b-str_len(b));}
 else str_cpy(b,"PSG OFF",sizeof b);
 cv_text(4,18,&FONT_S,b,enabled?Y_GREEN:Y_DIM);
 cv_rect(65,29,47,3,Y_EDGE);if(enabled)cv_rect(65,29,(int32_t)(value*47u/100u),3,Y_GREEN);
 cv_blit(52,178);
}
static void yui_effect(void){
 static const char*const names[]={"DECAY","COLOR","ECHO"};
 char b[20];uint32_t kind=yfx_kind[yinstrument],value=yfx_values[yinstrument][kind];
 cv_begin(60,38,Y_CARD);yui_frame(60,38);str_cpy(b,"ALG ",sizeof b);str_cpy(b+4,names[kind],sizeof b-4);
 cv_text(4,3,&FONT_XS,b,Y_DIM);fmt_int(b,(int32_t)value);str_cpy(b+str_len(b),"%",sizeof b-str_len(b));
 cv_text(4,17,&FONT_S,b,value?Y_AMBER:Y_DIM);cv_rect(4,34,52,2,Y_EDGE);cv_rect(4,34,(int32_t)(value*52u/100u),2,Y_AMBER);cv_blit(172,178);
}
static void yui_steps(uint32_t step){
 cv_begin(224,12,Y_BG);
 for(uint32_t k=0;k<16;k++)cv_rect((int32_t)(k*14u),4,10,4,yplaying&&k==step?Y_GREEN:k%4u==0u?Y_DIM:Y_EDGE);
 cv_blit(8,228);
}
static void ydisplay_invalidate(void){yui_ready=0;}
static void ydisplay(void){
 uint32_t bar=ym_bar(),step=ym_step();
 uint32_t overloaded=ym_overload>=8u,step_key=step|(yplaying<<4)|(overloaded<<5);
 if(!yui_ready){
  lcd_fill(0,0,240,240,Y_BG);yui_header();yui_volume_base();
  memset(&yui_old,255,sizeof yui_old);yui_old.transpose=1000;yui_ready=1;
 }
 if(yui_old.playing!=yplaying||yui_old.bar!=bar||yui_old.overload!=overloaded||yui_old.save!=ys_state){yui_transport(bar);yui_old.playing=yplaying;yui_old.bar=bar;yui_old.overload=overloaded;yui_old.save=ys_state;}
 {uint32_t position=yui_master_position(ymaster_adc,yui_old.master);
  if(yui_old.master!=position){yui_volume(position);yui_old.master=position;}}
 if(yui_old.usb_only!=yusb_only){yui_output_mode();yui_old.usb_only=yusb_only;}
 if(yui_old.tempo!=ytempo){yui_card(0,(int32_t)ytempo);yui_old.tempo=ytempo;}
 if(yui_old.density!=ydensity){yui_card(1,(int32_t)ydensity);yui_old.density=ydensity;}
 if(yui_old.mutation!=ymutation){yui_card(2,(int32_t)ymutation);yui_old.mutation=ymutation;}
 if(yui_old.transpose!=ytranspose){yui_card(3,ytranspose);yui_old.transpose=ytranspose;}
 if(yui_old.step!=step_key){yui_steps(step);yui_old.step=step_key;}
 if(yui_old.psg!=ypsg||yui_old.selected!=yinstrument||yui_old.level!=ylevels[yinstrument]){
  yui_voice();yui_old.psg=ypsg;yui_old.selected=yinstrument;yui_old.level=ylevels[yinstrument];}
 if(yui_old.fxsource!=yinstrument||yui_old.fxkind!=yfx_kind[yinstrument]||yui_old.fxvalue!=yfx_values[yinstrument][yfx_kind[yinstrument]]){yui_effect();yui_old.fxsource=yinstrument;yui_old.fxkind=yfx_kind[yinstrument];yui_old.fxvalue=yfx_values[yinstrument][yfx_kind[yinstrument]];}
}
